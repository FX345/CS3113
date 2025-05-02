#include "StartMenu.h"
#include "Utility.h"
#include <iostream>

StartMenu::StartMenu() {
    model_matrix = glm::mat4(1.0f);
    start_pressed = false;
    background_texture = 0; 
}

StartMenu::~StartMenu() {
}

void StartMenu::init(GLuint background_texture_id) {
    background_texture = background_texture_id;
}

void StartMenu::render(ShaderProgram* program, GLuint font_texture_id) {
    if (start_pressed) return; 
    
    //background
    if (background_texture != 0) {
        glm::mat4 original_model_matrix = model_matrix;
        //scale
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::scale(model_matrix, glm::vec3(24.0f, 18.0f, 1.0f)); 
        
        program->set_model_matrix(model_matrix);
        
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
        };
        
        glBindTexture(GL_TEXTURE_2D, background_texture);
        
        glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        model_matrix = original_model_matrix;
    }
    //tutorial
    Utility::draw_text(program, font_texture_id, "Press SPACE to Start", 
                      0.8f, -0.4f, glm::vec3(-4.0f, 0.0f, 0.0f)); 
    
    Utility::draw_text(program, font_texture_id, "Controls:", 
                      0.7f, -0.3f, glm::vec3(-1.5f, -2.0f, 0.0f)); 

    Utility::draw_text(program, font_texture_id, "A/D - Move Left/Right", 
                      0.6f, -0.3f, glm::vec3(-2.5f, -3.0f, 0.0f));
    
    Utility::draw_text(program, font_texture_id, "SPACE - Jump", 
                      0.6f, -0.3f, glm::vec3(-1.5f, -3.8f, 0.0f)); 
    
    Utility::draw_text(program, font_texture_id, "F - Attack", 
                      0.6f, -0.3f, glm::vec3(-1.0f, -4.6f, 0.0f));
    
    Utility::draw_text(program, font_texture_id, "LSHIFT - Defend", 
                      0.6f, -0.3f, glm::vec3(-1.8f, -5.4f, 0.0f)); 
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
//space to start 
bool StartMenu::process_input(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                start_pressed = true;
                std::cout << "Game started!" << std::endl;
                return true;
                break;
            default:
                break;
        }
    }
    
    return false; 
} 