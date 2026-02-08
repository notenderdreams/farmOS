#include <iostream>
#include <map>
#include <vector>
#include <iostream>
using namespace std;
using namespace std;
enum class AttendanceStatus {
    Present,
    Absent
};
struct AttendanceRecord {
    string date;
    AttendanceStatus status;

    AttendanceRecord(string d, AttendanceStatus s)
        : date(d), status(s) {}
};
enum class LeaveStatus { Pending, Approved, Rejected };
enum class LeaveType { Sick, Normal, Paid, Unpaid };

struct LeaveRecord {
    string date;
    LeaveType type;
    LeaveStatus status;

    LeaveRecord(string d, LeaveType t)
        : date(d), type(t), status(LeaveStatus::Pending) {}
};
class AttendanceManagement {
private:
    map<string, vector<AttendanceRecord>> attendanceDB;

public:
    void addAttendance(const string& employeeId,
                       const string& date,
                       AttendanceStatus status)
    {
        attendanceDB[employeeId].push_back(
            AttendanceRecord(date, status)
        );
    }

    void viewAttendance(const string& employeeId) const
    {
        auto it = attendanceDB.find(employeeId);
        if (it == attendanceDB.end()) {
            cout << "No attendance found for employee.\n";
            return;
        }

        cout << "Attendance for Employee ID: " << employeeId << endl;
        for (const auto& rec : it->second) {
            cout << rec.date << " : "
                 << (rec.status == AttendanceStatus::Present
                     ? "Present" : "Absent") << endl;
        }
    }
};


