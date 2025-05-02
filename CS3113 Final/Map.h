#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ShaderProgram.h"
#include <vector>
#include <glm/glm.hpp>

class Map {
private:
    GLuint background_texture_id;
    GLuint platform_texture_id;
    GLuint black_texture_id = 0;
    
    std::vector<glm::vec3> platforms;
    float platform_width;
    float platform_height;
    
    float left_wall_height;
    float left_wall_x;
    float left_wall_bottom_y;
    
    float right_wall_height;
    float right_wall_x;
    float right_wall_bottom_y;
    
    void load_textures();
    
public:
    float left_boundary = -10.0f;
    float right_boundary = 10.0f;
    float top_boundary = 6.0f;
    float bottom_boundary = -6.0f;

    Map();
    ~Map();
    

    void render(ShaderProgram* program);
    bool is_solid(float x, float y);
    bool check_platform_collision(float x, float y, float* y_correction);
    float check_point_y_collision(float x, float y, float prev_y);
    

    const float gravity = -9.8f;
}; 