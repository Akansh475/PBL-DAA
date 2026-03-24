#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <iomanip>
using namespace std;

// ─────────────────────────────────────────
//  STRUCTS  (from Member 1)
// ─────────────────────────────────────────

struct Room {
    int    roomID;
    string roomType;
    int    capacity;
    int    occupied;
};

struct Student {
    int    studentID;
    string name;
    float  cgpa;
    vector<int> preferences;  // list of preferred Room IDs
    int    assignedRoom = -1;
    size_t prefIndex    = 0;  // tracks which preference we're trying next
};

// ─────────────────────────────────────────
//  INPUT  (from Member 1)
// ─────────────────────────────────────────

void addRooms(vector<Room>& rooms) {
    int n;
    cout << "\nHow many rooms? ";
    cin >> n;

    for (int i = 0; i < n; i++) {
        Room r;
        cout << "\n-- Room " << (i + 1) << " --\n";
        cout << "Room ID       : "; cin >> r.roomID;
        cout << "Type (1=Single 2=Double 3=Triple): ";
        int t; cin >> t;
        if      (t == 1) { r.roomType = "Single"; r.capacity = 1; }
        else if (t == 2) { r.roomType = "Double"; r.capacity = 2; }
        else             { r.roomType = "Triple"; r.capacity = 3; }
        r.occupied = 0;
        rooms.push_back(r);
    }
    cout << "Rooms added.\n";
}

void addStudents(vector<Student>& students) {
    int n;
    cout << "\nHow many students? ";
    cin >> n;

    for (int i = 0; i < n; i++) {
        Student s;
        cout << "\n-- Student " << (i + 1) << " --\n";
        cout << "Student ID    : "; cin >> s.studentID;
        cout << "Name          : "; cin.ignore(); getline(cin, s.name);
        cout << "CGPA (0-10)   : "; cin >> s.cgpa;

        int p;
        cout << "No. of preferences (1-3): "; cin >> p;
        cout << "Enter preferred Room IDs:\n";
        for (int j = 0; j < p; j++) {
            int rid; cin >> rid;
            s.preferences.push_back(rid);
        }
        students.push_back(s);
    }
    cout << "Students added.\n";
}

// ─────────────────────────────────────────
//  STABLE MATCHING  (Member 3)
// ─────────────────────────────────────────

void stableMatching(vector<Student>& students, vector<Room>& rooms) {

    // Reset previous allocation
    for (Student& s : students) {
        s.assignedRoom = -1;
        s.prefIndex    = 0;
    }
    for (Room& r : rooms) {
        r.occupied = 0;
    }

    // roomID → Room pointer for fast lookup
    unordered_map<int, Room*> roomMap;
    for (Room& r : rooms)
        roomMap[r.roomID] = &r;

    // roomID → assigned students list
    unordered_map<int, vector<Student*>> roomStudents;

    // All students start as free
    queue<Student*> freeStudents;
    for (Student& s : students)
        freeStudents.push(&s);

    while (!freeStudents.empty()) {
        Student* s = freeStudents.front();
        freeStudents.pop();

        // No more preferences left → stays unallocated
        if (s->prefIndex >= s->preferences.size())
            continue;

        int roomID = s->preferences[s->prefIndex++];

        // Room doesn't exist → try next preference
        if (roomMap.find(roomID) == roomMap.end()) {
            freeStudents.push(s);
            continue;
        }

        Room* room = roomMap[roomID];
        auto& assigned = roomStudents[roomID];

        // Room has space → assign directly
        if ((int)assigned.size() < room->capacity) {
            assigned.push_back(s);
            s->assignedRoom = roomID;
            room->occupied++;
        }
        else {
            // Room full → check if this student has higher CGPA than lowest
            assigned.push_back(s);

            // Sort descending by CGPA
            sort(assigned.begin(), assigned.end(),
                 [](Student* a, Student* b) { return a->cgpa > b->cgpa; });

            // Kick out the weakest student
            Student* removed = assigned.back();
            assigned.pop_back();

            if (removed != s) {
                // New student gets the room
                s->assignedRoom = roomID;

                // Removed student goes back in queue
                removed->assignedRoom = -1;
                room->occupied--;
                freeStudents.push(removed);
            } else {
                // New student was the weakest → try next preference
                freeStudents.push(s);
            }
        }
    }
}

// ─────────────────────────────────────────
//  DISPLAY  (from Member 1)
// ─────────────────────────────────────────

void showRooms(const vector<Room>& rooms) {
    if (rooms.empty()) { cout << "\nNo rooms added yet.\n"; return; }

    cout << "\n" << string(45, '-') << "\n";
    cout << left << setw(8)  << "RoomID"
                 << setw(10) << "Type"
                 << setw(10) << "Capacity"
                 << setw(10) << "Occupied" << "\n";
    cout << string(45, '-') << "\n";
    for (const Room& r : rooms)
        cout << left << setw(8)  << r.roomID
                     << setw(10) << r.roomType
                     << setw(10) << r.capacity
                     << setw(10) << r.occupied << "\n";
    cout << string(45, '-') << "\n";
}

void showStudents(const vector<Student>& students) {
    if (students.empty()) { cout << "\nNo students added yet.\n"; return; }

    cout << "\n" << string(55, '-') << "\n";
    cout << left << setw(10) << "ID"
                 << setw(16) << "Name"
                 << setw(8)  << "CGPA"
                 << setw(12) << "Assigned" << "\n";
    cout << string(55, '-') << "\n";
    for (const Student& s : students)
        cout << left << setw(10) << s.studentID
                     << setw(16) << s.name
                     << setw(8)  << fixed << setprecision(2) << s.cgpa
                     << setw(12) << (s.assignedRoom == -1 ? "Not Assigned" : to_string(s.assignedRoom))
                     << "\n";
    cout << string(55, '-') << "\n";
}

void showAllocationResult(const vector<Student>& students) {
    int allocated = 0;
    for (const Student& s : students)
        if (s.assignedRoom != -1) allocated++;

    cout << "\n--- Stable Matching Result ---\n";
    showStudents(students);
    cout << "Allocated   : " << allocated << "\n";
    cout << "Unallocated : " << students.size() - allocated << "\n";
}

// ─────────────────────────────────────────
//  MAIN MENU
// ─────────────────────────────────────────

int main() {
    vector<Room>    rooms;
    vector<Student> students;
    int choice;

    cout << "==============================\n";
    cout << "  Hostel Allocation — M3\n";
    cout << "==============================\n";

    do {
        cout << "\n1. Add Rooms\n"
                "2. Add Students\n"
                "3. Show Rooms\n"
                "4. Show Students\n"
                "5. Run Stable Matching\n"
                "0. Exit\n"
                "Choice: ";
        cin >> choice;

        if      (choice == 1) addRooms(rooms);
        else if (choice == 2) addStudents(students);
        else if (choice == 3) showRooms(rooms);
        else if (choice == 4) showStudents(students);
        else if (choice == 5) {
            if (students.empty() || rooms.empty()) {
                cout << "Add students and rooms first.\n";
            } else {
                stableMatching(students, rooms);
                showAllocationResult(students);
            }
        }
    } while (choice != 0);

    return 0;
}