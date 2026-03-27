#pragma once
#include <string_view>

namespace farmos::models {

// ── Employees ─────────────────────────────────────────────────────────────────

inline constexpr std::string_view EMPLOYEES_TABLE = R"(
    CREATE TABLE IF NOT EXISTS employees (
        employee_id  INTEGER PRIMARY KEY AUTOINCREMENT,
        name         TEXT    NOT NULL,
        department   TEXT    NOT NULL,
        status       TEXT    NOT NULL DEFAULT 'ACTIVE',
        salary       REAL    NOT NULL DEFAULT 0.0,
        joined_at    TEXT    NOT NULL,
        created_at   TEXT    DEFAULT CURRENT_TIMESTAMP
    );
)";

// ── Leave requests ────────────────────────────────────────────────────────────

inline constexpr std::string_view LEAVE_REQUESTS_TABLE = R"(
    CREATE TABLE IF NOT EXISTS leave_requests (
        request_id   INTEGER PRIMARY KEY AUTOINCREMENT,
        employee_id  INTEGER NOT NULL,
        from_date    TEXT    NOT NULL,
        to_date      TEXT    NOT NULL,
        reason       TEXT    NOT NULL,
        status       TEXT    NOT NULL DEFAULT 'PENDING',
        created_at   TEXT    DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (employee_id) REFERENCES employees(employee_id)
    );
)";

// ── Raise requests ────────────────────────────────────────────────────────────

inline constexpr std::string_view RAISE_REQUESTS_TABLE = R"(
    CREATE TABLE IF NOT EXISTS raise_requests (
        request_id      INTEGER PRIMARY KEY AUTOINCREMENT,
        employee_id     INTEGER NOT NULL,
        current_salary  REAL    NOT NULL,
        raise_amount    REAL    NOT NULL,
        reason          TEXT    NOT NULL,
        status          TEXT    NOT NULL DEFAULT 'PENDING',
        created_at      TEXT    DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (employee_id) REFERENCES employees(employee_id)
    );
)";

// ── HR staff ──────────────────────────────────────────────────────────────────

inline constexpr std::string_view HR_STAFF_TABLE = R"(
    CREATE TABLE IF NOT EXISTS hr_staff (
        hr_id       INTEGER PRIMARY KEY AUTOINCREMENT,
        employee_id INTEGER NOT NULL,
        name        TEXT    NOT NULL,
        role        TEXT    NOT NULL,
        created_at  TEXT    DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (employee_id) REFERENCES employees(employee_id)
    );
)";

// ── Vacancies ─────────────────────────────────────────────────────────────────

inline constexpr std::string_view VACANCIES_TABLE = R"(
    CREATE TABLE IF NOT EXISTS vacancies (
        vacancy_id  INTEGER PRIMARY KEY AUTOINCREMENT,
        post        TEXT    NOT NULL,
        status      TEXT    NOT NULL DEFAULT 'OPEN',
        created_at  TEXT    DEFAULT CURRENT_TIMESTAMP
    );
)";

// ── Applicants ────────────────────────────────────────────────────────────────

inline constexpr std::string_view APPLICANTS_TABLE = R"(
    CREATE TABLE IF NOT EXISTS applicants (
        applicant_id     INTEGER PRIMARY KEY AUTOINCREMENT,
        name             TEXT    NOT NULL,
        experience_years INTEGER NOT NULL DEFAULT 0,
        post             TEXT    NOT NULL,
        status           TEXT    NOT NULL DEFAULT 'PENDING',
        created_at       TEXT    DEFAULT CURRENT_TIMESTAMP
    );
)";

// ── Attendance ────────────────────────────────────────────────────────────────

inline constexpr std::string_view ATTENDANCE_TABLE = R"(
    CREATE TABLE IF NOT EXISTS attendance (
        attendance_id INTEGER PRIMARY KEY AUTOINCREMENT,
        employee_id   INTEGER NOT NULL,
        date          TEXT    NOT NULL,
        status        TEXT    NOT NULL,
        note          TEXT    DEFAULT '',
        created_at    TEXT    DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (employee_id) REFERENCES employees(employee_id)
    );
)";

} // namespace farmos::models
