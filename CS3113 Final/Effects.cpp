#include "Effects.h"
#include <iostream>

Effects::Effects(glm::mat4 projection_matrix, glm::mat4 view_matrix) : 
    m_current_effect(NONE), 
    m_alpha(1.0f), 
    m_effect_speed(0.8f), 
    m_size(40.0f),
    m_view_offset(glm::vec3(0.0f)),
    m_effect_time(0.0f),
    m_max_effect_time(0.0f),
    m_shake_intensity(0.0f)
{
    m_shader_program.load("shaders/vertex_textured.glsl", "shaders/fragment_fade.glsl");
    m_shader_program.set_projection_matrix(projection_matrix);
    m_shader_program.set_view_matrix(view_matrix);
    glGenTextures(1, &m_overlay_texture);
    glBindTexture(GL_TEXTURE_2D, m_overlay_texture);

    GLubyte pixel[4] = {0, 0, 0, 255}; 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    
    //set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Effects::~Effects() {
    if (m_overlay_texture != 0) {
        glDeleteTextures(1, &m_overlay_texture);
        m_overlay_texture = 0;
    }
}

void Effects::draw_overlay() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
    };

    glVertexAttribPointer(m_shader_program.get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
    glEnableVertexAttribArray(m_shader_program.get_position_attribute());
    
    glVertexAttribPointer(m_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
    glEnableVertexAttribArray(m_shader_program.get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(m_shader_program.get_position_attribute());
    glDisableVertexAttribArray(m_shader_program.get_tex_coordinate_attribute());
}

void Effects::start(EffectType effect_type, float duration, float intensity) {
    m_current_effect = effect_type;
    m_effect_time = 0.0f;
    m_max_effect_time = duration;
    m_shake_intensity = intensity;

    switch (m_current_effect) {
        case NONE:   
            break;
        //fadein in stage 1
        case FADEIN: 
            m_alpha = 1.0f; 
            break;
        //camera shaking in stage 2    
        case SHAKE_H:
            m_view_offset = glm::vec3(0.0f);
            break;
    }
}

void Effects::update(float delta_time) {
    //update effect timer for all effects
    if (m_current_effect != NONE) {
        m_effect_time += delta_time;

        if (m_effect_time >= m_max_effect_time) {
            if (m_current_effect != FADEIN) {
                m_current_effect = NONE;
                m_view_offset = glm::vec3(0.0f); 
                return;
            }
        }
    }
    
    switch (m_current_effect) {
        case NONE:   
            break;
       
        case FADEIN: 
            m_alpha -= m_effect_speed * delta_time;
            if (m_alpha <= 0.0f) {
                m_alpha = 0.0f;
                m_current_effect = NONE;
            }
            break;
            
        case SHAKE_H:
            float frequency = 20.0f; 
            float remaining_time = m_max_effect_time - m_effect_time;
            float falloff = (remaining_time / m_max_effect_time); 
            //generate a horizontal shake only
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.2f;
            float shake_amount = sin(m_effect_time * frequency) * m_shake_intensity * falloff + noise;
            m_view_offset.x = shake_amount;
            break;
    }
}

void Effects::render() {
    if (m_current_effect == NONE) return;

    switch (m_current_effect) {
        case NONE:   
            break;
        case FADEIN:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            GLint current_program;
            glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

            glUseProgram(m_shader_program.get_program_id());

            m_shader_program.set_view_matrix(glm::mat4(1.0f));
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(m_size, m_size, 1.0f));
            m_shader_program.set_model_matrix(model);
            m_shader_program.set_colour(0.0f, 0.0f, 0.0f, m_alpha);
            
            glBindTexture(GL_TEXTURE_2D, m_overlay_texture);

            draw_overlay();

            glUseProgram(current_program);
            break;  
        case SHAKE_H:
            break;
    }
} 