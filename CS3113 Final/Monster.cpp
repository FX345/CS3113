#include "Monster.h"
#include "Knight.h"
#include "Utility.h"
#include <iostream>

Monster::Monster() {
    width = 1.0f;
    height = 1.0f;
    position = glm::vec3(0);
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
    //only summoned after boss casting spell
    active = false;

    texture_id = Utility::load_texture("assets/boss/individual sprites/monsters/idle.png");
    
    //random speed to change its moving path
    speed = 2.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX); 
}

Monster::~Monster() {
}

void Monster::update(float delta_time) {
    if (!active) return;
    animation_time += delta_time;
    
    animation_index = static_cast<int>((animation_time * animation_fps)) % animation_frames;
    
    glm::vec3 direction_vector = glm::normalize(target_position - position);
    
    //face the knight
    if (direction_vector.x > 0) {
        direction = 0; 
    } else {
        direction = 1; 
    }

    direction_vector.y *= 1.8f; 
    direction_vector = glm::normalize(direction_vector);
    velocity = direction_vector * speed;
    position += velocity * delta_time;
    
    acceleration = glm::vec3(0.0f);
}

void Monster::render(ShaderProgram* program) {
    if (!active) return;
    
    // Create model matrix
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    model_matrix = glm::scale(model_matrix, glm::vec3(visual_scale_x, visual_scale_y, 1.0f));
    
    // Apply horizontal flipping based on direction
    // Direction: 0 = right, 1 = left
    if (direction == 1) {
        model_matrix = glm::scale(model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
    
    // Set model matrix and render
    program->set_model_matrix(model_matrix);
    
    // Calculate texture coordinates for the current animation frame
    float u_width = 1.0f / static_cast<float>(animation_frames);
    float u_offset = static_cast<float>(animation_index) * u_width;
    
    // Draw the monster with the correct animation frame from the sprite sheet
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, u_offset, 1.0f,
         0.5f, -0.5f, 0.0f, u_offset + u_width, 1.0f,
         0.5f,  0.5f, 0.0f, u_offset + u_width, 0.0f,
        
        -0.5f, -0.5f, 0.0f, u_offset, 1.0f,
         0.5f,  0.5f, 0.0f, u_offset + u_width, 0.0f,
        -0.5f,  0.5f, 0.0f, u_offset, 0.0f
    };
    
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    // Set up attributes
    glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    // Draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Disable attributes
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Monster::set_active(bool is_active) {
    active = is_active;
}

void Monster::set_position(glm::vec3 new_position) {
    position = new_position;
}

void Monster::set_target(glm::vec3 new_target) {
    target_position = new_target;
}

bool Monster::is_active() const {
    return active;
}

//check collision with knight
bool Monster::check_collision_with_knight(Knight* knight) const {
    if (!active || !knight) return false;
    
    //hit box
    float knight_half_width = knight->width / 2.0f;
    float knight_half_height = knight->height / 2.0f;
    
    float monster_half_width = width / 2.0f;
    float monster_half_height = height / 2.0f;
    if (position.x - monster_half_width < knight->position.x + knight_half_width &&
        position.x + monster_half_width > knight->position.x - knight_half_width &&
        position.y - monster_half_height < knight->position.y + knight_half_height &&
        position.y + monster_half_height > knight->position.y - knight_half_height) {

        return true;
    }
    
    return false;
}

//dies if collides with fire 
bool Monster::check_collision_with_fire(const glm::vec3& fire_position, float fire_width, float fire_height) const {
    if (!active) return false;
    
    float fire_half_width = fire_width / 2.0f;
    float fire_half_height = fire_height / 2.0f;
    
    float monster_half_width = width / 2.0f;
    float monster_half_height = height / 2.0f;
    
    if (position.x - monster_half_width < fire_position.x + fire_half_width &&
        position.x + monster_half_width > fire_position.x - fire_half_width &&
        position.y - monster_half_height < fire_position.y + fire_half_height &&
        position.y + monster_half_height > fire_position.y - fire_half_height) {
        
        return true;
    }
    
    return false;
} 