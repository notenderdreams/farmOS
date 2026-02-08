#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "AttendenceAndShift.h"  // For AttendanceManagement

using namespace std;

// ------------------------ Enums ------------------------
enum class RoleType { HRManager, AttendanceOfficer, PayrollOfficer, RecruitmentOfficer };
enum class AttendanceStatus { Present, Absent, OnLeave };

// Forward declaration to avoid circular dependency
class Applicant;

// ------------------------ RequiredSkill ------------------------
class RequiredSkill {
public:
    float HSC_Result;
    float SSC_Result;
    int pastExperienceYears;
    string certification;

    RequiredSkill(float hsc = 0, float ssc = 0, int exp = 0, string cert = "");
    void display();
};

// ------------------------ HRRole (Base class) ------------------------
class HRRole {
protected:
    string name;
    RoleType role;

public:
    HRRole(string n, RoleType r);
    virtual void displayRole() = 0;

    string getName();
    RoleType getRole();

    bool canCreateVacancy();
    bool canPublishVacancy();
    bool canCloseVacancy();
    bool canApproveHire();
    bool canViewReports();

    bool canVerifyDocuments();
    bool canManageOnboarding();
    bool canScheduleTraining();

    bool canManagePayroll();
    bool canScreenApplicants();
    bool canShortlistApplicants();
};

// ------------------------ JobVacancy ------------------------
class JobVacancy {
public:
    string vacancyId;
    string jobTitle;
    string department;
    RequiredSkill skills;
    bool published;
    vector<Applicant*> shortlistedApplicants;

    JobVacancy(string id, string title, string dept, RequiredSkill reqSkill);
    void display();
};

// ------------------------ HiringOperations Interface ------------------------
class HiringOperations {
public:
    virtual void createVacancy(string id, string title, string dept,
                               float HSC_Result, float SSC_Result, int exp, string cert) = 0;
    virtual void publishVacancy(JobVacancy &vacancy) = 0;
    virtual void closeVacancy(JobVacancy &vacancy) = 0;
    virtual void verifyDocument(string docName) = 0;
    virtual void shortlistApplicant(vector<Applicant*>& applicants, JobVacancy& vacancy) = 0;
    virtual void startOnboarding(string employeeName) = 0;
    virtual void approveHire(JobVacancy &vacancy)= 0;
    virtual void addEmployee(Applicant* applicant) =0;
    virtual ~HiringOperations() {}
};

// ------------------------ Derived HR Roles ------------------------
class HRManager : public HRRole, public HiringOperations {
public:
    HRManager(string n);
    void displayRole() override;

    void createVacancy(string id, string title, string dept,
                       float HSC_Result, float SSC_Result, int exp, string cert) override;
    void publishVacancy(JobVacancy &vacancy) override;
    void closeVacancy(JobVacancy &vacancy) override;
    void approveHire(JobVacancy &vacancy) override;

    // Can't perform
    void verifyDocument(string docName) override {}
    void shortlistApplicant(vector<Applicant*>& applicants, JobVacancy& vacancy) override {}
    void startOnboarding(string employeeName) override {}
    void addEmployee(Applicant* applicant) override {}
};

class AttendanceOfficer : public HRRole, public HiringOperations {
private:
    string name;
    AttendanceManagement& attendanceSystem;

public:
    AttendanceOfficer(string n, AttendanceManagement& system);
    void displayRole() override;

    void verifyDocument(string docName) override;
    void GiveAttendance(const string& employeeId,
                        const string& date,
                        bool present);

    // Can't perform
    void createVacancy(string, string, string, float, float, int, string) override {}
    void publishVacancy(JobVacancy &) override {}
    void closeVacancy(JobVacancy &) override {}
    void approveHire(JobVacancy&) override {}
    void shortlistApplicant(vector<Applicant*>& applicants, JobVacancy& vacancy) override {}
    void addEmployee(Applicant* applicant) override {}
};

class RecruitmentOfficer : public HRRole, public HiringOperations {
public:
    RecruitmentOfficer(string n);
    void displayRole() override;

    void shortlistApplicant(vector<Applicant*>& applicants, JobVacancy& vacancy) override;
    void addEmployee(Applicant* applicant) override;

    // Can't perform
    void createVacancy(string, string, string, float, float, int, string) override {}
    void publishVacancy(JobVacancy &) override {}
    void closeVacancy(JobVacancy &) override {}
    void approveHire(JobVacancy&) override {}
    void verifyDocument(string) override {}
    void startOnboarding(string) override {}
};
