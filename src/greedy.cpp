#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;

// ================= STRUCTS =================

struct Student {
    int studentID;
    float cgpa;
    vector<int> preferences;

    int assignedRoomID = -1;
    bool allocated = false;
};

struct Room {
    int roomID;
    int capacity;
    int occupied = 0;
    bool isAvailable = true;

    int availableSeats() const {
        return capacity - occupied;
    }
};

// ================= GREEDY =================

void greedyAllocation(vector<Student>& students, vector<Room>& rooms) {

    // Sort by CGPA (high → low)
    sort(students.begin(), students.end(),
         [](Student &a, Student &b) {
             return a.cgpa > b.cgpa;
         });

    for (auto &s : students) {

        for (int pref : s.preferences) {

            for (auto &r : rooms) {

                if (r.roomID == pref &&
                    r.isAvailable &&
                    r.availableSeats() > 0) {

                    s.assignedRoomID = r.roomID;
                    s.allocated = true;

                    r.occupied++;

                    break;
                }
            }

            if (s.allocated)
                break;
        }
    }
}

// ================= STATS =================

void calculateStats(vector<Student>& students) {

    int firstPref = 0;
    int unallocated = 0;

    for (auto &s : students) {

        if (!s.allocated) {
            unallocated++;
        }
        else if (!s.preferences.empty() &&
                 s.assignedRoomID == s.preferences[0]) {
            firstPref++;
        }
    }

    cout << "\nFirst Preference Satisfaction: "
         << (double)firstPref / students.size() * 100
         << "%\n";

    cout << "Unallocated Students: "
         << unallocated << endl;
}

// ================= MAIN =================

int main() {

    int nStudents, nRooms;

    cout << "Enter number of students: ";
    cin >> nStudents;

    cout << "Enter number of rooms: ";
    cin >> nRooms;

    vector<Student> students(nStudents);
    vector<Room> rooms(nRooms);

    // Input Rooms
    cout << "\nEnter Room Details:\n";
    for (int i = 0; i < nRooms; i++) {
        cout << "Room ID and Capacity: ";
        cin >> rooms[i].roomID >> rooms[i].capacity;
    }

    // Input Students
    cout << "\nEnter Student Details:\n";
    for (int i = 0; i < nStudents; i++) {

        cout << "Student ID and CGPA: ";
        cin >> students[i].studentID >> students[i].cgpa;

        int prefCount;
        cout << "Number of Preferences: ";
        cin >> prefCount;

        students[i].preferences.resize(prefCount);

        cout << "Enter Preferences (Room IDs): ";
        for (int j = 0; j < prefCount; j++) {
            cin >> students[i].preferences[j];
        }
    }

    auto start = chrono::high_resolution_clock::now();

    greedyAllocation(students, rooms);

    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    // Output
    cout << "\n--- Allocation Result ---\n";
    for (auto &s : students) {
        cout << "Student " << s.studentID << " -> ";
        if (!s.allocated)
            cout << "Not Allocated\n";
        else
            cout << "Room " << s.assignedRoomID << "\n";
    }

    calculateStats(students);

    cout << "Execution Time: "
         << duration.count()
         << " microseconds\n";

    return 0;
}