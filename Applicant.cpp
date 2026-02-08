#include "Applicant.h"
#include <iostream>
using namespace std;

Applicant::Applicant(string name, string vId, string jTitle, string dept,
                     RequiredSkill reqSkill, EmploymentType empType)
    : applicantName(name), vacancyId(vId), jobTitle(jTitle), department(dept),
      skills(reqSkill), employmentType(empType), status(ApplicantStatus::Inactive) {}

void Applicant::setStatus(ApplicantStatus newStatus) {
    status = newStatus;
}

Applicant* Applicant::applyForVacancy(string name, string vId, string jTitle,
                                      string dept, RequiredSkill reqSkill,
                                      EmploymentType empType, JobVacancy &vacancy) {
    if (vId != vacancy.vacancyId || jTitle != vacancy.jobTitle || !vacancy.published) {
        cout << "Application rejected: Vacancy ID or Job Title mismatch, or vacancy not published.\n";
        return nullptr;
    }
    return new Applicant(name, vId, jTitle, dept, reqSkill, empType);
}
string Applicant::getName() { return applicantName; }
string Applicant::getVacancyId() { return vacancyId; }
RequiredSkill Applicant::getSkills() const { return skills; }
ApplicantStatus Applicant::getStatus() { return status; }
string Applicant:: getJobTitle() const { return jobTitle; }

void Applicant::display() {
    cout << "Applicant Name: " << applicantName
         << " | Vacancy ID: " << vacancyId
         << " | Job Title: " << jobTitle
         << " | Department: " << department
         << " | Employment Type: "
         << (employmentType == EmploymentType::FullTime ? "FullTime" :
             employmentType == EmploymentType::PartTime ? "PartTime" : "Contract")
         << " | Status: "
         << (status == ApplicantStatus::Active ? "Active" :
             status == ApplicantStatus::Inactive ? "Inactive" : "Rejected") << endl;
    skills.display();
}
