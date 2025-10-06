#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

map<string, vector<string>> studentCourses;
map<string, vector<string>> conflictGraph;
map<string, int> courseSlot;

void buildConflictGraph(const vector<string>& courses) {
    for (size_t i = 0; i < courses.size(); ++i) {
        for (size_t j = 0; j < courses.size(); ++j) {
            if (i != j && find(conflictGraph[courses[i]].begin(), conflictGraph[courses[i]].end(), courses[j]) == conflictGraph[courses[i]].end()) {
                conflictGraph[courses[i]].push_back(courses[j]);
            }
        }
    }
}

void processStudentData(const string& student, const vector<string>& courses) {
    studentCourses[student] = courses;
    buildConflictGraph(courses);
}

vector<string> getAllUniqueCourses() {
    vector<string> uniqueCourses;
    for (const auto& student : studentCourses) {
        for (const string& course : student.second) {
            if (find(uniqueCourses.begin(), uniqueCourses.end(), course) == uniqueCourses.end()) {
                uniqueCourses.push_back(course);
            }
        }
    }
    return uniqueCourses;
}

vector<int> getUsedSlots(const string& course) {
    vector<int> slots;
    if (conflictGraph.count(course)) {
        for (const string& neighbor : conflictGraph[course]) {
            if (courseSlot.count(neighbor)) {
                slots.push_back(courseSlot[neighbor]);
            }
        }
    }
    return slots;
}

int getFirstAvailableSlot(const vector<int>& usedSlots) {
    int slot = 1;
    while (find(usedSlots.begin(), usedSlots.end(), slot) != usedSlots.end()) {
        ++slot;
    }
    return slot;
}

void scheduleAllCourses() {
    vector<string> allCourses = getAllUniqueCourses();

    for (const string& course : allCourses) {
        vector<int> usedSlots = getUsedSlots(course);
        courseSlot[course] = getFirstAvailableSlot(usedSlots);
    }
}

int getMaxSlot() {
    int maxSlot = 0;
    for (const auto& pair : courseSlot) {
        maxSlot = max(maxSlot, pair.second);
    }
    return maxSlot;
}

void printCourseSchedule() {
    cout << "Exam Schedule (Course -> Slot):" << endl;
    cout << "================================" << endl;
    for (const auto& entry : courseSlot) {
        cout << entry.first << " -> Slot " << entry.second << endl;
    }
    cout << "\nTotal slots needed: " << getMaxSlot() << endl;
}

void printStudentSchedules() {
    cout << "\nStudent Exam Schedules:" << endl;
    cout << "======================" << endl;

    for (const auto& student : studentCourses) {
        cout << student.first << ": ";
        const vector<string>& courses = student.second;
        for (size_t i = 0; i < courses.size(); ++i) {
            if (i > 0) cout << ", ";
            const string& course = courses[i];
            cout << course << "(Slot " << courseSlot[course] << ")";
        }
        cout << endl;
    }
}

bool loadInputFile(const string& filename) {
    ifstream in(filename);
    if (!in) {
        cerr << "Error: Could not open " << filename << endl;
        return false;
    }

    int n;
    if (!(in >> n) || n <= 0) {
        cerr << "Error: Invalid number of students" << endl;
        return false;
    }

    string dummy;
    getline(in, dummy);  // consume newline

    int studentsProcessed = 0;

    for (int i = 0; i < n; ++i) {
        string line;
        if (!getline(in, line) || line.empty()) continue;

        istringstream iss(line);
        string studentID;
        if (!(iss >> studentID)) continue;

        vector<string> courses;
        string course;
        while (iss >> course) {
            courses.push_back(course);
        }

        if (!courses.empty()) {
            processStudentData(studentID, courses);
            studentsProcessed++;
        }
    }

    if (studentsProcessed == 0) {
        cerr << "Error: No valid student data found" << endl;
        return false;
    }

    cout << "Processing " << studentsProcessed << " students..." << endl << endl;
    return true;
}

int main() {
    if (!loadInputFile("input.txt")) {
        return 1;
    }

    scheduleAllCourses();
    printCourseSchedule();
    printStudentSchedules();

    return 0;
}
