#pragma once
#include "core/database.h"
#include "core/models/employee.h"
#include <vector>
#include <functional>

class EmployeeService : public Database
{
public:
    EmployeeService(const std::string& db_path);
    EmployeeService(sqlite3* db);

    void initTable();

    void                        addEmployee    (const EmployeeRecord& r);
    std::vector<EmployeeRecord> getAllEmployees ();
    EmployeeRecord              getEmployeeById(i64 employee_id);
    void                        updateStatus   (i64 employee_id, EmployeeStatus s);
    void                        updateSalary   (i64 employee_id, f64 new_salary);
    void                        deleteEmployee (i64 employee_id);

    void                      askLeave          (const LeaveRequest& r);
    std::vector<LeaveRequest> getAllLeaveRequests();
    std::vector<LeaveRequest> getLeaveByEmployee(i64 employee_id);
    LeaveRequest              getLeaveById      (i64 request_id);

    // Manager calls this — also triggers attendance update via callback
    // on_approved(employee_id, from_date, to_date) is called if approved
    void updateLeaveStatus(i64 request_id, LeaveStatus s,
                           std::function<void(i64, const std::string&, const std::string&)> on_approved = nullptr);

    void                      askRaise          (const RaiseRequest& r);
    std::vector<RaiseRequest> getAllRaiseRequests();
    std::vector<RaiseRequest> getRaiseByEmployee(i64 employee_id);
    RaiseRequest              getRaiseById      (i64 request_id);

    // Manager calls this — also triggers salary update via callback
    // on_approved(employee_id, new_salary) is called if approved
    void updateRaiseStatus(i64 request_id, RaiseStatus s,
                           std::function<void(i64, f64)> on_approved = nullptr);

    std::vector<RaiseRequest> getApprovedRaises();                        // raise history
    std::vector<EmployeeRecord> getPayrollForMonth(const std::string& month); // YYYY-MM
};
