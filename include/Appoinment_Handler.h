#pragma once

#include<string>
#include<map>
#include<sstream>
#include<vector>
#include<fstream>
#include<ctime>
#include "Doctor_Handler.h"
#include "Patient_Handler.h"

using namespace std;


struct Appointment {
    string appointment_id;
    string patient_id;
    string doctor_id;
    string appointment_date;
    string appointment_time;
    string reason_for_visit;
    string status;

    Appointment() : status("Scheduled") {}
};

class Appointment_handler {
private:
    map<string, Appointment> appointments;
    Doctor_handler* doc_handler;
    Patient_Handler* patient_handler;
    string filename;
    string get_current_datetime();
public:
    Appointment_handler(Doctor_handler* dh = nullptr,Patient_Handler* ph=nullptr,string filename="");
    void set_doctor_handler(Doctor_handler* dh);
    void set_patient_handler(Patient_Handler* ph);
    void set_file(string filename);
    void read_from_file();
    void write_to_file();
    string schedule_appointment(const string& appointment_id, const string& patient_id, const string& doctor_id, const string& reason_for_visit);
    string complete_appoinment(const string& appoinment_id);
    string cancel_appointment(const string& appointment_id);
    string get_status(const string& appointment_id);
    void print_all_appointments();
    string get_latest_app_id();

    std::vector<Appointment> get_all_appointments_vec() {
    std::vector<Appointment> a_list;
    for(auto const& [id, appt] : appointments) a_list.push_back(appt);
    return a_list;
}
};