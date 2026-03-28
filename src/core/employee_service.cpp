#include "core/employee_service.h"
#include "core/models/new_schema.h"
#include <stdexcept>
#include <functional>

EmployeeService::EmployeeService(const std::string& db_path) : Database(db_path) {}
EmployeeService::EmployeeService(sqlite3* db)                : Database(db)       {}

void EmployeeService::initTable()
{
    execute(std::string(farmos::models::EMPLOYEES_TABLE));
    execute(std::string(farmos::models::LEAVE_REQUESTS_TABLE));
    execute(std::string(farmos::models::RAISE_REQUESTS_TABLE));
}


static EmployeeRecord rowToEmployee(sqlite3_stmt* s)
{
    EmployeeRecord r;
    r.employee_id = sqlite3_column_int64 (s, 0);
    r.name        = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
    r.department  = emp::strToDept  (reinterpret_cast<const char*>(sqlite3_column_text(s, 2)));
    r.status      = emp::strToStatus(reinterpret_cast<const char*>(sqlite3_column_text(s, 3)));
    r.salary      = sqlite3_column_double(s, 4);
    r.joined_at   = reinterpret_cast<const char*>(sqlite3_column_text(s, 5));
    r.created_at  = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
    return r;
}

static LeaveRequest rowToLeave(sqlite3_stmt* s)
{
    LeaveRequest r;
    r.request_id  = sqlite3_column_int64(s, 0);
    r.employee_id = sqlite3_column_int64(s, 1);
    r.from_date   = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
    r.to_date     = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
    r.reason      = reinterpret_cast<const char*>(sqlite3_column_text(s, 4));
    r.status      = emp::strToLeaveStatus(reinterpret_cast<const char*>(sqlite3_column_text(s, 5)));
    r.created_at  = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
    return r;
}

static RaiseRequest rowToRaise(sqlite3_stmt* s)
{
    RaiseRequest r;
    r.request_id     = sqlite3_column_int64 (s, 0);
    r.employee_id    = sqlite3_column_int64 (s, 1);
    r.current_salary = sqlite3_column_double(s, 2);
    r.raise_amount   = sqlite3_column_double(s, 3);
    r.reason         = reinterpret_cast<const char*>(sqlite3_column_text(s, 4));
    r.status         = emp::strToRaiseStatus(reinterpret_cast<const char*>(sqlite3_column_text(s, 5)));
    r.created_at     = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
    return r;
}



void EmployeeService::addEmployee(const EmployeeRecord& r)
{
    const char* sql = R"(
        INSERT INTO employees (name, department, status, salary, joined_at)
        VALUES (?, ?, ?, ?, ?);
    )";
    sqlite3_stmt* stmt = prepare(sql);
    sqlite3_bind_text  (stmt, 1, r.name.c_str(),           -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 2, emp::toStr(r.department),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 3, emp::toStr(r.status),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, r.salary);
    sqlite3_bind_text  (stmt, 5, r.joined_at.c_str(),      -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to insert employee"); }
    sqlite3_finalize(stmt);
}

std::vector<EmployeeRecord> EmployeeService::getAllEmployees()
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM employees;");
    std::vector<EmployeeRecord> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToEmployee(stmt));
    sqlite3_finalize(stmt);
    return out;
}

EmployeeRecord EmployeeService::getEmployeeById(i64 id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM employees WHERE employee_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    { auto r = rowToEmployee(stmt); sqlite3_finalize(stmt); return r; }
    sqlite3_finalize(stmt);
    throw std::runtime_error("DB: Employee not found");
}

void EmployeeService::updateStatus(i64 id, EmployeeStatus s)
{
    sqlite3_stmt* stmt = prepare("UPDATE employees SET status = ? WHERE employee_id = ?;");
    sqlite3_bind_text (stmt, 1, emp::toStr(s), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to update employee status"); }
    sqlite3_finalize(stmt);
}

void EmployeeService::updateSalary(i64 id, f64 new_salary)
{
    sqlite3_stmt* stmt = prepare("UPDATE employees SET salary = ? WHERE employee_id = ?;");
    sqlite3_bind_double(stmt, 1, new_salary);
    sqlite3_bind_int64 (stmt, 2, id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to update salary"); }
    sqlite3_finalize(stmt);
}

void EmployeeService::deleteEmployee(i64 id)
{
    sqlite3_stmt* stmt = prepare("DELETE FROM employees WHERE employee_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to delete employee"); }
    sqlite3_finalize(stmt);
}



void EmployeeService::askLeave(const LeaveRequest& r)
{
    // Employee must exist and not be TERMINATED
    EmployeeRecord e = getEmployeeById(r.employee_id);
    if (e.status == EmployeeStatus::TERMINATED)
        throw std::runtime_error("Terminated employees cannot request leave");

    const char* sql = R"(
        INSERT INTO leave_requests (employee_id, from_date, to_date, reason, status)
        VALUES (?, ?, ?, ?, 'PENDING');
    )";
    sqlite3_stmt* stmt = prepare(sql);
    sqlite3_bind_int64(stmt, 1, r.employee_id);
    sqlite3_bind_text (stmt, 2, r.from_date.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 3, r.to_date.c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 4, r.reason.c_str(),    -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to submit leave request"); }
    sqlite3_finalize(stmt);
}

std::vector<LeaveRequest> EmployeeService::getAllLeaveRequests()
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM leave_requests;");
    std::vector<LeaveRequest> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToLeave(stmt));
    sqlite3_finalize(stmt);
    return out;
}

std::vector<LeaveRequest> EmployeeService::getLeaveByEmployee(i64 id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM leave_requests WHERE employee_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    std::vector<LeaveRequest> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToLeave(stmt));
    sqlite3_finalize(stmt);
    return out;
}

LeaveRequest EmployeeService::getLeaveById(i64 id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM leave_requests WHERE request_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    { auto r = rowToLeave(stmt); sqlite3_finalize(stmt); return r; }
    sqlite3_finalize(stmt);
    throw std::runtime_error("DB: Leave request not found");
}

void EmployeeService::updateLeaveStatus(
    i64 request_id,
    LeaveStatus s,
    std::function<void(i64, const std::string&, const std::string&)> on_approved)
{
    // Fetch request BEFORE updating so we have the data for the callback
    LeaveRequest req = getLeaveById(request_id);

    sqlite3_stmt* stmt = prepare("UPDATE leave_requests SET status = ? WHERE request_id = ?;");
    sqlite3_bind_text (stmt, 1, emp::toStr(s), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, request_id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to update leave status"); }
    sqlite3_finalize(stmt);

    if (s == LeaveStatus::APPROVED)
    {
        // Mark employee ON_LEAVE
        updateStatus(req.employee_id, EmployeeStatus::ON_LEAVE);

        // Notify AttendanceOfficer via callback
        if (on_approved)
            on_approved(req.employee_id, req.from_date, req.to_date);
    }
}

//  Raise 

void EmployeeService::askRaise(const RaiseRequest& r)
{
    EmployeeRecord e = getEmployeeById(r.employee_id);
    if (e.status == EmployeeStatus::TERMINATED)
        throw std::runtime_error("Terminated employees cannot request a raise");

    const char* sql = R"(
        INSERT INTO raise_requests (employee_id, current_salary, raise_amount, reason, status)
        VALUES (?, ?, ?, ?, 'PENDING');
    )";
    sqlite3_stmt* stmt = prepare(sql);
    sqlite3_bind_int64 (stmt, 1, r.employee_id);
    sqlite3_bind_double(stmt, 2, r.current_salary);
    sqlite3_bind_double(stmt, 3, r.raise_amount);
    sqlite3_bind_text  (stmt, 4, r.reason.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to submit raise request"); }
    sqlite3_finalize(stmt);
}

std::vector<RaiseRequest> EmployeeService::getAllRaiseRequests()
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM raise_requests;");
    std::vector<RaiseRequest> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToRaise(stmt));
    sqlite3_finalize(stmt);
    return out;
}

std::vector<RaiseRequest> EmployeeService::getRaiseByEmployee(i64 id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM raise_requests WHERE employee_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    std::vector<RaiseRequest> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToRaise(stmt));
    sqlite3_finalize(stmt);
    return out;
}

RaiseRequest EmployeeService::getRaiseById(i64 id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM raise_requests WHERE request_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    { auto r = rowToRaise(stmt); sqlite3_finalize(stmt); return r; }
    sqlite3_finalize(stmt);
    throw std::runtime_error("DB: Raise request not found");
}

void EmployeeService::updateRaiseStatus(
    i64 request_id,
    RaiseStatus s,
    std::function<void(i64, f64)> on_approved)
{
    // Fetch BEFORE updating for callback data
    RaiseRequest req = getRaiseById(request_id);

    sqlite3_stmt* stmt = prepare("UPDATE raise_requests SET status = ? WHERE request_id = ?;");
    sqlite3_bind_text (stmt, 1, emp::toStr(s), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, request_id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to update raise status"); }
    sqlite3_finalize(stmt);

    if (s == RaiseStatus::APPROVED)
    {
        f64 new_salary = req.current_salary + req.raise_amount;

        // Notify Accountant via callback — accountant updates the salary
        if (on_approved)
            on_approved(req.employee_id, new_salary);
    }
}

//  Accountant queries 

std::vector<RaiseRequest> EmployeeService::getApprovedRaises()
{
    sqlite3_stmt* stmt = prepare(
        "SELECT * FROM raise_requests WHERE status = 'APPROVED' ORDER BY created_at DESC;");
    std::vector<RaiseRequest> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToRaise(stmt));
    sqlite3_finalize(stmt);
    return out;
}

// Returns all active employees who joined on or before the given month
// Used for monthly payroll — YYYY-MM format
std::vector<EmployeeRecord> EmployeeService::getPayrollForMonth(const std::string& month)
{
    // month = "YYYY-MM", joined_at = "YYYY-MM-DD"
    // include employees who joined before or during the given month
    // and are not TERMINATED
    const char* sql = R"(
        SELECT * FROM employees
        WHERE status != 'TERMINATED'
        AND strftime('%Y-%m', joined_at) <= ?
        ORDER BY name ASC;
    )";
    sqlite3_stmt* stmt = prepare(sql);
    sqlite3_bind_text(stmt, 1, month.c_str(), -1, SQLITE_TRANSIENT);
    std::vector<EmployeeRecord> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToEmployee(stmt));
    sqlite3_finalize(stmt);
    return out;
}
