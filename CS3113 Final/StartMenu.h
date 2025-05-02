#pragma once

#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include <SDL.h>

class StartMenu {
public:
    StartMenu();
    ~StartMenu();
    
    void init(GLuint background_texture_id);
    void render(ShaderProgram* program, GLuint font_texture_id);
    bool process_input(SDL_Event& event);
    
private:
    glm::mat4 model_matrix;
    bool start_pressed;
    GLuint background_texture;
}; 