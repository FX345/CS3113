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
#include "Map.h"
#include <vector>

enum BossState { BOSS_IDLE, BOSS_MOVING, BOSS_ATTACKING, BOSS_CHARGING, BOSS_HURT, BOSS_FLAME_ATTACK };

struct Projectile {
    glm::vec3 position;
    glm::vec3 velocity;
    float width;
    float height;
    bool is_active;
    int animation_index;
    float animation_time;
    glm::mat4 model_matrix;
};

class Boss : public Entity {
public:
    Boss();
    ~Boss();
    
    void update(float delta_time);
    void render(ShaderProgram* program);
    
    void set_animation(BossState new_state);
    void update_animation();
    void set_target_position(const glm::vec3& target);
    void take_damage(int damage = 1);
    bool is_dead() const;
    void reset_health();
    void fire_projectile();
    void update_projectiles(float delta_time);
    void render_projectiles(ShaderProgram* program);
    void set_map(Map* map_pointer) { map = map_pointer; }
    
    int m_animation_frames;
    int m_animation_index;
    float m_animation_time;
    
    float visual_scale_x = 1.0f;
    float visual_scale_y = 1.0f;
    
    GLuint idle_textures[6];
    GLuint move_textures[8];
    GLuint attack_textures[6];  
    GLuint charge_textures[6];
    GLuint hurt_textures[6];
    GLuint flame_textures[3];
    
    BossState state;
    int direction;
    int health;
    int max_health;
    float action_cooldown;
    float attack_duration;
    
    glm::vec3 target_position;
    float detection_radius;
    float attack_radius;
    float flame_attack_radius;
    float flame_cooldown;
    
    std::vector<Projectile> projectiles;
    float projectile_speed;
    float projectile_cooldown;
    float max_projectile_distance;
    
    float attack_damage;
    
    Map* map = nullptr;
}; 