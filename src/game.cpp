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

  if (Ball->Position.y >= Height) {
    ResetLevel();
    ResetPlayer();
  }
}

void Game::ResetLevel() {
  std::string file;
  switch (Level) {
    case 0:
      file = "levels/one.lvl";
      break;
    case 1:
      file = "levels/two.lvl";
      break;
    case 2:
      file = "levels/three.lvl";
      break;
    case 3:
      file = "levels/four.lvl";
      break;
  }
  Levels[Level].Load(file.c_str(), Width, Height / 2);
}

void Game::ResetPlayer() {
  Player->Position = glm::vec2(Width / 2.0f - PLAYER_SIZE.x / 2.0f, Height - PLAYER_SIZE.y);
  Ball->Stuck = true;
  Ball->Position = Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
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
  // brick collisions
  for (GameObject &box : Levels[Level].Bricks) {
    if (!box.Destroyed) {
      Collision collision = CheckCollision(*Ball, box);
      if (std::get<0>(collision)) {
        if (!box.IsSolid)
          box.Destroyed = true;

        Direction dir = std::get<1>(collision);
        glm::vec2 diff_vector = std::get<2>(collision);
        if (dir == LEFT || dir == RIGHT) {
          Ball->Velocity.x = -Ball->Velocity.x;

          float penetration = Ball->Radius - std::abs(diff_vector.x);

          if (dir == LEFT)
            Ball->Position.x += penetration;
          else
            Ball->Position.x -= penetration;
        }
        else {
          Ball->Velocity.y = -Ball->Velocity.y;

          float penetration = Ball->Radius - std::abs(diff_vector.y);

          if (dir == UP)
            Ball->Position.y -= penetration;
          else
            Ball->Position.y += penetration;
        }
      }
    }
  }

  // player collisions
  Collision result = CheckCollision(*Ball, *Player);
  if (!Ball->Stuck && std::get<0>(result)) {
    float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
    float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
    float percentage = distance / (Player->Size.x / 2.0f);

    float strength = 2.0f;
    glm::vec2 oldVelocity = Ball->Velocity;
    Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    Ball->Velocity.y = -1.0f * std::abs(Ball->Velocity.y);
    Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
  }
}

Collision Game::CheckCollision(BallObject &ball, GameObject &obj) {
  glm::vec2 center(ball.Position + ball.Radius);

  glm::vec2 aabb_half_extents(obj.Size / 2.0f);
  glm::vec2 aabb_center(obj.Position + aabb_half_extents);

  glm::vec2 difference = center - aabb_center;
  glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

  glm::vec2 closest = aabb_center + clamped;

  difference = closest - center;

  if (glm::length(difference) <= ball.Radius)
    return std::make_tuple(true, VectorDirection(difference), difference);
  else
    return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

Direction VectorDirection(glm::vec2 target) {
  glm::vec2 compass[] = {
    glm::vec2(0.0f, 1.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(0.0f, -1.0f),
    glm::vec2(-1.0f, 0.0f),
  };
  float max = 0.0f;
  unsigned int best_match = -1;

  for (unsigned int i = 0; i < 4; i++) {
    float dot_product = glm::dot(glm::normalize(target), compass[i]);
    if (dot_product > max) {
      max = dot_product;
      best_match = i;
    }
  }
  return (Direction)best_match;
}
