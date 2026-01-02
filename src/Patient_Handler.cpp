
#include "Patient_Handler.h"

    

void Patient_Handler::load_OPD()
    {
        ifstream file(opd_file);
        string line;
        getline(file, line); // skip header
        while (getline(file, line))
        {
            stringstream ss(line);
            string id, name, age, blood, gender, contact, dept;
            getline(ss, id, ','); 
            getline(ss, name, ','); 
            getline(ss, age, ',');
            getline(ss, blood, ','); 
            getline(ss, gender, ','); 
            getline(ss, contact, ','); 
            getline(ss, dept, ',');

            OPD_Patient p;
            p.id = id; 
            p.name = name; 
            p.age = stoi(age);
             p.blood = blood;
            p.gender = gender; 
            p.contact_number = contact;
            p.department = dept;
            opd_patients[id] = p;
        }
        file.close();
    }


    void Patient_Handler::load_active_queues() {
    // Clear current in-memory queues to avoid duplicates
    opd_queue = queue<OPD_Patient>();
    emergency_queue = priority_queue<Emergency_Patient, vector<Emergency_Patient>, compare_sensitivity>();

    // 1. Load OPD Queue
    ifstream f_opd(curr_opd_file);
    string line;
    if (f_opd.is_open()) {
        getline(f_opd, line); // Skip header
        while (getline(f_opd, line)) {
            stringstream ss(line);
            string id, dept, status;
            getline(ss, id, ','); 
            getline(ss, dept, ','); 
            getline(ss, status, ',');
            
            // Only add if status is Waiting AND patient exists in main DB
            if (status == "Waiting" && opd_patients.count(id)) {
                opd_queue.push(opd_patients[id]);
            }
        }
        f_opd.close();
    }

    // 2. Load Emergency Queue
    ifstream f_em(curr_em_file);
    if (f_em.is_open()) {
        getline(f_em, line); // Skip header
        while (getline(f_em, line)) {
            stringstream ss(line);
            string id, status;
            getline(ss, id, ','); 
            getline(ss, status, ',');
            
            if (status == "Waiting" && emergency_patients.count(id)) {
                emergency_queue.push(emergency_patients[id]);
            }
        }
        f_em.close();
    }
}

// --- NEW: Save Active Queues to curr_*.csv ---
void Patient_Handler::save_active_queues() {
    // 1. Save OPD Queue
    ofstream f_opd(curr_opd_file); // Overwrite mode
    f_opd << "patient_id,department,status\n";
    vector<OPD_Patient> o_vec = get_opd_queue();
    for (const auto& p : o_vec) {
        f_opd << p.id << "," << p.department << ",Waiting\n";
    }
    f_opd.close();

    // 2. Save Emergency Queue
    ofstream f_em(curr_em_file); // Overwrite mode
    f_em << "patient_id,status\n";
    vector<Emergency_Patient> e_vec = get_emergency_queue();
    for (const auto& p : e_vec) {
        f_em << p.id << ",Waiting\n";
    }
    f_em.close();
}

    Patient_Handler::Patient_Handler(string opd_file,string em_file,string curr_opd_file,string curr_em_file) : opd_file(opd_file),em_file(em_file),curr_opd_file(curr_opd_file),curr_em_file(curr_em_file) {}

    void Patient_Handler::set_file(string opd_file,string em_file,string curr_opd_file,string curr_em_file)
    {
        this->opd_file=opd_file;
        this->em_file=em_file;
        this->curr_opd_file=curr_opd_file;
        this->curr_em_file=curr_em_file;
    }


    string Patient_Handler::get_lastest_opd_id()
    {
        if(opd_patients.empty())
        return "O0";
        return opd_patients.rbegin()->first;
    }

    string Patient_Handler::get_latest_em_id()
    {
        if(emergency_patients.empty())
        return "E0";
        return emergency_patients.rbegin()->first;
    }

    void Patient_Handler::load_Emergency()
    {
        ifstream file(em_file);
        string line;
        getline(file, line); // skip header
        while (getline(file, line))
        {
            stringstream ss(line);
            string id, name, age, blood, gender, relative, sens;
            string vals[10];
            getline(ss, id, ',');
            getline(ss, name, ','); 
            getline(ss, age, ',');
            getline(ss, blood, ','); 
            getline(ss, gender, ','); 
            getline(ss, relative, ','); 
            getline(ss, sens, ',');
            for (int i = 0; i < 10; i++) 
            {
                getline(ss, vals[i], ',');
            }

            Emergency_Patient p;
            p.id = id; p.name = name; p.age = stoi(age); p.blood = blood; p.gender = gender;
            p.relative_number = relative; p.sensitivity = stoi(sens); p.arrivalIndex = emergencyIndex++;
            p.symptoms.bloodPressure = stoi(vals[0]); p.symptoms.heartRate = stoi(vals[1]);
            p.symptoms.oxygenLevel = stoi(vals[2]); p.symptoms.temperature = stoi(vals[3]);
            p.symptoms.respiratoryRate = stoi(vals[4]); p.symptoms.consciousness = stoi(vals[5]);
            p.symptoms.bleeding = stoi(vals[6]); p.symptoms.pain = stoi(vals[7]);
            p.symptoms.chestPain = stoi(vals[8]); p.symptoms.nausea = stoi(vals[9]);

            emergency_patients[id] = p;
        }
        file.close();
    }

    // --- MODIFIED: Treat Patient (Trigger Save) ---
string Patient_Handler::treat_OPD() {
    if (!opd_queue.empty()) {
        OPD_Patient p = opd_queue.front();
        opd_queue.pop();
        save_active_queues(); // Update file after removal
        return "OPD patient treated: " + p.name;
    }
    return "No OPD patients.";
}

string Patient_Handler::treat_Emergency() {
    if (!emergency_queue.empty()) {
        Emergency_Patient p = emergency_queue.top();
        emergency_queue.pop();
        save_active_queues(); // Update file after removal
        return "Emergency patient treated: " + p.name;
    }
    return "No Emergency patients.";
}

    // ---------- Add Patients ----------
    string Patient_Handler::add_OPD_patient(const OPD_Patient& p)
    {
        opd_patients[p.id] = p;
        opd_queue.push(p);
        ofstream file(opd_file,ios::app);
        file << p.id << "," << p.name << "," << p.age << "," << p.blood << "," << p.gender << "," << p.contact_number << "," << p.department << "\n";
        file.close();
        save_active_queues();
        return "OPD patient added: " +  p.name ;
    }

    string Patient_Handler::add_Emergency_patient(Emergency_Patient p)
    {
        p.arrivalIndex = emergencyIndex++;
        emergency_patients[p.id] = p;
        emergency_queue.push(p);
        ofstream file(em_file,ios::app);
        file << p.id << "," << p.name << "," << p.age << "," << p.blood << "," << p.gender << "," << p.relative_number << "," << p.sensitivity <<","<<p.symptoms.bloodPressure<<
        ","<<p.symptoms.heartRate<<","<<p.symptoms.oxygenLevel<<","<<p.symptoms.temperature<<","<<p.symptoms.respiratoryRate<<","<<p.symptoms.consciousness<<","<<
        p.symptoms.bleeding<<","<<p.symptoms.pain<<","<<p.symptoms.chestPain<<","<<p.symptoms.nausea<<"\n";

        file.close();
        save_active_queues();
        return  "Emergency patient added: " + p.name;
    }

    // ---------- Write CSVs ----------
    void Patient_Handler::write_OPD_file()
    {
        ofstream file(opd_file);
        file << "patient_id,name,age,blood,gender,contact_number,department\n";
        for (auto& i : opd_patients)
        {
            OPD_Patient& p = i.second;
            file << p.id << "," << p.name << "," << p.age << "," << p.blood << "," << p.gender << "," << p.contact_number << "," << p.department << "\n";
        }
        file.close();
    }

    void Patient_Handler::write_Emergency_file()
    {
        ofstream file(em_file);
        file << "patient_id,name,age,blood,gender,relative_number,sensitivity,bloodPressure,heartRate,oxygenLevel,temperature,respiratoryRate,consciousness,bleeding,pain,chestPain,nausea\n";
        for (auto& i : emergency_patients)
        {
            Emergency_Patient& p = i.second;
            file << p.id << "," << p.name << "," << p.age << "," 
                 << p.blood << "," << p.gender << "," << p.relative_number << "," 
                 << p.sensitivity << ","
                 << p.symptoms.bloodPressure << "," << p.symptoms.heartRate << ","
                 << p.symptoms.oxygenLevel << "," << p.symptoms.temperature << ","
                 << p.symptoms.respiratoryRate << "," << p.symptoms.consciousness << ","
                 << p.symptoms.bleeding << "," << p.symptoms.pain << ","
                 << p.symptoms.chestPain << "," << p.symptoms.nausea << "\n";
        }
        file.close();
    }


    Emergency_Patient Patient_Handler::accessEmByID(string p_id)
    {
        if (emergency_patients.count(p_id) == 0)
            cout << "Patient Dont Exist" << "\n";
        else
            return emergency_patients[p_id];
        return Emergency_Patient();
    }

    OPD_Patient Patient_Handler::accessOPDByID(string p_id)
    {   
        if (opd_patients.count(p_id) == 0)
            cout << "Patient Dont Exist" << "\n";
        else
            return opd_patients[p_id];
        return OPD_Patient();
    }


    vector<OPD_Patient> Patient_Handler::return_all_opd_patients()
    {
        vector<OPD_Patient> o;
        for(auto& i: opd_patients)
        {
            o.push_back(i.second);
        }
        return o;
    }
    vector<Emergency_Patient>  Patient_Handler::return_all_emergency_patients()
    {
        vector<Emergency_Patient> e;
        for(auto& i :emergency_patients)
        {
            e.push_back(i.second);
        }
        return e;
    }

    vector<OPD_Patient> Patient_Handler::get_opd_queue() {
    vector<OPD_Patient> result;
    queue<OPD_Patient> temp = opd_queue; // Copy active queue
    while(!temp.empty()) {
        result.push_back(temp.front());
        temp.pop();
    }
    return result;
}

vector<Emergency_Patient> Patient_Handler::get_emergency_queue() {
    vector<Emergency_Patient> result;
    priority_queue<Emergency_Patient, vector<Emergency_Patient>, compare_sensitivity> temp = emergency_queue; // Copy active queue
    while(!temp.empty()) {
        result.push_back(temp.top());
        temp.pop();
    }
    return result;
}


string Patient_Handler::remove_patient(const string& p_id)
{
    // --- 1. CHECK OPD PATIENTS (std::queue) ---
    if (opd_patients.count(p_id) != 0)
    {
        // A. Remove from the Map
        opd_patients.erase(p_id);

        // B. Remove from std::queue
        queue<OPD_Patient> temp_q;
        bool found = false;

        // "Pour" patients into temp queue, skipping the one to delete
        while (!opd_queue.empty()) {
            OPD_Patient p = opd_queue.front();
            opd_queue.pop();

            if (p.id == p_id) {
                found = true; // Skip this one
            } else {
                temp_q.push(p); // Keep this one
            }
        }
        
        // Restore the main queue
        opd_queue = temp_q;

        // Update file
        write_OPD_file();
        save_active_queues();
        return "OPD Patient Deleted Successfully";
    }

    // --- 2. CHECK EMERGENCY PATIENTS (std::priority_queue) ---
    else if (emergency_patients.count(p_id) != 0)
    {
        // A. Remove from the Map
        emergency_patients.erase(p_id);

        // B. Remove from std::priority_queue
        priority_queue<Emergency_Patient, vector<Emergency_Patient>, compare_sensitivity> temp_pq;
        bool found = false;

        // "Pour" patients into temp PQ, skipping the one to delete
        while (!emergency_queue.empty()) {
            Emergency_Patient p = emergency_queue.top();
            emergency_queue.pop();

            if (p.id == p_id) {
                found = true; // Skip this one
            } else {
                temp_pq.push(p); // Keep this one (Heap re-orders automatically)
            }
        }

        // Restore the main priority queue
        emergency_queue = temp_pq;

        // Update file
        write_Emergency_file();
        save_active_queues();
        return "Emergency Patient Deleted Successfully";
    }
    
    return "Error: Patient ID not found.";
}