#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tuple>

#include "game_level.hpp"
#include "ball_object.hpp"


enum GameState {
  GAME_ACTIVE,
  GAME_MENU,
  GAME_WIN
};

enum Direction {
  UP,
  RIGHT,
  DOWN,
  LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game {
  public:
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;
    std::vector<GameLevel> Levels;
    unsigned int Level;

    Game(unsigned int width, unsigned int height);
    ~Game();

    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();

  private:
    Collision CheckCollision(BallObject &ball, GameObject &obj);
    void ResetLevel();
    void ResetPlayer();
};

Direction VectorDirection(glm::vec2 target);
