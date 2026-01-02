#pragma once
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

struct Vertice {
    string id;
    map<string, pair<double, Vertice*>> adj_vertices;

    Vertice(string id = "") {
        this->id = id;
    }
};

class Recom_Graph {
    private:
    map<string, Vertice*> vertices;

public:
    void add_edge(string src, string dest, double weight);
     string recommend(vector<string> user_symptoms);
};

class Recom_System {
private:
    Recom_Graph graph;
    map<string, vector<string>> precautions;
    map<string, vector<string>> medicines;
    string symptoms_file;
    string precautions_file;
    string medicines_file;

    public:
     Recom_System(string symptoms_file="", string precautions_file="",string medicines_file="");
     void set_file(string symptoms_file, string precautions_file,string medicines_file);
     void load_data();
     string get_condition(vector<string> symptoms);
     vector<string> get_precautions(string disease);
     vector<string> get_medicines(string disease);
};