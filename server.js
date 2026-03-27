const express = require('express');
const { execFileSync } = require('child_process');
const path = require('path');
const app = express();
let rooms = [], students = [];

app.use(express.json());
app.use(express.static('./frontend'));

// Run the compiled C++ binary with --json flag
const runCpp = (algorithm, sts, rms) => {
  const payload = JSON.stringify({ algorithm, students: sts, rooms: rms });
  const exePath = path.join(__dirname, 'allocation_app');
  const stdout = execFileSync(exePath, ['--json'], { input: payload, encoding: 'utf-8' });
  return JSON.parse(stdout);
};

app.post('/api/rooms', (req, res) => {
  let r = req.body;
  r.occupied = 0;
  rooms.push(r);
  res.json({ok: true, room: r});
});

app.post('/api/students', (req, res) => {
  let s = req.body;
  s.assignedRoomID = -1;
  s.allocated = false;
  students.push(s);
  res.json({ok: true, student: s});
});

app.get('/api/rooms', (req, res) => res.json(rooms));
app.get('/api/students', (req, res) => res.json(students));

app.post('/api/run-greedy', (req, res) => {
  if(!rooms.length || !students.length) return res.status(400).json({err: 'Add data first'});
  try {
    res.json(runCpp('greedy', students, rooms));
  } catch(e) {
    res.status(500).json({err: 'C++ execution failed: ' + e.message});
  }
});

app.post('/api/run-stable', (req, res) => {
  if(!rooms.length || !students.length) return res.status(400).json({err: 'Add data first'});
  try {
    res.json(runCpp('stable', students, rooms));
  } catch(e) {
    res.status(500).json({err: 'C++ execution failed: ' + e.message});
  }
});

app.post('/api/compare', (req, res) => {
  if(!rooms.length || !students.length) return res.status(400).json({err: 'Add data first'});
  try {
    res.json({
      greedy: runCpp('greedy', students, rooms),
      stable: runCpp('stable', students, rooms)
    });
  } catch(e) {
    res.status(500).json({err: 'C++ execution failed: ' + e.message});
  }
});

app.post('/api/reset', (req, res) => {
  rooms = [];
  students = [];
  res.json({ok: true});
});

app.listen(3000, () => console.log('🚀 http://localhost:3000'));
