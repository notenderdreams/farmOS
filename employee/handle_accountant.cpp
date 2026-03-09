#include "handle_accountant.h"
#include "app_state.h"
#include "utils.h"
#include "cli/widgets.h"
#include "cli/color.h"
#include "core/employee_service.h"
#include "core/transaction_service.h"
#include "core/models/employee.h"
#include "core/models/transaction.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// ── Role enforcement ──────────────────────────────────────────────────────────

static bool requireAccountant(AppState* state)
{
    if (!state->hasSession()) {
        color::printError("No session. Login with your HR ID at startup.");
        return false;
    }
    if (!state->isRole(HRRole::ACCOUNTANT)) {
        color::printError("Access denied. This action requires role: ACCOUNTANT.");
        return false;
    }
    return true;
}

// ── Salary → Transaction helper ───────────────────────────────────────────────
//
// Creates a single EXPENSE transaction that represents a salary payment.
// ref_note  — shows up as the transaction description, e.g.
//             "Salary [YYYY-MM]: Alice (ID:3)"
// amount    — the salary value (positive; stored as EXPENSE internally)
//
// Returns true on success; prints an error and returns false on failure.

static bool recordSalaryTransaction(TransactionService* tx_svc,
                                    const std::string&  ref_note,
                                    f64                 amount)
{
    Transaction tx;
    tx.type        = TransactionType::EXPENSE;   // salary is a farm outflow
    tx.category    = "SALARY";
    tx.description = ref_note;
    tx.amount      = amount;
    tx.status      = TransactionStatus::COMPLETED;

    // Use today's date (CURRENT_TIMESTAMP handled by DB default,
    // but we populate date explicitly to match the payroll month context)
    tx.date = "";   // empty → service uses CURRENT_TIMESTAMP

    try {
        tx_svc->addTransaction(tx);
        return true;
    } catch (const std::exception& e) {
        color::printError(std::string("TX record failed: ") + e.what());
        return false;
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────────

static void printSalaryRow(const EmployeeRecord& e)
{
    std::cout << "  [" << std::setw(4) << e.employee_id << "] "
              << std::left << std::setw(24) << e.name
              << std::left << std::setw(14) << emp::toStr(e.department)
              << "$" << std::fixed << std::setprecision(2) << e.salary
              << "\n";
}

// ── Commands ──────────────────────────────────────────────────────────────────

// View all employees with current salary + total payroll
int accountantSalaries(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireAccountant(state)) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    try {
        auto employees = svc->getAllEmployees();
        if (employees.empty()) {
            std::cout << color::YELLOW << "No employees found" << color::RESET << "\n";
            return 0;
        }

        std::cout << color::BLUE << "\n  Salary Ledger\n" << color::RESET;
        color::printSeperator();
        std::cout << "  " << std::left << std::setw(6)  << "ID"
                          << std::left << std::setw(24) << "Name"
                          << std::left << std::setw(14) << "Department"
                          << "Salary\n";
        color::printSeperator();

        f64 total = 0.0;
        for (const auto& e : employees) {
            printSalaryRow(e);
            total += e.salary;
        }

        color::printSeperator();
        std::cout << color::GREEN
                  << "  Total Employees : " << employees.size()  << "\n"
                  << "  Total Payroll   : $"
                  << std::fixed << std::setprecision(2) << total
                  << color::RESET << "\n\n";

    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// View all approved raises with employee, amount, and date
int accountantRaiseHistory(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireAccountant(state)) return 1;
    auto* svc = state->getEmployeeService();
    if (!svc) { color::printError("Employee service unavailable"); return 1; }

    try {
        auto raises = svc->getApprovedRaises();
        if (raises.empty()) {
            std::cout << color::YELLOW << "No approved raises found" << color::RESET << "\n";
            return 0;
        }

        std::cout << color::BLUE << "\n  Approved Raise History\n" << color::RESET;
        color::printSeperator();

        f64 total_raised = 0.0;
        for (const auto& r : raises) {
            std::string emp_name = "Unknown";
            try {
                EmployeeRecord e = svc->getEmployeeById(r.employee_id);
                emp_name = e.name;
            } catch (...) {}

            std::cout << color::GREEN << "  Raise #" << r.request_id << color::RESET << "\n"
                      << "    Employee  : " << emp_name << " (ID: " << r.employee_id << ")\n"
                      << "    Previous  : $" << std::fixed << std::setprecision(2) << r.current_salary << "\n"
                      << "    Raised By : $" << r.raise_amount << "\n"
                      << "    New Salary: $" << (r.current_salary + r.raise_amount) << "\n"
                      << "    Reason    : " << r.reason << "\n"
                      << "    Date      : " << r.created_at << "\n";
            color::printSeperator();
            total_raised += r.raise_amount;
        }

        std::cout << color::GREEN
                  << "  Total Raises    : " << raises.size() << "\n"
                  << "  Total Amount    : $" << std::fixed << std::setprecision(2) << total_raised
                  << color::RESET << "\n\n";

    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// accountantPayroll  <YYYY-MM>
//
// 1. Lists every active employee on the payroll for the given month.
// 2. Asks the accountant to confirm disbursement.
// 3. On confirmation, records one EXPENSE transaction per employee
//    (category=SALARY, description="Salary [YYYY-MM]: <name> (ID:<id>)").
// 4. Prints a summary showing total transactions recorded.
// ─────────────────────────────────────────────────────────────────────────────
int accountantPayroll(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireAccountant(state)) return 1;

    auto* svc    = state->getEmployeeService();
    auto* tx_svc = state->getTransactionService();
    if (!svc)    { color::printError("Employee service unavailable");    return 1; }
    if (!tx_svc) { color::printError("Transaction service unavailable"); return 1; }

    std::string month;
    loadArg(month, 0, "YYYY-MM");

    if (month.size() != 7 || month[4] != '-') {
        color::printError("Invalid format. Use YYYY-MM (e.g. 2025-03)");
        return 1;
    }

    try {
        auto employees = svc->getPayrollForMonth(month);
        if (employees.empty()) {
            std::cout << color::YELLOW << "No active employees for " << month << color::RESET << "\n";
            return 0;
        }

        // ── Display report ────────────────────────────────────────────────────
        std::cout << color::BLUE << "\n  Payroll Report — " << month << "\n" << color::RESET;
        color::printSeperator();
        std::cout << "  " << std::left << std::setw(6)  << "ID"
                          << std::left << std::setw(24) << "Name"
                          << std::left << std::setw(14) << "Department"
                          << std::left << std::setw(12) << "Joined"
                          << "Salary\n";
        color::printSeperator();

        f64 total = 0.0;
        for (const auto& e : employees) {
            std::cout << "  [" << std::setw(4) << e.employee_id << "] "
                      << std::left << std::setw(24) << e.name
                      << std::left << std::setw(14) << emp::toStr(e.department)
                      << std::left << std::setw(12) << e.joined_at
                      << "$" << std::fixed << std::setprecision(2) << e.salary << "\n";
            total += e.salary;
        }

        color::printSeperator();
        std::cout << color::GREEN
                  << "  Employees on Payroll : " << employees.size() << "\n"
                  << "  Total Payroll Cost   : $"
                  << std::fixed << std::setprecision(2) << total
                  << color::RESET << "\n\n";

        // ── Confirm disbursement ──────────────────────────────────────────────
        std::cout << color::YELLOW
                  << "  Record salary disbursements as transactions? [y/N]: "
                  << color::RESET;
        std::string confirm;
        std::getline(std::cin, confirm);

        if (confirm != "y" && confirm != "Y") {
            std::cout << color::GREY << "  Disbursement cancelled — no transactions recorded.\n"
                      << color::RESET;
            return 0;
        }

        // ── Record one EXPENSE transaction per employee ────────────────────────
        int recorded = 0;
        int failed   = 0;
        for (const auto& e : employees) {
            // Build a clear, searchable description
            // e.g.  "Salary [2025-03]: Alice (ID:3)"
            std::ostringstream desc;
            desc << "Salary [" << month << "]: " << e.name
                 << " (ID:" << e.employee_id << ")";

            if (recordSalaryTransaction(tx_svc, desc.str(), e.salary))
                ++recorded;
            else
                ++failed;
        }

        color::printSeperator();
        std::cout << color::GREEN
                  << "  ✓ Transactions recorded : " << recorded << "\n"
                  << color::RESET;
        if (failed > 0)
            std::cout << color::YELLOW
                      << "  ⚠ Failed to record     : " << failed << "\n"
                      << color::RESET;
        std::cout << "  Tip: run 'farmos tx list' to verify.\n\n";

    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// accountantAdjustSalary  <employee_id>
//
// Same as before but now also records a SALARY_ADJUSTMENT transaction so the
// raise delta is visible in the transaction ledger.
// ─────────────────────────────────────────────────────────────────────────────
int accountantAdjustSalary(const Args& args)
{
    auto* state = getAppState(args);
    if (!state) return 1;
    if (!requireAccountant(state)) return 1;

    auto* svc    = state->getEmployeeService();
    auto* tx_svc = state->getTransactionService();
    if (!svc)    { color::printError("Employee service unavailable");    return 1; }
    if (!tx_svc) { color::printError("Transaction service unavailable"); return 1; }

    std::string id_str;
    loadArg(id_str, 0, "employee_id");

    try {
        i64 eid = std::stoll(id_str);
        EmployeeRecord e = svc->getEmployeeById(eid);

        // Must have at least one approved raise
        auto raises = svc->getRaiseByEmployee(eid);
        bool has_approved = false;
        f64  latest_approved_salary = 0.0;
        for (const auto& r : raises) {
            if (r.status == RaiseStatus::APPROVED) {
                has_approved = true;
                f64 new_sal = r.current_salary + r.raise_amount;
                if (new_sal > latest_approved_salary)
                    latest_approved_salary = new_sal;
            }
        }

        if (!has_approved) {
            color::printError("No approved raise found for this employee. Salary adjustment not permitted.");
            return 1;
        }

        std::cout << color::BLUE << "  Employee: " << e.name << color::RESET << "\n"
                  << "  Current Salary     : $" << std::fixed << std::setprecision(2) << e.salary << "\n"
                  << "  Latest Approved To : $" << latest_approved_salary << "\n";

        std::cout << "  New Salary to apply: $";
        f64 new_salary;
        std::cin >> new_salary;
        std::cin.ignore();

        if (new_salary <= 0) {
            color::printError("Salary must be greater than 0");
            return 1;
        }

        f64 delta = new_salary - e.salary;

        // ── Persist the new salary ────────────────────────────────────────────
        svc->updateSalary(eid, new_salary);
        std::cout << color::GREEN
                  << "  ✓ Salary updated to $"
                  << std::fixed << std::setprecision(2) << new_salary
                  << " for " << e.name
                  << color::RESET << "\n";

        // ── Record the raise delta as a transaction ───────────────────────────
        // The delta is the one-off adjustment amount (positive = increase).
        // We log it as EXPENSE so it appears in the ledger as a cost to the farm.
        if (delta != 0.0) {
            std::ostringstream desc;
            desc << "Salary adjustment: " << e.name
                 << " (ID:" << eid << ")"
                 << " | " << (delta > 0 ? "+" : "")
                 << std::fixed << std::setprecision(2) << delta
                 << " (approved raise)";

            Transaction tx;
            tx.type        = TransactionType::EXPENSE;
            tx.category    = "SALARY_ADJUSTMENT";
            tx.description = desc.str();
            tx.amount      = (delta > 0 ? delta : -delta);  // always positive amount
            tx.status      = TransactionStatus::COMPLETED;
            tx.date        = "";

            tx_svc->addTransaction(tx);
            std::cout << color::GREY
                      << "  → Transaction recorded (SALARY_ADJUSTMENT, $"
                      << std::fixed << std::setprecision(2) << std::abs(delta) << ")\n"
                      << color::RESET;
        }

    } catch (const std::exception& e) {
        color::printError(e.what()); return 1;
    }
    return 0;
}
