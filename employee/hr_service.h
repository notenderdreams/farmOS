#pragma once
#include "core/database.h"
#include "core/models/hr.h"
#include "core/models/employee.h"
#include <vector>
#include <functional>
#include <functional>

class HRService : public Database
{
public:
    HRService(const std::string& db_path);
    HRService(sqlite3* db);

    void initTables();

    // ── HR staff ──────────────────────────────────────────────────────────────
    void                   addHRStaff        (const HRRecord& r);
    std::vector<HRRecord>  getAllHRStaff     ();
    HRRecord               getHRById        (i64 hr_id);
    HRRecord               getHRByEmployeeId(i64 employee_id);  // ← new

    // ── Vacancy (Manager only) ────────────────────────────────────────────────
    void                       createVacancy  (const VacancyRecord& v);
    std::vector<VacancyRecord> getAllVacancies ();
    std::vector<VacancyRecord> getOpenVacancies();
    void                       closeVacancy   (i64 vacancy_id);
    void                       fillVacancy    (i64 vacancy_id);

    // ── Applicant ─────────────────────────────────────────────────────────────
    void                          addApplicant   (const ApplicantRecord& a);
    std::vector<ApplicantRecord>  getAllApplicants();
    ApplicantRecord               getApplicantById(i64 applicant_id);
    void                          updateApplicantStatus(i64 applicant_id, ApplicantStatus s);

    // ── Hire (HiringOfficer) ──────────────────────────────────────────────────
    // Validates vacancy is OPEN, marks applicant SELECTED, marks vacancy FILLED,
    // returns a ready EmployeeRecord for the caller to persist via EmployeeService
    EmployeeRecord hireApplicant(i64 applicant_id,
                                 f64 starting_salary,
                                 const std::string& join_date);

    // ── Attendance (AttendanceOfficer only) ───────────────────────────────────
    void                          markAttendance    (const AttendanceRecord& r);
    // Called automatically when Manager approves leave —
    // marks each date in range as GRANTED_LEAVE
    void                          markGrantedLeave  (i64 employee_id,
                                                     const std::string& from_date,
                                                     const std::string& to_date);
    std::vector<AttendanceRecord> getAttendanceByEmployee(i64 employee_id);
    std::vector<AttendanceRecord> getAttendanceByDate    (const std::string& date);
};
