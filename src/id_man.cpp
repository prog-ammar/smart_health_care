
#include "id_man.h"

void ID_Manager::load_ids(Billing_Handler* bh,Patient_Handler* ph,Doctor_handler* dh,Appointment_handler* ah)
    {
        cout<<"h";
        curr_bill_id=stoi(bh->get_latest_bill_id().substr(1));
        cout<<"h";
        curr_doc_id=stoi(dh->get_latest_doc_id().substr(1));
        cout<<"h";
        curr_app_id=stoi(ah->get_latest_app_id().substr(1));
        cout<<"h";
        curr_opd_id=stoi(ph->get_lastest_opd_id().substr(1));
        cout<<"h";
        curr_em_id=stoi(ph->get_latest_em_id().substr(1));
        cout<<"h";
    }
    
    string ID_Manager::get_bill_id()
    {
        curr_bill_id++;
        return "B"+to_string(curr_bill_id);
    }

    string ID_Manager::get_Doctor_id()
    {
        curr_doc_id++;
        return "D"+to_string(curr_doc_id);
    }

    string ID_Manager::get_Appoinment_id()
    {
        curr_app_id++;
        return "A"+to_string(curr_app_id);
    }

    string ID_Manager::get_Emergency_id()
    {
        curr_em_id++;
        return "E"+to_string(curr_em_id);
    }

    string ID_Manager::get_OPD_id()
    {
        curr_opd_id++;
        return "O"+to_string(curr_opd_id);
    }