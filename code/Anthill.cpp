#include "Graph.h"
#include <cmath>
#include <fstream>

// Класс Ant
void Ant::run(Graph& graph) {
	// Муравей выбежал из домика
	if (trail.size() == 0) {
		int rr = rand() % graph.nodes.size();
		trail.push_back(graph.nodes[rr]->getName());
	}

	pheromone_recalculation(graph, trail[trail.size() - 1]);

	// Муравей закончил бежать
	// он оббежал все точки
	if (trail.size() == graph.nodes.size()) {
		end_trail = true;
		if (!(&graph.get_node_by_name(trail[trail.size() - 1]))->is_neighbour(trail[0])) {
			// вернулся домой
			impasse = true;
		}
		else {
			// вернулся домой
			Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));
			int neigh = node->neighbour_index_by_name(trail[0]);
			trail_summ += node->edgePrices[neigh];
			node->pheromones[neigh] += float(1. / node->edgePrices[neigh]);
			trail.push_back(trail[0]);
		}
		return;
	}
	// он в тупике
	if (is_all_trail(graph, trail[trail.size() - 1])) {
		end_trail = true;
		impasse = true;
		return;
	}

	float summ = 0.;
	float denom = denominator(graph, trail[trail.size() - 1]);
	float zero_value = ((1 - summ_pheromone(graph, trail[trail.size() - 1])) / zero_trail(graph, trail[trail.size() - 1]));
	
	Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));

	float random = rand() % 100;
	random /= 100;

	// проверяем в какой диаппазон входит значение random
	for (int i = 0; i < node->neighbours.size(); i++)
		if (!name_in_trail(node->neighbours[i]->getName())) {
			if (node->pheromones[i] == 0 && zero_value != 0) summ += zero_value;
			else
				summ += ((float(pow(node->pheromones[i], (1. - greed)) * pow((1. / node->edgePrices[i]), greed))) / denom);
			if (summ >= random) {	
				node->pheromones[i] += float(1. / node->edgePrices[i]);
				trail_summ += node->edgePrices[i];
				trail.push_back(node->neighbours[i]->getName());
				return;
			}
		}
}

void Ant::pheromone_recalculation(Graph& graph, const string node_name) {
	Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));
	for (int i = 0; i < node->neighbours.size(); i++)
		if (!name_in_trail(node->neighbours[i]->getName()))
			node->pheromones[i] *= float(1. - evaporation_rate);
}

float Ant::denominator(Graph& graph, const string node_name) {
	float summ = 0.;
	Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));
	for (int i = 0; i < node->neighbours.size(); i++)
		if (!name_in_trail(node->neighbours[i]->getName())) {
			summ += float(pow(node->pheromones[i], (1. - greed)) * pow((1. / node->edgePrices[i]), greed));
		}
	return summ;
}

float Ant::summ_pheromone(Graph& graph, const string node_name) {
	float summ = 0.;
	float denom = denominator(graph, node_name);
	Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));
	for (int i = 0; i < node->neighbours.size(); i++)
		if (!name_in_trail(node->neighbours[i]->getName()))
			summ += float(pow(node->pheromones[i], (1. - greed)) * pow((1. / node->edgePrices[i]), greed));
	return summ;
}

int Ant::zero_trail(Graph& graph, const string node_name) {
	int zero = 0;
	Node* node = (&graph.get_node_by_name(trail[trail.size() - 1]));
	for (int i = 0; i < node->neighbours.size(); i++)
		if (!name_in_trail(node->neighbours[i]->getName()) && node->pheromones[i] == 0.)
			zero += 1;
	return zero;
}

bool Ant::is_all_trail(Graph& graph, const string node_name) {
	for (auto i : graph.nodes[graph.get_index_node_by_name(trail[trail.size() - 1])]->neighbours)
		if (!name_in_trail(i->getName())) return false;
	return true;
}

bool Ant::name_in_trail(const string node_name) {
	for (auto i : trail) if (i == node_name) return true;
	return false;
}

void Ant::draw(RenderWindow& window, Graph& graph) {
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


// Класс Anthill
Anthill::Anthill(Graph start_graph) {
	srand((unsigned)time(0));

	graph.copy(start_graph);
	graph_copy.copy(start_graph);
	
	text.setCharacterSize(24);
	text.setFillColor(Color::Black);
	text.setStyle(Text::Bold);
}

bool Anthill::process() {
	if (ant_number == 0) {
		ant = Ant(evaporation_rate, greed);
		ant_number += 1;
	}
	if (ant_number > stop_ant_count) return true;
	if (ant.end_trail) {
		if (!ant.impasse) {
			graph.copy(graph_copy);
			if (ant.trail_summ < min_value) {
				min_value = ant.trail_summ;
				min_trail = ant.trail;
				great_ant_result = 0;
			}
			if (ant.trail_summ == min_value && ant_number > stop_count * 3) {
				great_ant_result += 1;
				if (great_ant_result > stop_count) return true;
			}
			if (ant.trail_summ > min_value)
				great_ant_result = 0;
		}
		ant_number += 1;
		graph_copy.copy(graph);
		ant = Ant(evaporation_rate, greed);
	}

	ant.run(graph_copy);
	return false;
}

void Anthill::draw(RenderWindow& window, Font font, const bool draw_pheromones) {
	text.setFont(font);

	text.setPosition(10, 500);
	text.setString("min train: " + to_string(min_value));
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

				int color = int(i->pheromones[l] * 100);

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
			if (i + 1 >= min_trail.size()) i = 0;
			else i += 1;
			Node n2 = (&graph)->get_node_by_name(min_trail[i]);

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

void Anthill::read_conf_file() {
	string text;
	ifstream in("conf.txt");

	in >> text;
	greed = stof(text);
	in >> text;
	evaporation_rate = stof(text);
	in >> text;
	stop_count = stoi(text);
	in >> text;
	stop_ant_count = stoi(text);
}