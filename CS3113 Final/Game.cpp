#include "Game.h"
#include <SDL_image.h>
#include <vector>
#include <string>
#include "Utility.h"
#include "stb_image.h"

#define LOG(argument) std::cout << argument << '\n'

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

const float MILLISECONDS_IN_SECOND = 1000.0f;

const char FONT_SPRITE_FILEPATH[] = "assets/font/font.png";
const char KNIGHT_IDLE_FILEPATH[] = "assets/Knight_1/Idle.png";
const char KNIGHT_RUN_FILEPATH[] = "assets/Knight_1/Run.png";
const char KNIGHT_ATTACK_FILEPATH[] = "assets/Knight_1/RUN+ATTACK.png";

Game::Game() {
    player = nullptr;
    map = nullptr;
    state = GAME_ACTIVE;
    running = true;
}

Game::~Game() {
    shutdown();
}

void Game::initialize() {
    shader_program.load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::ortho(-20.0f, 20.0f, -15.0f, 15.0f, -1.0f, 1.0f);
    
    shader_program.set_view_matrix(view_matrix);
    shader_program.set_projection_matrix(projection_matrix);
    
    // Load font texture using the Utility class
    font_texture_id = Utility::load_texture(FONT_SPRITE_FILEPATH);
    
    // Load knight textures
    GLuint knight_idle_texture_id = Utility::load_texture(KNIGHT_IDLE_FILEPATH);
    GLuint knight_run_texture_id = Utility::load_texture(KNIGHT_RUN_FILEPATH);
    GLuint knight_attack_texture_id = Utility::load_texture(KNIGHT_ATTACK_FILEPATH);
    
    // Create a single map
    map = new Map();
    
    // Create the knight
    player = new Knight();
    player->texture_id = knight_idle_texture_id;
    player->set_animation(IDLE);
    
    // Set initial player position
    player->position = glm::vec3(0.0f, 0.0f, 0.0f);
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            running = false;
        }
        
        //handle jump and attack actions on key press
        if (event.type == SDL_KEYDOWN && !event.key.repeat) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    // jump
                    if (state == GAME_ACTIVE) {
                        player->jump();
                    }
                    break;
                    
                case SDLK_f:
                    //attack
                    if (state == GAME_ACTIVE && !player->is_attacking) {
                        player->attack();
                    }
                    break;
            }
        }
    }
    
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (state == GAME_ACTIVE) {
        //movement
        player->move_left = key_state[SDL_SCANCODE_A];
        player->move_right = key_state[SDL_SCANCODE_D];
        player->move_up = false;
        player->move_down = false;
    }
}

void Game::update(float delta_time) {
    if (state == GAME_ACTIVE) {
        player->update(delta_time);
        if (map->is_solid(player->position.x, player->position.y)) {
            //if collision with boundaries, revert to previous position
            player->position = player->position - player->velocity * delta_time;
        }
        
        //platform collision
        float y_correction = 0.0f;
        if (map->check_platform_collision(
            player->position.x, 
            player->position.y - (player->height / 2.0f), 
            &y_correction)) {
            player->position.y = y_correction + (player->height / 2.0f); 
            player->velocity.y = 0;
            player->is_grounded = true;
            
            //if was jumping or falling, switch to idle
            if (player->state == JUMPING || player->state == FALLING) {
                player->is_jumping = false;
                player->set_animation(IDLE);
            }
        } else {
            player->is_grounded = false;
        }
    }
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    map->render(&shader_program);
    player->render(&shader_program);
    Utility::draw_text(&shader_program, font_texture_id, "Final Project", 
                       1.0f, 0.1f, glm::vec3(-5.0f, 14.0f, 0.0f));
}
//destruct
void Game::shutdown() {
    delete player;
    delete map;
    player = nullptr;
    map = nullptr;
} 