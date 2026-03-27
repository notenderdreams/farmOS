#include "core/hr_service.h"
#include "core/models/new_schema.h"
#include <stdexcept>
#include <sstream>
#include <ctime>
#include <iomanip>

HRService::HRService(const std::string& db_path) : Database(db_path) {}
HRService::HRService(sqlite3* db)                : Database(db)       {}

void HRService::initTable()
{
    execute(std::string(farmos::models::HR_STAFF_TABLE));
    execute(std::string(farmos::models::VACANCIES_TABLE));
    execute(std::string(farmos::models::APPLICANTS_TABLE));
    execute(std::string(farmos::models::ATTENDANCE_TABLE));
}

// ── Row readers ───────────────────────────────────────────────────────────────

static HRRecord rowToHR(sqlite3_stmt* s)
{
    HRRecord r;
    r.hr_id       = sqlite3_column_int64(s, 0);
    r.employee_id = sqlite3_column_int64(s, 1);
    r.name        = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
    r.role        = hr::strToRole(reinterpret_cast<const char*>(sqlite3_column_text(s, 3)));
    r.created_at  = reinterpret_cast<const char*>(sqlite3_column_text(s, 4));
    return r;
}

static VacancyRecord rowToVacancy(sqlite3_stmt* s)
{
    VacancyRecord r;
    r.vacancy_id = sqlite3_column_int64(s, 0);
    r.post       = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
    r.status     = hr::strToVacancyStatus(reinterpret_cast<const char*>(sqlite3_column_text(s, 2)));
    r.created_at = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
    return r;
}

static ApplicantRecord rowToApplicant(sqlite3_stmt* s)
{
    ApplicantRecord r;
    r.applicant_id    = sqlite3_column_int64(s, 0);
    r.name            = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
    r.experience_years= sqlite3_column_int(s, 2);
    r.post            = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
    r.status          = hr::strToApplicantStatus(reinterpret_cast<const char*>(sqlite3_column_text(s, 4)));
    r.created_at      = reinterpret_cast<const char*>(sqlite3_column_text(s, 5));
    return r;
}

static AttendanceRecord rowToAttendance(sqlite3_stmt* s)
{
    AttendanceRecord r;
    r.attendance_id = sqlite3_column_int64(s, 0);
    r.employee_id   = sqlite3_column_int64(s, 1);
    r.date          = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
    r.status        = hr::strToAttendanceStatus(reinterpret_cast<const char*>(sqlite3_column_text(s, 3)));
    r.note          = reinterpret_cast<const char*>(sqlite3_column_text(s, 4));
    r.created_at    = reinterpret_cast<const char*>(sqlite3_column_text(s, 5));
    return r;
}

// ── HR Staff ──────────────────────────────────────────────────────────────────

void HRService::addHRStaff(const HRRecord& r)
{
    if (r.employee_id == 0)
        throw std::runtime_error("HRRecord must have a valid employee_id before inserting");

    sqlite3_stmt* stmt = prepare(
        "INSERT INTO hr_staff (employee_id, name, role) VALUES (?, ?, ?);");
    sqlite3_bind_int64(stmt, 1, r.employee_id);
    sqlite3_bind_text (stmt, 2, r.name.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 3, hr::toStr(r.role), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to add HR staff"); }
    sqlite3_finalize(stmt);
}

std::vector<HRRecord> HRService::getAllHRStaff()
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM hr_staff;");
    std::vector<HRRecord> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToHR(stmt));
    sqlite3_finalize(stmt);
    return out;
}

HRRecord HRService::getHRById(i64 id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM hr_staff WHERE hr_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    { auto r = rowToHR(stmt); sqlite3_finalize(stmt); return r; }
    sqlite3_finalize(stmt);
    throw std::runtime_error("DB: HR staff not found");
}

HRRecord HRService::getHRByEmployeeId(i64 employee_id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM hr_staff WHERE employee_id = ?;");
    sqlite3_bind_int64(stmt, 1, employee_id);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    { auto r = rowToHR(stmt); sqlite3_finalize(stmt); return r; }
    sqlite3_finalize(stmt);
    throw std::runtime_error("DB: No HR staff found for that employee ID");
}

// ── Vacancy ───────────────────────────────────────────────────────────────────

void HRService::createVacancy(const VacancyRecord& v)
{
    sqlite3_stmt* stmt = prepare(
        "INSERT INTO vacancies (post, status) VALUES (?, 'OPEN');");
    sqlite3_bind_text(stmt, 1, v.post.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to create vacancy"); }
    sqlite3_finalize(stmt);
}

std::vector<VacancyRecord> HRService::getAllVacancies()
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM vacancies;");
    std::vector<VacancyRecord> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToVacancy(stmt));
    sqlite3_finalize(stmt);
    return out;
}

std::vector<VacancyRecord> HRService::getOpenVacancies()
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM vacancies WHERE status = 'OPEN';");
    std::vector<VacancyRecord> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToVacancy(stmt));
    sqlite3_finalize(stmt);
    return out;
}

void HRService::closeVacancy(i64 id)
{
    sqlite3_stmt* stmt = prepare("UPDATE vacancies SET status = 'CLOSED' WHERE vacancy_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to close vacancy"); }
    sqlite3_finalize(stmt);
}

void HRService::fillVacancy(i64 id)
{
    sqlite3_stmt* stmt = prepare("UPDATE vacancies SET status = 'FILLED' WHERE vacancy_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to fill vacancy"); }
    sqlite3_finalize(stmt);
}

// ── Applicant ─────────────────────────────────────────────────────────────────

void HRService::addApplicant(const ApplicantRecord& a)
{
    // Post must match an open vacancy
    auto open = getOpenVacancies();
    bool found = false;
    for (const auto& v : open)
        if (v.post == a.post) { found = true; break; }
    if (!found)
        throw std::runtime_error("No open vacancy for post: " + a.post);

    sqlite3_stmt* stmt = prepare(
        "INSERT INTO applicants (name, experience_years, post, status) VALUES (?, ?, ?, 'PENDING');");
    sqlite3_bind_text(stmt, 1, a.name.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 2, a.experience_years);
    sqlite3_bind_text(stmt, 3, a.post.c_str(),  -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to add applicant"); }
    sqlite3_finalize(stmt);
}

std::vector<ApplicantRecord> HRService::getAllApplicants()
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM applicants;");
    std::vector<ApplicantRecord> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToApplicant(stmt));
    sqlite3_finalize(stmt);
    return out;
}

ApplicantRecord HRService::getApplicantById(i64 id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM applicants WHERE applicant_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    { auto r = rowToApplicant(stmt); sqlite3_finalize(stmt); return r; }
    sqlite3_finalize(stmt);
    throw std::runtime_error("DB: Applicant not found");
}

void HRService::updateApplicantStatus(i64 id, ApplicantStatus s)
{
    sqlite3_stmt* stmt = prepare("UPDATE applicants SET status = ? WHERE applicant_id = ?;");
    sqlite3_bind_text (stmt, 1, hr::toStr(s), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, id);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to update applicant status"); }
    sqlite3_finalize(stmt);
}

// ── Hire ──────────────────────────────────────────────────────────────────────

EmployeeRecord HRService::hireApplicant(i64 applicant_id,
                                         f64 starting_salary,
                                         const std::string& join_date)
{
    ApplicantRecord a = getApplicantById(applicant_id);

    if (a.status == ApplicantStatus::REJECTED)
        throw std::runtime_error("Cannot hire a rejected applicant");

    // Check open vacancy for this post still exists
    auto open = getOpenVacancies();
    bool found = false;
    i64  vacancy_id = -1;
    for (const auto& v : open)
        if (v.post == a.post) { found = true; vacancy_id = v.vacancy_id; break; }
    if (!found)
        throw std::runtime_error("No open vacancy for post: " + a.post);

    // Mark applicant SELECTED and vacancy FILLED
    updateApplicantStatus(applicant_id, ApplicantStatus::SELECTED);
    fillVacancy(vacancy_id);

    // Build EmployeeRecord — caller saves via EmployeeService
    EmployeeRecord emp;
    emp.name       = a.name;
    emp.department = EmployeeDepartment::HR;   // default; caller can override
    emp.salary     = starting_salary;
    emp.joined_at  = join_date;
    emp.status     = EmployeeStatus::ACTIVE;
    return emp;
}

// ── Attendance ────────────────────────────────────────────────────────────────

void HRService::markAttendance(const AttendanceRecord& r)
{
    sqlite3_stmt* stmt = prepare(
        "INSERT INTO attendance (employee_id, date, status, note) VALUES (?, ?, ?, ?);");
    sqlite3_bind_int64(stmt, 1, r.employee_id);
    sqlite3_bind_text (stmt, 2, r.date.c_str(),          -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 3, hr::toStr(r.status),     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 4, r.note.c_str(),          -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    { sqlite3_finalize(stmt); throw std::runtime_error("DB: Failed to mark attendance"); }
    sqlite3_finalize(stmt);
}

// Called automatically when Manager approves a leave request
// Iterates every date in [from_date, to_date] and inserts GRANTED_LEAVE rows
void HRService::markGrantedLeave(i64 employee_id,
                                  const std::string& from_date,
                                  const std::string& to_date)
{
    // Simple date iteration using tm structs
    std::tm from_tm = {}, to_tm = {};
    std::istringstream f(from_date), t(to_date);
    f >> std::get_time(&from_tm, "%Y-%m-%d");
    t >> std::get_time(&to_tm,   "%Y-%m-%d");

    std::time_t from_t = std::mktime(&from_tm);
    std::time_t to_t   = std::mktime(&to_tm);

    std::string note = "Granted leave: " + from_date + " to " + to_date;

    for (std::time_t cur = from_t; cur <= to_t; cur += 86400)
    {
        std::tm* cur_tm = std::localtime(&cur);
        char buf[11];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", cur_tm);

        AttendanceRecord rec;
        rec.employee_id = employee_id;
        rec.date        = buf;
        rec.status      = AttendanceStatus::GRANTED_LEAVE;
        rec.note        = note;
        markAttendance(rec);
    }
}

std::vector<AttendanceRecord> HRService::getAttendanceByEmployee(i64 id)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM attendance WHERE employee_id = ?;");
    sqlite3_bind_int64(stmt, 1, id);
    std::vector<AttendanceRecord> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToAttendance(stmt));
    sqlite3_finalize(stmt);
    return out;
}

std::vector<AttendanceRecord> HRService::getAttendanceByDate(const std::string& date)
{
    sqlite3_stmt* stmt = prepare("SELECT * FROM attendance WHERE date = ?;");
    sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_TRANSIENT);
    std::vector<AttendanceRecord> out;
    while (sqlite3_step(stmt) == SQLITE_ROW) out.push_back(rowToAttendance(stmt));
    sqlite3_finalize(stmt);
    return out;
}
