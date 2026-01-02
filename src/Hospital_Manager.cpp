#include "Hospital_Manager.h"




    RegistrationResult Hospital_Management::Register_OPD_Workflow(string name, int age, string blood, string gender, string contact, vector<string> symptoms) {
    RegistrationResult result;
    
    // 1. Get Recommendation
    // If symptoms are empty, default to "General"
    string condition = "";
    if(!symptoms.empty()) {
        condition = rs->get_condition(symptoms);
    }
    
    if (condition.empty()) condition = "General Physician";
    
    result.predicted_disease = condition;

    // 2. Generate ID and Create Patient
    string new_id = id_m->get_OPD_id();
    
    // We set the 'Department' to the predicted disease/condition
    OPD_Patient p(name, age, blood, gender, contact, condition);
    p.set_id(new_id);
    
    // 3. Save Patient to File/Queue
    ph->add_OPD_patient(p);
    result.patient_id = new_id;
    result.success = true;

    // 4. Find Doctors available for this condition
    // This string can be displayed in a QMessageBox or TextEdit in PyQt
    result.suggested_doctor_list = dh->get_doctor_by_speciality(condition); 
    
    return result;
}
    
    string  Hospital_Management::Add_Emergency_Patient(string username,int age,string blood,string gender,string contact_no,string dept,string rel_no,int sens,int arr_in,Patient_Symptoms syms)
    {
        Emergency_Patient em(username,age,blood,gender,contact_no,dept,rel_no,sens,arr_in);
        em.symptoms=syms;
        em.set_id(id_m->get_Emergency_id());
        return ph->add_Emergency_patient(em);
    }

    string Hospital_Management::Admit_Emergency_Workflow(string name, int age, string blood, string gender, string relative, int sens, Patient_Symptoms symps) {
    string id = id_m->get_Emergency_id();
    
    // 0 is the arrival index placeholder, Patient_Handler manages the actual index internally
    Emergency_Patient p(name, age, blood, gender, relative, "Emergency", relative, sens, 0);
    p.set_id(id);
    p.symptoms = symps; // Assign the detailed vitals

    // Add to Priority Queue
    ph->add_Emergency_patient(p);

    // Emergency Workflow implies immediate billing start
    // Base fee 2000, plus surcharge handled inside create_emergency_bill based on sensitivity
    bh->create_emergency_bill(p, 2000); 

    return "Emergency Patient Admitted. ID: " + id + ". Triage Priority: " + to_string(sens);
}



    string Hospital_Management::Book_Appointment_Workflow(string patient_id, string doctor_id,bool ignore_availability) {
    // 1. Check Availability

    Doctor doc = dh->getdoctorbyid(doctor_id);
    if (doc.first_name == "") return "Error: Doctor not found.";

    // 2. Check Availability (ONLY if NOT ignoring)
    if (!ignore_availability && !doc.is_available) {
        return "Error: Doctor not available";
    }

    // 3. Verify Patient Exists
    OPD_Patient p = ph->accessOPDByID(patient_id);
    if (p.name == "") return "Error: Patient not found.";

    // 4. Schedule Appointment
    string app_id = id_m->get_Appoinment_id();
    string res = ah->schedule_appointment(app_id, patient_id, doctor_id, "Consultation");

    // 5. Initiate Bill (Consultation Fee)
    bh->create_opd_bill(p, doc.specialization);

    return "Success: " + res + " | Bill Generated.";

    }

    string Hospital_Management::Prescribe_Medication(string patient_id, string med_name, int quantity) 
    {
        bool success = bh->add_medicine_to_bill(patient_id, med_name, quantity);
        if(success) {
            return "Medicine added to bill.";
            }
        return "Error: Could not add medicine (Check patient ID or Medicine Name).";
    }

    string Hospital_Management::Treat_OPD_Patient()
    {
        return ph->treat_OPD();
    }

    string Hospital_Management::Treat_Emergency_Patient()
    {
        return ph->treat_Emergency();
    }

    OPD_Patient Hospital_Management::get_OPD_Patient_by_id(string id)
    {
        return ph->accessOPDByID(id);
    }

    string Hospital_Management::Discharge_Patient(string patient_id) 
    {
        bh->mark_as_paid(patient_id);
        return "Bill Paid. Patient Discharged.";
    }

    Emergency_Patient Hospital_Management::get_Emergency_Patient_by_id(string id)
    {
        return ph->accessEmByID(id);
    }

    Doctor Hospital_Management::Get_Doc_By_Id(string id)
    {
        return dh->getdoctorbyid(id);
    }

    
        void Hospital_Management::add_Doctor(string first_name, string last_name, string specialization, string phone_number, int years_experience, string hospital_branch, string email)
{
    // Generate the ID first
    string new_id = id_m->get_Doctor_id(); 
    
    // Pass all 8 arguments to the constructor
    Doctor new_doctor(new_id, first_name, last_name, specialization, phone_number, years_experience, hospital_branch, email);
    dh->add_doctor(new_doctor);
}








    vector<OPD_Patient> Hospital_Management::get_all_opd_patients() 
    {
        return ph->return_all_opd_patients();
    }

vector<Emergency_Patient> Hospital_Management::get_all_emergency_patients() {
    return ph->return_all_emergency_patients();
}

vector<Doctor> Hospital_Management::get_all_doctors() {
    return dh->get_all_doctors_vec();
}

vector<Bill> Hospital_Management::get_all_bills() {
    return bh->return_all_bills();
}

vector<Appointment> Hospital_Management::get_all_appointments() {
    return ah->get_all_appointments_vec(); 
}

Doctor Hospital_Management::get_doctor_details(string id) {
    return dh->getdoctorbyid(id);
}
