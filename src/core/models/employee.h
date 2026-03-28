#pragma once
#include <string>
#include <stdexcept>
#include "core/types.h"

// ── Enums ─────────────────────────────────────────────────────────────────────

enum class EmployeeDepartment {
    ANIMAL,
    HR
};

enum class EmployeeStatus {
    ACTIVE,
    ON_LEAVE,
    TERMINATED
};

enum class LeaveStatus {
    PENDING,
    APPROVED,
    REJECTED
};

enum class RaiseStatus {
    PENDING,
    APPROVED,
    REJECTED
};

// ── Structs ───────────────────────────────────────────────────────────────────

struct EmployeeRecord {
    i64         employee_id  = 0;
    std::string name;
    EmployeeDepartment department;
    EmployeeStatus     status = EmployeeStatus::ACTIVE;
    f64         salary        = 0.0;
    std::string joined_at;
    std::string created_at;
};

struct LeaveRequest {
    i64         request_id  = 0;
    i64         employee_id = 0;
    std::string from_date;   // YYYY-MM-DD
    std::string to_date;     // YYYY-MM-DD
    std::string reason;
    LeaveStatus status = LeaveStatus::PENDING;
    std::string created_at;
};

struct RaiseRequest {
    i64         request_id      = 0;
    i64         employee_id     = 0;
    f64         current_salary  = 0.0;
    f64         raise_amount    = 0.0;
    std::string reason;
    RaiseStatus status = RaiseStatus::PENDING;
    std::string created_at;
};

// ── String helpers ────────────────────────────────────────────────────────────

namespace emp {

    extern const char* DepartmentStrs[];
    extern const char* EmployeeStatusStrs[];
    extern const char* LeaveStatusStrs[];
    extern const char* RaiseStatusStrs[];

    static const char* toStr(EmployeeDepartment d) { return DepartmentStrs    [static_cast<int>(d)]; }
    static const char* toStr(EmployeeStatus s)     { return EmployeeStatusStrs[static_cast<int>(s)]; }
    static const char* toStr(LeaveStatus s)        { return LeaveStatusStrs   [static_cast<int>(s)]; }
    static const char* toStr(RaiseStatus s)        { return RaiseStatusStrs   [static_cast<int>(s)]; }

    inline EmployeeDepartment strToDept(const std::string& s) {
        if (s == "ANIMAL") return EmployeeDepartment::ANIMAL;
        if (s == "HR")     return EmployeeDepartment::HR;
        throw std::runtime_error("Invalid department: " + s);
    }

    inline EmployeeStatus strToStatus(const std::string& s) {
        if (s == "ACTIVE")     return EmployeeStatus::ACTIVE;
        if (s == "ON_LEAVE")   return EmployeeStatus::ON_LEAVE;
        if (s == "TERMINATED") return EmployeeStatus::TERMINATED;
        throw std::runtime_error("Invalid employee status: " + s);
    }

    inline LeaveStatus strToLeaveStatus(const std::string& s) {
        if (s == "PENDING")  return LeaveStatus::PENDING;
        if (s == "APPROVED") return LeaveStatus::APPROVED;
        if (s == "REJECTED") return LeaveStatus::REJECTED;
        throw std::runtime_error("Invalid leave status: " + s);
    }

    inline RaiseStatus strToRaiseStatus(const std::string& s) {
        if (s == "PENDING")  return RaiseStatus::PENDING;
        if (s == "APPROVED") return RaiseStatus::APPROVED;
        if (s == "REJECTED") return RaiseStatus::REJECTED;
        throw std::runtime_error("Invalid raise status: " + s);
    }

} // namespace emp
