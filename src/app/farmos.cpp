#include <iostream>
#include "cli.h"
#include "app_state.h"
#include "handle_tx.h"
#include "handle_animal.h"
#include "handle_employee.h"
#include "handle_hr.h"
#include "handle_accountant.h"
#include "cli/color.h"

int main(int argc, char** argv) {
    CLI cli("farmos");
    auto* state = new AppState("../../farm.db");
    cli.registerState(state);

    // ── Session login ─────────────────────────────────────────────────────────
    // Ask for HR ID at startup — load role from DB — enforce per command
    {
        auto* hr_svc = state->getHRService();
        if (hr_svc) {
            std::cout << color::BLUE << "\n  farmOS — HR Login\n" << color::RESET;
            std::cout << "  Enter your HR ID (or 0 to skip): ";
            i64 hr_id = 0;
            std::cin >> hr_id;
            std::cin.ignore();

            if (hr_id > 0) {
                try {
                    HRRecord staff = hr_svc->getHRById(hr_id);
                    state->setSession(staff.hr_id, staff.role);
                    std::cout << color::GREEN
                              << "  Logged in as: " << staff.name
                              << " [" << hr::toStr(staff.role) << "]"
                              << color::RESET << "\n\n";
                } catch (...) {
                    std::cout << color::YELLOW
                              << "  HR ID not found. Continuing without session.\n"
                              << color::RESET;
                }
            }
        }
    }

    // ── Transactions ──────────────────────────────────────────────────────────
    REG_MOD(cli, tx, "Transaction management") {
        ADD_CMD(add,    "Add a new transaction",     txAdd);
        ADD_CMD(list,   "List all transactions",     txList);
        ADD_CMD(show,   "Show transaction details",  txShow);
        ADD_CMD(status, "Update transaction status", txUpdateStatus);
    }

    // ── Animals ───────────────────────────────────────────────────────────────
    REG_MOD(cli, animal, "Animal management") {
        ADD_CMD(buy,    "Buy an animal",             animalBuy);
        ADD_CMD(sell,   "Sell an animal",            animalSell);
        ADD_CMD(list,   "List all animals",          animalList);
        ADD_CMD(show,   "Show animal details",       animalShow);
        ADD_CMD(status, "Update animal status",      animalUpdateStatus);
        ADD_CMD(delete, "Delete an animal record",   animalDelete);
    }

    // ── Employees ─────────────────────────────────────────────────────────────
    // Note: employees can only be added via hiring process (farmos hiring hire)
    REG_MOD(cli, employee, "Employee management") {
        ADD_CMD(list,      "List all employees",              employeeList);
        ADD_CMD(show,      "Show employee details <id>",      employeeShow);
        ADD_CMD(askleve,   "Ask for leave <employee_id>",     employeeAskLeave);
        ADD_CMD(leavelist, "List all leave requests",         employeeLeaveList);
        ADD_CMD(askraise,  "Ask for salary raise <emp_id>",   employeeAskRaise);
        ADD_CMD(raiselist, "List all raise requests",         employeeRaiseList);
    }

    // ── HR staff management ───────────────────────────────────────────────────
    REG_MOD(cli, hr, "HR staff management") {
        ADD_CMD(add,  "Add HR staff member",  hrStaffAdd);
        ADD_CMD(list, "List all HR staff",    hrStaffList);
    }

    // ── Manager actions ───────────────────────────────────────────────────────
    REG_MOD(cli, manager, "Manager actions") {
        ADD_CMD(vacancy,      "Create a job vacancy",                vacancyCreate);
        ADD_CMD(vacancylist,  "List all vacancies",                  vacancyList);
        ADD_CMD(vacancyclose, "Close a vacancy <vacancy_id>",        vacancyClose);
        ADD_CMD(leavedecide,  "Decide on leave request <req_id>",    managerLeaveDecide);
        ADD_CMD(raisedecide,  "Decide on raise request <req_id>",    managerRaiseDecide);
        ADD_CMD(deletemp,     "Delete an employee <employee_id>",    managerDeleteEmployee);
    }

    // ── Applicant actions ─────────────────────────────────────────────────────
    REG_MOD(cli, applicant, "Applicant actions") {
        ADD_CMD(apply,  "Apply for a job",         applicantApply);
        ADD_CMD(list,   "List all applicants",     applicantList);
    }

    // ── Hiring officer actions ────────────────────────────────────────────────
    REG_MOD(cli, hiring, "Hiring officer actions") {
        ADD_CMD(hire,   "Hire an applicant <applicant_id>",   hiringOfficerHire);
        ADD_CMD(reject, "Reject an applicant <applicant_id>", hiringOfficerReject);
    }

    // ── Attendance officer actions ────────────────────────────────────────────
    REG_MOD(cli, attendance, "Attendance officer actions") {
        ADD_CMD(mark,       "Mark attendance <employee_id>",        attendanceMark);
        ADD_CMD(byemployee, "View attendance <employee_id>",        attendanceByEmployee);
        ADD_CMD(bydate,     "View attendance by date <YYYY-MM-DD>", attendanceByDate);
    }

    // ── Accountant actions ────────────────────────────────────────────────────
    REG_MOD(cli, accountant, "Accountant actions") {
        ADD_CMD(salaries,     "View all employees + total payroll",       accountantSalaries);
        ADD_CMD(raisehistory, "View all approved raises",                 accountantRaiseHistory);
        ADD_CMD(payroll,      "Monthly payroll report <YYYY-MM>",         accountantPayroll);
        ADD_CMD(adjustsalary, "Adjust employee salary <employee_id>",     accountantAdjustSalary);
    }

    return cli.run(argc, argv);
}
