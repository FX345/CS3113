#pragma once

#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Entity.h"
#include "Map.h"
#include "Boss.h"
#include <vector>

enum Boss2State {
    BOSS2_IDLE,
    BOSS2_MELEE_ATTACK,
    BOSS2_CHARGING,
    BOSS2_HURT,
    BOSS2_TRANSFORM,
    BOSS2_DEMON_IDLE, 
    BOSS2_DEMON_WALK, 
    BOSS2_DEMON_ATTACK, 
    BOSS2_DEMON_SMASH, 
    BOSS2_DEMON_FIRE_BREATH,  
    BOSS2_DEMON_DEATH  
};

class Boss2 : public Entity {
public:
    Boss2();
    ~Boss2();
    
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    void set_target_position(const glm::vec3& target);

    void take_damage(int damage);
    
    bool is_dead() const;
    
    void reset_health();

    void set_map(Map* map_ref) { map = map_ref; }

    void transform();
    
    void play_death_animation();
    
    GLuint transform_textures[32];
    GLuint demon_idle_textures[6]; 
    GLuint demon_walk_textures[6]; 
    GLuint demon_cleave_textures[15];  
    GLuint demon_smash_textures[18]; 
    GLuint demon_fire_breath_textures[21]; 
    GLuint demon_death_textures[22]; 
    int health;
    int max_health;
    std::vector<Projectile> projectiles;
    float visual_scale_x;
    float visual_scale_y;
    bool is_attack_active; 
    bool is_smash_attack; 
    bool is_fire_breath_attack; 
    bool is_death_animation_done; 
    int direction; 
    
private:
    void update_animation();
    void set_animation(Boss2State new_state);
    
    void apply_physics(float delta_time);
    
    void initiate_attack();
    
    int m_animation_frames;
    int m_animation_index;
    float m_animation_time;
    
    Boss2State state;
    
    float melee_damage;
    float attack_radius;
    float detection_radius;
    float attack_cooldown;
    float attack_duration;
    float action_cooldown;
    float transform_duration;
    bool transformed;  
    

    glm::vec3 target_position;

    GLuint idle_textures[8];
    GLuint attack_textures[8];
    GLuint charge_textures[6];
    GLuint hurt_textures[4];
    
    Map* map;
}; 