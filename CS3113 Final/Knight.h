#pragma once

#include "Entity.h"
#include <vector>
#include "Boss.h"

enum KnightState { IDLE, WALKING, RUNNING, JUMPING, FALLING, ATTACKING, DEFENDING, HURT, DEAD };
enum KnightDirection { LEFT, RIGHT };

class Knight : public Entity {
public:
    Knight();
    
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    
    void set_animation(KnightState new_state);
    void update_animation();
    void set_direction(const std::string& new_direction);
    void attack();
    void jump();
    void defend();
    void hurt();
    
    void take_damage(int damage = 1);
    bool is_dead() const;
    void reset_health();
   
    bool check_projectile_collision(const std::vector<Projectile>& projectiles);
    
    KnightState state;
    KnightDirection direction;
    
    int* m_animation_indices = nullptr;
    int* m_walking[2]; 
    int* m_running[2]; 
    int* m_idle[2];   
    int* m_jumping[2]; 
    int* m_falling[2]; 
    int* m_attacking[2]; 
    int* m_defending[2]; 
    int* m_hurt[2];
    
    int m_animation_frames;
    int m_animation_index;
    float m_animation_time;
    int m_animation_cols;
    int m_animation_rows;
    
    bool move_left;
    bool move_right;
    bool move_up;
    bool move_down;
    bool is_attacking;
    bool is_jumping;
    bool is_defending;
    bool is_grounded;
    float attack_cooldown;
    float defend_cooldown;
    float defend_duration; 
    

    float jump_power;
    bool can_jump;
    
    bool is_hurt;
    bool is_invincible;
    float hurt_timer;
    

    int health;
    int max_health;
}; 