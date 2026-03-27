#pragma once
#include <string>
#include <stdexcept>
#include "core/types.h"

// ── Enums ─────────────────────────────────────────────────────────────────────

enum class HRRole {
    MANAGER,
    ATTENDANCE_OFFICER,
    ACCOUNTANT,
    HIRING_OFFICER
};

enum class VacancyStatus {
    OPEN,
    CLOSED,
    FILLED
};

enum class ApplicantStatus {
    PENDING,
    SELECTED,
    REJECTED
};

enum class AttendanceStatus {
    PRESENT,
    ABSENT,
    GRANTED_LEAVE
};

// ── Structs ───────────────────────────────────────────────────────────────────

// HR staff member
struct HRRecord {
    i64         hr_id       = 0;
    i64         employee_id = 0;   // ← linked employee record
    std::string name;
    HRRole      role;
    std::string created_at;
};

// Job vacancy created by Manager
struct VacancyRecord {
    i64         vacancy_id     = 0;
    std::string post;           // job title / post name
    VacancyStatus status = VacancyStatus::OPEN;
    std::string created_at;
};

// Applicant who applies for a vacancy
struct ApplicantRecord {
    i64         applicant_id      = 0;
    std::string name;
    int         experience_years  = 0;
    std::string post;             // post they are applying for
    ApplicantStatus status = ApplicantStatus::PENDING;
    std::string created_at;
};

// Attendance record — written only by AttendanceOfficer
struct AttendanceRecord {
    i64         attendance_id = 0;
    i64         employee_id   = 0;
    std::string date;          // YYYY-MM-DD
    AttendanceStatus status;
    std::string note;          // e.g. "Granted Leave: 2025-03-01 to 2025-03-05"
    std::string created_at;
};

// ── String helpers ────────────────────────────────────────────────────────────

namespace hr {

    extern const char* HRRoleStrs[];
    extern const char* VacancyStatusStrs[];
    extern const char* ApplicantStatusStrs[];
    extern const char* AttendanceStatusStrs[];

    static const char* toStr(HRRole r)           { return HRRoleStrs         [static_cast<int>(r)]; }
    static const char* toStr(VacancyStatus s)    { return VacancyStatusStrs  [static_cast<int>(s)]; }
    static const char* toStr(ApplicantStatus s)  { return ApplicantStatusStrs[static_cast<int>(s)]; }
    static const char* toStr(AttendanceStatus s) { return AttendanceStatusStrs[static_cast<int>(s)]; }

    inline HRRole strToRole(const std::string& s) {
        if (s == "MANAGER")            return HRRole::MANAGER;
        if (s == "ATTENDANCE_OFFICER") return HRRole::ATTENDANCE_OFFICER;
        if (s == "ACCOUNTANT")         return HRRole::ACCOUNTANT;
        if (s == "HIRING_OFFICER")     return HRRole::HIRING_OFFICER;
        throw std::runtime_error("Invalid HR role: " + s);
    }

    inline VacancyStatus strToVacancyStatus(const std::string& s) {
        if (s == "OPEN")   return VacancyStatus::OPEN;
        if (s == "CLOSED") return VacancyStatus::CLOSED;
        if (s == "FILLED") return VacancyStatus::FILLED;
        throw std::runtime_error("Invalid vacancy status: " + s);
    }

    inline ApplicantStatus strToApplicantStatus(const std::string& s) {
        if (s == "PENDING")  return ApplicantStatus::PENDING;
        if (s == "SELECTED") return ApplicantStatus::SELECTED;
        if (s == "REJECTED") return ApplicantStatus::REJECTED;
        throw std::runtime_error("Invalid applicant status: " + s);
    }

    inline AttendanceStatus strToAttendanceStatus(const std::string& s) {
        if (s == "PRESENT")      return AttendanceStatus::PRESENT;
        if (s == "ABSENT")       return AttendanceStatus::ABSENT;
        if (s == "GRANTED_LEAVE") return AttendanceStatus::GRANTED_LEAVE;
        throw std::runtime_error("Invalid attendance status: " + s);
    }

} // namespace hr
