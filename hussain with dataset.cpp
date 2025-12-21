#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <map>
using namespace std;

// ---------------- Structures ----------------
struct Patient_Symptoms
{
    int bloodPressure;
    int heartRate;
    int oxygenLevel;
    int temperature;
    int respiratoryRate;
    int consciousness;
    int bleeding;
    int pain;
    int chestPain;
    int nausea;

    Patient_Symptoms()
    {
        bloodPressure = heartRate = oxygenLevel = temperature = respiratoryRate =
            consciousness = bleeding = pain = chestPain = nausea = -1;
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
        name = blood = gender = contact_number = department = id = "";
        age = -1;
    }
};
struct OPD_Patient : public Patient {};
struct Emergency_Patient : public Patient
{
    string relative_number;
    int sensitivity;
    Patient_Symptoms symptoms;
    int arrivalIndex;

    Emergency_Patient()
    {
        sensitivity = -1; relative_number = ""; arrivalIndex = -1;
    }
};

struct compare_sensitivity
{
    bool operator()(Emergency_Patient const& p1, Emergency_Patient const& p2)
    {
        if (p1.sensitivity == p2.sensitivity)
        {
            return p1.arrivalIndex > p2.arrivalIndex;
        }
        else
        {
            return p1.sensitivity < p2.sensitivity;
        }
    }
};

class Patient_Handler
{
private:
    queue<OPD_Patient> opd_queue;
    map<string, OPD_Patient> opd_patients;
    priority_queue<Emergency_Patient, vector<Emergency_Patient>, compare_sensitivity> emergency_queue;
    map<string, Emergency_Patient> emergency_patients;
    int emergencyIndex = 0;

public:

    void load_OPD(string filename)
    {
        ifstream file(filename);
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
            opd_queue.push(p);
        }
        file.close();
    }

    void load_Emergency(string filename)
    {
        ifstream file(filename);
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
            emergency_queue.push(p);
        }
        file.close();
    }

    // ---------- Treat Patients ----------
    void treat_OPD()
    {
        if (!opd_queue.empty())
        {
            OPD_Patient p = opd_queue.front();
            opd_queue.pop();
            cout << "OPD patient treated: " << p.name << " | Department: " << p.department << endl;
        }
        else
        {
            cout << "No OPD patients.\n";
        }
    }

    void treat_Emergency()
    {
        if (!emergency_queue.empty())
        {
            Emergency_Patient p = emergency_queue.top();
            emergency_queue.pop();
            cout << "Emergency patient treated: " << p.name << " | Sensitivity: " << p.sensitivity << endl;
        }
        else
        {
            cout << "No Emergency patients.\n";
        }
    }

    // ---------- Add Patients ----------
    void add_OPD_patient(const OPD_Patient& p)
    {
        opd_patients[p.id] = p;
        opd_queue.push(p);
        write_OPD_file("OPD.csv", opd_patients);
        cout << "OPD patient added: " << p.name << endl;
    }

    void add_Emergency_patient(Emergency_Patient p)
    {
        p.arrivalIndex = emergencyIndex++;
        emergency_patients[p.id] = p;
        emergency_queue.push(p);
        write_Emergency_file("Emergency.csv", emergency_patients);
        cout << "Emergency patient added: " << p.name << endl;
    }

    // ---------- Write CSVs ----------
    void write_OPD_file(const string& filename, map<string, OPD_Patient>& patients)
    {
        ofstream file(filename);
        file << "patient_id,name,age,blood,gender,contact_number,department\n";
        for (auto& i : patients)
        {
            OPD_Patient& p = i.second;
            file << p.id << "," << p.name << "," << p.age << "," << p.blood << "," << p.gender << "," << p.contact_number << "," << p.department << "\n";
        }
        file.close();
    }

    void write_Emergency_file(const string& filename, map<string, Emergency_Patient>& patients)
    {
        ofstream file(filename);
        file << "patient_id,name,age,blood,gender,relative_number,sensitivity,bloodPressure,heartRate,oxygenLevel,temperature,respiratoryRate,consciousness,bleeding,pain,chestPain,nausea\n";
        for (auto& i : patients)
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
};

int main()
{
    Patient_Handler ph;

    ph.load_OPD("OPD.csv");
    ph.load_Emergency("Emergency.csv");

    cout << "\n=== Treating OPD Patients ===\n";
    ph.treat_OPD();
    ph.treat_OPD();
    ph.treat_OPD();
    ph.treat_OPD();

    cout << "\n=== Treating Emergency Patients ===\n";
    ph.treat_Emergency();
    ph.treat_Emergency();
    ph.treat_Emergency();
    ph.treat_Emergency();

    return 0;
}
