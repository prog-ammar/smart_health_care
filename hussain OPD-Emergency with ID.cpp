#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
using namespace std;

struct patient_symptoms
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

    patient_symptoms()
    {
        anxiety = depression = fever = pain = bleeding = dizziness = nausea = chestPain = breathingIssue = unconscious = -1;
    }
};

struct patient
{
    string name;
    int age;
    string blood;
    string gender;
    string contact_number;
    string department;

    string relative_number;
    patient_symptoms symptoms;
    int sensitivity;       //0 means stable higher means critical
    int id;    // id for pairing and accessing directly

    patient()
    {
        name = blood = gender = contact_number = department = relative_number = "";
        sensitivity = id = age = -1;
    }
};

struct compare_sensitivity
{
    //second element is inserted lower to first in heap and will be checked from below code and decisions will be made accordingly
    bool operator()(patient const& p1, patient const& p2)
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

int calculate_Sensitivity(patient_symptoms s)
{
    return s.anxiety + s.depression + s.fever + s.pain + s.bleeding + s.dizziness + s.nausea + s.chestPain + s.breathingIssue + s.unconscious;
}

class Patient_Handler
{
private:
    queue<patient> regular_queue;
    vector<pair<int, patient>> opd_patients;
    priority_queue<patient, vector<patient>, compare_sensitivity> emergency_queue;
    vector<pair<int, patient>> emergency_patients;
    vector<pair<int, patient>> all_patients;
    int id_counter = 1;

public:
    Patient_Handler()
    {

    }

    void write_to_file(string filename)
    {
        ofstream file(filename);
        file << "id,Name,Age,Blood,Gender,Contact-No,Department,Relative-No,sensitivity,anxiety,depression,fever,pain,bleeding ,dizziness,nausea,chestPain,breathingIssue,unconscious" << "\n";
        for (auto& i : all_patients)
        {
            file << i.second.id << "," << i.second.name << "," << i.second.age << "," << i.second.blood << "," << i.second.gender << "," << i.second.contact_number << ",";
            file << i.second.department << "," << i.second.relative_number << "," << i.second.sensitivity << ",";
            file << i.second.symptoms.anxiety << "," << i.second.symptoms.depression << "," << i.second.symptoms.fever << ",";
            file << i.second.symptoms.pain << "," << i.second.symptoms.bleeding << "," << i.second.symptoms.dizziness << ",";
            file << i.second.symptoms.nausea << "," << i.second.symptoms.chestPain << "," << i.second.symptoms.breathingIssue << "," << i.second.symptoms.unconscious << "\n";

        }
        file.close();
    }

    void load_from_file(string file_name)
    {
        ifstream file(file_name);
        string wholeline;
        int row = 0;
        string p_details[19];
        patient p;
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
            p.id = stoi(p_details[0]);
            p.name = p_details[1];
            p.age = stoi(p_details[2]);
            p.blood = p_details[3];
            p.gender = p_details[4];
            p.contact_number = p_details[5];
            p.department = p_details[6];
            p.relative_number = p_details[7];
            p.sensitivity = stoi(p_details[8]);
            p.symptoms.anxiety = stoi(p_details[9]);
            //anxiety,depression,fever,pain,bleeding ,dizziness,nausea,chestPain,breathingIssue,unconscious"<<"\n";
            p.symptoms.depression = stoi(p_details[10]);
            p.symptoms.fever = stoi(p_details[11]);
            p.symptoms.pain = stoi(p_details[12]);
            p.symptoms.bleeding = stoi(p_details[13]);
            p.symptoms.dizziness = stoi(p_details[14]);
            p.symptoms.nausea = stoi(p_details[15]);
            p.symptoms.chestPain = stoi(p_details[16]);
            p.symptoms.breathingIssue = stoi(p_details[17]);
            p.symptoms.unconscious = stoi(p_details[18]);

            all_patients.push_back({ p.id,p });
            row++;
        }
        file.close();
    }


    void addEmergency()
    {
        patient ep;
        ep.id = id_counter;
        id_counter++;

        cout << "\nAdding Emergency patient\n";
        cout << "Enter patient name: ";
        cin.ignore();
        getline(cin, ep.name);

        cout << "Enter patient age: ";
        cin >> ep.age;

        // Blood group validation
        while (true)
        {
            cout << "Enter patient blood group: ";
            cin >> ep.blood;

            if (ep.blood == "A+" || ep.blood == "A-" || ep.blood == "B+" || ep.blood == "B-" || ep.blood == "AB+" || ep.blood == "AB-" || ep.blood == "O+" || ep.blood == "O-")
            {
                break;
            }
            else
            {
                cout << "Invalid blood group! Try again.\n";
            }
        }

        cout << "Enter patient gender: ";
        cin >> ep.gender;

        // Relative contact number validation
        while (true)
        {
            cout << "Enter relative contact number (11 digits): ";
            cin >> ep.relative_number;

            if (ep.relative_number.length() != 11)
            {
                cout << "Number must be 11 digits\n";
                continue;
            }

            int i;
            for (i = 0; i < 11; i++)
            {
                if (ep.relative_number[i] < '0' || ep.relative_number[i] > '9')
                {
                    cout << "Only digits allowed\n";
                    break;
                }
            }

            if (i == 11) break;
        }



        cout << "Please enter sensitivity of symptom 0 means low and 10 means high \n";
        cout << "Anxiety: ";
        cin >> ep.symptoms.anxiety;
        cout << "Depression: ";
        cin >> ep.symptoms.depression;
        cout << "Fever: ";
        cin >> ep.symptoms.fever;
        cout << "Pain: ";
        cin >> ep.symptoms.pain;
        cout << "Bleeding: ";
        cin >> ep.symptoms.bleeding;
        cout << "Dizziness: ";
        cin >> ep.symptoms.dizziness;
        cout << "Nausea: ";
        cin >> ep.symptoms.nausea;
        cout << "Chest Pain: ";
        cin >> ep.symptoms.chestPain;
        cout << "Breathing Issue: ";
        cin >> ep.symptoms.breathingIssue;
        cout << "Unconscious: ";
        cin >> ep.symptoms.unconscious;

        ep.sensitivity = calculate_Sensitivity(ep.symptoms);

        emergency_queue.push(ep);
        emergency_patients.push_back({ ep.id, ep });
        cout << "Emergency patient " << ep.name << " added successfully!\n";
        all_patients.push_back({ ep.id,ep });
    }

    void addOPD()
    {
        /*
        patient ep;
        ep.id = id_counter;
        id_counter++;

        cout << "\nAdding Emergency patient\n";
        cout << "Enter patient name: ";
        cin.ignore();
        getline(cin, ep.name);
        */

        patient rp;
        rp.id = id_counter;
        id_counter++;

        cout << "\nAdding OPD patient";
        cout << "\nEnter patient name: ";
        getline(cin, rp.name);
        cout << "\nUser name entered is " << rp.name;

        cout << "Enter patient age: ";
        cin >> rp.age;

        // Blood group validation
        while (true)
        {
            cout << "Enter patient blood group: ";
            cin >> rp.blood;

            if (rp.blood == "A+" || rp.blood == "A-" || rp.blood == "B+" || rp.blood == "B-" || rp.blood == "AB+" || rp.blood == "AB-" || rp.blood == "O+" || rp.blood == "O-")
            {
                break;
            }
            else
            {
                cout << "Invalid blood group! Try again.\n";
            }
        }

        cout << "Enter patient gender: ";
        cin >> rp.gender;

        // Contact number validation
        while (true)
        {
            cout << "Enter contact number (11 digits): ";
            cin >> rp.contact_number;

            if (rp.contact_number.length() != 11)
            {
                cout << "Number must be 11 digits\n";
                continue;
            }

            int i;
            for (i = 0; i < 11; i++)
            {
                if (rp.contact_number[i] < '0' || rp.contact_number[i] > '9')
                {
                    cout << "Only digits allowed\n";
                    break;
                }
            }

            if (i == 11) break;
        }

        // Department selection
        int depChoice;
        cout << "Select department:\n1. General Physician\n2. Neurology\n3. Orthopedics\n4. Psychiatry\n5. ENT\n6. Denatistry \nChoice: ";
        cin >> depChoice;
        switch (depChoice)
        {
        case 1:
        {
            rp.department = "General Physician";
            break;
        }
        case 2:
        {
            rp.department = "Neurology";
            break;
        }
        case 3:
        {
            rp.department = "Orthopedics";
            break;
        }
        case 4:
        {
            rp.department = "Psychiatry";
            break;
        }
        case 5:
        {
            rp.department = "ENT";
            break;
        }
        case 6:
        {
            rp.department = "Denatistry";
            break;
        }
        default:
        {
            cout << "Invalid OPD appointment";
            break;
        }
        }
        regular_queue.push(rp);
        opd_patients.push_back({ rp.id, rp });
        cout << "OPD patient " << rp.name << " added to " << rp.department << " department.\n";

        all_patients.push_back({ rp.id,rp });

    }


    void treatOPD()
    {
        if (!regular_queue.empty())
        {
            patient rp = regular_queue.front();
            regular_queue.pop();
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
            patient ep = emergency_queue.top();
            emergency_queue.pop();
            cout << "Emergency patient treated: " << ep.name << " whose sensitivity was " << ep.sensitivity << endl;
        }
        else
        {
            cout << "No emergency patients available.\n";
        }
    }

    void accessPatientByID(int p_id)
    {
        if (p_id <= 0 || p_id >= id_counter)
        {
            cout << "Invalid ID!\n";
            return;
        }
        if (p_id <= opd_patients.size())
        {
            patient p = opd_patients[p_id - 1].second;
            cout << "[OPD] Patient name: " << p.name << " Department: " << p.department << endl;
        }
        else
        {
            patient p = emergency_patients[p_id - opd_patients.size() - 1].second;
            cout << "[Emergency] Patient name: " << p.name << " Sensitivity: " << p.sensitivity << endl;
        }
    }

};



// ID-based access for OPD


// ID-based access for emergency





// ---------------- TREAT OPD ----------------


// ---------------- TREAT EMERGENCY ----------------




int main()
{
    Patient_Handler ph;
    ph.addOPD();
    ph.addEmergency();
    ph.write_to_file("1.csv");
}
