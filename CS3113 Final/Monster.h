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
#include "Entity.h"


class Knight;
class Boss3;

class Monster : public Entity {
public:

    Monster();
    ~Monster();

    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    void set_active(bool active);
    void set_position(glm::vec3 position);
    void set_target(glm::vec3 target);
    bool is_active() const;
    

    bool check_collision_with_knight(Knight* knight) const;
    bool check_collision_with_fire(const glm::vec3& fire_position, float fire_width, float fire_height) const;
    

    float speed = 2.0f;
    GLuint texture_id;
    bool active = false;
    glm::vec3 target_position;
    

    float visual_scale_x = 3.0f;
    float visual_scale_y = 3.0f;
    

    float animation_time = 0.0f;
    int animation_frames = 4; 
    int animation_index = 0;
    float animation_fps = 4.0f;
    int direction = 0; 
}; 