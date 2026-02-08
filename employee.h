#include<iostream>
#include "AttandanceLog.h" 
using namespace std ;
//class AttendanceLog{};
class SalaryAccount{};
class LeaveHistory{};
class Employee {
protected:
    string employeeId;
    string name;
    string department;
    string jobTitle;
    //EmploymentType employmentType;
    private :
        //AttendanceLog attendanceLog;
        SalaryAccount salaryAccount;
        LeaveHistory leaveHistory;
    public :
        Employee(const string& id,
             const string& empName,
             const string& dept,
             const string& title)
        : employeeId(id),
          name(empName),
          department(dept),
          jobTitle(title) {}
        void viewAttendance() {
        AttendanceLog* log = AttendanceLog::getInstance();  // get singleton
        if (!log) {
            cout << "Attendance log not created yet!\n";
            return;
        }
        log->showAttendanceByEmployee(employeeId);  // show only this employee's records

    }
    string getId() const { return employeeId; }
    string getName() const { return name; }


};