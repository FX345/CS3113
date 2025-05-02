#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum EffectType { NONE, FADEIN, SHAKE_H };

class Effects {
private:
    ShaderProgram m_shader_program;
    float         m_alpha;
    EffectType    m_current_effect;
    float         m_effect_speed;
    float         m_size;
    GLuint        m_overlay_texture;  
    float         m_effect_time;     
    float         m_max_effect_time; 
    float         m_shake_intensity; 

public:
    glm::vec3 m_view_offset;
    
    Effects(glm::mat4 projection_matrix, glm::mat4 view_matrix);
    ~Effects();  

    void draw_overlay();
    void start(EffectType effect_type, float duration = 1.0f, float intensity = 1.0f);
    void update(float delta_time);
    void render();
    
  
    bool is_done() const { return m_current_effect == NONE; }
}; 