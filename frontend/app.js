/* ============================================================
   HOSTEL ROOM ALLOCATOR — app.js
   Implements Greedy Allocation (mirrors greedy.cpp logic)
   ============================================================ */

'use strict';

// ===================== STATE =====================
let rooms = [];
let students = [];
let roomIdCounter = 1;
let studentIdCounter = 1;
let lastResult = null;

// ===================== SEED DATA =====================
function seedData() {
  rooms = [
    { id: 101, capacity: 2 },
    { id: 102, capacity: 3 },
    { id: 103, capacity: 2 },
  ];
  roomIdCounter = 104;

  students = [
    { id: 1, cgpa: 9.5, preferences: [101, 102] },
    { id: 2, cgpa: 8.8, preferences: [102, 103] },
    { id: 3, cgpa: 9.1, preferences: [101, 103] },
    { id: 4, cgpa: 7.4, preferences: [103] },
    { id: 5, cgpa: 8.0, preferences: [102, 101] },
  ];
  studentIdCounter = 6;
}

// ===================== GREEDY ALGORITHM =====================
// Mirrors greedy.cpp logic:
//   1. Sort students by CGPA descending
//   2. For each student, try each preference in order
//   3. Assign first available room that has capacity
function runGreedy(studentsInput, roomsInput) {
  const t0 = performance.now();

  // Deep clone so we don't mutate originals
  const sts = studentsInput.map(s => ({
    ...s,
    preferences: [...s.preferences],
    assignedRoom: -1,
    allocated: false,
  }));
  const rms = roomsInput.map(r => ({ ...r, occupied: 0 }));

  // Sort by CGPA descending
  sts.sort((a, b) => b.cgpa - a.cgpa);

  for (const s of sts) {
    for (const pref of s.preferences) {
      const room = rms.find(r => r.id === pref && r.occupied < r.capacity);
      if (room) {
        s.assignedRoom = room.id;
        s.allocated = true;
        room.occupied++;
        break;
      }
    }
  }

  const t1 = performance.now();
  const execTime = ((t1 - t0) * 1000).toFixed(2); // microseconds

  return { students: sts, rooms: rms, execTime };
}

// ===================== STABLE MATCHING (simplified) =====================
// A preference-based stable matching simulation.
// Students propose to rooms in CGPA order, rooms accept/reject.
function runStableMatching(studentsInput, roomsInput) {
  const t0 = performance.now();

  const sts = studentsInput.map(s => ({
    ...s,
    preferences: [...s.preferences],
    assignedRoom: -1,
    allocated: false,
    nextPref: 0,
  }));
  const rms = roomsInput.map(r => ({ ...r, occupied: 0, assignedStudents: [] }));

  // Sort by CGPA desc as initial proposal order
  sts.sort((a, b) => b.cgpa - a.cgpa);

  let changed = true;
  const MAX_ROUNDS = studentsInput.length * roomsInput.length + 5;
  let round = 0;

  while (changed && round < MAX_ROUNDS) {
    changed = false;
    round++;

    for (const s of sts) {
      if (s.allocated) continue;
      if (s.nextPref >= s.preferences.length) continue;

      const prefRoomId = s.preferences[s.nextPref];
      const room = rms.find(r => r.id === prefRoomId);
      s.nextPref++;

      if (!room) continue;

      if (room.occupied < room.capacity) {
        // Room has space — accept directly
        s.assignedRoom = room.id;
        s.allocated = true;
        room.occupied++;
        room.assignedStudents.push(s.id);
        changed = true;
      } else {
        // Room full — check if student is better than worst occupant
        const worstId = room.assignedStudents.reduce((wId, sId) => {
          const w = sts.find(x => x.id === wId);
          const c = sts.find(x => x.id === sId);
          return (w && c && c.cgpa < w.cgpa) ? sId : wId;
        });
        const worst = sts.find(x => x.id === worstId);
        if (worst && s.cgpa > worst.cgpa) {
          // Evict worst
          worst.allocated = false;
          worst.assignedRoom = -1;
          room.assignedStudents = room.assignedStudents.filter(id => id !== worstId);
          // Assign current student
          s.assignedRoom = room.id;
          s.allocated = true;
          room.assignedStudents.push(s.id);
          changed = true;
        }
      }
    }
  }

  const t1 = performance.now();
  const execTime = ((t1 - t0) * 1000).toFixed(2);

  return { students: sts, rooms: rms, execTime };
}

// ===================== STATS =====================
function computeStats(result) {
  const { students: sts, rooms: rms } = result;
  const allocated = sts.filter(s => s.allocated).length;
  const unallocated = sts.length - allocated;
  const firstPref = sts.filter(s =>
    s.allocated && s.preferences.length > 0 && s.assignedRoom === s.preferences[0]
  ).length;
  const satisfaction = sts.length > 0 ? ((firstPref / sts.length) * 100).toFixed(1) + '%' : '—';
  const totalCapacity = rms.reduce((sum, r) => sum + r.capacity, 0);
  const emptyBeds = totalCapacity - allocated;
  return { allocated, unallocated, satisfaction, emptyBeds };
}

// ===================== RENDER =====================
function renderRooms() {
  const list = document.getElementById('rooms-list');
  if (rooms.length === 0) {
    list.innerHTML = '<p class="empty-msg">No rooms added yet.</p>';
    return;
  }
  list.innerHTML = rooms.map((r, i) => `
    <div class="row-item" id="room-row-${r.id}">
      <span class="row-label">#${i + 1}</span>
      <div class="input-group">
        <label>Room ID</label>
        <input type="number" min="1" value="${r.id}"
               onchange="updateRoom(${r.id}, 'id', +this.value)" />
      </div>
      <div class="input-group">
        <label>Capacity</label>
        <input type="number" min="1" max="20" value="${r.capacity}"
               onchange="updateRoom(${r.id}, 'capacity', +this.value)" />
      </div>
      <button class="btn-remove" onclick="removeRoom(${r.id})" title="Remove room">✕</button>
    </div>
  `).join('');
}

function renderStudents() {
  const list = document.getElementById('students-list');
  if (students.length === 0) {
    list.innerHTML = '<p class="empty-msg">No students added yet.</p>';
    return;
  }
  list.innerHTML = students.map((s, i) => `
    <div class="row-item" id="student-row-${s.id}">
      <span class="row-label">#${i + 1}</span>
      <div class="input-group">
        <label>Student ID</label>
        <input type="number" min="1" value="${s.id}"
               onchange="updateStudent(${s.id}, 'id', +this.value)" />
      </div>
      <div class="input-group">
        <label>CGPA</label>
        <input type="number" min="0" max="10" step="0.01" value="${s.cgpa}"
               onchange="updateStudent(${s.id}, 'cgpa', +this.value)" />
      </div>
      <div class="input-group" style="flex:2">
        <label>Preferences (Room IDs, comma-sep)</label>
        <input type="text" value="${s.preferences.join(', ')}"
               placeholder="e.g. 101, 102"
               onchange="updateStudent(${s.id}, 'preferences', this.value)" />
      </div>
      <button class="btn-remove" onclick="removeStudent(${s.id})" title="Remove student">✕</button>
    </div>
  `).join('');
}

function renderResults(greedy, stable) {
  const stats = computeStats(greedy);

  // Stat cards
  document.getElementById('stat-allocated').textContent = stats.allocated;
  document.getElementById('stat-unallocated').textContent = stats.unallocated;
  document.getElementById('stat-satisfaction').textContent = stats.satisfaction;
  document.getElementById('stat-time').textContent = greedy.execTime + ' μs';
  document.getElementById('stat-empty-beds').textContent = stats.emptyBeds;

  // Allocation table
  const tbody = document.getElementById('alloc-table-body');
  if (greedy.students.length === 0) {
    tbody.innerHTML = '<tr><td colspan="4" class="empty-msg">No data</td></tr>';
  } else {
    tbody.innerHTML = greedy.students.map(s => {
      const isFirst = s.allocated && s.preferences.length > 0 && s.assignedRoom === s.preferences[0];
      const badge = s.allocated
        ? (isFirst
          ? '<span class="badge badge-success">✓ 1st Pref</span>'
          : '<span class="badge badge-warning">✓ Other Pref</span>')
        : '<span class="badge badge-danger">✗ Unallocated</span>';
      return `
        <tr>
          <td>${s.id}</td>
          <td>${s.cgpa.toFixed(2)}</td>
          <td>${s.allocated ? 'Room ' + s.assignedRoom : '—'}</td>
          <td>${badge}</td>
        </tr>`;
    }).join('');
  }

  // Room occupancy bars
  const occDiv = document.getElementById('room-occupancy');
  occDiv.innerHTML = greedy.rooms.map(r => {
    const pct = r.capacity > 0 ? Math.round((r.occupied / r.capacity) * 100) : 0;
    return `
      <div class="room-bar-item">
        <div class="room-bar-header">
          <span class="room-bar-name">Room ${r.id}</span>
          <span class="room-bar-count">${r.occupied} / ${r.capacity} occupied</span>
        </div>
        <div class="room-bar-track">
          <div class="room-bar-fill" style="width:0%" data-target="${pct}%"></div>
        </div>
      </div>`;
  }).join('');

  // Animate bars
  requestAnimationFrame(() => {
    document.querySelectorAll('.room-bar-fill').forEach(bar => {
      bar.style.width = bar.dataset.target;
    });
  });

  // Comparison table
  const gStats = computeStats(greedy);
  const sStats = computeStats(stable);

  document.getElementById('cmp-greedy-sat').textContent = gStats.satisfaction;
  document.getElementById('cmp-stable-sat').textContent = sStats.satisfaction;
  document.getElementById('cmp-greedy-unalloc').textContent = gStats.unallocated;
  document.getElementById('cmp-stable-unalloc').textContent = sStats.unallocated;
  document.getElementById('cmp-greedy-beds').textContent = gStats.emptyBeds;
  document.getElementById('cmp-stable-beds').textContent = sStats.emptyBeds;
  document.getElementById('cmp-greedy-time').textContent = greedy.execTime;
  document.getElementById('cmp-stable-time').textContent = stable.execTime;

  document.getElementById('compare-note').textContent =
    'Last updated: ' + new Date().toLocaleTimeString();
}

// ===================== ACTIONS =====================
function addRoom() {
  rooms.push({ id: roomIdCounter++, capacity: 2 });
  renderRooms();
  showToast('Room added');
}

function removeRoom(id) {
  rooms = rooms.filter(r => r.id !== id);
  renderRooms();
  showToast('Room removed');
}

function updateRoom(id, field, value) {
  const r = rooms.find(r => r.id === id);
  if (!r) return;
  if (field === 'capacity') r.capacity = Math.max(1, value);
  if (field === 'id') r.id = value;
}

function addStudent() {
  students.push({ id: studentIdCounter++, cgpa: 8.0, preferences: [] });
  renderStudents();
  showToast('Student added');
}

function removeStudent(id) {
  students = students.filter(s => s.id !== id);
  renderStudents();
  showToast('Student removed');
}

function updateStudent(id, field, value) {
  const s = students.find(s => s.id === id);
  if (!s) return;
  if (field === 'cgpa') s.cgpa = Math.min(10, Math.max(0, value));
  if (field === 'id') s.id = value;
  if (field === 'preferences') {
    s.preferences = value.split(',')
      .map(v => parseInt(v.trim(), 10))
      .filter(n => !isNaN(n));
  }
}

function runAllocation() {
  if (rooms.length === 0 || students.length === 0) {
    showToast('⚠️  Add at least one room and one student');
    return;
  }

  const btn = document.getElementById('run-btn');
  btn.textContent = '⏳ Running…';
  btn.disabled = true;

  setTimeout(() => {
    const greedy = runGreedy(students, rooms);
    const stable = runStableMatching(students, rooms);
    lastResult = { greedy, stable };

    renderResults(greedy, stable);
    switchTab('result');

    btn.innerHTML = '<span class="btn-run-icon">⚡</span> Run Allocation';
    btn.disabled = false;

    showToast('✅ Allocation complete');
  }, 80); // slight delay for UX feel
}

function resetAll() {
  rooms = [];
  students = [];
  roomIdCounter = 1;
  studentIdCounter = 1;
  lastResult = null;
  seedData();
  renderRooms();
  renderStudents();
  switchTab('setup');
  showToast('Reset to defaults');
}

// ===================== TABS =====================
function switchTab(name) {
  document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
  document.querySelectorAll('.pill').forEach(el => el.classList.remove('active'));
  document.getElementById(`tab-content-${name}`).classList.add('active');
  document.getElementById(`tab-${name}`).classList.add('active');
}

// ===================== TOAST =====================
let toastTimer;
function showToast(msg) {
  const toast = document.getElementById('toast');
  toast.textContent = msg;
  toast.classList.add('show');
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => toast.classList.remove('show'), 2500);
}

// ===================== INIT =====================
document.addEventListener('DOMContentLoaded', () => {
  seedData();
  renderRooms();
  renderStudents();

  document.getElementById('add-room-btn').addEventListener('click', addRoom);
  document.getElementById('add-student-btn').addEventListener('click', addStudent);
  document.getElementById('run-btn').addEventListener('click', runAllocation);
  document.getElementById('reset-btn').addEventListener('click', resetAll);

  document.querySelectorAll('.pill').forEach(pill => {
    pill.addEventListener('click', () => switchTab(pill.dataset.tab));
  });
});
