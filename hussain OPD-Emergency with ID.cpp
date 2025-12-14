#include <iostream>
#include <string>
#include <queue>
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

int id_counter = 1;
queue<patient> regular_queue;
vector<pair<int, patient>> opd_patients; // ID-based access for OPD

priority_queue<patient, vector<patient>, compare_sensitivity> emergency_queue;
vector<pair<int, patient>> emergency_patients; // ID-based access for emergency

void addOPD(queue<patient>& regular_queue)
{
    patient rp;
    rp.id = id_counter;
    id_counter++;

    cout << "\nAdding OPD patient\n";
    cout << "Enter patient name: ";
    cin.ignore();
    getline(cin, rp.name);

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

}

void addEmergency(priority_queue<patient, vector<patient>, compare_sensitivity>& emergency_queue)
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
}

// ---------------- TREAT OPD ----------------
void treatOPD(queue<patient>& regular_queue)
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

// ---------------- TREAT EMERGENCY ----------------
void treatEmergency(priority_queue<patient, vector<patient>, compare_sensitivity>& emergency_queue)
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

void accessPatientByID(int id)
{
    if (id <= 0 || id >= id_counter)
    {
        cout << "Invalid ID!\n";
        return;
    }
    if (id <= opd_patients.size())
    {
        patient p = opd_patients[id - 1].second;
        cout << "[OPD] Patient name: " << p.name << " Department: " << p.department << endl;
    }
    else
    {
        patient p = emergency_patients[id - opd_patients.size() - 1].second;
        cout << "[Emergency] Patient name: " << p.name << " Sensitivity: " << p.sensitivity << endl;
    }
}

int main()
{
    queue <patient> regular_queue;
    priority_queue <patient, vector<patient>, compare_sensitivity > emergency_queue;

    while (true)
    {
        cout << "------Welcome to hospital managment system-------\n";
        cout << "Please choose your option from menu below";
        cout << "\n1--> OPD(simple Q)";
        cout << "\n2--> Emergency(Priority Q)";
        cout << "\n3--> Available doctor";
        cout << "\n4--> Medicine recommendation";
        cout << "\n5--> Treat OPD patient";
        cout << "\n6--> Treat Emergency patient";
        cout << "\n7--> Search patient by id";
        cout << "\n8--> Exit";

        int option;
        cout << "\nYour choice : ";
        cin >> option;

        switch (option)
        {
        case 1:
        {
            //adding regular patient
            addOPD(regular_queue);
            cout << endl;
            break;
        }
        case 2:
        {
            //adding emergency patient
            addEmergency(emergency_queue);
            cout << endl;
            break;
        }
        case 3:
        {
            cout << "\n\nAvalible doctor !";
            cout << "\nZain work   ---------- edit option 5 after it ";
            break;
        }
        case 4:
        {
            cout << "\n\nAllocation medicine !";
            cout << "\nAmmar work";
            break;
        }
        case 5:
        {
            treatOPD(regular_queue);
            cout << endl;
            break;
        }
        case 6:
        {
            treatEmergency(emergency_queue);
            cout << endl;
            break;
        }
        case 7:
        {
            int search_id;
            cout << "Enter patient id : ";
            cin >> search_id;
            accessPatientByID(search_id);
            cout << endl
                break;
        }
        case 8:
        {
            cout << "\n\nExiting !";
            return 0;
        }
        default:
        {
            cout << "\n\nInvalid choice !";
        }

        }
    }
    return 0;
}
