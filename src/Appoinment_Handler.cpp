
#include "Appoinment_Handler.h"



string Appointment_handler::get_current_datetime() {
    time_t now = time(nullptr);
    tm ltm;
    localtime_s(&ltm, &now);
    stringstream ss;
    ss << (1900 + ltm.tm_year) << "-"
        << (1 + ltm.tm_mon) << "-"
        << ltm.tm_mday << " "
        << ltm.tm_hour << ":"
        << ltm.tm_min << ":"
        << ltm.tm_sec;
    return ss.str();
}

Appointment_handler::Appointment_handler(Doctor_handler* dh,Patient_Handler* ph,string filename) : doc_handler(dh),patient_handler(ph),filename(filename) {}

void Appointment_handler::set_doctor_handler(Doctor_handler* dh) {
    doc_handler = dh;
}


void Appointment_handler::set_patient_handler(Patient_Handler* ph)
    {
        patient_handler= ph;
    }

void Appointment_handler::set_file(string filename)
{
    this->filename=filename;
}

void Appointment_handler::read_from_file()
{
    ifstream file(filename);
    string wholeline;
    int row = 0;
    Appointment temp;
    while (getline(file, wholeline))
    {
        if (row == 0)
        {
            row++;
            continue;
        }
        stringstream line(wholeline);
        getline(line, temp.appointment_id, ',');
        getline(line, temp.patient_id, ',');
        getline(line, temp.doctor_id, ',');
        getline(line, temp.appointment_date, ',');
        getline(line, temp.appointment_time, ',');
        getline(line, temp.reason_for_visit, ',');
        getline(line, temp.status, ',');
        
  
        appointments[temp.appointment_id] = temp;
    }
    file.close();
}

void Appointment_handler::write_to_file()
{
    ofstream file(filename);
    file << "AppointmentID,PatientID,DoctorID,AppointmentDate,AppointmentTime,ReasonForVisit,Status" << "\n";
    for (const auto& pair : appointments)
    {
        const Appointment& appt = pair.second;
        file << appt.appointment_id << "," << appt.patient_id << "," << appt.doctor_id << "," << appt.appointment_date << "," << appt.appointment_time << "," << appt.reason_for_visit << "," << appt.status << "\n";
    }
    file.close();
}

string Appointment_handler::schedule_appointment(const string& appointment_id, const string& patient_id, const string& doctor_id, const string& reason_for_visit) {
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
    if(patient_handler!=nullptr)
    {
        if(patient_handler->accessOPDByID(patient_id).id=="")
        return "Error : Patient Dont Exists";
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
    doc_handler->toggle_availability(appointments[appointment_id].doctor_id);
	fstream file(filename, ios::app);
	file << appointment_id << "," << patient_id << "," << doctor_id << "," << datetime << "," << datetime << "," << reason_for_visit << "," << "Scheduled" << "\n";
	file.close();
    return "Appointment " + appointment_id + " scheduled successfully for " + datetime;
}


string Appointment_handler::complete_appoinment(const string& appointment_id)
{
    if (appointments.find(appointment_id) == appointments.end()) {
        return "Error: Appointment with ID " + appointment_id + " not found";
    }
    if (appointments[appointment_id].status == "Compelted") {
        return "Appointment " + appointment_id + " is already completed";
    }

    doc_handler->toggle_availability(appointments[appointment_id].doctor_id);
    appointments[appointment_id].status = "Completed";
	write_to_file();
    return "Appointment " + appointment_id + " has been completed successfully";
}

string Appointment_handler::cancel_appointment(const string& appointment_id) {
    if (appointments.find(appointment_id) == appointments.end()) {
        return "Error: Appointment with ID " + appointment_id + " not found";
    }
    if (appointments[appointment_id].status == "Cancelled") {
        return "Appointment " + appointment_id + " is already cancelled";
    }
    doc_handler->toggle_availability(appointments[appointment_id].doctor_id);
    appointments[appointment_id].status = "Cancelled";
	write_to_file();
    return "Appointment " + appointment_id + " has been cancelled successfully";
}

string Appointment_handler::get_status(const string& appointment_id) {
    if (appointments.find(appointment_id) == appointments.end()) {
        return "Error: Appointment with ID " + appointment_id + " not found";
    }
    return "Appointment " + appointment_id + " status: " + appointments[appointment_id].status;
}

void Appointment_handler::print_all_appointments() {
    for (const auto& pair : appointments) {
        const Appointment& appt = pair.second;
        cout << "Appointment ID: " << appt.appointment_id << ", Patient ID: " << appt.patient_id
            << ", Doctor ID: " << appt.doctor_id << ", Date: " << appt.appointment_date
            << ", Time: " << appt.appointment_time << ", Status: " << appt.status << endl;
    }
}


string Appointment_handler::get_latest_app_id()
{
    if(appointments.empty())
    return "A0";
    return appointments.rbegin()->first;
}