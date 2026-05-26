# Hostel Room Allocation System (DAA PBL)

This project demonstrates hostel room allocation using Greedy Algorithm.

Features:
- Sort students by CGPA
- Allocate rooms based on preference
- Calculate satisfaction percentage
- Measure execution time

#  Member 4 – Result & Comparison Module

## Overview
This module is responsible for analyzing and presenting the results of the hostel room allocation system. It compares different allocation strategies (Greedy and Stable Matching) based on performance and effectiveness.

---

## Frontend & Build Instructions

 - Compile the C++ binary used by the server (creates `allocation_app`):

```bash
npm run build-cpp
```

 - Start the Node.js server which serves the frontend and invokes the binary:

```bash
npm start
```

 - Open http://localhost:3000 in your browser to use the frontend UI.

Notes:
 - The C++ program supports a JSON mode used by the server: the server invokes the binary with `--json` and sends students/rooms and the desired algorithm; the binary returns JSON results.
 - If building on Windows, ensure `g++` is available and `allocation_app.exe` is produced by the build command.

## Features

### Execution Time Measurement
- Uses `chrono` to measure how long each allocation algorithm takes to execute.

### Allocation Display
- Displays a table showing student-to-room assignments.
- Clearly indicates unallocated students.

### Satisfaction Percentage
- Calculates the percentage of students who received one of their preferred rooms.

### Empty Beds Calculation
- Computes the number of unused beds after allocation.

### Algorithm Comparison
- Compares **Greedy** and **Stable Matching** based on:
  - Execution Time  
  - Satisfaction %  
  - Empty Beds  

---
