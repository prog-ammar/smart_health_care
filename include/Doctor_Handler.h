#pragma once
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;



struct Doctor {
    string doctor_id;
    string first_name;
    string last_name;
    string specialization;
    string phone_number;
    int years_experience;
    string hospital_branch;
    string email;
    bool is_available;

    Doctor() : years_experience(0), is_available(true) {}
    Doctor(string d_id,string f_n,string l_n,string specs,string no,int exp,string h_b,string email)
    {
        doctor_id=d_id;
        first_name=f_n;
        last_name=l_n;
        specialization=specs;
        phone_number=no;
        years_experience=exp;
        hospital_branch=h_b;
        this->email=email;
        is_available=true;
    }

    void set_id(string id)
    {
        this->doctor_id=id;
    }

    void set_avalibality(bool a)
    {
        this->is_available=a;
    }
};

class Doctor_handler {
private:
    map<string, Doctor> doctors;
	string filename;

public:
    Doctor_handler(string filename="");
    void set_file(string filename);
    Doctor getdoctorbyid(const string& doctor_id);
    void read_from_file();
    void write_to_file();
    bool check_available(const string& doctor_id);
    string add_doctor(Doctor d);
    string remove_doctor(const string& doctor_id);
    string toggle_availability(const string& doctor_id);
    string get_doctor_by_speciality(const string& specialization);
    string get_doctor_info(const string& doctor_id);
    void print_all_doctors();
    string get_latest_doc_id();

    std::vector<Doctor> get_all_doctors_vec() {
    std::vector<Doctor> d_list;
    for(auto const& [id, doc] : doctors) d_list.push_back(doc);
    return d_list;
}

// In Appointment_handler class

};