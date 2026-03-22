#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

struct Student {
    int id;
    double cgpa;
    vector<int> preferences;
    int assignedRoom = -1;
};

struct Room {
    int id;
    int capacity;
    int filled = 0;
};

void greedyAllocation(vector<Student>& students, vector<Room>& rooms) {

    // Sort students by CGPA (highest first)
    sort(students.begin(), students.end(),
         [](Student &a, Student &b) {
             return a.cgpa > b.cgpa;
         });

    for (auto &s : students) {

        for (int pref : s.preferences) {

            for (auto &r : rooms) {

                if (r.id == pref && r.filled < r.capacity) {
                    s.assignedRoom = r.id;
                    r.filled++;
                    break;
                }
            }

            if (s.assignedRoom != -1)
                break;
        }
    }
}

void calculateStats(vector<Student>& students) {

    int firstPref = 0;
    int unallocated = 0;

    for (auto &s : students) {
        if (s.assignedRoom == -1) {
            unallocated++;
        }
        else if (!s.preferences.empty() &&
                 s.assignedRoom == s.preferences[0]) {
            firstPref++;
        }
    }

    cout << "\nFirst Preference Satisfaction: "
         << (double)firstPref / students.size() * 100
         << "%\n";

    cout << "Unallocated Students: "
         << unallocated << endl;
}

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
        cin >> rooms[i].id >> rooms[i].capacity;
    }

    // Input Students
    cout << "\nEnter Student Details:\n";
    for (int i = 0; i < nStudents; i++) {

        cout << "Student ID and CGPA: ";
        cin >> students[i].id >> students[i].cgpa;

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

    cout << "\n--- Allocation Result ---\n";
    for (auto &s : students) {
        cout << "Student " << s.id << " -> ";
        if (s.assignedRoom == -1)
            cout << "Not Allocated\n";
        else
            cout << "Room " << s.assignedRoom << "\n";
    }

    calculateStats(students);

    cout << "Execution Time: "
         << duration.count()
         << " microseconds\n";

    return 0;
}