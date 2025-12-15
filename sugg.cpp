#include<iostream>
#include<map>
#include<fstream>

using namespace std;

struct Vertice
{
    int in_degree;
    int out_degree;
    map<string, pair<double, Vertice*>> adj_vertices;
    string data;
    string id;
    double weight;

    Vertice(string id = "", string data = "")
    {
        this->id = id;
        this->data = data;
        in_degree = out_degree = 0;
    }
};


class Recom_System
{
private:
    Recom_Graph symtoms_to_diseases;
    Recom_Graph disease_to_medicine;

public:
    Recom_System()
    {

    }

    void read_symtoms_from_file(string filename)
    {
        ifstream file(filename);


    }
};

class Recom_Graph
{
private:
    map<string, Vertice*> vertices;

public:

    void add_vertice(string src, string dest, double weight)
    {
        if (vertices.find(src) == vertices.end())
        {
            Vertice* src_vertice = new Vertice(src);
            vertices[src] = src_vertice;
        }
        if (vertices.find(dest) == vertices.end())
        {
            Vertice* dest_vertice = new Vertice(dest);
            vertices[dest] = dest_vertice;
        }

        if (vertices[src]->adj_vertices.count(dest) == 0)
        {
            vertices[src]->adj_vertices.insert({ dest,{weight,vertices[dest]} });
            vertices[dest]->adj_vertices.insert({ src,{weight,vertices[src]} });

            vertices[src]->in_degree++;
            vertices[src]->out_degree++;
            vertices[dest]->in_degree++;
            vertices[dest]->out_degree++;
        }
    }

    void print(string id)
    {
        for (auto i : vertices[id]->adj_vertices)
        {
            cout << i.first << " " << i.second.first << endl;
        }
    }

    string recommend(string id)
    {
        string max_sim_id;
        double max_sim = 0;;
        for (auto i : vertices[id]->adj_vertices)
        {
            if (i.second.first > max_sim)
            {
                max_sim = i.second.first;
                max_sim_id = i.first;
            }
        }
        return max_sim_id;
    }
};