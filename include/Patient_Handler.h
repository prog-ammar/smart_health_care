#pragma once

#include<iostream>
#include<string>
#include<map>
#include<sstream>
#include<vector>
#include<fstream>
#include<ctime>
#include<queue>
#include<functional>

using namespace std;

struct Patient_Symptoms
{
    int bloodPressure;
    int heartRate;
    int oxygenLevel;
    int temperature;
    int respiratoryRate;
    int consciousness;
    int bleeding;
    int pain;
    int chestPain;
    int nausea;

    Patient_Symptoms()
    {
        bloodPressure = heartRate = oxygenLevel = temperature = respiratoryRate =
        consciousness = bleeding = pain = chestPain = nausea = 0;
    }
};

struct Patient
{
    string name;
    int age;
    string blood;
    string gender;
    string contact_number;
    string department;
    string id;

    Patient()
    {
        name = blood = gender = contact_number = department = id = "";
        age = -1;
    }

    Patient(string name,int age,string blood,string gender,string contact_number,string department)
    {
        this->name=name;
        this->age=age;
        this->blood=blood;
        this->gender=gender;
        this->contact_number=contact_number;
        this->department=department;
    }

    void set_id(string id)
    {
        this->id=id;
    }
};

struct OPD_Patient : public Patient 
{ 
    OPD_Patient(string name,int age,string blood,string gender,string contact_number,string department) : Patient(name,age,blood,gender,contact_number,department) {}
    OPD_Patient() : Patient() {}
};

struct Emergency_Patient : public Patient
{

    string relative_number;
    int sensitivity;
    Patient_Symptoms symptoms;
    int arrivalIndex; 

    Emergency_Patient() 
    { 
        sensitivity = -1; relative_number = ""; arrivalIndex = -1; 
    }

    Emergency_Patient(string name,int age,string blood,string gender,string contact_number,string department,string rel_no,int sens,int ar_in) : Patient(name,age,blood,gender,contact_number,department) 
    {
        this->sensitivity=sens;
        this->arrivalIndex=ar_in;
        this->relative_number=rel_no;
    }
};

struct compare_sensitivity
{
    bool operator()(Emergency_Patient const& p1, Emergency_Patient const& p2)
    {
        if (p1.sensitivity == p2.sensitivity)
        {
            return p1.arrivalIndex > p2.arrivalIndex; 
        }
        else
        {
            return p1.sensitivity < p2.sensitivity; 
        } 
    }
};


class Patient_Handler
{
private:
    queue<OPD_Patient> opd_queue;
    map<string, OPD_Patient> opd_patients;
    priority_queue<Emergency_Patient, vector<Emergency_Patient>, compare_sensitivity> emergency_queue;
    map<string, Emergency_Patient> emergency_patients;
    string opd_file;
    string em_file;
    string curr_opd_file;
    string curr_em_file;
    int emergencyIndex = 0; 
public:
    Patient_Handler(string opd_file="",string em_file="",string curr_opd_file="",string curr_em_file="");
    void set_file(string opd_file,string em_file,string curr_opd_file,string curr_em_file);
    void load_OPD();
    void load_Emergency();
    string add_OPD_patient(const OPD_Patient& p);
    string add_Emergency_patient(Emergency_Patient p);
    void write_OPD_file();
    void write_Emergency_file();
    string treat_OPD();
    string treat_Emergency();
    string get_lastest_opd_id();
    string get_latest_em_id();
    string remove_patient(const string& p_id);
    OPD_Patient accessOPDByID(string p_id);
    Emergency_Patient accessEmByID(string p_id);
    vector<OPD_Patient> return_all_opd_patients();
    vector<Emergency_Patient> return_all_emergency_patients();
    vector<OPD_Patient> get_opd_queue();
    vector<Emergency_Patient> get_emergency_queue();
    void load_active_queues();
    void save_active_queues();

    
};