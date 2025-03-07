#include <SFML/Graphics.hpp>
#include <deque>
#include <iostream>
#include <random>

sf::RectangleShape new_shape(sf::Vector2f pos, const float size) {
  sf::RectangleShape shape(sf::Vector2f(size, size));
  shape.setFillColor(sf::Color::Yellow);
  shape.setPosition(pos * size);
  return shape;
}

int main() {
  const unsigned int widthP = 512;
  const unsigned int heightP = 384;
  const float size = 16;

  const unsigned int width = widthP / size;
  const unsigned int height = heightP / size;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> randWidth(0, width - 1);
  std::uniform_int_distribution<> randHeight(0, height - 1);

  sf::RenderWindow window(sf::VideoMode({widthP, heightP}), "SFML works!");

  std::deque<sf::RectangleShape> snake;
  for (unsigned i = 0; i < 3; ++i)
    snake.push_front(new_shape(sf::Vector2f(i, 0), size));
  snake[0].setFillColor(sf::Color::Green);
  snake[1].setFillColor(sf::Color::Green);

  sf::RectangleShape food(sf::Vector2f(size, size));
  food.setFillColor(sf::Color::Blue);
  food.setPosition(sf::Vector2f(size * randWidth(gen), size * randHeight(gen)));
  while (std::any_of(snake.begin(), snake.end(),
                     [&food](sf::RectangleShape &shape) {
                       return food.getPosition() == shape.getPosition();
                     }))
    food.setPosition(
        sf::Vector2f(size * randWidth(gen), size * randHeight(gen)));

  sf::Font font;
  if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
    std::cout << "Error: cannot load font!" << '\n';
    return 1;
  }
  sf::Text text;
  text.setFont(font);
  text.setFillColor(sf::Color::White);
  text.setCharacterSize(24);
  text.setPosition(sf::Vector2f(widthP - 160, 0));
  unsigned int score = 0;
  text.setString("Score: " + std::to_string(score));

  sf::Keyboard::Scan::Scancode lastMoving = sf::Keyboard::Scan::D;
  sf::Keyboard::Scan::Scancode moving = sf::Keyboard::Scan::D;
  unsigned int baseTime = 1024;
  const unsigned int baseLimit = 48;
  bool lose = false;

  while (window.isOpen()) {
    sf::Event event;
    const int pause = sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::X)
                          ? baseLimit
                          : baseTime;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      else if (event.type == sf::Event::KeyPressed) {
        switch (event.key.scancode) {
        case sf::Keyboard::Scan::W:
          if (lastMoving != sf::Keyboard::Scan::S)
            moving = event.key.scancode;
          break;
        case sf::Keyboard::Scan::S:
          if (lastMoving != sf::Keyboard::Scan::W)
            moving = event.key.scancode;
          break;
        case sf::Keyboard::Scan::A:
          if (lastMoving != sf::Keyboard::Scan::D)
            moving = event.key.scancode;
          break;
        case sf::Keyboard::Scan::D:
          if (lastMoving != sf::Keyboard::Scan::A)
            moving = event.key.scancode;
          break;
        default:
          break;
        }
      }
    }
    if (lose) {
      sf::sleep(sf::milliseconds(pause));
      continue;
    }

    const sf::Vector2f pos = snake.front().getPosition() / size;
    sf::Vector2f move = sf::Vector2f(-1, -1);
    switch (moving) {
    case sf::Keyboard::Scan::W:
      move = pos +
             sf::Vector2f(0, pos.y != 0 ? -1 : static_cast<float>(height) - 1);
      break;
    case sf::Keyboard::Scan::S:
      move = pos + sf::Vector2f(0, pos.y != height - 1
                                       ? 1
                                       : 1 - static_cast<float>(height));
      break;
    case sf::Keyboard::Scan::A:
      move = pos +
             sf::Vector2f(pos.x != 0 ? -1 : static_cast<float>(width) - 1, 0);
      break;
    case sf::Keyboard::Scan::D:
      move =
          pos + sf::Vector2f(
                    pos.x != width - 1 ? 1 : 1 - static_cast<float>(width), 0);
      break;
    default:
      break;
    }
    if (std::any_of(snake.begin(), snake.end() - 1,
                    [&move, &size](sf::RectangleShape &shape) {
                      return move == shape.getPosition() / size;
                    })) {
      for (auto &shape : snake)
        shape.setFillColor(sf::Color::Red);
      lose = true;
      text.setString("Game over!");
    }
    if (!lose && move != sf::Vector2f(-1, -1)) {
      snake.push_front(new_shape(move, size));
      snake[1].setFillColor(sf::Color::Green);
      lastMoving = moving;
      if (move == food.getPosition() / size) {
        food.setPosition(size * randWidth(gen), size * randHeight(gen));
        score += 10;
        text.setString("Score: " + std::to_string(score));
        if (baseTime > baseLimit * 2)
          baseTime -= 16;
        while (std::any_of(snake.begin(), snake.end(),
                           [&food](sf::RectangleShape &shape) {
                             return food.getPosition() == shape.getPosition();
                           }))
          food.setPosition(
              sf::Vector2f(size * randWidth(gen), size * randHeight(gen)));

      } else
        snake.pop_back();
    }
    window.clear();
    for (const auto &shape : snake)
      window.draw(shape);
    window.draw(food);
    window.draw(text);
    window.display();

    sf::sleep(sf::milliseconds(pause));
  }

  return 0;
}
