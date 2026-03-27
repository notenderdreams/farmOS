#pragma once
#include "cli/cli.h"

// ── Employee commands 
// Note: employees are added only via hiring process (farmos hiring hire)
int employeeList   (const Args& args);
int employeeShow   (const Args& args);  // show <employee_id>

// ── Leave (employee asks) 
int employeeAskLeave  (const Args& args);  // <employee_id>
int employeeLeaveList (const Args& args);

// ── Raise (employee asks) 
int employeeAskRaise  (const Args& args);  // <employee_id>
int employeeRaiseList (const Args& args);
