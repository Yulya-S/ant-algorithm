#include <iostream>
#include <fstream>
#include <string>
#include "Graph.h"
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
	ofstream out;

	RenderWindow window(VideoMode(1280, 540), "ANT");
	Font font;
	font.loadFromFile("arial.ttf");

    char inFileName[140] = { "graph.txt" };
    Graph graph;
    graph.makeGraph(inFileName);
	std::cout << "Создан граф:" << endl;
	for (auto i : graph.nodes)
		for (int j = 0; j != i->neighbours.size(); j++) {
			std::cout << i->getName() << " " << i->neighbours[j]->getName() << " " << i->edgePrices[j] << endl;
		}

	Vertex line[] = {
		Vertex(Vector2f(640, 0), Color(180, 180, 180)),
		Vertex(Vector2f(640, 540), Color(180, 180, 180))
	};

	Anthill anthill(graph);
	anthill.read_conf_file();

	Text text;
	text.setFont(font);
	text.setCharacterSize(24);
	text.setFillColor(Color(0, 0, 0));
	text.setPosition(20, 20);

	bool gamePause = false;
	bool step = false;
	bool end = false;
	int iter = 0;

	while (window.isOpen())
	{
		// click handler
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyReleased && event.key.code == Keyboard::P)
				gamePause = !gamePause;
			if (event.type == sf::Event::KeyReleased && event.key.code == Keyboard::S) {
				step = !step;
				gamePause = false;
			}
		}

		if (!gamePause && !end) {
			if (step) gamePause = true;
			if (anthill.process()) {
				if (anthill.great_ant_result >= anthill.stop_count || anthill.max_ant_count <= anthill.ant_number) {
					cout << "Найден кратчайший путь: " << anthill.min_value << " за " << anthill.ant_number << " муравья" << endl;
					//break;

					if (iter < 10) {
						iter += 1;
						out.open("results.txt", ios::app);
						out << anthill.min_value << " " << anthill.ant_number << endl;
						out.close();

						anthill = Anthill(graph);
						anthill.read_conf_file();
						anthill.file_name = "result" + to_string(iter) + ".txt";
					}
					else break;
				}
			}
		}

		// interface display
		window.clear(Color(255, 255, 255));
		if (end) window.draw(text);
		else {
			window.draw(line, 2, Lines);
			anthill.draw(window, font, true);
			graph.draw(window, font);
		}
		window.display();
	}
}