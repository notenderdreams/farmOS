#pragma once
#include "cli/cli.h"

// ── HR staff ──────────────────────────────────────────────────────────────────
int hrStaffAdd  (const Args& args);   // add HR staff member
int hrStaffList (const Args& args);   // list all HR staff

// ── Manager: vacancy ──────────────────────────────────────────────────────────
int vacancyCreate (const Args& args);
int vacancyList   (const Args& args);
int vacancyClose  (const Args& args);

// ── Manager: leave / raise decisions ─────────────────────────────────────────
int managerLeaveDecide (const Args& args);
int managerRaiseDecide (const Args& args);

// ── Manager: employee delete ──────────────────────────────────────────────────
int managerDeleteEmployee (const Args& args);  // delete <employee_id>

// ── Applicant ─────────────────────────────────────────────────────────────────
int applicantApply  (const Args& args);  // apply (name, experience, post)
int applicantList   (const Args& args);  // list all applicants

// ── HiringOfficer: hire ───────────────────────────────────────────────────────
int hiringOfficerHire   (const Args& args);  // hire <applicant_id>
int hiringOfficerReject (const Args& args);  // reject <applicant_id>

// ── AttendanceOfficer: attendance ─────────────────────────────────────────────
int attendanceMark       (const Args& args);  // mark <employee_id>
int attendanceByEmployee (const Args& args);  // show <employee_id>
int attendanceByDate     (const Args& args);  // show <date>
