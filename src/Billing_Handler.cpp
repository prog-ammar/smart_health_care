#include "Billing_Handler.h"

// Helper to prevent 'std::invalid_argument' crashes from CSV headers or empty lines
int Billing_Handler::safe_stoi(const string& str) {
    if (str.empty()) return 0;
    try {
        // Remove potential whitespace
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == string::npos) return 0;
        size_t last = str.find_last_not_of(" \t\n\r");
        return stoi(str.substr(first, (last - first + 1)));
    } catch (...) {
        return 0; 
    }
}

string Billing_Handler::get_current_time() {
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &ltm);
    return string(buffer);
}

void Billing_Handler::recompute_total(Bill& b) {
    b.total_amount = 0;
    for (const auto& item : b.items) {
        b.total_amount += item.amount;
    }
}

// Fixed: Added safe_stoi to prevent medicine_file crashes
void Billing_Handler::load_medicine_prices() {
    ifstream file(medicine_file);
    if (!file.is_open()) return;
    string line, name, price,category,id;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, category, ',');
        getline(ss, price, ',');
        if(!name.empty())
            medicine_prices[name] = Medicine(id,name,category,safe_stoi(price));
    }
    file.close();
}

// Fixed: Now automatically looks up fee if spec_fee is passed as -1
void Billing_Handler::create_opd_bill(const OPD_Patient& p, string specialization) {
    
    // 1. Calculate the Fee Logic
    int final_fee = specialization_fees[specialization];
    if (final_fee <= 0 && specialization_fees.count(p.department)) {
        final_fee = specialization_fees[p.department];
    } else if (final_fee <= 0) {
        final_fee = 500; // Default fallback
    }

    // 2. Check if an active bill ALREADY exists
    if (active_bills.count(p.id) != 0) {
        
        // --- MERGE LOGIC ---
        // Get reference to the existing bill
        Bill& existing_bill = active_bills[p.id];
        
        // Add the new item to the EXISTING list
        existing_bill.items.push_back(BillItem("Consultation Fee (" + p.department + ")", final_fee));
        
        // Re-calculate total based on all items now in the list
        recompute_total(existing_bill); 
        
    } 
    else {
        // --- NEW BILL LOGIC ---
        Bill b;
        b.patient_id = p.id;
        b.patient_name = p.name;
        b.bill_type = "OPD";
        b.created_at = get_current_time();
        b.status = "UNPAID"; // Make sure to set status
        
        b.items.push_back(BillItem("Consultation Fee (" + p.department + ")", final_fee));
        recompute_total(b);
        
        active_bills[p.id] = b;
    }

    save_bills_to_file();
}

void Billing_Handler::create_emergency_bill(const Emergency_Patient& p, int base_fee) {
    
    // 1. Calculate charges upfront
    int severity_charge = p.sensitivity * 100; 

    // 2. Check if active bill exists
    if (active_bills.count(p.id) != 0) {
        
        // --- MERGE WITH EXISTING BILL ---
        Bill& existing_bill = active_bills[p.id]; // Get reference
        
        // Add new items to the existing list
        existing_bill.items.push_back(BillItem("Emergency Base Fee", base_fee));
        existing_bill.items.push_back(BillItem("Severity Surcharge", severity_charge));
        
        // Recalculate total
        recompute_total(existing_bill);
    } 
    else {
        
        // --- CREATE NEW BILL ---
        Bill b;
        b.patient_id = p.id;
        b.patient_name = p.name;
        b.bill_type = "Emergency";
        b.created_at = get_current_time();
        b.status = "UNPAID"; // Important for logic checks later

        b.items.push_back(BillItem("Emergency Base Fee", base_fee));
        b.items.push_back(BillItem("Severity Surcharge", severity_charge));
        
        recompute_total(b);
        active_bills[p.id] = b;
    }

    save_bills_to_file();
}

bool Billing_Handler::add_medicine_to_bill(string patient_id, string med_name, int qty) {
    if (active_bills.count(patient_id) == 0) {return false;}
    if (medicine_prices.count(med_name) == 0) {
        return false;
    }

    int total_med_cost =active_bills[patient_id].total_amount;
    total_med_cost += medicine_prices[med_name].price * qty;
    active_bills[patient_id].total_amount=total_med_cost;
    active_bills[patient_id].items.push_back(BillItem("Med: " + med_name + " x" + to_string(qty), total_med_cost));
    save_bills_to_file();
    return true;
}

// Merged and Cleaned up Print Function
void Billing_Handler::print_bill(string patient_id) {
    if (active_bills.find(patient_id) == active_bills.end()) {
        cout << "Bill not found for ID: " << patient_id << endl;
        return;
    }
    Bill& b = active_bills[patient_id];
    cout << "\n======= HOSPITAL BILL =======" << endl;
    cout << "Patient ID: " << b.patient_id << " | Name: " << b.patient_name << endl;
    cout << "Date: " << b.created_at << " | Type: " << b.bill_type << " | Status: " << b.status << endl;
    cout << "-----------------------------" << endl;
    for (const auto& item : b.items) {
        cout << "- " << item.title << " : Rs. " << item.amount << endl;
    }
    cout << "-----------------------------" << endl;
    cout << "TOTAL AMOUNT: Rs. " << b.total_amount << endl;
    cout << "=============================\n" << endl;
}

bool Billing_Handler::add_custom_charge(string patient_id, string service_name, int cost) {
    if (active_bills.find(patient_id) == active_bills.end()) return false;
    active_bills[patient_id].items.push_back(BillItem(service_name, cost));
    recompute_total(active_bills[patient_id]);
    save_bills_to_file();
    return true;
}

bool Billing_Handler::mark_as_paid(string patient_id) {
    if (active_bills.find(patient_id) != active_bills.end()) {
        active_bills[patient_id].status = "PAID";
        save_bills_to_file();
        return true;
    }
    return false;
}

// Fixed: Added safe_stoi for total column
void Billing_Handler::load_bills_from_file() {
    ifstream file(billing_file);
    if (!file.is_open()) return;

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty() || line.find_first_not_of(" \t\n\r") == string::npos) continue;
        stringstream ss(line);
        string id, name, type, status, total,date;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, type, ',');
        getline(ss, status, ',');
        getline(ss, total, ',');
        getline(ss,date,',');

        Bill b;
        b.patient_id = id;
        b.patient_name = name;
        b.bill_type = type;
        b.status = status;
        b.created_at = date;
        b.total_amount = safe_stoi(total);
        active_bills[id] = b;
    }
    file.close();
}

void Billing_Handler::save_bills_to_file() {
    ofstream file(billing_file);
    file<<"Patient ID,Patient Name,Bill Type,Status,Amount,Date And Time\n";
    for(auto& [a,b] : active_bills)
    file << b.patient_id << "," << b.patient_name << "," << b.bill_type << "," << b.status << "," << b.total_amount <<  "," <<b.created_at<<"\n";
    file.close();
}

void Billing_Handler::load_specialization_fees() {
    ifstream file(specs_file);
    if (!file.is_open()) return;
    string line;
    int rows=0; // Skip header

    while (getline(file, line)) {
        if (rows==0) continue;
        if (line.empty()) continue;
        stringstream ss(line);
        string spec, feeStr;
        getline(ss, spec, ',');
        getline(ss, feeStr, ',');
        rows++;

        if (!spec.empty()) {
            specialization_fees[spec] = safe_stoi(feeStr);
        }
    }
    file.close();
}


Billing_Handler::Billing_Handler(string billing_file,string medicine_file,string specs_file): billing_file(billing_file),medicine_file(medicine_file),specs_file(specs_file) {}

void Billing_Handler::set_file(string billing_file,string medicine_file,string specs_file)
{
    this->billing_file=billing_file;
    this->medicine_file=medicine_file;
    this->specs_file=specs_file;
}

string Billing_Handler::get_latest_bill_id()
{
    if (active_bills.empty()) {
        return "B0"; // Default value if no bills exist yet
    }
    return active_bills.rbegin()->first;
}

vector<Bill> Billing_Handler::return_all_bills()
{
    vector<Bill> bills;
    for(auto& i : active_bills)
    {
        bills.push_back(i.second);
    }
    return bills;
}