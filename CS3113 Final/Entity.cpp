#include "Entity.h"

Entity::Entity() {
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    
    width = 1.0f;
    height = 1.0f;
    
    speed = 0.0f;
    
    model_matrix = glm::mat4(1.0f);
    
    is_active = true;
}

Entity::~Entity() {}

void Entity::update(float delta_time) {
    if (!is_active) return;
    
    velocity += acceleration * delta_time;
    position += velocity * delta_time;
    
    model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
}

void Entity::render(ShaderProgram* program) {
    if (!is_active) return;
    
    program->set_model_matrix(model_matrix);
    
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };
    float tex_coords[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
} 