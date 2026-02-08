#include "HR_Roles.h"
#include "Applicant.h"   // Full definition needed to call methods
#include <iostream>
using namespace std;

// ------------------------ RequiredSkill ------------------------
RequiredSkill::RequiredSkill(float hsc, float ssc, int exp, string cert)
    : HSC_Result(hsc), SSC_Result(ssc), pastExperienceYears(exp), certification(cert) {}

void RequiredSkill::display() {
    cout << "Required Skills/Criteria -> "
         << "HSC: " << HSC_Result << "%, "
         << "SSC: " << SSC_Result << "%, "
         << "Experience: " << pastExperienceYears << " yrs, "
         << "Certification: " << certification << endl;
}

// ------------------------ HRRole ------------------------
HRRole::HRRole(string n, RoleType r) : name(n), role(r) {}
string HRRole::getName() { return name; }
RoleType HRRole::getRole() { return role; }

bool HRRole::canCreateVacancy() { return role == RoleType::HRManager; }
bool HRRole::canPublishVacancy() { return role == RoleType::HRManager; }
bool HRRole::canCloseVacancy() { return role == RoleType::HRManager; }
bool HRRole::canApproveHire() { return role == RoleType::HRManager; }
bool HRRole::canViewReports() { return role == RoleType::HRManager; }

bool HRRole::canVerifyDocuments() { return role == RoleType::AttendanceOfficer; }
bool HRRole::canManageOnboarding() { return role == RoleType::AttendanceOfficer; }
bool HRRole::canScheduleTraining() { return role == RoleType::AttendanceOfficer; }

bool HRRole::canManagePayroll() { return role == RoleType::PayrollOfficer; }
bool HRRole::canScreenApplicants() { return role == RoleType::RecruitmentOfficer; }
bool HRRole::canShortlistApplicants() { return role == RoleType::RecruitmentOfficer; }

// ------------------------ JobVacancy ------------------------
JobVacancy::JobVacancy(string id, string title, string dept, RequiredSkill reqSkill)
    : vacancyId(id), jobTitle(title), department(dept), skills(reqSkill), published(false) {}

void JobVacancy::display() {
    cout << "Vacancy ID: " << vacancyId
         << " | Job: " << jobTitle
         << " | Dept: " << department
         << " | Published: " << (published ? "Yes" : "No") << endl;
    skills.display();
    if (!shortlistedApplicants.empty()) {
        cout << "Shortlisted Applicants: ";
        for (auto& app : shortlistedApplicants)
            cout << app->getName() << " ";
        cout << endl;
    }
}

// ------------------------ HRManager ------------------------
HRManager::HRManager(string n) : HRRole(n, RoleType::HRManager) {}
void HRManager::displayRole() { cout << name << " is an HR Manager.\n"; }

// Implement createVacancy, publishVacancy, closeVacancy, approveHire here
// (Use the code you already wrote in your monolithic file)


// ------------------------ AttendanceOfficer ------------------------
AttendanceOfficer::AttendanceOfficer(string n, AttendanceManagement& system)
    : HRRole(n, RoleType::AttendanceOfficer), name(n), attendanceSystem(system) {}

void AttendanceOfficer::displayRole() { cout << name << " is an Attendance Officer.\n"; }

void AttendanceOfficer::verifyDocument(string docName) {
    if (!canVerifyDocuments()) { cout << "Access Denied!\n"; return; }
    cout << name << " verified document: " << docName << endl;
}

void AttendanceOfficer::GiveAttendance(const string& employeeId,
                                       const string& date,
                                       bool present) {
    AttendanceStatus status = present ? AttendanceStatus::Present : AttendanceStatus::Absent;
    attendanceSystem.addAttendance(employeeId, date, status);
    cout << name << " marked attendance for "
         << employeeId << " on " << date
         << " as " << (present ? "Present" : "Absent") << endl;
}

// ------------------------ RecruitmentOfficer ------------------------
RecruitmentOfficer::RecruitmentOfficer(string n) : HRRole(n, RoleType::RecruitmentOfficer) {}
void RecruitmentOfficer::displayRole() { cout << name << " is a Recruitment Officer.\n"; }

// ------------------------ JobVacancy ------------------------
JobVacancy::JobVacancy(string id, string title, string dept, RequiredSkill reqSkill)
    : vacancyId(id), jobTitle(title), department(dept), skills(reqSkill), published(false) {}

void JobVacancy::display() {
    cout << "Vacancy ID: " << vacancyId
         << " | Job: " << jobTitle
         << " | Dept: " << department
         << " | Published: " << (published ? "Yes" : "No") << endl;
    skills.display();
    if (!shortlistedApplicants.empty()) {
        cout << "Shortlisted Applicants: ";
        for (auto& app : shortlistedApplicants)
            cout << app->getName() << " ";
        cout << endl;
    }
}

// ------------------------ HRManager ------------------------
HRManager::HRManager(string n) : HRRole(n, RoleType::HRManager) {}
void HRManager::displayRole() { cout << name << " is an HR Manager.\n"; }

void HRManager::createVacancy(string id, string title, string dept,
                              float HSC_Result, float SSC_Result, int exp, string cert) {
    if (!canCreateVacancy()) { 
        cout << "Access Denied!\n"; 
        return; 
    }
    RequiredSkill reqSkill(HSC_Result, SSC_Result, exp, cert);
    JobVacancy vacancy(id, title, dept, reqSkill);
    cout << name << " created vacancy:\n";
    vacancy.display();
}

void HRManager::publishVacancy(JobVacancy &vacancy) {
    if (!canPublishVacancy()) { 
        cout << "Access Denied!\n"; 
        return; 
    }
    vacancy.published = true;
    cout << name << " published vacancy: " << vacancy.jobTitle << endl;
}

void HRManager::closeVacancy(JobVacancy &vacancy) {
    if (!canCloseVacancy()) { 
        cout << "Access Denied!\n"; 
        return; 
    }
    vacancy.published = false;
    cout << name << " closed vacancy: " << vacancy.jobTitle << endl;
}

void HRManager::approveHire(JobVacancy &vacancy) {
    if (!canApproveHire()) { 
        cout << "Access Denied!\n"; 
        return; 
    }

    if (vacancy.shortlistedApplicants.empty()) {
        cout << "No applicants to approve for vacancy: " << vacancy.jobTitle << endl;
        return;
    }

    cout << "Approving hires for vacancy: " << vacancy.jobTitle << endl;

    for (auto &app : vacancy.shortlistedApplicants) {
        char choice;
        cout << "Applicant: " << app->getName()
             << " | Job: " << app->getJobTitle() << endl;

        do {
            cout << "Approve (a) / Reject (r): ";
            cin >> choice;
            choice = tolower(choice);
        } while (choice != 'a' && choice != 'r');

        if (choice == 'a') {
            app->setStatus(ApplicantStatus::Active);
            cout << "Approved: " << app->getName() << endl;
        } else {
            app->setStatus(ApplicantStatus::Rejected);
            cout << "Rejected: " << app->getName() << endl;
        }
    }

    cout << "All shortlisted applicants processed for vacancy: " << vacancy.jobTitle << endl;
}

// ------------------------ AttendanceOfficer ------------------------
AttendanceOfficer::AttendanceOfficer(string n, AttendanceManagement& system)
    : HRRole(n, RoleType::AttendanceOfficer), name(n), attendanceSystem(system) {}

void AttendanceOfficer::displayRole() { cout << name << " is an Attendance Officer.\n"; }

void AttendanceOfficer::verifyDocument(string docName) {
    if (!canVerifyDocuments()) { 
        cout << "Access Denied!\n"; 
        return; 
    }
    cout << name << " verified document: " << docName << endl;
}

void AttendanceOfficer::GiveAttendance(const string& employeeId,
                                       const string& date,
                                       bool present) {
    AttendanceStatus status = present ? AttendanceStatus::Present : AttendanceStatus::Absent;
    attendanceSystem.addAttendance(employeeId, date, status);
    cout << name << " marked attendance for "
         << employeeId << " on " << date
         << " as " << (present ? "Present" : "Absent") << endl;
}

// ------------------------ RecruitmentOfficer ------------------------
RecruitmentOfficer::RecruitmentOfficer(string n) : HRRole(n, RoleType::RecruitmentOfficer) {}
void RecruitmentOfficer::displayRole() { cout << name << " is a Recruitment Officer.\n"; }

void RecruitmentOfficer::shortlistApplicant(vector<Applicant*>& applicants, JobVacancy& vacancy) {
    if (!canShortlistApplicants()) { 
        cout << "Access Denied!\n"; 
        return; 
    }

    vacancy.shortlistedApplicants.clear();
    for (auto& app : applicants) {
        if (app->getVacancyId() == vacancy.vacancyId &&
            app->getSkills().HSC_Result >= vacancy.skills.HSC_Result &&
            app->getSkills().SSC_Result >= vacancy.skills.SSC_Result &&
            app->getSkills().pastExperienceYears >= vacancy.skills.pastExperienceYears) {
            vacancy.shortlistedApplicants.push_back(app);
            cout << "Shortlisted: " << app->getName() << " for " << app->getJobTitle() << endl;
        }
    }

    if (vacancy.shortlistedApplicants.empty())
        cout << "No applicants matched the requirements.\n";
}

void RecruitmentOfficer::addEmployee(Applicant* applicant) {
    if (!canShortlistApplicants()) { 
        cout << "Access Denied!\n"; 
        return; 
    }

    if (applicant->getStatus() == ApplicantStatus::Active)
        cout << "Applicant " << applicant->getName() << " added as Employee for role: " << applicant->getJobTitle() << endl;
    else
        cout << "Cannot add " << applicant->getName() << " as Employee. Status is not Active.\n";
}

