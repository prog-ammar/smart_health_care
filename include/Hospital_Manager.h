#pragma once

#include<iostream>
#include "Billing_Handler.h"
#include "Appoinment_Handler.h"
#include "Patient_Handler.h"
#include "Doctor_Handler.h"
#include "Recomendation_System.h"
#include "id_man.h"

using namespace std;

struct RegistrationResult {
    string patient_id;
    string predicted_disease;
    string suggested_doctor_list; // String representation of doctors for easy display
    bool success;
};

class Hospital_Management
{
    private:
    Doctor_handler* dh;
    Patient_Handler* ph;
    Appointment_handler * ah;
    Recom_System* rs;
    Billing_Handler* bh;
    ID_Manager* id_m;

    public:
    Hospital_Management()
    {

        ph=new Patient_Handler("csv_files/opd.csv","csv_files/emergency.csv","csv_files/curr_opd.csv","csv_files/curr_em.csv");
        ph->load_OPD();
        ph->load_Emergency();
        ph->load_active_queues();


        dh=new Doctor_handler("csv_files/doctors.csv");
        dh->read_from_file();


        ah=new Appointment_handler(dh,ph,"csv_files/appointments.csv");
        ah->read_from_file();

        

        bh =new Billing_Handler("csv_files/billing.csv","csv_files/medicines.csv","csv_files/doctor_specialization_fees.csv");
        bh->load_bills_from_file();
        bh->load_medicine_prices();
        bh->load_specialization_fees();

        

        rs=new Recom_System("csv_files/symptoms.csv","csv_files/precautions.csv","csv_files/disease_med.csv");
        rs->load_data();
        

        id_m=new ID_Manager();
        id_m->load_ids(bh,ph,dh,ah);

    }


    RegistrationResult Register_OPD_Workflow(string name, int age, string blood, string gender, string contact, vector<string> symptoms) ;
    string  Add_Emergency_Patient(string username,int age,string blood,string gender,string contact_no,string dept,string rel_no,int sens,int arr_in,Patient_Symptoms symps);
    string Admit_Emergency_Workflow(string name, int age, string blood, string gender, string relative, int sens, Patient_Symptoms symps) ;
    string Book_Appointment_Workflow(string patient_id, string doctor_id,bool ignore_availability) ;
    string Prescribe_Medication(string patient_id, string med_name, int quantity); 
    string Treat_OPD_Patient();
    string Treat_Emergency_Patient();
    OPD_Patient get_OPD_Patient_by_id(string id);
    string Discharge_Patient(string patient_id);
    Emergency_Patient get_Emergency_Patient_by_id(string id);
    Doctor Get_Doc_By_Id(string id);
    void add_Doctor(string first_name, string last_name, string specialization, string phone_number, int years_experience, string hospital_branch, string email);
    vector<OPD_Patient> get_all_opd_patients();
    vector<Emergency_Patient> get_all_emergency_patients(); 
    vector<Doctor> get_all_doctors(); 
    vector<Bill> get_all_bills(); 
    vector<Appointment> get_all_appointments(); 
    Doctor get_doctor_details(string id); 


    Billing_Handler* return_Bill_Handler() { return bh;}
    Patient_Handler* return_Patient_Handler() { return ph;}
    Recom_System* return_Recom_System() {return rs;}
    Doctor_handler* return_Doctor_Handler() {return dh;}
    Appointment_handler* return_Appoinment_Handler() {return ah;}

    ~Hospital_Management()
    {
        delete ph;delete ah;delete dh;delete bh;delete rs;
    }
};