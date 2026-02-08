#include <iostream>
#include <string>
using namespace std;
enum class RoleType {
    HRManager,
    HROfficer,
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

    bool canCreateVacancy() { return role == RoleType::HRManager; }
    bool canCloseVacancy() { return role == RoleType::HRManager; }
    bool canApproveHire() { return role == RoleType::HRManager; }
    bool canViewReports() { return role == RoleType::HRManager; }
    bool canVerifyDocuments() { return role == RoleType::HROfficer; }
    bool canManageOnboarding() { return role == RoleType::HROfficer; }
    bool canScheduleTraining() { return role == RoleType::HROfficer; }
    bool canManagePayroll() { return role == RoleType::PayrollOfficer; }
    bool canScreenApplicants() { return role == RoleType::RecruitmentOfficer; }
    bool canShortlistApplicants() { return role == RoleType::RecruitmentOfficer; }
};

class JobVacancy {
public:
    string vacancyId;
    string jobTitle;
    string department;
    RequiredSkill skills; 
    bool published;

    JobVacancy(string id, string title, string dept, RequiredSkill reqSkill)
        : vacancyId(id), jobTitle(title), department(dept), skills(reqSkill), published(false) {}

    void display() {
        cout << "Vacancy ID: " << vacancyId
             << " | Job: " << jobTitle
             << " | Dept: " << department
             << " | Published: " << (published ? "Yes" : "No") << endl;
        skills.display(); 
    }
};


class HRManager : public HRRole {
public:
    HRManager(string n) : HRRole(n, RoleType::HRManager) {}

    void displayRole() override {
        cout << name << " is an HR Manager." << endl;
    }

    void createJobVacancy(string id, string title, string dept,
                      float HSC_Result, float SSC_Result,
                      int pastExperienceYears, string certification)
    {
            if (!canCreateVacancy()) {
                cout << "Access Denied: Cannot create job vacancy!" << endl;
                return;
            }
            RequiredSkill reqSkill(HSC_Result, SSC_Result, pastExperienceYears, certification);
            JobVacancy vacancy(id, title, dept, reqSkill);
            cout << name << " created a new job vacancy:" << endl;
            vacancy.display();
    }

    void closeVacancy(JobVacancy &vacancy) {
        if (!canCloseVacancy()) {
            cout << "Access Denied: Cannot close vacancy!" << endl;
            return;
        }
        vacancy.published = false;
        cout << name << " closed vacancy: " << vacancy.jobTitle << endl;
    }

    void approveHire(string employeeName) {
        if (!canApproveHire()) {
            cout << "Access Denied: Cannot approve hire!" << endl;
            return;
        }
        cout << name << " approved the hiring of " << employeeName << endl;
    }

    void viewReports() {
        if (!canViewReports()) {
            cout << "Access Denied: Cannot view reports!" << endl;
            return;
        }
        cout << name << " is viewing HR reports." << endl;
    }
};

class HROfficer : public HRRole {
public:
    HROfficer(string n) : HRRole(n, RoleType::HROfficer) {}

    void displayRole() override {
        cout << name << " is an HR Officer." << endl;
    }

    void verifyDocument(string docName) {
        if (!canVerifyDocuments()) {
            cout << "Access Denied: Cannot verify documents!" << endl;
            return;
        }
        cout << name << " verified document: " << docName << endl;
    }

    void startOnboarding(string employeeName) {
        if (!canManageOnboarding()) {
            cout << "Access Denied: Cannot manage onboarding!" << endl;
            return;
        }
        cout << name << " started onboarding for " << employeeName << endl;
    }

    void scheduleTraining(string trainingName) {
        if (!canScheduleTraining()) {
            cout << "Access Denied: Cannot schedule training!" << endl;
            return;
        }
        cout << name << " scheduled training: " << trainingName << endl;
    }
};

class PayrollOfficer : public HRRole {
public:
    PayrollOfficer(string n) : HRRole(n, RoleType::PayrollOfficer) {}

    void displayRole() override {
        cout << name << " is a Payroll Officer." << endl;
    }

    void processSalary(string employeeName, double amount) {
        if (!canManagePayroll()) {
            cout << "Access Denied: Cannot manage payroll!" << endl;
            return;
        }
        cout << name << " processed salary $" << amount << " for " << employeeName << endl;
    }
};

class RecruitmentOfficer : public HRRole {
public:
    RecruitmentOfficer(string n) : HRRole(n, RoleType::RecruitmentOfficer) {}

    void displayRole() override {
        cout << name << " is a Recruitment Officer." << endl;
    }

    void screenApplicant(string applicantName) {
        if (!canScreenApplicants()) {
            cout << "Access Denied: Cannot screen applicants!" << endl;
            return;
        }
        cout << name << " screened applicant: " << applicantName << endl;
    }

    void shortlistApplicant(string applicantName) {
        if (!canShortlistApplicants()) {
            cout << "Access Denied: Cannot shortlist applicants!" << endl;
            return;
        }
        cout << name << " shortlisted applicant: " << applicantName << endl;
    }
};

