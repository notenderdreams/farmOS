#include "handle_hr.h"
#include "app_state.h"
#include "utils.h"
#include "cli/widgets.h"
#include "cli/color.h"
#include "core/hr_service.h"
#include "core/employee_service.h"
#include "core/transaction_service.h"
#include "core/models/hr.h"
#include "core/models/employee.h"
#include "core/models/transaction.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <iomanip>


// ── Role enforcement 

static bool requireRole(AppState* state, HRRole required)
{
    if (!state->hasSession()) {
        color::printError("No session. Login with your HR ID at startup.");
        return false;
    }
    if (!state->isRole(required)) {
        std::string msg = "Access denied. This action requires role: ";
        msg += hr::toStr(required);
        msg += ". Your role: ";
        msg += hr::toStr(state->getSessionRole());
        color::printError(msg);
        return false;
    }
    return true;
}

// ── Salary → Transaction helper 
//
// Shared by both hire (SALARY_INIT) and raise-approve (SALARY_ADJUSTMENT).
// Silently logs on failure so that the primary operation is never blocked.

static void tryRecordSalaryTx(TransactionService* tx_svc,
                               const std::string&  category,
                               const std::string&  description,
                               f64                 amount,
                               const std::string&  entity_id = "")
{
    if (!tx_svc || amount <= 0.0) return;
    try {
        Transaction tx;
        tx.type        = TransactionType::SALARY;
        tx.direction   = TransactionDirection::OUT;
        tx.amount      = amount;
        tx.entity_type = TransactionEntityType::EMPLOYEE;
        tx.entity_id   = entity_id;
        tx.description = "[" + category + "] " + description;  // category preserved here
        tx.date        = getCurrentDate();
        tx.status      = TransactionStatus::COMPLETED;
        tx_svc->addTransaction(tx);
    } catch (const std::exception& e) {
        // Non-fatal: warn but don't abort the HR action
        color::printError(std::string("TX record warning: ") + e.what());
    }
}

// ── Print helpers 

static void printHRStaff(const HRRecord& r, bool sep = true)
{
    std::cout << color::GREEN << "HR ID: " << color::RESET << r.hr_id << "\n"
              << "\tEmployee ID : " << r.employee_id       << "\n"
              << "\tName        : " << r.name              << "\n"
              << "\tRole        : " << hr::toStr(r.role)   << "\n";
    if (sep) color::printSeperator();
}

static void printVacancy(const VacancyRecord& v, bool sep = true)
{
    std::cout << color::GREEN << "ID: " << color::RESET << v.vacancy_id << "\n"
              << "\tPost:   " << v.post               << "\n"
              << "\tStatus: " << hr::toStr(v.status)  << "\n";
    if (sep) color::printSeperator();
}

static void printApplicant(const ApplicantRecord& a, bool sep = true)
{
    std::cout << color::GREEN << "ID: " << color::RESET << a.applicant_id << "\n"
              << "\tName:       " << a.name                      << "\n"
              << "\tPost:       " << a.post                      << "\n"
              << "\tExperience: " << a.experience_years << " yr" << "\n"
              << "\tStatus:     " << hr::toStr(a.status)         << "\n";
    if (sep) color::printSeperator();
}

static void printAttendance(const AttendanceRecord& r, bool sep = true)
{
    std::cout << color::GREEN << "ID: " << color::RESET << r.attendance_id << "\n"
              << "\tEmployee ID: " << r.employee_id            << "\n"
              << "\tDate:        " << r.date                   << "\n"
              << "\tStatus:      " << hr::toStr(r.status)      << "\n";
    if (!r.note.empty())
        std::cout << "\tNote:        " << r.note << "\n";
    if (sep) color::printSeperator();
}

// ── HR Staff 

int hrStaffAdd(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* hr_svc  = state->getHRService();
    auto* emp_svc = state->getEmployeeService();
    auto* tx_svc  = state->getTransactionService();
    if (!hr_svc || !emp_svc) { color::printError("Service unavailable"); return 1; }

    // ── Collect all info upfront 
    std::cout << color::BLUE << "\n  Add HR Staff Member\n" << color::RESET;
    color::printSeperator();

    std::string name   = wx::lineInput<std::string>("Full Name: ");
    HRRole      role   = wx::selectInput<HRRole>(
        "Role:", hr::HRRoleStrs, 4, hr::strToRole);
    f64         salary = wx::lineInput<double>("Salary: $");
    if (salary <= 0) { color::printError("Salary must be greater than 0"); return 1; }
    std::string jdate  = wx::lineInput<std::string>("Join Date (YYYY-MM-DD): ");

    // ── Confirm 
    std::cout << color::BLUE << "\n  Confirm:\n" << color::RESET;
    color::printSeperator();
    std::cout << "  Name   : " << name             << "\n"
              << "  Role   : " << hr::toStr(role)  << "\n"
              << "  Salary : $" << std::fixed << std::setprecision(2) << salary << "\n"
              << "  Joined : " << jdate            << "\n";
    color::printSeperator();
    std::cout << color::YELLOW << "  Proceed? [y/N]: " << color::RESET;
    std::string confirm;
    std::getline(std::cin, confirm);
    if (confirm != "y" && confirm != "Y") {
        std::cout << color::GREY << "  Cancelled.\n" << color::RESET;
        return 0;
    }

    try {
        // ── Step 1: create employee record (department = HR) 
        EmployeeRecord emp;
        emp.name       = name;
        emp.department = EmployeeDepartment::HR;
        emp.salary     = salary;
        emp.joined_at  = jdate;
        emp.status     = EmployeeStatus::ACTIVE;
        emp_svc->addEmployee(emp);

        // fetch back the auto-assigned employee_id
        i64 emp_id = 0;
        try {
            auto all = emp_svc->getAllEmployees();
            if (!all.empty()) emp_id = all.back().employee_id;
        } catch (...) {}

        // ── Step 2: create HR staff record linked to that employee 
        HRRecord r;
        r.employee_id = emp_id;
        r.name        = name;
        r.role        = role;
        hr_svc->addHRStaff(r);

        // ── Step 3: record SALARY_INIT transaction 
        std::ostringstream desc;
        desc << "Salary init: " << name
             << " (ID:" << emp_id << ")"
             << " | Role: " << hr::toStr(role)
             << " | Start: " << jdate;
        tryRecordSalaryTx(tx_svc, "SALARY_INIT", desc.str(), salary,
                          std::to_string(emp_id));

        std::cout << color::GREEN
                  << "\n    HR staff member added"
                  << "\n    Name        : " << name
                  << "\n    Employee ID : " << emp_id
                  << "\n    Role        : " << hr::toStr(role)
                  << "\n    Salary      : $" << std::fixed << std::setprecision(2) << salary
                  << "\n    Joined      : " << jdate
                  << color::RESET << "\n";
        if (tx_svc)
            std::cout << color::GREY
                      << "  Transaction recorded (SALARY_INIT, $"
                      << std::fixed << std::setprecision(2) << salary << ")\n"
                      << color::RESET;

    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int hrStaffList(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    try {
        auto list = svc->getAllHRStaff();
        if (list.empty()) { std::cout << color::YELLOW << "No HR staff found" << color::RESET << "\n"; return 0; }
        std::cout << color::BLUE << "HR Staff:" << color::RESET << "\n";
        color::printSeperator();
        for (const auto& r : list) printHRStaff(r);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── Manager: Vacancy 

int vacancyCreate(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::MANAGER)) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    VacancyRecord v;
    v.post = wx::lineInput<std::string>("Post / Job Title: ");

    try {
        svc->createVacancy(v);
        std::cout << color::GREEN << " Vacancy created" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int vacancyList(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    try {
        auto list = svc->getAllVacancies();
        if (list.empty()) { std::cout << color::YELLOW << "No vacancies" << color::RESET << "\n"; return 0; }
        std::cout << color::BLUE << "Vacancies:" << color::RESET << "\n";
        color::printSeperator();
        for (const auto& v : list) printVacancy(v);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int vacancyClose(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::MANAGER)) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "vacancy_id");

    try {
        svc->closeVacancy(std::stoll(id_str));
        std::cout << color::GREEN << "Vacancy closed" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── Manager: Leave decision 

int managerLeaveDecide(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::MANAGER)) return 1;
    auto* emp_svc = state->getEmployeeService();
    auto* hr_svc  = state->getHRService();
    if (!emp_svc || !hr_svc) { color::printError("Service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "request_id");

    try {
        i64 rid = std::stoll(id_str);

        LeaveRequest req = emp_svc->getLeaveById(rid);
        std::cout << color::BLUE << "Leave Request #" << rid << color::RESET << "\n";
        color::printSeperator();
        std::cout << "\tEmployee ID: " << req.employee_id << "\n"
                  << "\tFrom:        " << req.from_date   << "\n"
                  << "\tTo:          " << req.to_date     << "\n"
                  << "\tReason:      " << req.reason      << "\n"
                  << "\tStatus:      " << emp::toStr(req.status) << "\n";
        color::printSeperator();

        LeaveStatus decision = wx::selectInput<LeaveStatus>(
            "Decision:", emp::LeaveStatusStrs, 3, emp::strToLeaveStatus);

        emp_svc->updateLeaveStatus(rid, decision,
            [&hr_svc](i64 employee_id, const std::string& from, const std::string& to) {
                hr_svc->markGrantedLeave(employee_id, from, to);
                std::cout << color::GREY
                          << "  Attendance log updated with GRANTED_LEAVE"
                          << color::RESET << "\n";
            });

        std::cout << color::GREEN << " Leave request updated" << color::RESET << "\n";

    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── Manager: Raise decision 

int managerRaiseDecide(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::MANAGER)) return 1;

    auto* emp_svc = state->getEmployeeService();
    auto* tx_svc  = state->getTransactionService();
    if (!emp_svc) { color::printError("Employee service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "request_id");

    try {
        i64 rid = std::stoll(id_str);

        RaiseRequest req = emp_svc->getRaiseById(rid);
        std::cout << color::BLUE << "Raise Request #" << rid << color::RESET << "\n";
        color::printSeperator();
        std::cout << "\tEmployee ID:     " << req.employee_id                         << "\n"
                  << "\tCurrent Salary:  $" << req.current_salary                      << "\n"
                  << "\tRequested Raise: $" << req.raise_amount                        << "\n"
                  << "\tNew Total:       $" << (req.current_salary + req.raise_amount) << "\n"
                  << "\tReason:          " << req.reason                               << "\n";
        color::printSeperator();

        RaiseStatus decision = wx::selectInput<RaiseStatus>(
            "Decision:", emp::RaiseStatusStrs, 3, emp::strToRaiseStatus);

        std::string emp_name = "Unknown";
        try {
            EmployeeRecord e = emp_svc->getEmployeeById(req.employee_id);
            emp_name = e.name;
        } catch (...) {}

        emp_svc->updateRaiseStatus(rid, decision,
            [&emp_svc, &tx_svc, &emp_name, &req](i64 employee_id, f64 new_salary) {
                emp_svc->updateSalary(employee_id, new_salary);
                std::cout << color::GREY
                          << "  Salary updated to $" << new_salary
                          << color::RESET << "\n";

                std::ostringstream desc;
                desc << "Salary raise: " << emp_name
                     << " (ID:" << employee_id << ")"
                     << " | +" << std::fixed << std::setprecision(2) << req.raise_amount
                     << " | Reason: " << req.reason;

                tryRecordSalaryTx(tx_svc,
                                  "SALARY_ADJUSTMENT",
                                  desc.str(),
                                  req.raise_amount,
                                  std::to_string(employee_id));

                std::cout << color::GREY
                          << "  → Transaction recorded (SALARY_ADJUSTMENT, $"
                          << std::fixed << std::setprecision(2) << req.raise_amount << ")\n"
                          << color::RESET;
            });

        std::cout << color::GREEN << " Raise request updated" << color::RESET << "\n";

    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── Applicant 

int applicantApply(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    try {
        auto open = svc->getOpenVacancies();
        if (open.empty()) {
            std::cout << color::YELLOW << "No open vacancies available" << color::RESET << "\n";
            return 0;
        }
        std::cout << color::BLUE << "Open Vacancies:" << color::RESET << "\n";
        color::printSeperator();
        for (const auto& v : open) printVacancy(v);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }

    ApplicantRecord a;
    a.name             = wx::lineInput<std::string>("Your Name: ");
    a.experience_years = wx::lineInput<int>        ("Years of Experience: ");
    a.post             = wx::lineInput<std::string>("Post Applying For: ");

    try {
        svc->addApplicant(a);
        std::cout << color::GREEN << "Application submitted" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int applicantList(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    try {
        auto list = svc->getAllApplicants();
        if (list.empty()) { std::cout << color::YELLOW << "No applicants" << color::RESET << "\n"; return 0; }
        std::cout << color::BLUE << "Applicants:" << color::RESET << "\n";
        color::printSeperator();
        for (const auto& a : list) printApplicant(a);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── HiringOfficer 

int hiringOfficerHire(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::HIRING_OFFICER)) return 1;

    auto* hr_svc  = state->getHRService();
    auto* emp_svc = state->getEmployeeService();
    auto* tx_svc  = state->getTransactionService();
    if (!hr_svc || !emp_svc) { color::printError("Service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "applicant_id");

    try {
        i64 aid = std::stoll(id_str);

        ApplicantRecord a = hr_svc->getApplicantById(aid);
        std::cout << color::BLUE << "\n  Hiring: " << a.name
                  << " (applicant #" << aid << ")" << color::RESET << "\n";
        color::printSeperator();
        printApplicant(a, false);
        color::printSeperator();

        std::cout << color::GREY << "  Pre-filled from application. Press Enter to keep, or type to override.\n"
                  << color::RESET;
        std::cout << color::GREEN << "Full Name [" << a.name << "]: " << color::RESET;
        std::string name_input;
        std::getline(std::cin, name_input);
        std::string final_name = name_input.empty() ? a.name : name_input;

        EmployeeDepartment dept = wx::selectInput<EmployeeDepartment>(
            "Department:", emp::DepartmentStrs, 2, emp::strToDept);

        f64 salary = wx::lineInput<double>("Starting Salary: $");
        if (salary <= 0) {
            color::printError("Salary must be greater than 0");
            return 1;
        }

        std::string jdate = wx::lineInput<std::string>("Join Date (YYYY-MM-DD): ");

        std::cout << color::BLUE << "\n  Confirm new employee record:\n" << color::RESET;
        color::printSeperator();
        std::cout << "  Name       : " << final_name             << "\n"
                  << "  Department : " << emp::toStr(dept)       << "\n"
                  << "  Salary     : $" << std::fixed << std::setprecision(2) << salary << "\n"
                  << "  Join Date  : " << jdate                  << "\n"
                  << "  Post       : " << a.post                 << "\n";
        color::printSeperator();
        std::cout << color::YELLOW << "  Proceed? [y/N]: " << color::RESET;
        std::string confirm;
        std::getline(std::cin, confirm);
        if (confirm != "y" && confirm != "Y") {
            std::cout << color::GREY << "  Hiring cancelled.\n" << color::RESET;
            return 0;
        }

        EmployeeRecord emp = hr_svc->hireApplicant(aid, salary, jdate);
        emp.name       = final_name;
        emp.department = dept;
        emp.salary     = salary;
        emp.joined_at  = jdate;

        emp_svc->addEmployee(emp);

        i64 new_emp_id = 0;
        try {
            auto all_emps = emp_svc->getAllEmployees();
            if (!all_emps.empty()) new_emp_id = all_emps.back().employee_id;
        } catch (...) {}

        std::cout << color::GREEN
                  << "\n  Employee added"
                  << "\n    Name       : " << emp.name
                  << "\n    ID         : " << new_emp_id
                  << "\n    Department : " << emp::toStr(emp.department)
                  << "\n    Salary     : $" << std::fixed << std::setprecision(2) << emp.salary
                  << "\n    Joined     : " << emp.joined_at
                  << color::RESET << "\n";

        std::ostringstream desc;
        desc << "Salary init: " << emp.name
             << " (ID:" << new_emp_id << ")"
             << " | Post: "  << a.post
             << " | Start: " << jdate;

        tryRecordSalaryTx(tx_svc, "SALARY_INIT", desc.str(), salary,
                          std::to_string(new_emp_id));

        if (tx_svc)
            std::cout << color::GREY
                      << "  Transaction recorded (SALARY_INIT, $"
                      << std::fixed << std::setprecision(2) << salary << ")\n"
                      << color::RESET;

    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int hiringOfficerReject(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::HIRING_OFFICER)) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "applicant_id");

    try {
        i64 aid = std::stoll(id_str);
        ApplicantRecord a = svc->getApplicantById(aid);
        svc->updateApplicantStatus(aid, ApplicantStatus::REJECTED);
        std::cout << color::GREEN << "Applicant rejected: "
                  << a.name << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── AttendanceOfficer 

int attendanceMark(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::ATTENDANCE_OFFICER)) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "employee_id");

    try {
        AttendanceRecord r;
        r.employee_id = std::stoll(id_str);
        r.date        = wx::lineInput<std::string>("Date (YYYY-MM-DD): ");
        r.status      = wx::selectInput<AttendanceStatus>(
            "Status:", hr::AttendanceStatusStrs, 3, hr::strToAttendanceStatus);
        r.note        = wx::lineInput<std::string>("Note (optional, press Enter to skip): ");

        svc->markAttendance(r);
        std::cout << color::GREEN << " Attendance marked" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int attendanceByEmployee(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::ATTENDANCE_OFFICER)) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "employee_id");

    try {
        auto list = svc->getAttendanceByEmployee(std::stoll(id_str));
        if (list.empty()) { std::cout << color::YELLOW << "No attendance records" << color::RESET << "\n"; return 0; }
        std::cout << color::BLUE << "Attendance for Employee #" << id_str << color::RESET << "\n";
        color::printSeperator();
        for (const auto& r : list) printAttendance(r);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

int attendanceByDate(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::ATTENDANCE_OFFICER)) return 1;
    auto* svc = state->getHRService();
    if (!svc) { color::printError("HR service unavailable"); return 1; }

    std::string date;
    loadArg(date, 0, "date");

    try {
        auto list = svc->getAttendanceByDate(date);
        if (list.empty()) { std::cout << color::YELLOW << "No attendance records for " << date << color::RESET << "\n"; return 0; }
        std::cout << color::BLUE << "Attendance for " << date << color::RESET << "\n";
        color::printSeperator();
        for (const auto& r : list) printAttendance(r);
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ── Manager: Delete Employee 

int managerDeleteEmployee(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireRole(state, HRRole::MANAGER)) return 1;
    auto* emp_svc = state->getEmployeeService();
    if (!emp_svc) { color::printError("Employee service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "employee_id");

    try {
        i64 eid = std::stoll(id_str);
        EmployeeRecord e = emp_svc->getEmployeeById(eid);
        std::cout << color::YELLOW << "Deleting employee: "
                  << e.name << " [" << emp::toStr(e.department) << "]"
                  << color::RESET << "\n";
        emp_svc->deleteEmployee(eid);
        std::cout << color::GREEN << "Employee deleted" << color::RESET << "\n";
    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}