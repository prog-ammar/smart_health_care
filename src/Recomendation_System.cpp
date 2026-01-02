#include "Recomendation_System.h"


Recom_System::Recom_System(string symptoms_file, string precautions_file,string medicines_file):symptoms_file(symptoms_file),precautions_file(precautions_file),medicines_file(medicines_file) {}

void Recom_System::set_file(string symptoms_file, string precautions_file,string medicines_file)
{
    this->symptoms_file=symptoms_file;
    this->precautions_file=precautions_file;
    this->medicines_file=medicines_file;
}

void Recom_System::load_data() {
     
     ifstream s_file(symptoms_file);
     string line;
     getline(s_file, line); 

     while (getline(s_file, line)) {
         stringstream ss(line);
         string disease, symptom;
         getline(ss, disease, ',');

         while (getline(ss, symptom, ',')) {
             
             symptom.erase(0, symptom.find_first_not_of(" "));
             symptom.erase(symptom.find_last_not_of(" ") + 1);
             
             if (!symptom.empty()) {
                 graph.add_edge(disease, symptom, 1.0);
             }
         }
     }

     
     ifstream p_file(precautions_file);
     getline(p_file, line); 
     while (getline(p_file, line)) {
         stringstream ss(line);
         string disease, prec;
         getline(ss, disease, ',');
         while (getline(ss, prec, ',')) {
             if (!prec.empty()) precautions[disease].push_back(prec);
         }
     }

     ifstream m_file(medicines_file);
     getline(m_file, line);         
     while (getline(m_file, line)) {
             stringstream ss(line);
             string disease, meds_part;
             getline(ss, disease, ',');
             getline(ss, meds_part);

             disease.erase(0, disease.find_first_not_of(" "));
             disease.erase(disease.find_last_not_of(" ") + 1);

             meds_part.erase(0, meds_part.find_first_not_of("\""));
             meds_part.erase(meds_part.find_last_not_of("\"") + 1);

             stringstream ss_meds(meds_part);
             string med;
             while (getline(ss_meds, med, ',')) {
                 
                 if (!med.empty()) medicines[disease].push_back(med);
             }
     }

    }

  string Recom_System::get_condition(vector<string> symptoms) {
     return graph.recommend(symptoms);
 }

 vector<string> Recom_System::get_precautions(string disease) {
     if (precautions.count(disease)) return precautions[disease];
     return {};
 }

 vector<string> Recom_System::get_medicines(string disease) {
     if (medicines.count(disease)) return medicines[disease];
     return {};
 }

  void Recom_Graph::add_edge(string src, string dest, double weight) {
     
     if (vertices.find(src) == vertices.end()) vertices[src] = new Vertice(src);
     if (vertices.find(dest) == vertices.end()) vertices[dest] = new Vertice(dest);

     
     vertices[src]->adj_vertices[dest] = {weight, vertices[dest]};
     vertices[dest]->adj_vertices[src] = {weight, vertices[src]};
 }

 string Recom_Graph::recommend(vector<string> user_symptoms) {
     map<string, double> disease_scores;
     
     for (const string& symptom : user_symptoms) {
         if (vertices.count(symptom)) {
             
             for (auto const& i:  vertices[symptom]->adj_vertices) {
                 disease_scores[i.first] += i.second.first;
             }
         }
     }

    
     string best_match = "";
     double max_score = -1;
     for (auto const& [disease, score] : disease_scores) {
         if (score > max_score) {
             max_score = score;
             best_match = disease;
         }
     }
     return best_match;
 }