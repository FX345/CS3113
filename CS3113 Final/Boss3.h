#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"
#include "Entity.h"
#include "Monster.h"

class Knight;

enum Boss3State {
    BOSS3_INIT,            
    BOSS3_IDLE_BEFORE_CAST, 
    BOSS3_CAST_SPELL,      
    BOSS3_DEMON_IDLE,   
    BOSS3_DEMON_WALK,     
    BOSS3_DEMON_ATTACK,   
    BOSS3_DEMON_SMASH,      
    BOSS3_DEMON_FIRE_BREATH, 
    BOSS3_DIE  
};


struct FireEffect {
    glm::vec3 position;
    float width;
    float height;
    float collision_width;
    float collision_height;
    bool is_active;
    bool is_obstacle;
    glm::mat4 model_matrix;
    int animation_index;
    float animation_time;
    
    bool contains_point(float x, float y) const {
        float half_width = collision_width / 2.0f;
        float half_height = collision_height / 2.0f;
        return (x >= position.x - half_width && x <= position.x + half_width &&
                y >= position.y - half_height && y <= position.y + half_height);
    }
};

class Boss3 : public Entity {
public:
    Boss3();
    ~Boss3();

    Boss3State state;
    float animation_time = 0.0f;
    float animation_fps = 5.0f; 
    float attack_cooldown = 0.0f;
    int animation_frames = 0;
    int animation_index = 0;
    int health = 10;
    int max_health = 10;
    
    Map* map = nullptr;
    
    float visual_scale_x = 9.0f;
    float visual_scale_y = 9.0f;
    
    bool is_active = true;
    bool is_attack_active = false;
    bool is_death_animation_done = false;
    
    bool is_smash_attack = false;
    bool is_fire_breath_attack = false;
    
    float melee_damage = 1.0f;
    float attack_radius = 5.0f;
    float detection_radius = 10.0f;
    float attack_duration = 0.0f;
    float action_cooldown = 0.0f;
    int direction = 0;

    glm::vec3 target_position;

    GLuint cast_spell_textures[6]; 
    GLuint demon_idle_textures[6]; 
    GLuint demon_walk_textures[6]; 
    GLuint demon_cleave_textures[15]; 
    GLuint demon_smash_textures[27];
    GLuint demon_fire_breath_textures[21]; 
    GLuint demon_death_textures[22]; 
    GLuint fire_effect_texture;  
    
    std::vector<FireEffect> fire_effects;
    bool fire_effects_active = false;
    
    static const int NUM_MONSTERS = 2;
    Monster monsters[NUM_MONSTERS];
    bool monsters_active = false;
    glm::vec4 render_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); 

    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    void cast_spell();
    void set_idle_before_cast();
    void set_demon_idle();
    void set_demon_walk();
    void die();
    void play_death_animation();
    bool is_dead() const;
    void set_target_position(const glm::vec3& position);
    void take_damage(int damage);
    

    void create_fire_effects();
    void update_fire_effects(float delta_time);
    void render_fire_effects(ShaderProgram* program);
    bool check_fire_collision(const glm::vec3& entity_position, float entity_width, float entity_height) const;
    void check_knight_fire_collision(Knight* knight);
    

    void create_monsters();
    void update_monsters(float delta_time);
    void render_monsters(ShaderProgram* program);
    void check_monster_knight_collision(Knight* knight);
    void check_monster_fire_collision();
    

    void initiate_attack();
    void set_animation(Boss3State new_state);
    void move_toward_target(float delta_time);
    

    void set_map(Map* map_param);
}; 