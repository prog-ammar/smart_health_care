#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <map>
using namespace std;

struct Patient_Symptoms
{
    int anxiety;
    int depression;
    int fever;
    int pain;
    int bleeding;
    int dizziness;
    int nausea;
    int chestPain;
    int breathingIssue;
    int unconscious;

    Patient_Symptoms()
    {
        anxiety = depression = fever = pain = bleeding = dizziness = nausea = chestPain = breathingIssue = unconscious = -1;
    }
};

struct Patient
{
    string name;
    int age;
    string blood;
    string gender;
    string contact_number;
    string department;

    string id;
    Patient()
    {
        id = name = blood = gender = contact_number = department = "";
         age = -1;
    }
};

struct OPD_Patient :public Patient
{
    Patient_Symptoms symptoms;
};

struct Medicine
{
    string medicine_name;
    string def_prescription;
};

struct Doctor
{
    //TODO
    //Dep
    //Sep
    //ID
    //Name
    //Age
};

struct Emergency_Patient :public Patient
{
    string relative_number;
    int sensitivity;

    Emergency_Patient()
    {
        relative_number = "";
        sensitivity = -1;
    }


};

struct Record
{
    string Date;
    Doctor doctor;
    Patient patinet;
    string prescribtion;
    //Medicine

};

struct compare_sensitivity
{
    //second element is inserted lower to first in heap and will be checked from below code and decisions will be made accordingly
    bool operator()(Emergency_Patient const& p1, Emergency_Patient const& p2)
    {
        if (p1.sensitivity < p2.sensitivity)
        {
            return true;       // no change p2 is lower in heap
        }
        else
        {
            return false;      // swap big value with lower one so parent in heap is larger than child in heap
        }

    }
};

int calculate_Sensitivity(Patient_Symptoms s)
{
    return s.anxiety + s.depression + s.fever + s.pain + s.bleeding + s.dizziness + s.nausea + s.chestPain + s.breathingIssue + s.unconscious;
}

class Patient_Handler
{
private:
    queue<OPD_Patient> opd_queue;
    map<string, OPD_Patient> opd_patients;
    priority_queue<Emergency_Patient, vector<Emergency_Patient>, compare_sensitivity> emergency_queue;
    map<string, Emergency_Patient> emergency_patients;
    int opd_id_counter = 1;
    int em_id_counter = 1;

public:

    void write_to_all_files()
    {
        write_Emergency_file("emergency.csv", emergency_patients);
        write_OPD_file("opd.csv", opd_patients);
    }

    void read_from_all_files()
    {
        load_Emergency_file("emergency.csv", emergency_patients);
        load_OPD_file("opd.csv", opd_patients);
    }

    void write_OPD_file(string filename, map<string, OPD_Patient>& vec)
    {
        ofstream file(filename);
        file << "id,Name,Age,Blood,Gender,Contact-No,Department,anxiety,depression,fever,pain,bleeding ,dizziness,nausea,chestPain,breathingIssue,unconscious" << "\n";
        for (auto& i : vec)
        {
            file << i.second.id << "," << i.second.name << "," << i.second.age << "," << i.second.blood << "," << i.second.gender << "," << i.second.contact_number << ",";
            file << i.second.department << ",";
            file << i.second.symptoms.anxiety << "," << i.second.symptoms.depression << "," << i.second.symptoms.fever << ",";
            file << i.second.symptoms.pain << "," << i.second.symptoms.bleeding << "," << i.second.symptoms.dizziness << ",";
            file << i.second.symptoms.nausea << "," << i.second.symptoms.chestPain << "," << i.second.symptoms.breathingIssue << "," << i.second.symptoms.unconscious << "\n";

        }
        file.close();
    }

    int load_OPD_file(string file_name, map<string, OPD_Patient>& vec)
    {
        ifstream file(file_name);
        string wholeline;
        int row = 0;
        string p_details[19];
        OPD_Patient p;
        while (getline(file, wholeline))
        {
            if (row == 0)
            {
                row++;
                continue;
            }

            stringstream line(wholeline);
            for (int i = 0; i < 17; i++)
            {
                getline(line, p_details[i], ',');
            }
            p.id = p_details[0];
            p.name = p_details[1];
            p.age = stoi(p_details[2]);
            p.blood = p_details[3];
            p.gender = p_details[4];
            p.contact_number = p_details[5];
            p.department = p_details[6];
            p.symptoms.anxiety = stoi(p_details[7]);
            //anxiety,depression,fever,pain,bleeding ,dizziness,nausea,chestPain,breathingIssue,unconscious"<<"\n";
            p.symptoms.depression = stoi(p_details[8]);
            p.symptoms.fever = stoi(p_details[9]);
            p.symptoms.pain = stoi(p_details[10]);
            p.symptoms.bleeding = stoi(p_details[11]);
            p.symptoms.dizziness = stoi(p_details[12]);
            p.symptoms.nausea = stoi(p_details[13]);
            p.symptoms.chestPain = stoi(p_details[14]);
            p.symptoms.breathingIssue = stoi(p_details[15]);
            p.symptoms.unconscious = stoi(p_details[16]);

            vec[p.id] = p;
            row++;
        }
        file.close();
        return row;
    }

    void write_Emergency_file(string filename, map<string, Emergency_Patient>& vec)
    {
        ofstream file(filename);
        file << "id,Name,Age,Blood,Gender,Contact-No,Department,Relative-No,sensitivity" << "\n";
        for (auto& i : vec)
        {
            file << i.second.id << "," << i.second.name << "," << i.second.age << "," << i.second.blood << "," << i.second.gender << "," << i.second.contact_number << ",";
            file << i.second.department << "," << i.second.relative_number << "," << i.second.sensitivity << ",";
        }
        file.close();
    }

    int load_Emergency_file(string file_name, map<string, Emergency_Patient>& vec)
    {
        ifstream file(file_name);
        string wholeline;
        int row = 0;
        string p_details[19];
        Emergency_Patient p;
        while (getline(file, wholeline))
        {
            if (row == 0)
            {
                row++;
                continue;
            }

            stringstream line(wholeline);
            for (int i = 0; i < 19; i++)
            {
                getline(line, p_details[i], ',');
            }
            p.id = p_details[0];
            p.name = p_details[1];
            p.age = stoi(p_details[2]);
            p.blood = p_details[3];
            p.gender = p_details[4];
            p.contact_number = p_details[5];
            p.department = p_details[6];
            p.relative_number = p_details[7];
            p.sensitivity = stoi(p_details[8]);
            //anxiety,depression,fever,pain,bleeding ,dizziness,nausea,chestPain,breathingIssue,unconscious"<<"\n";
            vec[p.id] = p;
            row++;
        }
        file.close();
        return row;
    }

    void set_symptoms(string p_id, int anx, int dep, int bleed, int b_issue, int pain, int nau, int uncous, int fever, int diz, int c_p)
    {
        opd_patients[p_id].symptoms.anxiety = anx;
        opd_patients[p_id].symptoms.depression = dep;
        opd_patients[p_id].symptoms.bleeding = bleed;
        opd_patients[p_id].symptoms.breathingIssue = b_issue;
        opd_patients[p_id].symptoms.pain = pain;
        opd_patients[p_id].symptoms.nausea = nau;
        opd_patients[p_id].symptoms.unconscious = uncous;
        opd_patients[p_id].symptoms.fever = fever;
        opd_patients[p_id].symptoms.dizziness = diz;
        opd_patients[p_id].symptoms.chestPain = c_p;
    }

    void addEmergencyPatient(string p_name, int p_age, string p_blood, string p_gender, string p_r_contact, string p_dep, int sensitivity)
    {
        Emergency_Patient p;
        p.name = p_name;
        p.age = p_age;
        p.blood = p_blood;
        p.gender = p_gender;
        p.department = p_dep;
        p.id = "E" + to_string(em_id_counter++);
        p.relative_number = p_r_contact;
        p.sensitivity = sensitivity;
        emergency_patients[p.id] = p;
        emergency_queue.push(p);
    }

    void addOPDPatient(string p_name, int p_age, string p_blood, string p_gender, string p_contact, string p_dep, int p_id, int anx, int dep, int bleed, int b_issue, int pain, int nau, int uncous, int fever, int diz, int c_p)
    {
        OPD_Patient p;
        p.name = p_name;
        p.age = p_age;
        p.blood = p_blood;
        p.gender = p_gender;
        p.contact_number = p_contact;
        p.department = p_dep;
        p.id = "O" + to_string(opd_id_counter++);
        set_symptoms(p.id, anx, dep, bleed, b_issue, pain, nau, uncous, fever, diz, c_p);
        opd_patients[p.id] = p;
        opd_queue.push(p);

    }

    // Patient addPatient(int id,string p_name,int p_age,string p_blood,string p_gender,string p_dep,string p_contact="",string p_r_contact="")
    // {
    //     Patient p;
    //     p.name=p_name;
    //     p.age=p_age;
    //     p.blood=p_blood;
    //     p.gender=p_gender;
    //     p.contact_number=p_contact;
    //     p.department=p_dep;
    //     p.id=id;
    //     return p;
    // }


    void treatOPD()
    {
        if (!opd_queue.empty())
        {
            OPD_Patient rp = opd_queue.front();
            opd_queue.pop();
            cout << "OPD patient treated: " << rp.name << "\nDepartment: " << rp.department << endl;
        }
        else
        {
            cout << "No OPD patients available.\n";
        }
    }

    void treatEmergency()
    {
        if (!emergency_queue.empty())
        {
            Emergency_Patient ep = emergency_queue.top();
            emergency_queue.pop();
            cout << "Emergency patient treated: " << ep.name << " whose sensitivity was " << ep.sensitivity << endl;
        }
        else
        {
            cout << "No emergency patients available.\n";
        }
    }

    OPD_Patient accessOPDPatientByID(string p_id)
    {

        if ( opd_patients.count(p_id) > 0 )
        {
            return opd_patients[p_id];
        }
        return;
    }

    Emergency_Patient returnEmPatientByID(string p_id)
    {
         if (emergency_patients.count(p_id) > 0)
        {
            return emergency_patients[p_id];
          }
            return;
    }

};



// ID-based access for OPD


// ID-based access for emergency





// ---------------- TREAT OPD ----------------


// ---------------- TREAT EMERGENCY ----------------




int main()
{
    Patient_Handler ph;
    ph.read_from_all_files();
    ph.accessPatientByID(1);
}
