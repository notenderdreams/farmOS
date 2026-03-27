#pragma once
#include "cli/cli.h"

// All commands require ACCOUNTANT role

int accountantSalaries    (const Args& args);  // view all employees + total payroll
int accountantRaiseHistory(const Args& args);  // view all approved raises
int accountantPayroll     (const Args& args);  // payroll report <YYYY-MM>
int accountantAdjustSalary(const Args& args);  // adjust salary <employee_id> — only if manager approved a raise
