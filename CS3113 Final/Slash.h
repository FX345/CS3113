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

class Slash {
public:
    Slash();
    ~Slash();
    
    void update(float delta_time);
    void render(ShaderProgram* program);

    void activate(glm::vec3 const& position, bool facing_right);
    

    glm::vec3 position;
    glm::mat4 model_matrix;

    int current_frame;
    float animation_time;
    float frame_duration;
    

    bool is_active;
    bool facing_right;
    

    float width;
    float height;
    GLuint texture_ids[5]; 
    

    static const int TOTAL_FRAMES = 5;
}; 