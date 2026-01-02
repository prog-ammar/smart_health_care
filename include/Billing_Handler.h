

#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include "Patient_Handler.h"
#include "Doctor_Handler.h"

using namespace std;

struct BillItem {
    string title;
    int amount;
    BillItem(string t, int a) : title(t), amount(a) {}
};

struct Medicine
{
    string id;
    string name;
    string category;
    int price;

    Medicine(string id="",string name="",string category="",int price=0) : id(id),name(name),category(category),price(price) {}
};

struct Bill {
    string patient_id;
    string patient_name;
    string bill_type;      // "OPD" or "Emergency"
    string status;         // "OPEN", "PAID"
    string created_at;
    vector<BillItem> items;
    int total_amount;

    Bill() : total_amount(0), status("OPEN") {}
};

class Billing_Handler {
private:
    map<string, Bill> active_bills;
    map<string, Medicine> medicine_prices;
    map<string, int> specialization_fees;
    string billing_file ;
    string medicine_file ;
    string specs_file;

    string get_current_time();
    void recompute_total(Bill& b);

public:
    Billing_Handler(string billing_file="",string medicine_file="",string specs_file="");
    void set_file(string billing_file,string medicine_file,string specs_file);
    void load_medicine_prices();
    void load_bills_from_file();
    void save_bills_to_file();
     int safe_stoi(const string& str) ;

    // Create bill for OPD Patient
    void create_opd_bill(const OPD_Patient& p, string specialization);
    void load_specialization_fees();
    
    // Create bill for Emergency Patient
    void create_emergency_bill(const Emergency_Patient& p, int base_fee);

    // Add items
    bool add_medicine_to_bill(string patient_id, string med_name, int qty);
    bool add_custom_charge(string patient_id, string service_name, int cost);
    vector<Bill> return_all_bills();
    
    // Finalize
    bool mark_as_paid(string patient_id);
    void print_bill(string patient_id);

    string get_latest_bill_id();
};