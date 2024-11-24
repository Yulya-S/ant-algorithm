#include "Graph.h"
#include <cmath>
#include <fstream>

// ant step
void Ant::run(Graph& graph, float alpha, float beta) {
	// select the node from which the ant will go
	if (trail.size() == 0) {
		int rr = rand() % graph.nodes.size();
		trail.push_back(graph.nodes[rr]->getName());
	}
	
	// check if the ant has passed all the nodes
	if (trail.size() == graph.nodes.size()) {
		end_trail = true;
		if (!(&graph.get_node_by_name(trail[trail.size() - 1]))->is_neighbour(trail[0]))
			impasse = true;
		else {
			Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));
			int end_node = node->neighbour_index_by_name(trail[0]);
			trail_summ += node->edgePrices[end_node];
			trail.push_back(trail[0]);
		}
		return;
	}

	float summ = 0.;
	// calculate the denominator
	float denom = denominator(graph, trail[trail.size() - 1], alpha, beta);
	if (denom == 0) {
		end_trail = true;
		impasse = true;
		return;
	}

	Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));
	float random = rand() % 100;
	random /= 100;

	for (int i = 0; i < node->neighbours.size(); i++)
		if (!name_in_trail(node->neighbours[i]->getName())) {
			// calculate the denominator
			summ += float((float(pow(node->pheromones[i], alpha) * pow((1. / node->edgePrices[i]), beta))) / denom);
			// check if random is in the current period
			if (summ >= random) {
				trail_summ += node->edgePrices[i];
				trail.push_back(node->neighbours[i]->getName());
				return;
			}
		}
}

// denominator calculation
float Ant::denominator(Graph& graph, const string node_name, float alpha, float beta) {
	float summ = 0.;
	Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));
	for (int i = 0; i < node->neighbours.size(); i++)
		if (!name_in_trail(node->neighbours[i]->getName()))
			summ += float(pow(node->pheromones[i], alpha) * pow((1. / node->edgePrices[i]), beta));
	return summ;
}

// checking that all nodes have been visited
bool Ant::is_all_trail(Graph& graph, const string node_name) {
	for (auto i : graph.nodes[graph.get_index_node_by_name(trail[trail.size() - 1])]->neighbours)
		if (!name_in_trail(i->getName())) return false;
	return true;
}

// checking whether the node has already been visited
bool Ant::name_in_trail(const string node_name) {
	for (auto i : trail) if (i == node_name) return true;
	return false;
}

// displaying the ant's path on the screen
void Ant::draw(RenderWindow& window, Graph& graph) {
	if (trail.size() >= 2)
		for (int i = 0; i < trail.size() - 1; i++) {
			Node n1 = (&graph)->get_node_by_name(trail[i]);
			Node n2 = (&graph)->get_node_by_name(trail[i + 1]);
			Vertex line[] = {
				Vertex(Vector2f(n1.x + 665, n1.y + 45), Color::Red),
				Vertex(Vector2f(n2.x + 665, n2.y + 45), Color::Red)
			};
			window.draw(line, 2, Lines);
		}
}


Anthill::Anthill(Graph start_graph) {
	srand((unsigned)time(0));
	graph.copy(start_graph);
	graph_copy.copy(start_graph);
	text.setCharacterSize(24);
	text.setFillColor(Color::Black);
	text.setStyle(Text::Bold);
}

bool Anthill::process() {
	// creating the first ant
	if (ant_number == 0) {
		ant = Ant();
		ant_number += 1;
	}
	if (ant.end_trail) {
		out.open(file_name, ios::app);

		// check if the ant has reached a dead end
		if (!ant.impasse) {
			// recalculate the amount of pheromones on the ribs
			if (ant.trail_summ < min_value) {
				min_value = ant.trail_summ;
				pheramone_recalculation(10);
			}
			else pheramone_recalculation();
			// copy the graph along which the ant passed
			graph.copy(graph_copy);
			// checking the length of the path traveled by the ant
			if (ant.trail_summ == min_value) great_ant_result += 1;
			else great_ant_result = 0;
			last_value = ant.trail_summ;
			out << last_value << " " << ant_number << "\n";
			out.close();
		}
		else {
			last_value = -1;
			great_ant_result = 0;
		}
		// create a new ant
		graph_copy.copy(graph);
		ant = Ant();
		ant_number += 1;
		return true;
	}
	ant.run(graph_copy, alpha, beta);
	return false;
}

// function for recalculating the number of pheramones in a graph
void Anthill::pheramone_recalculation(int factor) {
	for (auto i : graph_copy.nodes) for (int l = 0; l < i->neighbours.size(); l++) {
		float delta_t = 0.;
		for (int z = 0; z < ant.trail.size() - 1; z++)
			if (ant.trail[z] == i->getName() && ant.trail[z + 1] == i->neighbours[l]->getName()) {
				delta_t = float(1. / ant.trail_summ) * float(factor);
				break;
			}
		i->pheromones[l] = float(1. - evaporation_rate) * i->pheromones[l] + delta_t;
	}
}

// interface display
void Anthill::draw(RenderWindow& window, Font font, const bool draw_pheromones) {
	// draw_pheromones = true - displaying the amount of pheramones
	// draw_pheromones = false - display the best path
	
	text.setFont(font);

	text.setPosition(10, 500);
	text.setString("min train: " + to_string(min_value));
	window.draw(text);

	text.setPosition(900, 500);
	text.setString("last summ: " + to_string(last_value));
	window.draw(text);

	text.setPosition(650, 500);
	text.setString("ant number: " + to_string(ant_number));
	window.draw(text);

	text.setPosition(650, 10);
	text.setString("current path");
	window.draw(text);

	text.setPosition(10, 10);
	text.setString("minimum path");
	window.draw(text);

	text.setPosition(880, 10);
	text.setString("number ant with great result: "+ to_string(great_ant_result));
	window.draw(text);

	if (draw_pheromones) {
		for (auto i : (&graph)->nodes)
			for (int l = 0; l < i->neighbours.size(); l++) {
				Node n2 = (&graph)->get_node_by_name(i->neighbours[l]->getName());
				int color = 255 * float(i->pheromones[l]);
				Vertex line[] = {
					Vertex(Vector2f((*i).x + 25, (*i).y + 45), Color(255, 255 - color, 255)),
					Vertex(Vector2f(n2.x + 25, n2.y + 45), Color(255, 255 - color, 255))
				};
				window.draw(line, 2, Lines);
			}
	}
	else {
		for (int i = 0; i < min_trail.size(); i++) {
			Node n1 = (&graph)->get_node_by_name(min_trail[i]);
			int i2 = i + 1;
			if (i2 >= min_trail.size()) i2 = 0;
			Node n2 = (&graph)->get_node_by_name(min_trail[i2]);

			Vertex line[] = {
				Vertex(Vector2f(n1.x + 25, n1.y + 45), Color(0, 0, 0)),
				Vertex(Vector2f(n2.x + 25, n2.y + 45), Color(0, 0, 0))
			};

			window.draw(line, 2, Lines);
		}
	}

	if (ant_number > 0)
		ant.draw(window, graph);
}

// reading settings
void Anthill::read_conf_file() {
	string text;
	ifstream in("conf.txt");
	in >> text;
	alpha = stof(text);
	in >> text;
	beta = stof(text);
	in >> text;
	evaporation_rate = stof(text);
	in >> text;
	stop_count = stoi(text);
	in >> text;
	max_ant_count = stoi(text);
}