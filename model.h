#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

struct Room {
    int roomID;
    int capacity;
    int occupied;
    std::string roomType;
    std::string block;
    bool hasAC;
    bool isAvailable;

    int availableSeats() const {
        return capacity - occupied;
    }
};

struct Student {
    int studentID;
    std::string name;
    std::string gender;
    float cgpa;
    int year;
    std::string course;
    std::vector<int> preferences;
    int assignedRoomID;
    bool allocated;
    size_t prefIndex;

    Student() : studentID(0), year(0), assignedRoomID(-1), cgpa(0), allocated(false), prefIndex(0) {}
};

#endif // MODEL_H
