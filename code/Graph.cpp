#include "Graph.h"
#include <fstream>
using namespace std;

// Класс Node
Node::Node(const string& text, int coord_x, int coord_y) {
	name = text;
	x = coord_x;
	y = coord_y;

	shape = CircleShape(float(5));
	shape.setFillColor(Color::Black);

	shape2 = (*new CircleShape(shape));
	shape.setPosition(float(coord_x + 20), float(coord_y + 40));
	shape2.setPosition(float(coord_x + 660), float(coord_y + 40));

	txt1.setCharacterSize(14);
	txt1.setFillColor(Color(155, 155, 155));
	txt1.setString(name);

	txt2 = (*new Text(txt1));
	txt1.setPosition(x + 30, y + 40);
	txt2.setPosition(x + 670, y + 40);
}

void Node::addNeighbour(Node* neighbour, const int edgePrice, const float pheromone) {
	for (int i = 0; i != neighbours.size(); i++)
		if (neighbours[i]->getName() == neighbour->getName()) return;
	neighbours.push_back(neighbour);
	edgePrices.push_back(edgePrice);
	pheromones.push_back(pheromone);
}

void Node::removeNeighbour(Node* neighbour) {
	int i = 0;
	for (int i = 0; i != neighbours.size(); i++)
		if (neighbours[i]->getName() == neighbour->getName()) {
			neighbours.erase(neighbours.begin() + i);
			edgePrices.erase(edgePrices.begin() + i);
			return;
		}
}

int Node::neighbour_index_by_name(const string name) {
	for (int i = 0; i < neighbours.size(); i++) if (neighbours[i]->getName() == name)
		return i;
	return -1;
}

bool Node::is_neighbour(const string name) {
	for (auto i : neighbours) if (i->getName() == name) return true;
	return false;
}

void Node::draw(RenderWindow& window, Font font) {
	txt1.setFont(font);
	txt2.setFont(font);
	window.draw(shape);
	window.draw(shape2);
	window.draw(txt1);
	window.draw(txt2);
}

// Класс Graph
void Graph::addNode(Node* node) {
	for (auto it : nodes) if (it->getName() == node->getName()) return;
	nodes.push_back(node);
}

void Graph::removeNode(Node* node) {
	for (int i = 0; i < nodes.size(); i++) if (nodes[i]->getName() == node->getName()) {
		nodes[i]->removeNeighbour(node);
		nodes.erase(nodes.begin() + i);
		return;
	}
}

void Graph::addEdge(Node* begin, Node* end, const int edgePrice, const float pheromone) {
	for (auto it : nodes) if (it->getName() == begin->getName()) {
		begin = it;
		break;
	}
	for (auto it : nodes) if (it->getName() == end->getName()) {
		end = it;
		break;
	}
	begin->addNeighbour(end, edgePrice, pheromone);

}

void Graph::removeEdge(Node* begin, Node* end) {
	begin->removeNeighbour(end);
	end->removeNeighbour(begin);
}

void Graph::makeGraph(char* fileName) {
	ifstream in(fileName);
	string a, b, price;

	srand((unsigned)time(0));

	while (!in.eof()) {
		in >> a >> b >> price;
		Node* node1 = new Node(a, int(rand() % 600), int(rand() % 450));
		Node* node2 = new Node(b, int(rand() % 600), int(rand() % 450));
		addNode(node1);
		addNode(node2);

		addEdge(node1, node2, stoi(price));
	}
	in.close();
}

int Graph::get_index_node_by_name(const string name) {
	for (int i = 0; i < nodes.size(); i++) if (nodes[i]->getName() == name) return i;
	return -1;
}

Node& Graph::get_node_by_name(const string name) {
	for (auto i : nodes) if (i->getName() == name) return *i;
}

void Graph::clear() {
	for (auto i : nodes) removeNode(i);
}

void Graph::copy(Graph& graph) {
	nodes.clear();
	for (auto i : graph.nodes) {
		Node* node = new Node(i->getName(), i->x, i->y);
		addNode(node);
	}
	for (auto i : graph.nodes) for (int j = 0; j < i->neighbours.size(); j++) {
		Node* node1 = (&get_node_by_name(i->neighbours[j]->getName()));
		Node* node2 = (&get_node_by_name(i->getName()));
		addEdge(node1, node2, i->edgePrices[j], i->pheromones[j]);
	}
}

bool nodeInGraph(const Graph start_graph, const string& nodeName) {
	for (auto it : start_graph.nodes)
		if (it->getName() == nodeName) return true;
	return false;
}