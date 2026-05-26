const fetch = globalThis.fetch || require('node-fetch');
(async ()=>{
  const student = { studentID: 201, name: 'Alice', gender: 'Female', cgpa: 8.5, year: 2, course: 'CS', preferences: [101] };
  let res = await fetch('http://localhost:3000/api/students', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify(student) });
  console.log('POST student', res.status, await res.text());
  res = await fetch('http://localhost:3000/api/run-greedy', { method:'POST' });
  console.log('\nRUN GREEDY', res.status, await res.text());
})();