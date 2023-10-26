#include "game.hpp"

#include "sprite_renderer.hpp"
#include "resource_manager.hpp"

SpriteRenderer *Renderer;

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

  ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
}

void Game::Update(float dt) {
  
}

void Game::ProcessInput(float dt) {
  
}

void Game::Render() {
  Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}
