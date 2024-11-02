#ifndef _GRAPH_
#define _GRAPH_

#include <vector>
#include <string>
#include <iostream>
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
	void addNeighbour(Node* neighbour, const int edgePrice = 0, const float pheromone = 0.);
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
	void addEdge(Node* begin, Node* end, const int edgePrice = 0, const float pheromone = 0.);
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
	float evaporation_rate;

	float greed;

	Ant(const float rate = 0.5, const float Greed = 1.) { evaporation_rate = rate; greed = Greed; }
	void run(Graph& graph);
	void pheromone_recalculation(Graph& graph, const string node_name);
	float denominator(Graph& graph, const string node_name);
	float summ_pheromone(Graph& graph, const string node_name);
	int zero_trail(Graph& graph, const string node_name);
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
	int great_ant_result = 0;

	float greed = 0.5;
	float evaporation_rate = 0.5;
	int stop_count = 100;
	int stop_ant_count = 5000;

public:
	int ant_number = 0;
	int min_value = 99999999;
	int last_value = 99999999;
	vector<string> min_trail;

	Anthill(Graph start_graph);
	bool process();
	void fixing_best_path(int fix_value = 10);
	void draw(RenderWindow& window, Font font, const bool draw_pheromones = false);
	void read_conf_file();
};

#endif