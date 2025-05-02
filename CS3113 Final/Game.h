#pragma once

#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Knight.h"
#include "Map.h"
#include <vector>

enum GameState { GAME_ACTIVE, GAME_OVER, GAME_WIN };

class Game {
public:
    Game();
    ~Game();
    
    void initialize();
    void process_input();
    void update(float delta_time);
    void render();
    void shutdown();
    

    GameState state;
    bool running;
    Knight* player;
    Map* map;  
    GLuint font_texture_id;
    ShaderProgram shader_program;
}; 