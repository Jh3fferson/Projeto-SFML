#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <utility>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

// ---------------------- Objetivos --------------------------------------------------------------------------------------------------- 
// Modularizar melhor o código.
// Diminuir o uso de if.
// Talvez refazer tudo e melhorar as estruturações, pois parecem difícies de se entender. 
// Adicionar função para que objetos colidam entre si.
// Criar novas direções, diagonal UpLeft, UpRight, DownLeft e DownRight. Usos potenciais : movimento e colisão.
// Adicionar função de escalar em blocos.
// Diminuir a quantidade de variáveis na class Game, passa-las para um struct?
// itens que se movimentam aleatoriamente.
// Talvez deixar o Player fora de um Map.
// struct diferente para Player?
// Classes separadas para itens com hitBox?
// Criar botões pelo mapa que façam coisas e tentar guardar o que cada um faz em um arquivo binário.
// Possível bug: quinas e jumpStrengh "A quina se entende como player no chão e a velocityY se iguala a zero";
// Bug: quina se entende como chao quando pressionado em direção ao objeto, logo se faz possível ficar em cima de ["wall"].
// Bug: Ao mover a tela ou diminuir, o boneco tende a se mover ou parar de entender as colisões.
// ------------------------------------------------------------------------------------------------------------------------------------


enum WhereDirection { None, Left, Right, Up, Down};

struct Transform {
	sf::Vector2f size;
	sf::Vector2f position;
	sf::Color color;
};

struct Movement {
	WhereDirection directionMove = None;
	float velocity = { 0.f };
	sf::Vector2f distance = {0.f, 0.f};
	bool jump = false;
};

struct Collision {
	bool canCollide = true;
	bool canMove = false;
	bool colliding = false;
	WhereDirection directionColision = None;
};

struct Interaction {
	bool interacting = false;
};

struct ItemData {
	Transform transform;
	Movement movement;
	Collision collision;
	Interaction interaction;
};
void saveMap(const std::map<std::string, ItemData>& itemRectangles, const std::string& filename);

class Game
{
public:
	Game();
	void run();

private:
	void loadMap(const std::string& filename);
	void createLevel();
	void processEvents();
	void update(sf::Time delta);
	void updateCamera();
	void updateInput(float dt);
	void updateGravity(float dt);
	void checkCollisions();
	void mapSpecials(float dt);
	void movementBlocks(std::string name);
	void updateMovement();
	void resetMap();
	void resetPlayer();
	void render();

	sf::RenderWindow window;

	std::map<std::string, ItemData> mapItems;
	std::map<std::string, ItemData> playerThings;
	std::map<std::string, sf::RectangleShape> mapShape;
	std::map<std::string, sf::RectangleShape> playerShape;
	std::map<std::string, bool> pressedKeys;
	WhereDirection playerDirection = None;

	float velocityY = 0.f;
	float gravity = 980.f; // ou 800.f, ajuste como quiser
	float jumpStrength = 900.f;
	float speed = 300.f;
	std::string actualMap = "level_1";
	int level = 1;
	bool shouldResetMap = false;
	sf::Vector2f playerMovement = { 0.f, 0.f };

};
