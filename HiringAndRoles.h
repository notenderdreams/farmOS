#include <iostream>
#include <string>
#include<Applicant.h>
using namespace std;

enum class RoleType {
    HRManager,
    AttendanceOfficer,
    PayrollOfficer,
    RecruitmentOfficer
};

class RequiredSkill {
public:
    float HSC_Result;
    float SSC_Result;
    int pastExperienceYears;
    string certification;

    RequiredSkill(float hsc = 0, float ssc = 0, int exp = 0, string cert = "")
        : HSC_Result(hsc), SSC_Result(ssc), pastExperienceYears(exp), certification(cert) {}

    void display() {
        cout << "Required Skills/Criteria -> "
             << "HSC: " << HSC_Result << "%, "
             << "SSC: " << SSC_Result << "%, "
             << "Experience: " << pastExperienceYears << " yrs, "
             << "Certification: " << certification << endl;
    }
};

class HRRole {
protected:
    string name;
    RoleType role;

public:
    HRRole(string n, RoleType r) : name(n), role(r) {}
    virtual void displayRole() = 0;

    string getName() { return name; }
    RoleType getRole() { return role; }

    bool canCreateVacancy() { return role == RoleType::HRManager; }
    bool canPublishVacancy() { return role == RoleType::HRManager; }
    bool canCloseVacancy() { return role == RoleType::HRManager; }
    bool canApproveHire() { return role == RoleType::HRManager; }
    bool canViewReports() { return role == RoleType::HRManager; }

    bool canVerifyDocuments() { return role == RoleType::AttendanceOfficer; }
    bool canManageOnboarding() { return role == RoleType::AttendanceOfficer; }
    bool canScheduleTraining() { return role == RoleType::AttendanceOfficer; }

    bool canManagePayroll() { return role == RoleType::PayrollOfficer; }
    bool canScreenApplicants() { return role == RoleType::RecruitmentOfficer; }
    bool canShortlistApplicants() { return role == RoleType::RecruitmentOfficer; }
};

#include <vector>
class JobVacancy {
public:
    string vacancyId;
    string jobTitle;
    string department;
    RequiredSkill skills;
    bool published;

    vector<Applicant*> shortlistedApplicants;

    JobVacancy(string id, string title, string dept, RequiredSkill reqSkill)
        : vacancyId(id), jobTitle(title), department(dept), skills(reqSkill), published(false) {}

    void display() {
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
};


class HiringOperations {
public:
    virtual void createVacancy(string id, string title, string dept,
                               float HSC_Result, float SSC_Result, int exp, string cert) = 0;
    virtual void publishVacancy(JobVacancy &vacancy) = 0;
    virtual void closeVacancy(JobVacancy &vacancy) = 0;
    virtual void verifyDocument(string docName) = 0;
    virtual void shortlistApplicant(vector<Applicant*>&applicants,JobVacancy&vacancy) = 0;
    virtual void startOnboarding(string employeeName) = 0;
    virtual void approveHire(JobVacancy &vacancy)= 0;
    virtual void addEmployee(Applicant* applicant) =0;
    virtual ~HiringOperations() {}
};

class HRManager : public HRRole, public HiringOperations {
public:
    HRManager(string n) : HRRole(n, RoleType::HRManager) {}
    void displayRole() override { cout << name << " is an HR Manager.\n"; }

    void createVacancy(string id, string title, string dept,
                       float HSC_Result, float SSC_Result, int exp, string cert) override
    {
        if (!canCreateVacancy()) { cout << "Access Denied!\n"; return; }
        RequiredSkill reqSkill(HSC_Result, SSC_Result, exp, cert);
        JobVacancy vacancy(id, title, dept, reqSkill);
        cout << name << " created vacancy:\n";
        vacancy.display();
    }

    void publishVacancy(JobVacancy &vacancy) override
    {
        if (!canPublishVacancy()) { cout << "Access Denied!\n"; return; }
        vacancy.published = true;
        cout << name << " published vacancy: " << vacancy.jobTitle << endl;
    }

    void closeVacancy(JobVacancy &vacancy) override
    {
        if (!canCloseVacancy()) { cout << "Access Denied!\n"; return; }
        vacancy.published = false;
        cout << name << " closed vacancy: " << vacancy.jobTitle << endl;
    }

void approveHire(JobVacancy &vacancy) override {
    if (!canApproveHire()) {
        cout << "Access Denied: Cannot approve hire!\n";
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
             << " | Job Title: " << app->getJobTitle() 
             << " | Status: " << (app->getStatus() == ApplicantStatus::Inactive ? "Inactive" : "Unknown") 
             << endl;

        // Ask manager to approve or reject
        do {
            cout << "Approve this applicant? (a = approve / r = reject): ";
            cin >> choice;
            choice = tolower(choice);
        } while (choice != 'a' && choice != 'r');

        if (choice == 'a') {
            app->setStatus(ApplicantStatus::Active);
            cout << "Approved: " << app->getName() << " as Active.\n";
        } else {
            app->setStatus(ApplicantStatus::Rejected);
            cout << "Rejected: " << app->getName() << endl;
        }
    }

    cout << "All shortlisted applicants processed for vacancy: " << vacancy.jobTitle << endl;
}

//can't perform
    void verifyDocument(string docName) override {}
    void shortlistApplicant(vector<Applicant*>&applicants,JobVacancy&vacancy) override {};
    void startOnboarding(string employeeName) override {}
    void addEmployee(Applicant* applicant) override {}

};

class AttendanceOfficer : public HRRole, public HiringOperations {
public:
    AttendanceOfficer(string n) : HRRole(n, RoleType::AttendanceOfficer) {}
    void displayRole() override { cout << name << " is an HR Officer.\n"; }

    void verifyDocument(string docName) override
    {
        if (!canVerifyDocuments()) { cout << "Access Denied!\n"; return; }
        cout << name << " verified document: " << docName << endl;
    }

    void startOnboarding(string employeeName) override
    {
        if (!canManageOnboarding()) { cout << "Access Denied!\n"; return; }
        cout << name << " started onboarding for " << employeeName << endl;
    }
//can't perform
    void createVacancy(string, string, string, float, float, int, string) override {}
    void publishVacancy(JobVacancy &) override {}
    void closeVacancy(JobVacancy &) override {}
    void approveHire(JobVacancy&) override {}
    void shortlistApplicant(vector<Applicant*>& applicants,JobVacancy& vacancy) override {}
    void addEmployee(Applicant* applicant) override {}

};
class RecruitmentOfficer : public HRRole, public HiringOperations {
public:
    RecruitmentOfficer(string n) : HRRole(n, RoleType::RecruitmentOfficer) {}

    void displayRole() override {
        cout << name << " is a Recruitment Officer.\n";
    }

    void shortlistApplicant(vector<Applicant*>& applicants, JobVacancy& vacancy) override {
        if (!canShortlistApplicants()) { cout << "Access Denied!\n"; return; }

        vacancy.shortlistedApplicants.clear(); 

        cout << name << " is shortlisting applicants for job: " << vacancy.jobTitle << endl;

        for (auto& app : applicants) {
            if (app->getVacancyId() == vacancy.vacancyId &&
                app->getSkills().HSC_Result >= vacancy.skills.HSC_Result &&
                app->getSkills().SSC_Result >= vacancy.skills.SSC_Result &&
                app->getSkills().pastExperienceYears >= vacancy.skills.pastExperienceYears)
            {
                vacancy.shortlistedApplicants.push_back(app);
                cout << "Shortlisted: " << app->getName() 
                    << " for job: " << app->getJobTitle() << endl;
            }
        }
        if (vacancy.shortlistedApplicants.empty()) {
            cout << "No applicants matched the requirements.\n";
        }
    }

    void addEmployee(Applicant* applicant) override {
        if (!canShortlistApplicants()) { cout << "Access Denied!\n"; return; }

        if (applicant->getStatus() == ApplicantStatus::Active) {
            cout << "Applicant " << applicant->getName() 
                 << " added as Employee for role: " << applicant->getJobTitle() << endl;
        } else {
            cout << "Cannot add " << applicant->getName() 
                 << " as Employee. Status is not Active.\n";
        }
    }
//can't Perform
    void createVacancy(string, string, string, float, float, int, string) override {}
    void publishVacancy(JobVacancy &) override {}
    void closeVacancy(JobVacancy &) override {}
    void approveHire(JobVacancy&) override {}
    void verifyDocument(string) override {}
    void startOnboarding(string) override {}
};
