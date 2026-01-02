#pragma once

#include<iostream>
#include<string>

#include "Billing_Handler.h"
#include "Patient_Handler.h"
#include "Appoinment_Handler.h"
#include "Doctor_Handler.h"

using namespace std;

class ID_Manager
{
    private:
    int curr_bill_id;
    int curr_opd_id;
    int curr_em_id;
    int curr_doc_id;
    int curr_app_id;

    public:

    void load_ids(Billing_Handler* bh,Patient_Handler* ph,Doctor_handler* dh,Appointment_handler* ah);
    string get_bill_id();
    string get_Doctor_id();
    string get_Appoinment_id();
    string get_Emergency_id();
    string get_OPD_id();
};