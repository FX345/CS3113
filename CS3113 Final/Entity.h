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

class Entity {
public:
    Entity();
    ~Entity();

    virtual void update(float delta_time);
    virtual void render(ShaderProgram* program);
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float width;
    float height;
    
    float speed;
    
    GLuint texture_id;
    
    glm::mat4 model_matrix;
    
    bool is_active;
}; 