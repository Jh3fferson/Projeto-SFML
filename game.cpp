#include "Game.hpp"


Game::Game() : window(sf::VideoMode({ 1920, 1080 }), "2D Engine") // Initialize fpsText
{
	sf::Vector2f baseMenor = { 50.f, 20.f };
	sf::Vector2f base = { 100.f, 20.f };
	sf::Vector2f baseMaior = { 150.f, 20.f };
	sf::Color corBase = sf::Color(255, 13, 5);
	sf::Color corBaseCorrer = sf::Color(57, 235, 193);
	sf::Color corBasePulo = sf::Color(101, 193, 55);
	sf::Color corParede = sf::Color(255, 153, 51);
	playerThings["player"].transform = { {20.f, 40.f}, {1200.f, 800.f}, sf::Color(144, 143, 164) };
	mapItems["ground"].transform = { {1920.f, 100.f}, {0.f, 1030.f}, sf::Color(55, 1, 51) };
	mapItems["second_ground"].transform = { {5000.f, 110.f}, {0.f, 1000.f}, sf::Color(255, 13, 51) };
	mapItems["iterect_object"].transform = { {40.f, 50.f}, {1350.f, 870.f}, sf::Color(25, 11, 51) };
	mapItems["iterect_object"].interaction.interacting = true;
	mapItems["iterect_object"].collision.canCollide = false;
	mapItems["novo"].transform = { {100.f, 20.f}, {1500.f, 970.f}, sf::Color(14, 143, 164) };
	mapItems["novo"].movement = { {Right}, 1600.f, {0.f, 2000.f} };
	mapItems["movimentavel"].transform = { {100.f, 50.f}, {1000.f, 950.f}, sf::Color(14, 143, 164) };
	mapItems["movimentavel"].collision.canMove = true;
	mapItems["movimentavel2"].transform = { {100.f, 50.f}, {800.f, 950.f}, sf::Color(14, 143, 164) };
	mapItems["movimentavel2"].collision.canMove = true;
	mapItems["movimentavel3"].transform = { {100.f, 50.f}, {1300.f, 950.f}, sf::Color(14, 143, 164) };
	mapItems["movimentavel"].collision.canMove = true;
	createLevel();
	//resetMap();
}

void Game::createLevel() {
	for (const auto& [name, rectData] : playerThings) {

		playerShape[name] = sf::RectangleShape(rectData.transform.size);
		playerShape[name].setFillColor(rectData.transform.color);
		playerShape[name].setPosition(rectData.transform.position);

	}
	for (const auto& [name, rectData] : mapItems) {

		mapShape[name] = sf::RectangleShape(rectData.transform.size);
		mapShape[name].setFillColor(rectData.transform.color);
		mapShape[name].setPosition(rectData.transform.position);
		/*saveMap(mapItems, "map_test");*/
	}
}

void Game::run()
{
	sf::Clock clock;
	float fps = 0.f;

	while (window.isOpen())
	{
		sf::Time delta = clock.restart();
		processEvents();
		update(delta);
		render();

	}
}

void Game::processEvents()
{
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			window.close();

		if (const auto* resized = event->getIf<sf::Event::Resized>())
		{
			sf::Vector2f center(
				resized->size.x / 2.f,
				resized->size.y / 2.f
			);

			sf::Vector2f size(
				static_cast<float>(resized->size.x),
				static_cast<float>(resized->size.y)
			);

			sf::View view(center, size);
			window.setView(view);
		}
	}
}

void Game::update(sf::Time delta)
{
	float dt = delta.asSeconds();
	updateInput(dt);
	updateGravity(dt);
	checkCollisions();  // Agora não precisa passar movement
	updateMovement();
	mapSpecials(dt);
	updateCamera();
	playerMovement = { 0.f, 0.f };

	if (shouldResetMap) {
		resetMap();
		shouldResetMap = false;
	}
}

void Game::updateCamera() {
	sf::Vector2f playerPos = playerShape["player"].getPosition();

	// Define a nova view centralizada no jogador
	sf::View view = window.getView();
	view.setCenter(playerPos);

	// Limitar a câmera para não sair do mapa (opcional)
	// view.setCenter(clamp(playerPos.x, minX, maxX), clamp(playerPos.y, minY, maxY));

	window.setView(view);
}


void Game::updateInput(float dt)
{
	// Reseta o estado
	for (auto& [keyName, pressed] : pressedKeys)
		pressed = false;

	// Atualiza teclas que estão pressionadas
	playerDirection = None;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		playerMovement.x -= speed * dt;
		playerDirection = Left;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		playerMovement.x += speed * dt;
		playerDirection = Right;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && velocityY == 0.f) {
		velocityY = -jumpStrength;
		playerDirection = Up;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		pressedKeys["S"] = true;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
		resetPlayer();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
		pressedKeys["E"] = true;

}

void Game::updateGravity(float dt)
{
	if (velocityY < gravity) {
		velocityY += gravity * dt;
	}
	playerMovement.y += velocityY * dt;
}

void Game::checkCollisions()
{
	auto playerRect = playerShape["player"].getGlobalBounds();
	const sf::Vector2f player = playerShape["player"].getPosition();

	for (auto& [name, rect] : mapShape)
	{
		auto intersection = playerRect.findIntersection(rect.getGlobalBounds());
		if (intersection) {

			{
				const sf::FloatRect& inter = *intersection;
				const sf::Vector2f objectColision = rect.getPosition();
				if (mapItems[name].collision.canCollide)
					if (inter.size.y <= inter.size.x)
					{
						if (player.y < objectColision.y) {
							playerMovement.y -= inter.size.y;
							velocityY = 0.f;
							mapItems[name].collision.directionColision = Up;
						}
						else {
							playerMovement.y += inter.size.y;
							if (velocityY < 0.f) {
								velocityY *= -0.5f; // efeito de quicar um pouco
								mapItems[name].collision.directionColision = Down;
							}
						}
					}
					else
					{
						if (player.x < objectColision.x) {
							playerMovement.x -= inter.size.x;
							mapItems[name].collision.directionColision = Left;
						}
						else {
							playerMovement.x += inter.size.x;
							mapItems[name].collision.directionColision = Right;
						}
					}
			}

			mapItems[name].collision.colliding = true;

		}
		else
		{
			mapItems[name].collision.colliding = false;
		}
	}
}

void Game::mapSpecials(float dt) {
	for (auto& [name, mapItem] : mapItems)
	{
		if (mapItem.collision.colliding)
		{

			if (name == "ground")
			{
				resetPlayer();
			}
			if (name == "win")
			{
				std::cout << "Venceu";
				level++;
				actualMap = "level_" + std::to_string(level);
				shouldResetMap = true;  // marca para resetar depois
			}
			if (name == "back")
			{
				std::cout << "Voltar";
			}
			if (mapItem.movement.jump)
			{
				jumpStrength = 800.f;
			}
			if (name == "test_ground")
			{
				mapShape.erase("fake_ground");
				mapItems.erase("fake_ground");
			}
			if (pressedKeys["E"] && mapItem.interaction.interacting)
			{
				playerShape["player"].setPosition({ 1700.f, 800.f });
			}
			if (mapItem.collision.canMove) {
				movementBlocks(name);
			}
		}
		if (mapItem.movement.directionMove == Right)
		{
			sf::Vector2f movement = { mapItem.movement.velocity * dt, 0.f };
			mapItem.movement.distance.x += movement.x;

			if (mapItem.collision.colliding)
			{
				playerShape["player"].move(movement);
				playerThings["player"].transform.position = playerShape["player"].getPosition();
			}
			if (mapItem.movement.distance.x >= mapItem.movement.distance.y)
			{
				mapItem.movement.directionMove = Left;
				mapItem.movement.distance.x = 0;
			}
			mapShape[name].move(movement);
			mapItem.transform.position = mapShape[name].getPosition();
		}
		else if (mapItem.movement.directionMove == Left)
		{
			sf::Vector2f movement = { mapItem.movement.velocity * dt, 0.f };
			mapItem.movement.distance.x += movement.x;
			movement.x *= -1;

			if (mapItem.collision.colliding)
			{
				playerShape["player"].move(movement);
				playerThings["player"].transform.position = playerShape["player"].getPosition();
			}
			if (mapItem.movement.distance.x >= mapItem.movement.distance.y)
			{
				mapItem.movement.directionMove = Right;
				mapItem.movement.distance.x = 0;
			}
			mapShape[name].move(movement);
			mapItem.transform.position = mapShape[name].getPosition();
		}
	}
}

void Game::movementBlocks(std::string name) {
	if (playerDirection == Left && mapItems[name].collision.directionColision == Right) {
		mapShape[name].move({ playerMovement.x, 0.f});
	}
	if (playerDirection == Right && mapItems[name].collision.directionColision == Left) {
		mapShape[name].move({playerMovement.x, 0.f});
	}
	std::cout << playerDirection << " : " << mapItems[name].collision.directionColision << '\n';
}

void Game::updateMovement() {
	playerShape["player"].move(playerMovement);
	playerThings["player"].transform.position = playerShape["player"].getPosition();
}

void Game::resetMap() {

	playerThings["player"].transform = { {20.f, 40.f}, {0.f, 950.f}, sf::Color(144, 143, 164) };
	mapShape.clear();
	mapItems.clear();
	loadMap("map_test");
	resetPlayer();
	createLevel();
}

void Game::resetPlayer() {
	playerShape["player"].setPosition({ 0.f, 950.f });
	velocityY = 0.f;
	gravity = 980.f; // ou 800.f, ajuste como quiser
}

void Game::render()
{
	window.clear();
	for (const auto& [name, rect] : mapShape) {
		window.draw(rect);
	}
	for (const auto& [name, rect] : playerShape) {
		window.draw(rect);
	}
	window.display();
}

void Game::loadMap(const std::string& filename) {
	std::ifstream inFile("levels/"+filename, std::ios::binary);
	if (!inFile) {
		throw std::runtime_error("Failed to open file for reading.");
	}

	size_t mapSize;
	inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

	for (size_t i = 0; i < mapSize; ++i) {
		std::string key;
		size_t keySize;
		inFile.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
		key.resize(keySize);
		inFile.read(&key[0], keySize);

		ItemData item;

		// Transform
		inFile.read(reinterpret_cast<char*>(&item.transform.size), sizeof(item.transform.size));
		inFile.read(reinterpret_cast<char*>(&item.transform.position), sizeof(item.transform.position));
		inFile.read(reinterpret_cast<char*>(&item.transform.color), sizeof(item.transform.color));

		// Movement
		int directionInt;
		inFile.read(reinterpret_cast<char*>(&directionInt), sizeof(directionInt));
		item.movement.directionMove = static_cast<WhereDirection>(directionInt);
		inFile.read(reinterpret_cast<char*>(&item.movement.velocity), sizeof(item.movement.velocity));
		inFile.read(reinterpret_cast<char*>(&item.movement.distance), sizeof(item.movement.distance));
		inFile.read(reinterpret_cast<char*>(&item.movement.jump), sizeof(item.movement.jump));


		// Collision
		inFile.read(reinterpret_cast<char*>(&item.collision.colliding), sizeof(item.collision.colliding));
		inFile.read(reinterpret_cast<char*>(&item.collision.canCollide), sizeof(item.collision.canCollide));
		inFile.read(reinterpret_cast<char*>(&item.collision.canMove), sizeof(item.collision.canMove));
		inFile.read(reinterpret_cast<char*>(&directionInt), sizeof(directionInt));
		item.collision.directionColision = static_cast<WhereDirection>(directionInt);



		// Interaction
		inFile.read(reinterpret_cast<char*>(&item.interaction.interacting), sizeof(item.interaction.interacting));

		mapItems[key] = item;
	}

	inFile.close();
}

void saveMap(const std::map<std::string, ItemData>& mapItems, const std::string& filename) {
	std::ofstream outFile("levels/" + filename, std::ios::binary);
	if (!outFile) {
		throw std::runtime_error("Failed to open file for writing.");
	}

	size_t mapSize = mapItems.size();
	outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

	for (const auto& pair : mapItems) {
		const std::string& key = pair.first;
		const ItemData& item = pair.second;

		// Save string size + string data
		size_t keySize = key.size();
		outFile.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
		outFile.write(key.data(), keySize);

		// Transform
		outFile.write(reinterpret_cast<const char*>(&item.transform.size), sizeof(item.transform.size));
		outFile.write(reinterpret_cast<const char*>(&item.transform.position), sizeof(item.transform.position));
		outFile.write(reinterpret_cast<const char*>(&item.transform.color), sizeof(item.transform.color));

		// Movement
		int directionInt = static_cast<int>(item.movement.directionMove);  // 👈 SALVAR COMO INT
		outFile.write(reinterpret_cast<const char*>(&directionInt), sizeof(directionInt));
		outFile.write(reinterpret_cast<const char*>(&item.movement.velocity), sizeof(item.movement.velocity));
		outFile.write(reinterpret_cast<const char*>(&item.movement.distance), sizeof(item.movement.distance));
		outFile.write(reinterpret_cast<const char*>(&item.movement.jump), sizeof(item.movement.jump));


		// Collision
		outFile.write(reinterpret_cast<const char*>(&item.collision.colliding), sizeof(item.collision.colliding));
		outFile.write(reinterpret_cast<const char*>(&item.collision.canCollide), sizeof(item.collision.canCollide));
		outFile.write(reinterpret_cast<const char*>(&item.collision.canMove), sizeof(item.collision.canMove));
		directionInt = static_cast<int>(item.collision.directionColision);  // 👈 SALVAR COMO INT
		outFile.write(reinterpret_cast<const char*>(&directionInt), sizeof(directionInt));



		// Interaction
		outFile.write(reinterpret_cast<const char*>(&item.interaction.interacting), sizeof(item.interaction.interacting));

	}

	//outFile.close();
}
