async function api(path, method = 'GET', body = null) {
  const opts = { method, headers: {} };
  if (body) {
    opts.headers['Content-Type'] = 'application/json';
    opts.body = JSON.stringify(body);
  }
  const res = await fetch(path, opts);
  if (!res.ok) {
    const text = await res.text();
    throw new Error(res.status + ' ' + res.statusText + '\n' + text);
  }
  return res.json();
}

function el(id) { return document.getElementById(id); }

function formatPrefs(prefs) {
  return prefs && prefs.length ? prefs.join(', ') : 'None';
}

function renderRoomRows(rooms) {
  if (!rooms.length) return '<div class="message-box">No rooms added yet.</div>';
  return `
    <table>
      <thead>
        <tr><th>ID</th><th>Type</th><th>Capacity</th><th>Occupied</th><th>Free</th><th>Block</th><th>AC</th><th>Status</th></tr>
      </thead>
      <tbody>
        ${rooms.map(r => `
          <tr>
            <td>${r.roomID}</td>
            <td>${r.roomType}</td>
            <td>${r.capacity}</td>
            <td>${r.occupied}</td>
            <td>${Math.max(0, r.capacity - r.occupied)}</td>
            <td>${r.block || '—'}</td>
            <td>${r.hasAC ? 'Yes' : 'No'}</td>
            <td>${r.isAvailable ? 'Available' : 'Unavailable'}</td>
          </tr>`).join('')}
      </tbody>
    </table>`;
}

function renderStudentRows(students) {
  if (!students.length) return '<div class="message-box">No students added yet.</div>';
  return `
    <table>
      <thead>
        <tr><th>ID</th><th>Name</th><th>Gender</th><th>CGPA</th><th>Year</th><th>Course</th><th>Preferences</th><th>Assigned</th></tr>
      </thead>
      <tbody>
        ${students.map(s => `
          <tr>
            <td>${s.studentID}</td>
            <td>${s.name}</td>
            <td>${s.gender}</td>
            <td>${parseFloat(s.cgpa).toFixed(2)}</td>
            <td>${s.year}</td>
            <td>${s.course || '—'}</td>
            <td>${formatPrefs(s.preferences)}</td>
            <td>${s.allocated ? s.assignedRoomID : 'Not allocated'}</td>
          </tr>`).join('')}
      </tbody>
    </table>`;
}

function renderResultCard(title, data) {
  return `
    <div class="result-card">
      <h3>${title}</h3>
      <dl>
        ${Object.entries(data).map(([key, value]) => `
          <dt>${key}</dt><dd>${value}</dd>`).join('')}
      </dl>
    </div>`;
}

function renderAllocationResult(result) {
  const cards = [];
  const metrics = {
    'Exec Time (ms)': result.executionTime ?? 'N/A',
    'Satisfaction': result.satisfaction ? `${result.satisfaction.toFixed(2)}%` : 'N/A',
    'Remaining Beds': result.remainingBeds ?? 'N/A'
  };
  cards.push(renderResultCard('Metrics', metrics));

  if (result.students && result.students.length) {
    cards.push(`
      <div class="result-card">
        <h3>Allocated Students</h3>
        <table>
          <thead>
            <tr><th>ID</th><th>Name</th><th>Room</th><th>Allocated</th></tr>
          </thead>
          <tbody>
            ${result.students.map(s => `
              <tr>
                <td>${s.studentID}</td>
                <td>${s.name}</td>
                <td>${s.allocated ? s.assignedRoomID : '—'}</td>
                <td>${s.allocated ? 'Yes' : 'No'}</td>
              </tr>`).join('')}
          </tbody>
        </table>
      </div>`);
  }

  return cards.join('');
}

function renderCompareResults(compare) {
  return `
    <div class="result-card">
      <h3>Greedy vs Stable Comparison</h3>
      <table>
        <thead><tr><th>Algorithm</th><th>Time(ms)</th><th>Satisfaction</th><th>Remaining Beds</th></tr></thead>
        <tbody>
          <tr><td>Greedy</td><td>${compare.greedy.executionTime}</td><td>${compare.greedy.satisfaction.toFixed(2)}%</td><td>${compare.greedy.remainingBeds}</td></tr>
          <tr><td>Stable</td><td>${compare.stable.executionTime}</td><td>${compare.stable.satisfaction.toFixed(2)}%</td><td>${compare.stable.remainingBeds}</td></tr>
        </tbody>
      </table>
    </div>`;
}

function setMessage(text, type = 'info') {
  const box = el('messageBox');
  box.textContent = text;
  if (type === 'error') {
    box.style.borderColor = '#f5cbcb';
    box.style.backgroundColor = '#fff0f0';
    box.style.color = '#9f2a2a';
  } else {
    box.style.borderColor = '#c9d6f5';
    box.style.backgroundColor = '#eef3ff';
    box.style.color = '#2345a9';
  }
}

async function loadLists() {
  try {
    const rooms = await api('/api/rooms');
    const students = await api('/api/students');
    el('roomsContainer').innerHTML = renderRoomRows(rooms);
    el('studentsContainer').innerHTML = renderStudentRows(students);
    setMessage('Data loaded successfully.');
  } catch (e) {
    setMessage('Error loading lists: ' + e.message, 'error');
  }
}

document.addEventListener('DOMContentLoaded', () => {
  loadLists();

  el('refreshBtn').addEventListener('click', loadLists);

  el('roomForm').addEventListener('submit', async (ev) => {
    ev.preventDefault();
    const f = ev.target;
    const data = {
      roomID: Number(f.roomID.value),
      roomType: f.roomType.value,
      capacity: f.roomType.value === 'Single' ? 1 : (f.roomType.value === 'Double' ? 2 : 3),
      block: f.block.value || '',
      hasAC: f.hasAC.checked,
      isAvailable: f.isAvailable.checked
    };
    try {
      await api('/api/rooms', 'POST', data);
      setMessage('Room added successfully.');
      f.reset();
      loadLists();
    } catch (e) {
      setMessage('Error adding room: ' + e.message, 'error');
    }
  });

  el('studentForm').addEventListener('submit', async (ev) => {
    ev.preventDefault();
    const f = ev.target;
    const prefs = f.preferences.value.split(',').map(s => s.trim()).filter(s => s.length).map(Number);
    const data = {
      studentID: Number(f.studentID.value),
      name: f.name.value,
      gender: f.gender.value,
      cgpa: Number(f.cgpa.value) || 0,
      year: Number(f.year.value) || 1,
      course: f.course.value || '',
      preferences: prefs
    };
    try {
      await api('/api/students', 'POST', data);
      setMessage('Student added successfully.');
      f.reset();
      loadLists();
    } catch (e) {
      setMessage('Error adding student: ' + e.message, 'error');
    }
  });

  el('runGreedy').addEventListener('click', async () => {
    try {
      const res = await api('/api/run-greedy', 'POST');
      el('resultsContainer').innerHTML = renderAllocationResult(res);
      setMessage('Greedy allocation completed.');
    } catch (e) {
      setMessage('Error running greedy: ' + e.message, 'error');
    }
  });

  el('runStable').addEventListener('click', async () => {
    try {
      const res = await api('/api/run-stable', 'POST');
      el('resultsContainer').innerHTML = renderAllocationResult(res);
      setMessage('Stable matching completed.');
    } catch (e) {
      setMessage('Error running stable: ' + e.message, 'error');
    }
  });

  el('compare').addEventListener('click', async () => {
    try {
      const res = await api('/api/compare', 'POST');
      el('resultsContainer').innerHTML = renderCompareResults(res);
      setMessage('Comparison complete.');
    } catch (e) {
      setMessage('Error comparing algorithms: ' + e.message, 'error');
    }
  });

  el('reset').addEventListener('click', async () => {
    if (!confirm('Clear all rooms and students?')) return;
    try {
      await api('/api/reset', 'POST');
      el('roomsContainer').innerHTML = '<div class="message-box">No rooms yet.</div>';
      el('studentsContainer').innerHTML = '<div class="message-box">No students yet.</div>';
      el('resultsContainer').innerHTML = '';
      setMessage('Data reset successfully.');
    } catch (e) {
      setMessage('Error resetting data: ' + e.message, 'error');
    }
  });
});
