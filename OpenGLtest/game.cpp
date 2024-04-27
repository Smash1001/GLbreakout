/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <algorithm>
#include <sstream>
#include <iostream>

#include "game.h"
#include "resourceManager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "text_renderer.h"

// Initial velocity of the Ball
//const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
//const float BALL_RADIUS = 12.5f;

// Values to determine elements to be used
std::string paddleNum = "";
std::string ballNum = "";
std::string blocksNum = "";
std::string solidNum = "";
std::string backgroundNum = "";
unsigned int menuPart = 1;

// Game-related State data
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
TextRenderer* Text;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_START), Keys(), Width(width), Height(height) //game starts as GAME_START
{

}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Text;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("fonts/SuperMario256.ttf", 24);
    // load textures
    ResourceManager::LoadTexture("textures/background2.jpg", true, "background");
    ResourceManager::LoadTexture("textures/background3.jpg", true, "background2");
    ResourceManager::LoadTexture("textures/background4.jpg", true, "background3");
    ResourceManager::LoadTexture("textures/ball.png", true, "face");
    ResourceManager::LoadTexture("textures/ball2.png", true, "face2");
    ResourceManager::LoadTexture("textures/ball3.png", true, "face3");
    ResourceManager::LoadTexture("textures/block.png", false, "block");
    ResourceManager::LoadTexture("textures/block2.png", false, "block2");
    ResourceManager::LoadTexture("textures/block3.png", false, "block3");
    ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("textures/block_solid2.png", false, "block_solid2");
    ResourceManager::LoadTexture("textures/block_solid3.png", false, "block_solid3");
    ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("textures/paddle2.png", true, "paddle2");
    ResourceManager::LoadTexture("textures/paddle3.png", true, "paddle3");

    // configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::Update(float dt)
{
    if (this->State == GAME_ACTIVE) {
        Ball->Move(dt, this->Width);
        // check for collisions
        this->DoCollisions();

        if (Ball->Position.y >= this->Height) 
        {
            this->lifeNum--;
            if (this->lifeNum == 0) {
                this->ResetLevel();
                this->State = GAME_MENU;
            }
            this->ResetPlayer();
        }
    }
    
}

void Game::ProcessInput(float dt)
{
    // when game is started
    if (this->State == GAME_START) {
        if (this->Keys[GLFW_KEY_SPACE]) { //press space to start game
            this->State = GAME_MENU;
            this->Keys[GLFW_KEY_SPACE] = false;
        }
    }
    // when game is active
    if (this->State == GAME_ACTIVE)
    {
        bool once = false;
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_LEFT])
        {
            if (Player->Position.x >= 0.0f) 
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_RIGHT])
        {
            if (Player->Position.x <= this->Width - Player->Size.x) 
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE]) 
        {
            Ball->Stuck = false;
        }
        if (this->Keys[GLFW_KEY_Z]) //press z to pause
        {
            this->State = GAME_PAUSE;
            this->Keys[GLFW_KEY_Z] = false;
        }
    }

    //when game is paused
    if (this->State == GAME_PAUSE) 
    {
        if (this->Keys[GLFW_KEY_Z]) {
            this->State = GAME_ACTIVE;
            this->Keys[GLFW_KEY_Z] = false;
        }
    }

    //item select and menu
    if (this->State == GAME_MENU) {
        if (this->Keys[GLFW_KEY_1] && menuPart == 1) {
            paddleNum = "paddle";
            this->Keys[GLFW_KEY_1] = false;
            menuPart++;
        }
        if (this->Keys[GLFW_KEY_2] && menuPart == 1) {
            paddleNum = "paddle2";
            this->Keys[GLFW_KEY_2] = false;
            menuPart++;
        }
        if (this->Keys[GLFW_KEY_3] && menuPart == 1) {
            paddleNum = "paddle3";
            this->Keys[GLFW_KEY_3] = false;
            menuPart++;
        }
        if (this->Keys[GLFW_KEY_1] && menuPart == 2) {
            ballNum = "face";
            this->Keys[GLFW_KEY_1] = false;
            menuPart++;
        }
        if (this->Keys[GLFW_KEY_2] && menuPart == 2) {
            ballNum = "face2";
            this->Keys[GLFW_KEY_2] = false;
            menuPart++;
        }
        if (this->Keys[GLFW_KEY_3] && menuPart == 2) {
            ballNum = "face3";
            this->Keys[GLFW_KEY_3] = false;
            menuPart++;
        }
        if (this->Keys[GLFW_KEY_1] && menuPart == 3) {
            blocksNum = "block";
            solidNum = "block_solid";
            this->Keys[GLFW_KEY_1] = false;
            menuPart++;
        }
        if (this->Keys[GLFW_KEY_2] && menuPart == 3) {
            blocksNum = "block2";
            solidNum = "block_solid2";
            this->Keys[GLFW_KEY_2] = false;
            menuPart++;
        }
        if (this->Keys[GLFW_KEY_3] && menuPart == 3) {
            blocksNum = "block3";
            solidNum = "block_solid3";
            this->Keys[GLFW_KEY_3] = false;
            menuPart++;
        }
        if (menuPart == 4) {
            delete Player;
            delete Ball;
            glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
            Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture(paddleNum));
            glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
            Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture(ballNum));

            // load levels
            GameLevel one; one.Load("levels/one.txt", this->Width, this->Height / 2, blocksNum, solidNum);
            GameLevel two; two.Load("levels/two.txt", this->Width, this->Height / 2, blocksNum, solidNum);
            GameLevel three; three.Load("levels/three.txt", this->Width, this->Height / 2, blocksNum, solidNum);
            GameLevel four; four.Load("levels/four.txt", this->Width, this->Height / 2, blocksNum, solidNum);
            this->Levels.push_back(two);
            this->Levels.push_back(one);
            this->Levels.push_back(three);
            this->Levels.push_back(four);
            this->Level = 0;

            this->State = GAME_ACTIVE;
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_START) 
    {
        // draw title (increasing denominator moves left, moves up)
        Text->RenderText("Controls", this->Width / 2.5f, this->Height / 30.0f, 1.0f);
        Text->RenderText("Z - pause game      space - serve ball", this->Width / 7.0f, this->Height / 10.0f, 1.0f);
        Text->RenderText("Ultra Cool Hyperspace Breakout", this->Width / 5.0f, this->Height / 2.6f, 1.0f);
        Text->RenderText("(Press space to play)", this->Width / 3.4f, this->Height / 1.9f, 1.0f);
        Text->RenderText("2024 Julian Lee and Adam Bowen", this->Width / 5.0f, this->Height / 1.1f, 1.0f);
    }

    if (this->State == GAME_ACTIVE || this->State == GAME_PAUSE)
    {
        // draw background
        Texture2D texture;
        texture = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(texture, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        // draw player
        Player->Draw(*Renderer);
        // draw ball
        Ball->Draw(*Renderer);
        // draw UI
        Text->RenderText("Score", this->Width / 30.0f, this->Height / 30.0f, 1.0f);
        Text->RenderText("Lives", this->Width / 1.2f + 10, this->Height / 30.0f, 1.0f);
        Text->RenderText(life, this->Width / 1.2f + 100, this->Height / 30.0f, 1.0f);
    }

    if (this->State == GAME_MENU) 
    {
        if (menuPart == 1) 
        {
            //increasing denominator moves left (WIDTH), moves up (HEIGHT)
            Texture2D paddle, paddle2, paddle3;
            Text->RenderText("Select a paddle", this->Width / 3.0f, this->Height / 3.6f, 1.0f);
            Text->RenderText("1                  2                   3", this->Width / 5.0f, this->Height / 1.5f, 1.0f);
            paddle = ResourceManager::GetTexture("paddle");
            paddle2 = ResourceManager::GetTexture("paddle2");
            paddle3 = ResourceManager::GetTexture("paddle3");
            Renderer->DrawSprite(paddle, glm::vec2(this->Width / 7.0f, this->Height / 2.0f), PLAYER_SIZE, 0.0f);
            Renderer->DrawSprite(paddle2, glm::vec2(this->Width / 2.3f, this->Height / 2.0f), PLAYER_SIZE, 0.0f);
            Renderer->DrawSprite(paddle3, glm::vec2(this->Width / 1.35f, this->Height / 2.0f), PLAYER_SIZE, 0.0f);
        }
        if (menuPart == 2) 
        {
            Texture2D ball, ball2, ball3;
            Text->RenderText("Select a ball", this->Width / 3.0f, this->Height / 3.6f, 1.0f);
            Text->RenderText("1                  2                   3", this->Width / 5.0f, this->Height / 1.5f, 1.0f);
            ball = ResourceManager::GetTexture("face");
            ball2 = ResourceManager::GetTexture("face2");
            ball3 = ResourceManager::GetTexture("face3");
            Renderer->DrawSprite(ball, glm::vec2(this->Width / 7.0f + 30, this->Height / 2.0f), glm::vec2(BALL_RADIUS * 4.0f, BALL_RADIUS * 4.0f), 0.0f);
            Renderer->DrawSprite(ball2, glm::vec2(this->Width / 2.3f + 30, this->Height / 2.0f), glm::vec2(BALL_RADIUS * 4.0f, BALL_RADIUS * 4.0f), 0.0f);
            Renderer->DrawSprite(ball3, glm::vec2(this->Width / 1.35f + 30, this->Height / 2.0f), glm::vec2(BALL_RADIUS * 4.0f, BALL_RADIUS * 4.0f), 0.0f);
        }
        if (menuPart == 3) 
        {
            Texture2D block, block2, block3;
            Text->RenderText("Select block type", this->Width / 3.0f, this->Height / 3.6f, 1.0f);
            Text->RenderText("1                  2                   3", this->Width / 5.0f, this->Height / 1.5f, 1.0f);
            block = ResourceManager::GetTexture("block");
            block2 = ResourceManager::GetTexture("block2");
            block3 = ResourceManager::GetTexture("block3");
            Renderer->DrawSprite(block, glm::vec2(this->Width / 7.0f + 20, this->Height / 2.0f), glm::vec2(20.0f * 4.0f, 20.0f * 4.0f), 0.0f);
            Renderer->DrawSprite(block2, glm::vec2(this->Width / 2.3f + 20, this->Height / 2.0f), glm::vec2(20.0f * 4.0f, 20.0f * 4.0f), 0.0f);
            Renderer->DrawSprite(block3, glm::vec2(this->Width / 1.35f + 20, this->Height / 2.0f), glm::vec2(20.0f * 4.0f, 20.0f * 4.0f), 0.0f);
        }
    }
}

// collision detection
bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                    box.Destroyed = true;
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // move ball to right
                    else
                        Ball->Position.x -= penetration; // move ball to left;
                }
                else // vertical collision
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // move ball bback up
                    else
                        Ball->Position.y += penetration; // move ball back down
                }
            }
        }
    }
    // check collisions for player pad (unless stuck)
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    }
}

Collision CheckCollision(BallObject& one, GameObject& two) // AABB - Circle collision
{
    // get center point circle first 
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // now that we know the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// calculates which direction a vector is facing (N,E,S or W)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("levels/one.txt", this->Width, this->Height / 2, blocksNum, solidNum);
    else if (this->Level == 1)
        this->Levels[1].Load("levels/two.txt", this->Width, this->Height / 2, blocksNum, solidNum);
    else if (this->Level == 2)
        this->Levels[2].Load("levels/three.txt", this->Width, this->Height / 2, blocksNum, solidNum);
    else if (this->Level == 3)
        this->Levels[3].Load("levels/four.txt", this->Width, this->Height / 2, blocksNum, solidNum);
}

void Game::ResetPlayer()
{
    // reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}
