#include "Slash.h"

Slash::Slash() {
    //default 
    position = glm::vec3(0.0f);
    
    //visual size
    width = 2.0f;    
    height = 2.5f;  

    current_frame = 0;
    animation_time = 0.0f;
    frame_duration = 0.05f;
    
    //states
    is_active = false;
    facing_right = true;
    
    model_matrix = glm::mat4(1.0f);
    
    for (int i = 0; i < TOTAL_FRAMES; i++) {
        texture_ids[i] = 0;
    }
}

Slash::~Slash() {}

void Slash::update(float delta_time) {
    if (!is_active) return;
    
    animation_time += delta_time;
    if (animation_time >= frame_duration) {
        animation_time = 0.0f;
        current_frame++;
        
        if (current_frame >= TOTAL_FRAMES) {
            is_active = false;
            current_frame = 0;
        }
    }
    
    model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    //facing
    if (!facing_right) {
        model_matrix = glm::scale(model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
    
    model_matrix = glm::scale(model_matrix, glm::vec3(width, height, 1.0f));
}

void Slash::render(ShaderProgram* program) {
    if (!is_active) return;
    program->set_model_matrix(model_matrix);
    GLuint current_texture = texture_ids[current_frame];
   
    float tex_coords[] = {
        0.0f, 1.0f, 
        1.0f, 1.0f, 
        1.0f, 0.0f,  
        0.0f, 0.0f  
    };
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, tex_coords[0], tex_coords[1],  // Bottom-left
         0.5f, -0.5f, 0.0f, tex_coords[2], tex_coords[3],  // Bottom-right
         0.5f,  0.5f, 0.0f, tex_coords[4], tex_coords[5],  // Top-right
        
        -0.5f, -0.5f, 0.0f, tex_coords[0], tex_coords[1],  // Bottom-left
         0.5f,  0.5f, 0.0f, tex_coords[4], tex_coords[5],  // Top-right
        -0.5f,  0.5f, 0.0f, tex_coords[6], tex_coords[7]   // Top-left
    };
    
    glBindTexture(GL_TEXTURE_2D, current_texture);
    
    glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Slash::activate(glm::vec3 const& new_position, bool is_facing_right) {
    position = new_position;
    //slash offset
    float x_offset = 1.2f; 
    float y_offset = -0.2f; 
    
    if (is_facing_right) {
        position.x += x_offset;
    } else {
        position.x -= x_offset;
    }
    
    position.y += y_offset;
    
    //states
    is_active = true;
    facing_right = is_facing_right;
    
    //reset animation
    current_frame = 0;
    animation_time = 0.0f;
} 