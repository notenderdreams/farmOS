#include "handle_employee.h"
#include "app_state.h"
#include "utils.h"
#include "cli/widgets.h"
#include "cli/color.h"
#include "core/employee_service.h"
#include "core/models/employee.h"
#include <iostream>

// ── Print helpers ─────────────────────────────────────────────────────────────

static void printEmployee(const EmployeeRecord& r, bool sep = true)
{
    std::cout << color::GREEN << "ID: " << color::RESET << r.employee_id << "\n"
              << "\tName:       " << r.name                    << "\n"
              << "\tDepartment: " << emp::toStr(r.department)  << "\n"
              << "\tStatus:     " << emp::toStr(r.status)      << "\n"
              << "\tSalary:     $" << r.salary                 << "\n"
              << "\tJoined:     " << r.joined_at               << "\n";
    if (sep) color::printSeperator();
}

static void printLeave(const LeaveRequest& r, bool sep = true)
{
    std::cout << color::GREEN << "Request ID: " << color::RESET << r.request_id << "\n"
              << "\tEmployee ID: " << r.employee_id           << "\n"
              << "\tFrom:        " << r.from_date             << "\n"
              << "\tTo:          " << r.to_date               << "\n"
              << "\tReason:      " << r.reason                << "\n"
              << "\tStatus:      " << emp::toStr(r.status)    << "\n";
    if (sep) color::printSeperator();
}

static void printRaise(const RaiseRequest& r, bool sep = true)
{
    std::cout << color::GREEN << "Request ID: " << color::RESET << r.request_id << "\n"
              << "\tEmployee ID:     " << r.employee_id                              << "\n"
              << "\tCurrent Salary:  $" << r.current_salary                          << "\n"
              << "\tRequested Raise: $" << r.raise_amount                            << "\n"
              << "\tNew Total:       $" << (r.current_salary + r.raise_amount)       << "\n"
              << "\tReason:          " << r.reason                                   << "\n"
              << "\tStatus:          " << emp::toStr(r.status)                       << "\n";
    if (sep) color::printSeperator();
}

// ── Employee commands ─────────────────────────────────────────────────────────

int employeeAdd(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    EmployeeRecord r;
    r.name       = wx::lineInput<std::string>("Full Name: ");
    r.department = wx::selectInput<EmployeeDepartment>(
        "Department:", emp::DepartmentStrs, 2, emp::strToDept);
    r.salary     = wx::lineInput<double>("Starting Salary: $");
    r.joined_at  = wx::lineInput<std::string>("Join Date (YYYY-MM-DD): ");

    try {
        svc->addEmployee(r);
        std::cout << color::GREEN << "✓ Employee added" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int employeeList(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    try {
        auto list = svc->getAllEmployees();
        if (list.empty()) { std::cout << color::YELLOW << "No employees found" << color::RESET << "\n"; return 0; }
        std::cout << color::BLUE << "Employees:" << color::RESET << "\n";
        color::printSeperator();
        for (const auto& e : list) printEmployee(e);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int employeeShow(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "employee_id");

    try {
        auto r = svc->getEmployeeById(std::stoll(id_str));
        std::cout << color::BLUE << "Employee #" << r.employee_id << color::RESET << "\n";
        color::printSeperator();
        printEmployee(r, false);
        std::cout << "\tRegistered: " << r.created_at << "\n";
        color::printSeperator();
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int employeeDelete(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "employee_id");

    try {
        svc->deleteEmployee(std::stoll(id_str));
        std::cout << color::GREEN << "✓ Employee deleted" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── Leave ─────────────────────────────────────────────────────────────────────

int employeeAskLeave(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "employee_id");

    try {
        LeaveRequest r;
        r.employee_id = std::stoll(id_str);
        r.from_date   = wx::lineInput<std::string>("From Date (YYYY-MM-DD): ");
        r.to_date     = wx::lineInput<std::string>("To Date   (YYYY-MM-DD): ");
        r.reason      = wx::lineInput<std::string>("Reason: ");

        svc->askLeave(r);
        std::cout << color::GREEN << "✓ Leave request submitted" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int employeeLeaveList(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    try {
        auto list = svc->getAllLeaveRequests();
        if (list.empty()) { std::cout << color::YELLOW << "No leave requests" << color::RESET << "\n"; return 0; }
        std::cout << color::BLUE << "Leave Requests:" << color::RESET << "\n";
        color::printSeperator();
        for (const auto& r : list) printLeave(r);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── Raise ─────────────────────────────────────────────────────────────────────

int employeeAskRaise(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "employee_id");

    try {
        i64 eid = std::stoll(id_str);
        EmployeeRecord emp = svc->getEmployeeById(eid);
        std::cout << color::GREY << "Current salary: $" << emp.salary << color::RESET << "\n";

        RaiseRequest r;
        r.employee_id    = eid;
        r.current_salary = emp.salary;
        r.raise_amount   = wx::lineInput<double>("Raise Amount: $");
        r.reason         = wx::lineInput<std::string>("Reason: ");

        svc->askRaise(r);
        std::cout << color::GREEN << "✓ Raise request submitted" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int employeeRaiseList(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    try {
        auto list = svc->getAllRaiseRequests();
        if (list.empty()) { std::cout << color::YELLOW << "No raise requests" << color::RESET << "\n"; return 0; }
        std::cout << color::BLUE << "Raise Requests:" << color::RESET << "\n";
        color::printSeperator();
        for (const auto& r : list) printRaise(r);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}
