#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // Enables automatic conversion between C++ STL and Python types
#include "Patient_Handler.h"
#include "Doctor_Handler.h"
#include "Appoinment_Handler.h"
#include "Billing_Handler.h"
#include "Recomendation_System.h"
#include "id_man.h"
#include "Hospital_Manager.h"

namespace py = pybind11;

PYBIND11_MODULE(hospital_backend, m) {
    m.doc() = "Hospital Management System Python Bindings";

    // --- Patient_Handler Structs ---

    
    py::class_<Patient_Symptoms>(m, "PatientSymptoms")
        .def(py::init<>())
        .def_readwrite("bloodPressure", &Patient_Symptoms::bloodPressure)
        .def_readwrite("heartRate", &Patient_Symptoms::heartRate)
        .def_readwrite("oxygenLevel", &Patient_Symptoms::oxygenLevel)
        .def_readwrite("temperature", &Patient_Symptoms::temperature)
        .def_readwrite("respiratoryRate", &Patient_Symptoms::respiratoryRate)
        .def_readwrite("consciousness", &Patient_Symptoms::consciousness)
        .def_readwrite("bleeding", &Patient_Symptoms::bleeding)
        .def_readwrite("pain", &Patient_Symptoms::pain)
        .def_readwrite("chestPain", &Patient_Symptoms::chestPain)
        .def_readwrite("nausea", &Patient_Symptoms::nausea);

    py::class_<RegistrationResult>(m, "RegistrationResult")
        .def_readwrite("patient_id", &RegistrationResult::patient_id)
        .def_readwrite("predicted_disease", &RegistrationResult::predicted_disease)
        .def_readwrite("suggested_doctor_list", &RegistrationResult::suggested_doctor_list)
        .def_readwrite("success", &RegistrationResult::success);

    py::class_<Patient>(m, "Patient")
        .def(py::init<>())
        .def_readwrite("name", &Patient::name)
        .def_readwrite("age", &Patient::age)
        .def_readwrite("blood", &Patient::blood)
        .def_readwrite("gender", &Patient::gender)
        .def_readwrite("contact_number", &Patient::contact_number)
        .def_readwrite("department", &Patient::department)
        .def_readwrite("id", &Patient::id);
        

    py::class_<OPD_Patient, Patient>(m, "OPDPatient")
        .def(py::init<>());

    py::class_<Emergency_Patient, Patient>(m, "EmergencyPatient")
        .def(py::init<>())
        .def_readwrite("relative_number", &Emergency_Patient::relative_number)
        .def_readwrite("sensitivity", &Emergency_Patient::sensitivity)
        .def_readwrite("symptoms", &Emergency_Patient::symptoms)
        .def_readwrite("arrivalIndex", &Emergency_Patient::arrivalIndex);

    // --- Patient_Handler Class ---
    py::class_<Patient_Handler>(m, "PatientHandler")
        .def(py::init<>())
        .def("load_OPD", &Patient_Handler::load_OPD)
        .def("load_Emergency", &Patient_Handler::load_Emergency)
        .def("add_OPD_patient", &Patient_Handler::add_OPD_patient)
        .def("add_Emergency_patient", &Patient_Handler::add_Emergency_patient)
        .def("treat_OPD", &Patient_Handler::treat_OPD)
        .def("treat_Emergency", &Patient_Handler::treat_Emergency)
        .def("accessOPDByID", &Patient_Handler::accessOPDByID)
        .def("accessEmByID", &Patient_Handler::accessEmByID)
        .def("return_all_opd_patients",&Patient_Handler::return_all_opd_patients)
        .def("get_opd_queue", &Patient_Handler::get_opd_queue)
    .def("get_emergency_queue", &Patient_Handler::get_emergency_queue)
    .def("load_active_queues", &Patient_Handler::load_active_queues) // ADD THIS
    .def("save_active_queues",&Patient_Handler::save_active_queues)
    .def("remove_patient",&Patient_Handler::remove_patient)
        .def("return_all_emergency_patients",&Patient_Handler::return_all_emergency_patients);
    // --- Doctor_Handler Structs & Class ---
    py::class_<Doctor>(m, "Doctor")
        .def(py::init<>())
        .def_readwrite("doctor_id", &Doctor::doctor_id)
        .def_readwrite("first_name", &Doctor::first_name)
        .def_readwrite("last_name", &Doctor::last_name)
        .def_readwrite("specialization", &Doctor::specialization)
        .def_readwrite("phone_number", &Doctor::phone_number)
        .def_readwrite("years_experience", &Doctor::years_experience)
        .def_readwrite("hospital_branch", &Doctor::hospital_branch)
        .def_readwrite("email", &Doctor::email)
        .def_readwrite("is_available", &Doctor::is_available);

    py::class_<Doctor_handler>(m, "DoctorHandler")
        .def(py::init<>())
        .def("getdoctorbyid", &Doctor_handler::getdoctorbyid)
        .def("read_from_file", &Doctor_handler::read_from_file)
        .def("write_to_file", &Doctor_handler::write_to_file)
        .def("check_available", &Doctor_handler::check_available)
        .def("add_doctor", &Doctor_handler::add_doctor)
        .def("remove_doctor", &Doctor_handler::remove_doctor)
        .def("toggle_availability", &Doctor_handler::toggle_availability)
        .def("get_doctor_by_speciality", &Doctor_handler::get_doctor_by_speciality)
        .def("get_doctor_info", &Doctor_handler::get_doctor_info)
        .def("print_all_doctors", &Doctor_handler::print_all_doctors)
        .def("get_all_doctors_vec", &Doctor_handler::get_all_doctors_vec);

    // --- Appointment_Handler Structs & Class ---
    py::class_<Appointment>(m, "Appointment")
        .def(py::init<>())
        .def_readwrite("appointment_id", &Appointment::appointment_id)
        .def_readwrite("patient_id", &Appointment::patient_id)
        .def_readwrite("doctor_id", &Appointment::doctor_id)
        .def_readwrite("appointment_date", &Appointment::appointment_date)
        .def_readwrite("appointment_time", &Appointment::appointment_time)
        .def_readwrite("reason_for_visit", &Appointment::reason_for_visit)
        .def_readwrite("status", &Appointment::status);

    py::class_<Appointment_handler>(m, "AppointmentHandler")
        .def(py::init<Doctor_handler*, Patient_Handler*>(), py::arg("dh") = nullptr, py::arg("ph") = nullptr)
        .def("set_doctor_handler", &Appointment_handler::set_doctor_handler)
        .def("set_patient_handler", &Appointment_handler::set_patient_handler)
        .def("read_from_file", &Appointment_handler::read_from_file)
        .def("write_to_file", &Appointment_handler::write_to_file)
        .def("schedule_appointment", &Appointment_handler::schedule_appointment)
        .def("cancel_appointment", &Appointment_handler::cancel_appointment)
        .def("complete_appointment",&Appointment_handler::complete_appoinment)
        .def("get_status", &Appointment_handler::get_status)
        .def("print_all_appointments", &Appointment_handler::print_all_appointments)
        .def("get_all_appointments_vec", &Appointment_handler::get_all_appointments_vec);

    // --- Billing_Handler Structs & Class ---
    py::class_<BillItem>(m, "BillItem")
        .def(py::init<string, int>())
        .def_readwrite("title", &BillItem::title)
        .def_readwrite("amount", &BillItem::amount);

    py::class_<Bill>(m, "Bill")
        .def(py::init<>())
        .def_readwrite("patient_id", &Bill::patient_id)
        .def_readwrite("patient_name", &Bill::patient_name)
        .def_readwrite("bill_type", &Bill::bill_type)
        .def_readwrite("status", &Bill::status)
        .def_readwrite("created_at", &Bill::created_at)
        .def_readwrite("items", &Bill::items)
        .def_readwrite("total_amount", &Bill::total_amount);

    py::class_<Billing_Handler>(m, "BillingHandler")
        .def(py::init<>())
        .def("load_medicine_prices", &Billing_Handler::load_medicine_prices)
        .def("load_bills_from_file", &Billing_Handler::load_bills_from_file)
        .def("save_bills_to_file", &Billing_Handler::save_bills_to_file)
        .def("safe_stoi", &Billing_Handler::safe_stoi)
        .def("create_opd_bill", &Billing_Handler::create_opd_bill)
        .def("load_specialization_fees", &Billing_Handler::load_specialization_fees)
        .def("create_emergency_bill", &Billing_Handler::create_emergency_bill)
        .def("add_medicine_to_bill", &Billing_Handler::add_medicine_to_bill)
        .def("add_custom_charge", &Billing_Handler::add_custom_charge)
        .def("mark_as_paid", &Billing_Handler::mark_as_paid)
        .def("print_bill", &Billing_Handler::print_bill)
        .def("return_all_bills",&Billing_Handler::return_all_bills);

    // --- Recommendation_System Classes ---
    py::class_<Recom_System>(m, "RecomSystem")
        .def(py::init<>())
        .def("load_data", &Recom_System::load_data)
        .def("get_condition", &Recom_System::get_condition)
        .def("get_precautions", &Recom_System::get_precautions)
        .def("get_medicines", &Recom_System::get_medicines);


    py::class_<Hospital_Management>(m, "Hospital_Management")
        .def(py::init<>())
        // Main Workflows
        .def("Register_OPD_Workflow", &Hospital_Management::Register_OPD_Workflow)
        .def("Admit_Emergency_Workflow", &Hospital_Management::Admit_Emergency_Workflow)
        .def("Book_Appointment_Workflow", &Hospital_Management::Book_Appointment_Workflow)
        .def("Prescribe_Medication", &Hospital_Management::Prescribe_Medication)
        .def("Discharge_Patient", &Hospital_Management::Discharge_Patient)
        
        // Treatment
        .def("Treat_OPD_Patient", &Hospital_Management::Treat_OPD_Patient)
        .def("Treat_Emergency_Patient", &Hospital_Management::Treat_Emergency_Patient)
        
        // Admin & Getters (Returning Vectors)
        .def("add_Doctor", &Hospital_Management::add_Doctor)
        .def("get_all_opd_patients", &Hospital_Management::get_all_opd_patients)
        .def("get_all_emergency_patients", &Hospital_Management::get_all_emergency_patients)
        .def("get_all_doctors", &Hospital_Management::get_all_doctors)
        .def("get_all_bills", &Hospital_Management::get_all_bills)

        .def("return_Bill_Handler",&Hospital_Management::return_Bill_Handler)
        .def("return_Patient_Handler",&Hospital_Management::return_Patient_Handler)
        .def("return_Doctor_Handler",&Hospital_Management::return_Doctor_Handler)
        .def("return_Recom_System",&Hospital_Management::return_Recom_System)
        .def("return_Appoinment_Handler",&Hospital_Management::return_Appoinment_Handler);
}