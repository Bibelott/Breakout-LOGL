#include "game.hpp"

#include "sprite_renderer.hpp"
#include "resource_manager.hpp"

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

SpriteRenderer *Renderer;
GameObject *Player;

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
}

void Game::Update(float dt) {
  
}

void Game::ProcessInput(float dt) {
  if (State == GAME_ACTIVE) {
    float velocity = PLAYER_VELOCITY * dt;

    if (Keys[GLFW_KEY_A]) {
      if (Player->Position.x >= 0.0f)
        Player->Position.x -= velocity;
    }
    if (Keys[GLFW_KEY_D]) {
      if (Player->Position.x <= Width - Player->Size.x)
        Player->Position.x += velocity;
    }
  }
}

void Game::Render() {
  if (State == GAME_ACTIVE) {
    Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height));
    Levels[Level].Draw(*Renderer);
    Player->Draw(*Renderer);
  }
}
