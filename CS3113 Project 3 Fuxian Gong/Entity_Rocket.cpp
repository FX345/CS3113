#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"               // We'll talk about these later in the course
#include "Entity_Platform.h"
#include "Entity_Rocket.h"

//gravity
constexpr float GRAVITY = -9.8f;
Rocket::Rocket(glm::vec3 position, glm::vec3 scale, float speed, GLuint textid){
    m_position = position;
    m_scale = scale;
    m_speed = speed;
    m_model_matrix = glm::mat4(1.0f);
    m_texture_id = textid;
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(-30.0f, GRAVITY, 0.0f);
    m_fuel = 500.0f; 
}

//fuel is reduced when moving positively, not reduced when moving due to inertia
void Rocket::inc_x(){
    if (m_fuel > 0.0f) {
        m_acceleration.x += 0.4f;
        m_fuel -= 0.2f; 
    }
}

void Rocket::dec_x(){
    if (m_fuel > 0.0f) {
        m_acceleration.x -= 0.4f;
        m_fuel -= 0.2f;
    }
}

void Rocket::inc_y(){
    if (m_fuel > 0.0f) {
        m_acceleration.y += 0.4f;
        m_fuel -= 0.2f;
    }
}

void Rocket::dec_y(){
    if (m_fuel > 0.0f) {
        m_acceleration.y -= 0.4f;
        m_fuel -= 0.2f;
    }
}

//collision detecting platform
bool Rocket::collision(Platform* platform){
    float x_distance = fabs(m_position.x - platform->get_position().x) - ((m_scale.x + platform->get_scale().x) / 2.0f);
    float y_distance = fabs(m_position.y - platform->get_position().y) - ((m_scale.y + platform->get_scale().y) / 2.0f);

    if (x_distance < 0.0f && y_distance < 0.0f) {
        return true;
    }
    return false;
}
//update motion
void Rocket::update(float delta_time){
    m_acceleration.y += GRAVITY * delta_time;
    m_velocity += m_acceleration * delta_time;
    m_position += m_velocity * delta_time;

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);

    if (glm::length(m_velocity) > 1.0f) {
        m_velocity = glm::normalize(m_velocity);
    }
}
//render
void Rocket::render(ShaderProgram* program) {
    program->set_model_matrix(m_model_matrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
