#ifndef APPLICANT_H
#define APPLICANT_H

#include <string>
#include "HR_Roles.h" // <-- include RequiredSkill here
using namespace std;

enum class ApplicantStatus { Inactive, Active, Rejected };
enum class EmploymentType { FullTime, PartTime, Contract };

class HRManager; // friend class declaration
class JobVacancy; // forward declaration is fine here

class Applicant {
private:
    string applicantName;
    string vacancyId;
    string jobTitle;
    string department;
    RequiredSkill skills;  // Now RequiredSkill is fully defined
    EmploymentType employmentType;
    ApplicantStatus status;

    Applicant(string name, string vId, string jTitle, string dept,
              RequiredSkill reqSkill, EmploymentType empType);

    void setStatus(ApplicantStatus newStatus); 

    friend class HRManager;  

public:
    static Applicant* applyForVacancy(string name, string vId, string jTitle,
                                      string dept, RequiredSkill reqSkill,
                                      EmploymentType empType, JobVacancy &vacancy);

    string getName();
    string getVacancyId();
    ApplicantStatus getStatus();
    RequiredSkill getSkills() const;
    string getJobTitle() const;
    void display();
};

#endif
