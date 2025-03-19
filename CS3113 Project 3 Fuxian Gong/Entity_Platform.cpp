#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity_Platform.h"
#include "Entity_Rocket.h"

Platform::Platform(glm::vec3 position, glm::vec3 scale, GLuint texture_id) {
    m_position = position;
    m_scale = scale;
    m_texture_id = texture_id;
    m_model_matrix = glm::mat4(1.0f);
}

void Platform::update() {
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}

Platform::Platform() {
    m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
    m_model_matrix = glm::mat4(1.0f);
}

//render
void Platform::render(ShaderProgram* program) {
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
