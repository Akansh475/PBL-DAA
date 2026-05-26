#include<bits/stdc++.h>
#include<iostream>
#include<chrono>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
// JSON single-header (nlohmann)
#include "json.hpp"

using namespace std;
using namespace chrono;
using json = nlohmann::json;

// When true the program runs in JSON mode (suppress interactive/diagnostic prints)
static bool jsonMode = false;

struct Room {
    int    roomID;
    string roomType;      
    int    capacity;      
    int    occupied;      
    string block;         
    bool   hasAC;
    bool   isAvailable;   
    int availableSeats() const {
        return capacity - occupied;
    }
};

struct Student {
    int    studentID;
    string name;
    string gender;        
    float  cgpa;          
    int    year;          
    string course;        
    int prefIndex;
    vector<int> preferences;
    int  assignedRoomID;  
    bool allocated;

    Student() : assignedRoomID(-1), allocated(false) {}
};


void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getIntInput(const string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            clearInput();
            return value;
        }
        cout << "  [!] Invalid. Enter a number between "
             << minVal << " and " << maxVal << ".\n";
        clearInput();
    }
}

float getFloatInput(const string& prompt, float minVal, float maxVal) {
    float value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            clearInput();
            return value;
        }
        cout << "  [!] Invalid. Enter a value between "
             << minVal << " and " << maxVal << ".\n";
        clearInput();
    }
}

// Safe string input 
string getStringInput(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        // Trim leading/trailing spaces
        size_t start = value.find_first_not_of(" \t");
        size_t end   = value.find_last_not_of(" \t");
        if (start != string::npos) {
            value = value.substr(start, end - start + 1);
            return value;
        }
        cout << "  [!] Input cannot be empty.\n";
    }
}

string getChoiceInput(const string& prompt, const vector<string>& choices) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        for (char& c : value) c = toupper(c);
        for (const string& ch : choices) {
            string upper = ch;
            for (char& c : upper) c = toupper(c);
            if (value == upper) return ch;
        }
        cout << "  [!] Choose from: ";
        for (size_t i = 0; i < choices.size(); i++) {
            cout << choices[i];
            if (i < choices.size() - 1) cout << " / ";
        }
        cout << "\n";
    }
}

// Yes/No 
bool getYesNo(const string& prompt) {
    string answer;
    while (true) {
        cout << prompt << " (y/n): ";
        getline(cin, answer);
        if (answer == "y" || answer == "Y" || answer == "yes" || answer == "Yes") return true;
        if (answer == "n" || answer == "N" || answer == "no"  || answer == "No")  return false;
        cout << "  [!] Enter y or n.\n";
    }
}


bool roomIDExists(const vector<Room>& rooms, int id) {
    for (const Room& r : rooms)
        if (r.roomID == id) return true;
    return false;
}

bool studentIDExists(const vector<Student>& students, int id) {
    for (const Student& s : students)
        if (s.studentID == id) return true;
    return false;
}

//  ROOM INPUT
Room inputRoom(const vector<Room>& existingRooms) {
    Room r;

    cout << "\n--- Enter Room Details ---\n";

    // Room ID (unique)
    while (true) {
        r.roomID = getIntInput("  Room ID        : ", 1, 9999);
        if (!roomIDExists(existingRooms, r.roomID)) break;
        cout << "  [!] Room ID " << r.roomID << " already exists. Try another.\n";
    }

    // Room type and capacity
    r.roomType = getChoiceInput("  Room Type (Single/Double/Triple): ",
                                {"Single", "Double", "Triple"});
    if(r.roomType == "Single") r.capacity = 1;
    else if (r.roomType == "Double") r.capacity = 2;
    else r.capacity = 3;

    r.occupied = 0;
    r.block = getStringInput("  Block (e.g. A, B, C)             : ");
    r.hasAC = getYesNo("  Has AC?");
    r.isAvailable = getYesNo("  Is Available?");
    return r;
}

void inputRooms(vector<Room>& rooms) {
    int n = getIntInput("\nHow many rooms to add? ", 1, 200);
    for (int i = 0; i < n; i++) {
        cout << "\n[Room " << (i + 1) << " of " << n << "]";
        rooms.push_back(inputRoom(rooms));
        cout << "  Room " << rooms.back().roomID << " added.\n";
    }
}

//  STUDENT INPUT

Student inputStudent(const vector<Student>& existingStudents,
                     const vector<Room>&    rooms) {
    Student s;

    cout << "\n--- Enter Student Details ---\n";

    // Student ID (unique)
    while (true) {
        s.studentID = getIntInput("  Student ID  : ", 1, 99999);
        if (!studentIDExists(existingStudents, s.studentID)) break;
        cout << "  [!] Student ID " << s.studentID << " already exists.\n";
    }

    s.name = getStringInput("  Name        : ");
    s.gender = getChoiceInput("  Gender (Male/Female): ", {"Male", "Female"});
    s.cgpa= getFloatInput ("  CGPA (0.0 - 10.0)   : ", 0.0f, 10.0f);
    s.year= getIntInput   ("  Year (1-4)          : ", 1, 4);
    s.course= getStringInput("  Course              : ");

    // Room preferences
    if (rooms.empty()) {
        cout << "  [!] No rooms available yet. Preferences skipped.\n";
    } else {
        cout << "  Available Room IDs: ";
        for (const Room& r : rooms)
            cout << r.roomID << "(" << r.roomType << ") ";
        cout << "\n";

        int prefCount = getIntInput("  How many preferences (1-3)? ", 1, 3);
        for (int i = 0; i < prefCount; i++) {
            while (true) {
                int pid = getIntInput(
                    "    Preference " + to_string(i + 1) + " (Room ID): ",
                    1, 9999
                );
                // Validate room exists
                if (!roomIDExists(rooms, pid)) {
                    cout << "    [!] Room ID " << pid << " does not exist.\n";
                    continue;
                }
                // Prevent duplicate preferences
                bool dup = false;
                for (int p : s.preferences)
                    if (p == pid) { dup = true; break; }
                if (dup) {
                    cout << "    [!] Already selected. Choose a different room.\n";
                    continue;
                }
                s.preferences.push_back(pid);
                break;
            }
        }
    }

    return s;
}

void inputStudents(vector<Student>& students, const vector<Room>& rooms) {
    int n = getIntInput("\nHow many students to add? ", 1, 500);
    for (int i = 0; i < n; i++) {
        cout << "\n[Student " << (i + 1) << " of " << n << "]";
        students.push_back(inputStudent(students, rooms));
        cout << "  Student " << students.back().name << " added.\n";
    }
}
//  DISPLAY FUNCTIONS
void displayRooms(const vector<Room>& rooms) {
    if (rooms.empty()) {
        cout << "\n  [!] No rooms entered yet.\n";
        return;
    }

    cout << "\n"
         << string(70, '=') << "\n"
         << "  ROOM LIST\n"
         << string(70, '=') << "\n";
    cout << left
         << setw(6)  << "ID"
         << setw(10) << "Type"
         << setw(10) << "Capacity"
         << setw(10) << "Occupied"
         << setw(6)  << "Free"
         << setw(8)  << "Block"
         << setw(6)  << "AC"
         << setw(12) << "Status"
         << "\n"
         << string(70, '-') << "\n";

    for (const Room& r : rooms) {
        cout << left
             << setw(6)  << r.roomID
             << setw(10) << r.roomType
             << setw(10) << r.capacity
             << setw(10) << r.occupied
             << setw(6)  << r.availableSeats()
             << setw(8)  << r.block
             << setw(6)  << (r.hasAC ? "Yes" : "No")
             << setw(12) << (r.isAvailable ? "Available" : "Unavailable")
             << "\n";
    }
    cout << string(70, '=') << "\n";
    cout << "Total rooms: " << rooms.size() << "\n";
}

void displayStudents(const vector<Student>& students) {
    if (students.empty()) {
        cout << "\n  [!] No students entered yet.\n";
        return;
    }

    cout << "\n"
         << string(80, '=') << "\n"
         << "  STUDENT LIST\n"
         << string(80, '=') << "\n";
    cout << left
         << setw(8)  << "ID"
         << setw(18) << "Name"
         << setw(8)  << "Gender"
         << setw(8)  << "CGPA"
         << setw(6)  << "Year"
         << setw(12) << "Course"
         << setw(20) << "Preferences"
         << "\n"
         << string(80, '-') << "\n";

    for (const Student& s : students) {
        // Build preference string
        string prefs = "";
        for (size_t i = 0; i < s.preferences.size(); i++) {
            prefs += to_string(s.preferences[i]);
            if (i < s.preferences.size() - 1) prefs += ",";
        }
        if (prefs.empty()) prefs = "None";

        cout << left
             << setw(8)  << s.studentID
             << setw(18) << s.name.substr(0, 17)
             << setw(8)  << s.gender
             << setw(8)  << fixed << setprecision(2) << s.cgpa
             << setw(6)  << s.year
             << setw(12) << s.course.substr(0, 11)
             << setw(20) << prefs
             << "\n";
    }
    cout << string(80, '=') << "\n";
    cout << "Total students: " << students.size() << "\n";
}


// if more students than total seats
void checkOverflow(const vector<Student>& students, const vector<Room>& rooms) {
    int totalSeats = 0;
    for (const Room& r : rooms)
        if (r.isAvailable) totalSeats += r.availableSeats();

    if ((int)students.size() > totalSeats) {
        cout << "\n  [WARNING] More students (" << students.size()
             << ") than available seats (" << totalSeats
             << "). Some students will remain unallocated.\n";
    }
}

// if no rooms available at all
bool hasAvailableRooms(const vector<Room>& rooms) {
    for (const Room& r : rooms)
        if (r.isAvailable && r.availableSeats() > 0)
            return true;
    return false;
}

// if student has no preferences set
bool studentHasNoPreferences(const Student& s) {
    return s.preferences.empty();
}

// if no valid preferences point to real rooms
void validatePreferences(vector<Student>& students, const vector<Room>& rooms) {
    for (Student& s : students) {
        vector<int> validPrefs;
        for (int pid : s.preferences) {
            if (roomIDExists(rooms, pid))
                validPrefs.push_back(pid);
            else
                cout << "  [!] Student " << s.name
                     << ": Room ID " << pid
                     << " in preferences not found. Removed.\n";
        }
        s.preferences = validPrefs;
    }
}

// if duplicate student IDs are present
void checkDuplicateStudentIDs(const vector<Student>& students) {
    for (size_t i = 0; i < students.size(); i++)
        for (size_t j = i + 1; j < students.size(); j++)
            if (students[i].studentID == students[j].studentID)
                cout << "  [!] Duplicate Student ID: " << students[i].studentID << "\n";
}

// if all rooms are at capacity
void checkRoomCapacity(const vector<Room>& rooms) {
    bool anyFree = false;
    for (const Room& r : rooms)
        if (r.availableSeats() > 0) { anyFree = true; break; }
    if (!anyFree)
        cout << "\n  [WARNING] All rooms are full. No allocation possible.\n";
}


// Find room by ID; returns nullptr if not found
Room* findRoom(vector<Room>& rooms, int id) {
    for (Room& r : rooms)
        if (r.roomID == id) return &r;
    return nullptr;
}

// Find student by ID; returns nullptr if not found
Student* findStudent(vector<Student>& students, int id) {
    for (Student& s : students)
        if (s.studentID == id) return &s;
    return nullptr;
}

// Reset all allocations 
void resetAllocations(vector<Student>& students, vector<Room>& rooms) {
    for (Student& s : students) {
        s.assignedRoomID = -1;
        s.allocated = false;
    }
    for (Room& r : rooms)
        r.occupied = 0;
    cout << "\n  Allocations reset.\n";
}



// Member 4
template <typename Func>
int calculateExecTime(Func func){
    auto start = high_resolution_clock::now();
    // function call
    func();
    auto end= high_resolution_clock::now();
    auto duration =duration_cast<milliseconds>(end-start);
    if(!jsonMode) cout<<"Duration: "<<duration.count()<<" ms\n";
    return duration.count();
}

// --- JSON helpers ---
vector<Room> roomsFromJson(const json &jr) {
    vector<Room> rooms;
    for (const auto &r : jr) {
        Room room;
        room.roomID = r.value("roomID", 0);
        room.roomType = r.value("roomType", string(""));
        room.capacity = r.value("capacity", 0);
        room.occupied = r.value("occupied", 0);
        room.block = r.value("block", string(""));
        room.hasAC = r.value("hasAC", false);
        room.isAvailable = r.value("isAvailable", true);
        rooms.push_back(room);
    }
    return rooms;
}

vector<Student> studentsFromJson(const json &js) {
    vector<Student> students;
    for (const auto &s : js) {
        Student st;
        st.studentID = s.value("studentID", 0);
        st.name = s.value("name", string(""));
        st.gender = s.value("gender", string(""));
        st.cgpa = s.value("cgpa", 0.0f);
        st.year = s.value("year", 0);
        st.course = s.value("course", string(""));
        st.prefIndex = 0;
        st.assignedRoomID = s.value("assignedRoomID", -1);
        st.allocated = s.value("allocated", false);
        if (s.find("preferences") != s.end() && s["preferences"].is_array()) {
            for (const auto &p : s["preferences"]) st.preferences.push_back((int)p);
        }
        students.push_back(st);
    }
    return students;
}

json studentsToJson(const vector<Student> &students) {
    json arr = json::array();
    for (const auto &s : students) {
        json obj;
        obj["studentID"] = s.studentID;
        obj["name"] = s.name;
        obj["gender"] = s.gender;
        obj["cgpa"] = s.cgpa;
        obj["year"] = s.year;
        obj["course"] = s.course;
        obj["preferences"] = s.preferences;
        obj["assignedRoomID"] = s.assignedRoomID;
        obj["allocated"] = s.allocated;
        arr.push_back(obj);
    }
    return arr;
}

json roomsToJson(const vector<Room> &rooms) {
    json arr = json::array();
    for (const auto &r : rooms) {
        json obj;
        obj["roomID"] = r.roomID;
        obj["roomType"] = r.roomType;
        obj["capacity"] = r.capacity;
        obj["occupied"] = r.occupied;
        obj["block"] = r.block;
        obj["hasAC"] = r.hasAC;
        obj["isAvailable"] = r.isAvailable;
        arr.push_back(obj);
    }
    return arr;
}

// Run selected algorithm and return structured JSON result
json runAlgorithmJson(const string &algorithm, const vector<Student> &students, const vector<Room> &rooms) {
    if (algorithm == "greedy") {
        auto s = students;
        auto r = rooms;
        json result;
        jsonMode = true;
        auto start = high_resolution_clock::now();
        greedyAllocation(s, r);
        auto end = high_resolution_clock::now();
        int execTime = duration_cast<milliseconds>(end - start).count();
        result["executionTime"] = execTime;
        result["satisfaction"] = calculateSatisfaction(s);
        result["remainingBeds"] = remainingBeds(r);
        result["students"] = studentsToJson(s);
        result["rooms"] = roomsToJson(r);
        jsonMode = false;
        return result;
    } else if (algorithm == "stable") {
        auto s = students;
        auto r = rooms;
        json result;
        jsonMode = true;
        auto start = high_resolution_clock::now();
        stableMatching(s, r);
        auto end = high_resolution_clock::now();
        int execTime = duration_cast<milliseconds>(end - start).count();
        result["executionTime"] = execTime;
        result["satisfaction"] = calculateSatisfaction(s);
        result["remainingBeds"] = remainingBeds(r);
        result["students"] = studentsToJson(s);
        result["rooms"] = roomsToJson(r);
        jsonMode = false;
        return result;
    } else if (algorithm == "compare") {
        json obj;
        obj["greedy"] = runAlgorithmJson("greedy", students, rooms);
        obj["stable"] = runAlgorithmJson("stable", students, rooms);
        return obj;
    }
    return json::object();
}

void displayAllocation(const vector<Student>& students) {
    cout << "\n===== Allocation Result =====\n";
    for (const Student& s : students) {
        cout<<s.studentID << " | "
             <<s.name << " | ";
        if (s.allocated)
            cout<<s.assignedRoomID;
        else
            cout<<"Not Allocated";
        cout<<"\n";
    }
}

double calculateSatisfaction(const vector<Student>& students) {
    int satisfied=0;
    for (const Student& s : students) {
        if (!s.allocated) continue;
        for (int pref:s.preferences) {
            if (pref==s.assignedRoomID) {
                satisfied++;
                break;
            }
        }
    }
    double perc=satisfied*100.0/students.size();
    return perc;
}

int remainingBeds(const vector<Room>& rooms){
    int empty=0;
    for(auto room :rooms){
        empty+=(room.capacity-room.occupied);
    }
    return empty;
}

void displaySummary(const vector<Student>& students, const vector<Room>& rooms) {
    int totalSeats = 0, usedSeats = 0;
    for (const Room& r : rooms) {
        totalSeats += r.capacity;
        usedSeats  += r.occupied;
    }

    int allocated = 0;
    for (const Student& s : students)
        if (s.allocated) allocated++;

    cout << "\n--- Summary ---\n"
         << "  Students entered : " << students.size() << "\n"
         << "  Rooms entered    : " << rooms.size()<< "\n"
         << "  Total seats      : " << totalSeats << "\n"
         << "  Seats used       : " << usedSeats<< "\n"
         << "  Allocated        : " << allocated<< "\n"
         << "  Unallocated      : " << (students.size() - allocated) << "\n";
}



void displayRoomAllocations(const vector<Student>& students,
                            const vector<Room>& rooms) {

    cout << "\n===== ROOM-WISE ALLOCATION =====\n";

    for (const Room& r : rooms) {
        cout << "\nRoom " << r.roomID 
             << " (Capacity: " << r.capacity << ")\n";
        cout << "----------------------------------\n";

        vector<const Student*> assigned;
        for (const Student& s : students) {
            if (s.assignedRoomID == r.roomID) {
                assigned.push_back(&s);
            }
        }
        if (assigned.empty()) {
            cout << "No students assigned\n";
            continue;
        }
        sort(assigned.begin(), assigned.end(),
             [](const Student* a, const Student* b) {
                 return a->cgpa > b->cgpa;
             });
        cout << left
             << setw(10) << "ID"
             << setw(15) << "Name"
             << setw(10) << "CGPA"
             << setw(10) << "Year"
             << "\n";

        for (auto s : assigned) {
            cout << left
                 << setw(10) << s->studentID
                 << setw(15) << s->name
                 << setw(10) << fixed << setprecision(2) << s->cgpa
                 << setw(10) << s->year
                 << "\n";
        }
    }

    cout << "\n=================================\n";
}



// Member 2
void stableMatching(vector<Student>& students, vector<Room>& rooms) {
    // Reset previous allocation
    for (Student& s : students) {
        s.assignedRoomID = -1;
        s.prefIndex    = 0;
    }
    for (Room& r : rooms) {
        r.occupied = 0;
    }

    // roomID 
    unordered_map<int, Room*> roomMap;
    for (Room& r : rooms)
        roomMap[r.roomID] = &r;


    unordered_map<int, vector<Student*>> roomStudents;

    // All students start as free
    queue<Student*> freeStudents;
    for (Student& s : students)
        freeStudents.push(&s);

    while (!freeStudents.empty()) {
        Student* s = freeStudents.front();
        freeStudents.pop();

        // No more preferences left  stays unallocated
        if (s->prefIndex >= s->preferences.size())
            continue;

        int roomID = s->preferences[s->prefIndex++];

        if (roomMap.find(roomID) == roomMap.end()) {
            freeStudents.push(s);
            continue;
        }

        Room* room = roomMap[roomID];
        auto& assigned = roomStudents[roomID];

        // Room has space  assign directly
        if ((int)assigned.size() < room->capacity) {
            assigned.push_back(s);
            s->assignedRoomID = roomID;
            room->occupied++;
            s->allocated = true;
        }
        else {
            // Room full:check if this student has higher CGPA than lowest
            assigned.push_back(s);

            // Sort descending by CGPA
            sort(assigned.begin(), assigned.end(),
                 [](Student* a, Student* b) { return a->cgpa > b->cgpa; });

            // Kick out the weakest student
            Student* removed = assigned.back();
            assigned.pop_back();

            if (removed != s) {
                // New student gets the room
                s->assignedRoomID = roomID;

                // Removed student goes back in queue
                removed->assignedRoomID = -1;
                removed->allocated = false;
                room->occupied = assigned.size();
                freeStudents.push(removed);
            } else {
                freeStudents.push(s);
            }
        }
    }
}


// Member 1 

void greedyAllocation(vector<Student>& students, vector<Room>& rooms) {

    // Sort by CGPA (high-low)
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


void compareResults(const vector<Student> &students,const vector<Room> &rooms){
    // Greedy Allocation 
    auto student_greedy=students;
    auto rooms_greedy= rooms;
    int greedy_time = calculateExecTime([&]() {
    greedyAllocation(student_greedy, rooms_greedy);
    });
    cout<<"\nGreedy Allocation Result:\n";
    displayAllocation(student_greedy);
    double satisfactory_rate_greedy=calculateSatisfaction(student_greedy);
    int rembeds_greedy=remainingBeds(rooms_greedy);


    // Stable Matching Allocation
    auto student_stable=students;
    auto rooms_stable= rooms;
    int stable_time = calculateExecTime([&]() {
    stableMatching(student_stable, rooms_stable);
    });
    cout<<"\nStable Matching Allocation Result:\n";
    displayAllocation(student_stable);
    double satisfactory_rate_stable=calculateSatisfaction(student_stable);
    int rembeds_stable=remainingBeds(rooms_stable);

    // Results Comparisons

    cout << "\n===== Results Comparison =====\n\n";


    cout << left 
        << setw(12) << "Algorithm"
        << setw(18) << "Execution Time"
        << setw(22) << "Satisfaction Rate"
        << setw(18) << "Remaining Beds"
        << "\n";
    cout << string(70, '-') << "\n";
    cout << left
        << setw(12) << "Greedy"
        << setw(18) << greedy_time
        << setw(22) << fixed << setprecision(2) << satisfactory_rate_greedy
        << setw(18) << rembeds_greedy
        << "\n";

    cout << left
        << setw(12) << "Stable"
        << setw(18) << stable_time
        << setw(22) << fixed << setprecision(2) << satisfactory_rate_stable
        << setw(18) << rembeds_stable
        << "\n";
}
 

// Main menu
void member1Menu() {
    vector<Student> students;
    vector<Room>    rooms;

    int choice;
    do {
        cout << "\n============================\n"
             << "  Menu  "
             << "============================\n"
             << "  1. Add Rooms\n"
             << "  2. Add Students\n"
             << "  3. Display Rooms\n"
             << "  4. Display Students\n"
             << "  5. Display Summary\n"
             << "  6. Run Data Validations\n"
             << "  7. Reset Allocations\n"
             << "  8. Run Allocation Comparison\n"
             << "  0. Exit\n"
             << "Choose: ";

        choice = getIntInput("", 0, 8);

        switch (choice) {
            case 1:
                inputRooms(rooms);
                break;

            case 2:
                if (rooms.empty()) {
                    cout << "  [!] Add rooms first so students can set preferences.\n";
                    if (getYesNo("  Continue without rooms?"))
                        inputStudents(students, rooms);
                } else {
                    inputStudents(students, rooms);
                }
                break;

            case 3:
                displayRooms(rooms);
                break;

            case 4:
                displayStudents(students);
                break;

            case 5:
                displaySummary(students, rooms);
                break;

            case 6:
                cout << "\n-- Running Validations --\n";
                checkDuplicateStudentIDs(students);
                validatePreferences(students, rooms);
                checkOverflow(students, rooms);
                checkRoomCapacity(rooms);
                if (!hasAvailableRooms(rooms))
                    cout << "  [WARNING] No available rooms.\n";
                cout << "  Validation complete.\n";
                break;

            case 7:
                resetAllocations(students, rooms);
                break;

            case 0:
                cout << "  Exiting..\n";
                break;
            case 8:
                compareResults(students, rooms);
                break;
        }
    } while (choice != 0);
}

int main(int argc, char** argv) {
    // If run with --json, read JSON from stdin and output JSON result
    if (argc > 1 && string(argv[1]) == "--json") {
        // read stdin fully
        std::string input((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
        if (input.empty()) return 0;
        json payload = json::parse(input);
        string algorithm = payload.value("algorithm", string("compare"));
        vector<Student> students;
        vector<Room> rooms;
        if (payload.find("students") != payload.end() && payload["students"].is_array()) {
            students = studentsFromJson(payload["students"]);
        }
        if (payload.find("rooms") != payload.end() && payload["rooms"].is_array()) {
            rooms = roomsFromJson(payload["rooms"]);
        }
        json out = runAlgorithmJson(algorithm, students, rooms);
        // ensure single JSON object on stdout
        cout << out.dump();
        return 0;
    }

    cout << "========================================\n";
    cout << "  Hostel Room Allocation \n";
    cout << "========================================\n";
    member1Menu();
    return 0;
}
