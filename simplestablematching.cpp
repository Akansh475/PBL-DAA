#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
using namespace std;

// -------------------- Structures --------------------

struct Student {
    int id;
    float cgpa;
    vector<int> preferences;
    int assignedRoom = -1;
    size_t prefIndex = 0;   // ✅ FIXED (was int)
};

struct Room {
    int id;
    int capacity;
    vector<Student*> assignedStudents;
};

// -------------------- Comparator --------------------

bool compareCGPA(Student* a, Student* b) {
    return a->cgpa > b->cgpa;
}

// -------------------- Stable Matching --------------------

void stableMatching(vector<Student>& students, vector<Room>& rooms) {

    unordered_map<int, Room*> roomMap;
    for (auto &room : rooms) {
        roomMap[room.id] = &room;
    }

    queue<Student*> freeStudents;

    // Initially all students are free
    for (auto &s : students) {
        freeStudents.push(&s);
    }

    while (!freeStudents.empty()) {
        Student* student = freeStudents.front();
        freeStudents.pop();

        // ✅ Safe comparison (both size_t)
        if (student->prefIndex >= student->preferences.size())
            continue;

        int roomId = student->preferences[student->prefIndex];
        student->prefIndex++;

        // ✅ Check if room exists (prevents crash)
        if (roomMap.find(roomId) == roomMap.end()) {
            freeStudents.push(student);
            continue;
        }

        Room* room = roomMap[roomId];

        // If room has space
        if (room->assignedStudents.size() < (size_t)room->capacity) {
            room->assignedStudents.push_back(student);
            student->assignedRoom = roomId;
        } 
        else {
            // Room full → try replacement
            room->assignedStudents.push_back(student);

            sort(room->assignedStudents.begin(), room->assignedStudents.end(), compareCGPA);

            Student* removed = room->assignedStudents.back();
            room->assignedStudents.pop_back();

            if (removed != student) {
                student->assignedRoom = roomId;

                removed->assignedRoom = -1;
                freeStudents.push(removed);
            } 
            else {
                freeStudents.push(student);
            }
        }
    }
}

// -------------------- Output --------------------

void printAllocation(vector<Student>& students) {
    cout << "\nFinal Allocation:\n";
    cout << "Student ID\tAssigned Room\n";

    for (auto &s : students) {
        cout << s.id << "\t\t";
        if (s.assignedRoom == -1)
            cout << "Not Allocated\n";
        else
            cout << s.assignedRoom << "\n";
    }
}

// -------------------- Main --------------------

int main() {

    int n, m;

    cout << "Enter number of students: ";
    cin >> n;

    vector<Student> students(n);

    for (int i = 0; i < n; i++) {
        cout << "\nStudent " << i + 1 << " ID: ";
        cin >> students[i].id;

        cout << "CGPA: ";
        cin >> students[i].cgpa;

        int p;
        cout << "Number of preferences: ";
        cin >> p;

        cout << "Enter preferred Room IDs:\n";
        for (int j = 0; j < p; j++) {
            int roomId;
            cin >> roomId;
            students[i].preferences.push_back(roomId);
        }
    }

    cout << "\nEnter number of rooms: ";
    cin >> m;

    vector<Room> rooms(m);

    for (int i = 0; i < m; i++) {
        cout << "\nRoom " << i + 1 << " ID: ";
        cin >> rooms[i].id;

        cout << "Capacity: ";
        cin >> rooms[i].capacity;
    }

    // Run algorithm
    stableMatching(students, rooms);

    // Output result
    printAllocation(students);

    return 0;
}
