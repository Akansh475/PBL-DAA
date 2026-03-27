#!/bin/bash

#  HOSTEL ALLOCATION SYSTEM — Quick Start Script
echo " Hostel Room Allocation System"
echo "=================================="
echo ""

# Check if Node.js is installed
if ! command -v node &> /dev/null; then
    echo " Node.js is not installed."
    echo "Please install Node.js from https://nodejs.org/"
    exit 1
fi

echo " Node.js installed: $(node --version)"
echo ""

# Check if npm packages are installed
if [ ! -d "node_modules" ]; then
    echo "Installing dependencies..."
    npm install
    echo "Dependencies installed"
    echo ""
fi

# Show startup info
echo " Starting server..."
echo "   Frontend:     http://localhost:3000"
echo "   API Base:     http://localhost:3000/api"
echo ""
echo " Available endpoints:"
echo "   POST   /api/rooms              - Add room"
echo "   GET    /api/rooms              - Get all rooms"
echo "   POST   /api/students           - Add student"
echo "   GET    /api/students           - Get all students"
echo "   POST   /api/run-greedy         - Run Greedy allocation"
echo "   POST   /api/run-stable         - Run Stable Matching"
echo "   POST   /api/compare            - Compare algorithms"
echo "   POST   /api/reset              - Reset data"
echo ""
echo "Press Ctrl+C to stop the server"
echo "=================================="
echo ""

# Start the server
npm start
