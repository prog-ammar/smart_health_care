
#include "Doctor_Handler.h"

Doctor Doctor_handler::getdoctorbyid(const string& doctor_id) {
    if (doctors.find(doctor_id) != doctors.end()) {
        return doctors[doctor_id];
    }
    return Doctor();
}

void Doctor_handler::read_from_file()
{
    ifstream file(filename);
    string wholeline;
    int row = 0;
    Doctor temp;
    string years_exp_str;
	string available_str;
    while (getline(file, wholeline))
    {
        if (row == 0)
        {
            row++;
            continue;
        }

        stringstream line(wholeline);
        getline(line, temp.doctor_id, ',');
		getline(line, temp.first_name, ',');
		getline(line, temp.last_name, ',');
		getline(line, temp.specialization, ',');
		getline(line, temp.phone_number, ',');
		getline(line, years_exp_str, ',');
		getline(line, temp.hospital_branch, ',');
		getline(line, temp.email, ',');
		getline(line, available_str, ',');
		temp.years_experience = stoi(years_exp_str);
        temp.is_available = (available_str) == "1"?  true : false;
        row++;
        doctors[temp.doctor_id] = temp;
    }
    file.close();
}

void Doctor_handler::write_to_file()
{
    ofstream file(filename);
    file << "DoctorID,FirstName,LastName,Specialization,PhoneNumber,YearsExperience,HospitalBranch,Email,Avaliablity" << "\n";
    for (const auto& pair : doctors)
    {
		const Doctor& doc = pair.second;
        int available_flag = doc.is_available ? 1 : 0;
        file << doc.doctor_id << "," << doc.first_name << "," << doc.last_name << "," << doc.specialization << "," << doc.phone_number << "," << doc.years_experience << "," << doc.hospital_branch << "," << doc.email << "," << available_flag << "\n";
    }
    file.close();
}

bool Doctor_handler::check_available(const string& doctor_id) {
    if (doctors.find(doctor_id) != doctors.end()) {
        return doctors[doctor_id].is_available;
    }
    return false;
}

string Doctor_handler::add_doctor(Doctor new_doctor) {

    doctors[new_doctor.doctor_id] = new_doctor;

    fstream file(filename, ios::app);
	bool available_flag = new_doctor.is_available ? 1 : 0;
	file << new_doctor.doctor_id << "," << new_doctor.first_name << "," << new_doctor.last_name << "," << new_doctor.specialization << "," << new_doctor.phone_number << "," << new_doctor.years_experience << "," << new_doctor.hospital_branch << "," << new_doctor.email << "," << available_flag<< "\n";
    file.close();

    return "Doctor " + new_doctor.first_name + " " + new_doctor.last_name + " added successfully";
}

string Doctor_handler::remove_doctor(const string& doctor_id) {
    if (doctors.find(doctor_id) == doctors.end()) {
        return "Error: Doctor with ID " + doctor_id + " not found";
    }

    string name = doctors[doctor_id].first_name + " " + doctors[doctor_id].last_name;
    doctors.erase(doctor_id);
	write_to_file();
    return "Doctor " + name + " removed successfully";
}

string Doctor_handler::toggle_availability(const string& doctor_id) {
    if (doctors.find(doctor_id) == doctors.end()) {
        return "Error: Doctor with ID " + doctor_id + " not found";
    }

    doctors[doctor_id].is_available = !doctors[doctor_id].is_available;

    string status = doctors[doctor_id].is_available ? "available" : "unavailable";
    write_to_file();
    return "Doctor " + doctors[doctor_id].first_name + " " + doctors[doctor_id].last_name + " is now " + status;
}

string Doctor_handler::get_doctor_info(const string& doctor_id) {
    if (doctors.find(doctor_id) == doctors.end()) {
        return "Doctor not found";
    }

    Doctor doc = doctors[doctor_id];
    string status = doc.is_available ? "Available" : "Not Available";
    return "Doctor: " + doc.first_name + " " + doc.last_name + " - Status: " + status;
}

string Doctor_handler::get_doctor_by_speciality(const string& specialization) {
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

Doctor_handler::Doctor_handler(string filename) : filename(filename) {}

void Doctor_handler::set_file(string filename)
{
    this->filename=filename;
}

void Doctor_handler::print_all_doctors() {
    for (const auto& pair : doctors) {
        const Doctor& doc = pair.second;
        cout << "ID: " << doc.doctor_id << ", Name: " << doc.first_name << " " << doc.last_name
            << ", Specialization: " << doc.specialization << ", Available: " << (doc.is_available ? "Yes" : "No") << endl;
    }
}

string Doctor_handler::get_latest_doc_id()
{
    if(doctors.empty())
    {
        return "D0";
    }
    return doctors.rbegin()->first;
}