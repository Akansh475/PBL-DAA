//  ANALYTICS & COMPARISON MODULE
//  Hostel Room Allocation System
#include "model.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <functional>

using namespace std;
using namespace chrono;
//  STRUCTS (shared)
//  FORWARD DECLARATIONS (from separate algorithm files)
void greedyAllocation(vector<Student>& students, vector<Room>& rooms);
void stableMatching(vector<Student>& students, vector<Room>& rooms);

//  ANALYTICS FUNCTIONS
int calculateExecTime(function<void()> func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Duration: " << duration.count() << "ms";
    return duration.count();
}

void displayAllocation(const vector<Student>& students) {
    cout << "\n===== Allocation Result =====\n";

    for (const Student& s : students) {
        cout << s.studentID << " | "
             << s.name << " | ";

        if (s.allocated)
            cout << s.assignedRoomID;
        else
            cout << "Not Allocated";
        cout << "\n";
    }
}

double calculateSatisfaction(const vector<Student>& students) {
    int satisfied = 0;
    for (const Student& s : students) {
        if (!s.allocated) continue;

        for (int pref : s.preferences) {
            if (pref == s.assignedRoomID) {
                satisfied++;
                break;
            }
        }
    }
    double perc = satisfied * 100.0 / students.size();
    return perc;
}

int remainingBeds(const vector<Room>& rooms) {
    int empty = 0;
    for (auto room : rooms) {
        empty += (room.capacity - room.occupied);
    }
    return empty;
}

//  COMPARISON FUNCTION
void compareResults(const vector<Student>& students, const vector<Room>& rooms) {
    // Greedy Allocation 
    auto student_greedy = students;
    auto rooms_greedy = rooms;
    int greedy_time = calculateExecTime([&]() {
        greedyAllocation(student_greedy, rooms_greedy);
    });
    double satisfactory_rate_greedy = calculateSatisfaction(student_greedy);
    int rembeds_greedy = remainingBeds(rooms_greedy);


    // Stable Matching Allocation
    auto student_stable = students;
    auto rooms_stable = rooms;
    int stable_time = calculateExecTime([&]() {
        stableMatching(student_stable, rooms_stable);
    });
    double satisfactory_rate_stable = calculateSatisfaction(student_stable);
    int rembeds_stable = remainingBeds(rooms_stable);

    // Results Comparison
    cout << "\n=====Results Comparison=====\n";
    cout << "Algorithm\tExecution Time\tSatisfaction Rate\tRemaining Beds\n";
    cout << "Greedy\t" << greedy_time << "\t" << satisfactory_rate_greedy << "\t" << rembeds_greedy << "\n";
    cout << "Stable\t" << stable_time << "\t" << satisfactory_rate_stable << "\t" << rembeds_stable << "\n";
}
