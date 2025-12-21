#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <ctime>

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
};

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

class Doctor_handler {
private:
    map<string, Doctor> doctors;

public:
    Doctor getdoctorbyid(const string& doctor_id) {
        if (doctors.find(doctor_id) != doctors.end()) {
            return doctors[doctor_id];
        }
        return Doctor();
    }

    bool check_available(const string& doctor_id) {
        if (doctors.find(doctor_id) != doctors.end()) {
            return doctors[doctor_id].is_available;
        }
        return false;
    }

    string add_doctor(const string& doctor_id, const string& first_name, const string& last_name, const string& specialization, const string& phone_number, int years_experience, const string& hospital_branch, const string& email) {
        if (doctors.find(doctor_id) != doctors.end()) {
            return "Error: Doctor with ID " + doctor_id + " already exists";
        }

        Doctor new_doctor;
        new_doctor.doctor_id = doctor_id;
        new_doctor.first_name = first_name;
        new_doctor.last_name = last_name;
        new_doctor.specialization = specialization;
        new_doctor.phone_number = phone_number;
        new_doctor.years_experience = years_experience;
        new_doctor.hospital_branch = hospital_branch;
        new_doctor.email = email;
        new_doctor.is_available = true;

        doctors[doctor_id] = new_doctor;
        return "Doctor " + first_name + " " + last_name + " added successfully";
    }

    string remove_doctor(const string& doctor_id) {
        if (doctors.find(doctor_id) == doctors.end()) {
            return "Error: Doctor with ID " + doctor_id + " not found";
        }

        string name = doctors[doctor_id].first_name + " " + doctors[doctor_id].last_name;
        doctors.erase(doctor_id);
        return "Doctor " + name + " removed successfully";
    }

    string toggle_availability(const string& doctor_id) {
        if (doctors.find(doctor_id) == doctors.end()) {
            return "Error: Doctor with ID " + doctor_id + " not found";
        }

        doctors[doctor_id].is_available = !doctors[doctor_id].is_available;

        string status = doctors[doctor_id].is_available ? "available" : "unavailable";
        return "Doctor " + doctors[doctor_id].first_name + " " + doctors[doctor_id].last_name + " is now " + status;
    }

    string get_doctor_info(const string& doctor_id) {
        if (doctors.find(doctor_id) == doctors.end()) {
            return "Doctor not found";
        }

        Doctor doc = doctors[doctor_id];
        string status = doc.is_available ? "Available" : "Not Available";
        return "Doctor: " + doc.first_name + " " + doc.last_name + " - Status: " + status;
    }

    string get_doctor_by_speciality(const string& specialization) {
        stringstream result;
        bool found = false;

        for (const auto& pair : doctors) {
            const Doctor& doct = pair.second;
            if (doct.specialization == specialization) {
                found = true;
                result << "ID: " << doct.doctor_id << " Name: " << doct.first_name << " " << doct.last_name
                    << " Branch: " << doct.hospital_branch << " Available: " << (doct.is_available ? "Yes" : "No") << "\n";
            }
        }

        if (!found) {
            return "No doctors found with specialization: " + specialization;
        }
        return result.str();
    }
};

class Appointment_handler {
private:
    map<string, Appointment> appointments;
    Doctor_handler* doc_handler;

    string get_current_datetime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);

        stringstream ss;
        ss << (1900 + ltm->tm_year) << "-"
            << (1 + ltm->tm_mon) << "-"
            << ltm->tm_mday << " "
            << ltm->tm_hour << ":"
            << ltm->tm_min << ":"
            << ltm->tm_sec;

        return ss.str();
    }

public:
    Appointment_handler(Doctor_handler* dh = nullptr) : doc_handler(dh) {}

    void set_doctor_handler(Doctor_handler* dh) {
        doc_handler = dh;
    }

    string schedule_appointment(const string& appointment_id, const string& patient_id, const string& doctor_id, const string& reason_for_visit) {
        if (appointments.find(appointment_id) != appointments.end()) {
            return "Error: Appointment with ID " + appointment_id + " already exists";
        }

        if (doc_handler != nullptr) {
            if (!doc_handler->check_available(doctor_id)) {
                Doctor doc = doc_handler->getdoctorbyid(doctor_id);
                if (doc.doctor_id.empty()) {
                    return "Error: Doctor with ID " + doctor_id + " not found";
                }
                return "Error: Doctor " + doc.first_name + " " + doc.last_name + " is not available";
            }
        }

        string datetime = get_current_datetime();

        Appointment new_appointment;
        new_appointment.appointment_id = appointment_id;
        new_appointment.patient_id = patient_id;
        new_appointment.doctor_id = doctor_id;
        new_appointment.appointment_date = datetime;
        new_appointment.appointment_time = datetime;
        new_appointment.reason_for_visit = reason_for_visit;
        new_appointment.status = "Scheduled";

        appointments[appointment_id] = new_appointment;
        return "Appointment " + appointment_id + " scheduled successfully for " + datetime;
    }

    string cancel_appointment(const string& appointment_id) {
        if (appointments.find(appointment_id) == appointments.end()) {
            return "Error: Appointment with ID " + appointment_id + " not found";
        }

        if (appointments[appointment_id].status == "Cancelled") {
            return "Appointment " + appointment_id + " is already cancelled";
        }

        appointments[appointment_id].status = "Cancelled";
        return "Appointment " + appointment_id + " has been cancelled successfully";
    }

    string get_status(const string& appointment_id) {
        if (appointments.find(appointment_id) == appointments.end()) {
            return "Error: Appointment with ID " + appointment_id + " not found";
        }
        return "Appointment " + appointment_id + " status: " + appointments[appointment_id].status;
    }
};

// int main() {
//     Doctor_handler doc_manager;
//     Appointment_handler appt_manager(&doc_manager);

//     cout << "--- Testing Doctors ---" << endl;
//     cout << doc_manager.add_doctor("D101", "Alice", "Brown", "Cardiology", "555-0123", 12, "Downtown", "alice.b@hospital.com") << endl;
//     cout << doc_manager.add_doctor("D102", "Bob", "Wilson", "Neurology", "555-0456", 8, "Uptown", "bob.w@hospital.com") << endl;

//     cout << "\nSearching for Cardiology:" << endl;
//     cout << doc_manager.get_doctor_by_speciality("Cardiology") << endl;

//     cout << "Toggling availability for D101:" << endl;
//     cout << doc_manager.toggle_availability("D101") << endl;
//     cout << doc_manager.get_doctor_info("D101") << endl;

//     cout << "\n--- Testing Appointments ---" << endl;
//     cout << appt_manager.schedule_appointment("A001", "P500", "D102", "Routine Checkup") << endl;

//     cout << appt_manager.get_status("A001") << endl;

//     cout << appt_manager.cancel_appointment("A001") << endl;
//     cout << appt_manager.get_status("A001") << endl;

//     cout << "\n--- Testing Error Cases ---" << endl;
//     cout << doc_manager.remove_doctor("D999") << endl;
//     cout << appt_manager.get_status("A999") << endl;

//     return 0;
// }