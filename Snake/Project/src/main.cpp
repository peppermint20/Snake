#include <SFML/Graphics.hpp>

#include <algorithm>
#include <deque>
#include <filesystem>
#include <imgui-sfml.h>
#include <imgui.h>
#include <iostream>
#include <print>
#include <queue>
#include <random>
constexpr int ROWS        = 30;
constexpr int COLUMNS     = 20;
constexpr int CELL_SIZE   = 20;
constexpr int aspectRatio = 2;

constexpr int GRID_SIZE_X = ROWS * CELL_SIZE;
constexpr int GRID_SIZE_Y = COLUMNS * CELL_SIZE;

constexpr int screen_x = ROWS * CELL_SIZE;
constexpr int screen_y = COLUMNS * CELL_SIZE;

enum Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};


int main()
{
    std::mt19937                  mt{std::random_device{}()};
    std::uniform_int_distribution rowDice{0, ROWS - 1};
    std::uniform_int_distribution colDice{0, COLUMNS - 1};
    sf::RenderWindow window(sf::VideoMode({screen_x * aspectRatio, screen_y * aspectRatio}), "SFML works!", sf::Style::Titlebar);

    if (!ImGui::SFML::Init(window))
    {
        return -1;
    }

    sf::RectangleShape body({CELL_SIZE, CELL_SIZE});
    body.setFillColor(sf::Color(0, 140, 0, 200));
    body.setPosition({GRID_SIZE_X / 2.f, GRID_SIZE_Y / 2.f});
    float snakeSpeed = 16.f;

    sf::RectangleShape grid;
    grid.setSize({CELL_SIZE, CELL_SIZE});
    grid.setFillColor(sf::Color(245, 245, 245, 255));
    grid.setOutlineColor(sf::Color(255, 0, 0));
    grid.setOutlineThickness(1.f);

    sf::RectangleShape food({CELL_SIZE, CELL_SIZE});
    food.setFillColor(sf::Color::Black);
    food.setPosition({static_cast<float>(rowDice(mt) * CELL_SIZE), static_cast<float>(colDice(mt) * CELL_SIZE)});

    sf::Font font("./Project/resources/fonts/Acme_9_Regular_Bold.ttf");
    sf::Text text(font, "Game over you Lost!");
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Regular);
    text.setPosition({50.f, GRID_SIZE_Y / 2.f});
    bool foodSpawned = false;

    sf::Clock deltaClock;

    float     snakeColor[4] = {0, 0.613f, 0.054f, 1.f};
    float     bodyColor[4]  = {0, 0.613f, 0.054f, 1.f};
    int       snakeSize     = 4;
    bool      eatFruit      = false;
    bool      isPlaying     = true;
    Direction direction{};

    window.setFramerateLimit(30);

    std::deque<sf::Vector2f> queue;

    queue.push_back(body.getPosition());
    queue.push_back({body.getPosition().x - CELL_SIZE, body.getPosition().y});
    queue.push_back({body.getPosition().x - 2 * CELL_SIZE, body.getPosition().y});
    queue.push_back({body.getPosition().x - 3 * CELL_SIZE, body.getPosition().y});


    while (window.isOpen())
    {
        // Handle Events
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() &&
                 event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
            {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::ShowDemoWindow();
        ImGui::Begin("Hello, world!");

        if (ImGui::Button("Eat Fruit"))
        {
            snakeSize++;
            eatFruit = true;
        }
        ImGui::ColorEdit4("Snake Color", snakeColor);
        ImGui::ColorEdit4("Body Color", bodyColor);
        ImGui::InputInt("Snake Size", &snakeSize);
        ImGui::Text("Food position x: %f", food.getPosition().x);
        ImGui::Text("Food position y: %f", food.getPosition().y);
        ImGui::End();


        if (isPlaying)
        {
            // Store key pushed
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && direction != Direction::RIGHT)
            {
                direction = Direction::LEFT;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && direction != Direction::LEFT)
            {
                direction = Direction::RIGHT;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && direction != Direction::DOWN)
            {
                direction = Direction::UP;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && direction != Direction::UP)
            {
                direction = Direction::DOWN;
            }

            // Store movement in queue
            sf::Vector2f nextpos;
            switch (direction)
            {
                case Direction::UP:
                    nextpos = {body.getPosition().x, body.getPosition().y - CELL_SIZE};
                    break;
                case Direction::RIGHT:
                    nextpos = {body.getPosition().x + CELL_SIZE, body.getPosition().y};
                    //body.setPosition({body.getPosition().x + CELL_SIZE, body.getPosition().y});
                    break;
                case Direction::DOWN:
                    nextpos = {body.getPosition().x, body.getPosition().y + CELL_SIZE};
                    //body.setPosition({body.getPosition().x, body.getPosition().y + CELL_SIZE});
                    break;
                case Direction::LEFT:
                    nextpos = {body.getPosition().x - CELL_SIZE, body.getPosition().y};
                    //body.setPosition({body.getPosition().x - CELL_SIZE, body.getPosition().y});
                    break;
            }
            // Store the next positions of the snake body
            // If the snake body goes out of bounds rotate it around
            if (nextpos.x < 0)
            {
                nextpos.x = GRID_SIZE_X - CELL_SIZE;
            }
            else if (nextpos.x > GRID_SIZE_X - CELL_SIZE)
            {
                nextpos.x = 0;
            }
            else if (nextpos.y < 0)
            {
                nextpos.y = GRID_SIZE_Y - CELL_SIZE;
            }
            else if (nextpos.y > GRID_SIZE_Y - CELL_SIZE)
            {
                nextpos.y = 0;
            }

            // Handle Collision with self
            for (int i = 0; i < queue.size(); ++i)
            {
                for (int j = i + 1; j < queue.size(); ++j)
                {
                    if (queue[i] == queue[j])
                    {
                        isPlaying = false;
                    }
                }
            }

            // Handle collision with food
            for (int i = 0; i < queue.size(); ++i)
            {
                // grow in size and then move position elseewhere

                if (queue[i] == food.getPosition())
                {
                    eatFruit = true;
                    snakeSize++;
                    break;
                }
            }

            // "add" to the snake length, pop the back if we don't eat fruit to "move" the snake
            queue.push_front(nextpos);
            if (eatFruit)
            {
                eatFruit                     = false;
                bool         badFoodSpawn    = false;
                sf::Vector2f newFoodPosition = {static_cast<float>(rowDice(mt) * CELL_SIZE),
                                                static_cast<float>(colDice(mt) * CELL_SIZE)};

                do
                {
                    for (int i = 0; i < queue.size(); ++i)
                    {
                        // We need to spawn it else where
                        if (newFoodPosition == queue[i])
                        {
                            badFoodSpawn = true;
                            break;
                        }
                    }
                } while (badFoodSpawn);

                food.setPosition(newFoodPosition);
            }
            else
            {
                queue.pop_back();
            }


            body.setFillColor(sf::Color(static_cast<uint8_t>(255 * snakeColor[0]),
                                        static_cast<uint8_t>(255 * snakeColor[1]),
                                        static_cast<uint8_t>(255 * snakeColor[2]),
                                        static_cast<uint8_t>(255 * snakeColor[3])));
            window.clear();

            // Draw

            for (int i = 0; i < ROWS; ++i)
            {
                for (int j = 0; j < COLUMNS; ++j)
                {
                    grid.setPosition({static_cast<float>(i * CELL_SIZE), static_cast<float>(j * CELL_SIZE)});
                    window.draw(grid);
                }
            }

            std::for_each(queue.rbegin(),
                          queue.rend(),
                          [&](const sf::Vector2f& pos)
                          {
                              body.setPosition(pos);
                              window.draw(body);
                          });

            window.draw(food);

            ImGui::SFML::Render(window);
            window.display();
        }
        else
        {
            window.clear();
            window.draw(text);
            ImGui::SFML::Render(window);
            window.display();
        }
    }
        ImGui::SFML::Shutdown();
}