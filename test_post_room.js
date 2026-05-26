const fetch = globalThis.fetch || require('node-fetch');
(async ()=>{
  const res = await fetch('http://localhost:3000/api/rooms', {
    method: 'POST',
    headers: {'Content-Type':'application/json'},
    body: JSON.stringify({roomID:101, roomType:'Double', capacity:2, block:'A', hasAC:false, isAvailable:true})
  });
  const txt = await res.text();
  console.log(res.status, txt);
})();