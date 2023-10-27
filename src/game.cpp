#include "game.hpp"

#include "sprite_renderer.hpp"
#include "resource_manager.hpp"

SpriteRenderer *Renderer;

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
GameObject *Player;

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;
BallObject *Ball;

Game::Game(unsigned int width, unsigned int height) : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {
  
}

Game::~Game() {
  delete Renderer;
}


void Game::Init() {
  ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", "sprite");

  glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
  ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
  ResourceManager::GetShader("sprite").SetMat4("projection", proj);

  Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

  ResourceManager::LoadTexture("textures/background.jpg", false, "background");
  ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
  ResourceManager::LoadTexture("textures/block.png", false, "block");
  ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
  ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");

  GameLevel one; one.Load("levels/one.lvl", Width, Height / 2);
  GameLevel two; two.Load("levels/two.lvl", Width, Height / 2);
  GameLevel three; three.Load("levels/three.lvl", Width, Height / 2);
  GameLevel four; four.Load("levels/four.lvl", Width, Height / 2);

  Levels.push_back(one);
  Levels.push_back(two);
  Levels.push_back(three);
  Levels.push_back(four);

  Level = 0;

  glm::vec2 playerPos = glm::vec2(Width / 2.0f - PLAYER_SIZE.x / 2.0f, Height - PLAYER_SIZE.y);
  Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

  glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
  Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::Update(float dt) {
  Ball->Move(dt, Width);
  DoCollisions();
}

void Game::ProcessInput(float dt) {
  if (State == GAME_ACTIVE) {
    float velocity = PLAYER_VELOCITY * dt;

    if (Keys[GLFW_KEY_A]) {
      if (Player->Position.x >= 0.0f) {
        Player->Position.x -= velocity;
        if (Ball->Stuck)
          Ball->Position.x -= velocity;
      }
    }
    if (Keys[GLFW_KEY_D]) {
      if (Player->Position.x <= Width - Player->Size.x) {
        Player->Position.x += velocity;
        if (Ball->Stuck)
          Ball->Position.x += velocity;
      }
    }
    if (Keys[GLFW_KEY_SPACE])
      Ball->Stuck = false;
  }
}

void Game::Render() {
  if (State == GAME_ACTIVE) {
    Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height));
    Levels[Level].Draw(*Renderer);
    Player->Draw(*Renderer);
    Ball->Draw(*Renderer);
  }
}

void Game::DoCollisions() {
  for (GameObject &box : Levels[Level].Bricks) {
    if (!box.Destroyed && !box.IsSolid) {
      if (CheckCollision(*Ball, box)) {
        box.Destroyed = true;
      }
    }
  }
}

bool Game::CheckCollision(BallObject &ball, GameObject &obj) {
  glm::vec2 center(ball.Position + ball.Radius);

  glm::vec2 aabb_half_extents(obj.Size / 2.0f);
  glm::vec2 aabb_center(obj.Position + aabb_half_extents);

  glm::vec2 difference = center - aabb_center;
  glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

  glm::vec2 closest = aabb_center + clamped;

  difference = closest - center;

  return glm::length(difference) < ball.Radius;
}
