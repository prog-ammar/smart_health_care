#include <iostream>
#include <string>
#include <queue>
using namespace std;

struct patient
{
    string name;
    int age;
    string blood;
    string gender;
    string contact_number;
    string relative_number;
    int sensitivity;       //0 means stable higher means critical
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
        cout << "\n5--> Treat next patient";
        cout << "\n6--> Exit";

        int option;
        cout << "\nYour choice : ";
        cin >> option;

        switch (option)
        {
        case 1:
        {
            patient rp;        // regular patient

            cout << "\n\nAdding regular patient ! ";
            cout << "\nEnter patient name : ";
            cin.ignore();
            getline(cin, rp.name);

            cout << "Enter patient age : ";
            cin >> rp.age;

            // for correct blood group
            while (true)
            {
                cout << "Enter patient blood group : ";
                cin >> rp.blood;

                if (rp.blood == "A+" || rp.blood == "A-" || rp.blood == "B+" || rp.blood == "B-" || rp.blood == "AB+" || rp.blood == "AB-" || rp.blood == "O+" || rp.blood == "O-")
                {
                    break;
                }
                else
                {
                    cout << "Invalid blood group! Valid blood groups are : A+, A-, B+, B-, AB+, AB-, O+, O-\n";
                }
            }

            cout << "Enter patient gender : ";
            cin >> rp.gender;

            //for correct number entering
            while (true)
            {
                cout << "Enter patients contact number (no space): ";
                cin >> rp.contact_number;

                if (rp.contact_number.length() != 11)
                {
                    cout << "Invalid number\nNumber must be 11 digits";
                    continue;
                }

                int i;
                for (i = 0; i < 11; i++)
                {
                    if (rp.contact_number[i] < '0' || rp.contact_number[i] > '9') {
                        cout << "Invalid input! Must contain only digits from 0 to 9.\n";
                        break;
                    }
                }

                if (i == 11) break;
            }

            // patient adding in simple queue / note that push and pop works also for stack not enque and dequeue
            regular_queue.push(rp);
            cout << "\nRegular patient " << rp.name << " added ! \n\n";
            break;
        }
        case 2:
        {
            patient ep;        // emergency patient

            cout << "\n\nAdding emergency patient !";
            cout << "\nEnter patient name : ";
            cin.ignore(); // clear newline left in buffer
            getline(cin, ep.name);

            cout << "Enter patient age : ";
            cin >> ep.age;


            // for correct blood group
            while (true)
            {
                cout << "Enter patient blood group : ";
                cin >> ep.blood;

                if (ep.blood == "A+" || ep.blood == "A-" || ep.blood == "B+" || ep.blood == "B-" || ep.blood == "AB+" || ep.blood == "AB-" || ep.blood == "O+" || ep.blood == "O-")
                {
                    break;
                }
                else
                {
                    cout << "Invalid blood group! Valid blood groups are : A+, A-, B+, B-, AB+, AB-, O+, O-\n";
                }
            }

            cout << "Enter patient gender : ";
            cin >> ep.gender;

            //for correct number entering
            while (true)
            {
                cout << "Enter patients relative contact number (no space): ";
                cin >> ep.relative_number;

                if (ep.relative_number.length() != 11)
                {
                    cout << "Invalid number\nNumber must be 11 digits";
                    continue;
                }

                int i;
                for (i = 0; i < 11; i++)
                {
                    if (ep.relative_number[i] < '0' || ep.relative_number[i] > '9')
                    {
                        cout << "Invalid input! Must contain only digits from 0 to 9.\n";
                        break;
                    }
                }

                if (i == 11) break;
            }

            cout << "Enter sensitvity level (0 for less sensitve and higher for more) : ";
            cin >> ep.sensitivity;

            //patient adding in priority queue
            emergency_queue.push(ep);
            cout << "\nEmergency patient " << ep.name << " added ! \n\n";
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
            cout << "\n\nChecking next patient next patient !";
            if (!emergency_queue.empty())
            {
                cout << "Treating Emergency patient currently";
                patient ep = emergency_queue.top();       //gives the patient entered first info
                emergency_queue.pop();
                cout << ep.name << " treated ! ";
            }

            else if (!regular_queue.empty())
            {
                cout << "Treating OPD patient currently";
                patient rp = regular_queue.front();       //gives the patient entered first info
                regular_queue.pop();
                cout << rp.name << " treated ! ";
            }
            else
            {
                cout << "No Patient are avalible to be treated ! ";
            }

            //-------------------------------------------------------------------------
            cout << "\n\nDoctor allocated from BST from zain work";
            break;
        }
        case 6:
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
