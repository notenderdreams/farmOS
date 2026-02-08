#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

enum class AttendanceStatus {
    Present,
    Absent
};


struct AttendanceRecord {
    string date;
    string employeeName;
    string employeeId;
    AttendanceStatus status;
};

class AttendanceLog {
private:
    string monthName;
    string fileName;
    vector<AttendanceRecord> records;
    static AttendanceLog* instance; 
    // Only AttendanceOfficer can write records
    void writeRecord(const AttendanceRecord& rec) {
        records.push_back(rec);
        ofstream out(fileName, ios::app);
        if (out.is_open()) {
            out << rec.date << ","
                << rec.employeeName << ","
                << rec.employeeId << ","
                << attendanceStatusToString(rec.status)<< "\n";
            out.close();
        } else {
            cout << "Error opening file for writing.\n";
        }
    }

    // Load file into memory
    void loadFromFile() {
        records.clear();
        ifstream in(fileName);
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string date, name, id, statusStr;
            if (!getline(ss, date, ',')) continue;
            if (!getline(ss, name, ',')) continue;
            if (!getline(ss, id, ',')) continue;
            if (!getline(ss, statusStr, ',')) continue;
            AttendanceRecord rec;
            rec.date = date;
            rec.employeeName = name;
            rec.employeeId = id;
            rec.status = static_cast<AttendanceStatus>(stoi(statusStr));
            records.push_back(rec);
        }
        in.close();
    }

    // Only AttendanceOfficer can create or write logs
   AttendanceLog(const string& month) : monthName(month) {
        fileName = monthName + ".txt";
        ofstream out(fileName, ios::app); // create file if not exist
        out.close();
        loadFromFile();
    }
    void createMonthlyFile(const string& month) {
        fileName = "leave_" + month + ".csv";
        ofstream out(fileName, ios::app);
        out.close();
    }


    friend class AttendanceOfficer;

    // officer can create and write
public:

    AttendanceLog(const AttendanceLog&) = delete;             // disable copy
    AttendanceLog& operator=(const AttendanceLog&) = delete;  // disable assignment
    static AttendanceLog* getInstance() {
        return instance; // returns existing log (nullptr if not created)
    }
    // Employees can view their own attendance
    void showAttendanceByEmployee(const string& employeeId) {
        loadFromFile();
        cout << "Attendance for employee ID: " << employeeId << endl;
        for (auto& rec : records) {
            if (rec.employeeId == employeeId) {
                cout << rec.date << " : " 
                     << rec.employeeName << " : "
                     << attendanceStatusToString(rec.status) << endl;
            }
        }
    }

    // Anyone can view all records (officer only in practice)
    void showAllAttendance() {
        loadFromFile();
        for (auto& rec : records) {
            cout << rec.date << " : " 
                 << rec.employeeName << " : " 
                 << rec.employeeId << " : "
                 << attendanceStatusToString(rec.status) << endl;
        }
    }

private:
    string attendanceStatusToString(AttendanceStatus status) {
        switch (status) {
            case AttendanceStatus::Present: return "Present";
            case AttendanceStatus::Absent: return "Absent";
        }
        return "Unknown";
    }
};