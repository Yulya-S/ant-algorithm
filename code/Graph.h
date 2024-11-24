#ifndef _GRAPH_
#define _GRAPH_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class Node {
private:
	string name;
	CircleShape shape;
	CircleShape shape2;
	Text txt1;
	Text txt2;
public:
	vector<int> edgePrices;
	vector<float> pheromones;
	vector<Node*> neighbours;
	int x;
	int y;

	Node(const string& text, int coord_x, int coord_y);
	void addNeighbour(Node* neighbour, const int edgePrice = 0, const float pheromone = 1.);
	void removeNeighbour(Node* neighbour);
	int neighbour_index_by_name(const string name);
	const string& getName() const { return name; }
	bool is_neighbour(const string name);
	void draw(RenderWindow& window, Font font);
	friend class Graph;
};

class Graph {
public:
	vector<Node*> nodes;

	void addNode(Node* node);
	void removeNode(Node* node);
	void addEdge(Node* begin, Node* end, const int edgePrice = 0, const float pheromone = 1.);
	void removeEdge(Node* begin, Node* end);
	void makeGraph(char* fileName);
	int get_index_node_by_name(const string name);
	Node& get_node_by_name(const string name);
	void clear();
	void draw(RenderWindow& window, Font font) { for (auto i : nodes) i->draw(window, font); }
	void copy(Graph& graph);
};

bool nodeInGraph(const Graph start_graph, const string& nodeName);

class Ant {
public:
	vector<string> trail;
	int trail_summ = 0;
	bool end_trail = false;
	bool impasse = false;

	void run(Graph& graph, float alpha, float beta);
	float denominator(Graph& graph, const string node_name, float alpha, float beta);
	bool is_all_trail(Graph& graph, const string node_name);
	bool name_in_trail(const string node_name);
	void draw(RenderWindow& window, Graph& graph);
};

class Anthill {
private:
	Graph graph_copy;
	Graph graph;
	Text text;
	Ant ant;
	ofstream out;

	float alpha = 1;
	float beta = 1;
	float evaporation_rate = 0.5;

public:
	int ant_number = 0;
	int stop_count = 100; // number of repetitions of the best result for recovery
	int great_ant_result = 0;
	int min_value = 99999999;
	int last_value = 99999999;
	int max_ant_count = 10000;

	vector<string> min_trail;
	string file_name = "result.txt";

	Anthill(Graph start_graph);
	bool process();
	void pheramone_recalculation(int factor = 1);
	void draw(RenderWindow& window, Font font, const bool draw_pheromones = false);
	void read_conf_file();
};

#endif