#include "Boss2.h"
#include "ShaderProgram.h"
#include "Utility.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib> 
#include <ctime>   

Boss2::Boss2() :
    //initial parameters
    m_animation_frames(8),
    m_animation_index(0),
    m_animation_time(0.0f),
    state(BOSS2_IDLE),
    direction(0), 
    health(25),
    max_health(25),
    melee_damage(3.0f),
    attack_radius(5.0f),
    detection_radius(6.0f),
    attack_cooldown(0.0f),
    attack_duration(0.0f),
    action_cooldown(0.0f),
    transform_duration(0.0f),
    transformed(false),
    is_attack_active(false),
    is_smash_attack(false),
    is_fire_breath_attack(false),
    is_death_animation_done(false),
    visual_scale_x(1.2f),
    visual_scale_y(1.2f),
    map(nullptr)
{
    //reandom number generator
    srand(static_cast<unsigned int>(time(nullptr)));
    
  
    width = 1.0f;
    height = 1.0f;
    speed = 1.5f;
    is_active = true;
    
    //default position
    position = glm::vec3(0.0f);
    
    //gravity
    acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
    velocity = glm::vec3(0.0f);
    
    //load idle textures
    for (int i = 0; i < 8; i++) {
        std::string path = "assets/boss2/idle/idle_" + std::to_string(i + 1) + ".png";
        idle_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load attack textures
    for (int i = 0; i < 8; i++) {
        std::string path = "assets/boss2/attack/attack_" + std::to_string(i + 1) + ".png";
        attack_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load charge textures 
    for (int i = 0; i < 6; i++) {
        std::string path = "assets/boss2/charge/charge_" + std::to_string(i + 1) + ".png";
        charge_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load hurt textures
    for (int i = 0; i < 4; i++) {
        std::string path = "assets/boss2/hurt/hurt_" + std::to_string(i + 1) + ".png";
        hurt_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load transform textures
    for (int i = 0; i < 32; i++) {
        std::string path = "assets/boss/individual sprites/04_transform/transform_" + std::to_string(i + 1) + ".png";
        transform_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load demon idle textures
    for (int i = 0; i < 6; i++) {
        std::string path = "assets/boss/individual sprites/05_demon_idle/demon_idle_" + std::to_string(i + 1) + ".png";
        demon_idle_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load demon walk textures
    for (int i = 0; i < 6; i++) {
        std::string path = "assets/boss/individual sprites/06_demon_walk/demon_walk_" + std::to_string(i + 1) + ".png";
        demon_walk_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load demon cleave attack textures - updated for all 15 frames
    for (int i = 0; i < 15; i++) {
        std::string path = "assets/boss/individual sprites/07_demon_cleave/demon_cleave_" + std::to_string(i + 1) + ".png";
        demon_cleave_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load demon smash attack textures - 18 frames
    for (int i = 0; i < 18; i++) {
        std::string path = "assets/boss/individual sprites/08_demon_smash/demon_smash_" + std::to_string(i + 1) + ".png";
        demon_smash_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load demon fire breath attack textures - 21 frames
    for (int i = 0; i < 21; i++) {
        std::string path = "assets/boss/individual sprites/09_demon_fire_breath/demon_fire_breath_" + std::to_string(i + 1) + ".png";
        demon_fire_breath_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load demon death animation textures - 22 frames
    for (int i = 0; i < 22; i++) {
        std::string path = "assets/boss/individual sprites/12_demon_death/demon_death_" + std::to_string(i + 1) + ".png";
        demon_death_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //idle as initial
    set_animation(BOSS2_IDLE);

    texture_id = idle_textures[0];
}

Boss2::~Boss2() {
}

void Boss2::update(float delta_time) {
    
    if (!is_active) return;
    
   
    m_animation_time += delta_time;

    if (action_cooldown > 0) {
        action_cooldown -= delta_time;
    }
    
    if (attack_cooldown > 0) {
        attack_cooldown -= delta_time;
    }
    
    if (attack_duration > 0) {
        attack_duration -= delta_time;
        
        //if attack is over, return to idle/walk
        if (attack_duration <= 0) {
            if (state == BOSS2_MELEE_ATTACK || state == BOSS2_DEMON_ATTACK || state == BOSS2_DEMON_SMASH) {
          
                is_attack_active = false;
                is_smash_attack = false;
           
                if (transformed) {
                    set_animation(BOSS2_DEMON_WALK);
                } else {
                    set_animation(BOSS2_IDLE);
                }
            }
        }
    }

    if (transform_duration > 0) {
        transform_duration -= delta_time;
    }
    
    //distance check
    float x_distance = target_position.x - position.x;
    float y_distance = target_position.y - position.y;
    float distance = glm::sqrt(x_distance * x_distance + y_distance * y_distance);
    
    //set direction to player's position
    if (x_distance > 0) {
        direction = 1; 
    } else {
        direction = 0; 
    }
    
    //if attacking, forbid all other actions
    if (state == BOSS2_DEMON_ATTACK || state == BOSS2_MELEE_ATTACK) {
        update_animation();
        velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        apply_physics(delta_time);
        return;
    }
    //boss chasing player logic, starting attacking when near
    switch (state) {
        case BOSS2_IDLE:
            if (action_cooldown <= 0) {
                if (distance <= attack_radius && attack_cooldown <= 0) {
                    initiate_attack();
                } else if (distance > 5.0f) {
                    //move toward player until within 5 units
                    float x_dir = x_distance / (distance + 0.01f); 
                    velocity.x = x_dir * speed;
                    // set walking animation if transformed and moving
                    if (transformed) {
                        set_animation(BOSS2_DEMON_WALK);
                    }
                } else {
                    //within attack radius but on cooldown, stop moving
                    velocity.x = 0.0f;
                    if (transformed) {
                        set_animation(BOSS2_DEMON_WALK);
                    }
                }
            }
            break;
            
        case BOSS2_DEMON_WALK:
            if (distance <= attack_radius && attack_cooldown <= 0) {
                initiate_attack();
            } else if (distance > 5.0f) {
                float x_dir = x_distance / (distance + 0.01f); 
                velocity.x = x_dir * speed;
            } else {
                //within attack radius but on cooldown, stop moving
                velocity.x = 0.0f;
            }
            break;
            
        case BOSS2_TRANSFORM:
            //during transform, halt all action
            velocity = glm::vec3(0.0f);
            if (map) {
                float feet_position = position.y - (height / 2.0f);
                if (!map->is_solid(position.x, feet_position - 0.1f)) {
                    for (float y_check = feet_position; y_check > feet_position - 5.0f; y_check -= 0.2f) {
                        if (map->is_solid(position.x, y_check)) {
                            position.y = y_check + (height / 2.0f) + 0.5f; 
                            break;
                        }
                    }
                }
            }
            break;
    }
    
    update_animation();
    apply_physics(delta_time);
}

//helper method to apply physics and platform detection
void Boss2::apply_physics(float delta_time) {
    if (state != BOSS2_TRANSFORM) {
        velocity.y += acceleration.y * delta_time;
    }

    position.y += velocity.y * delta_time;
    //boss should always stay on platform
    if (map) {
        float feet_y = position.y - (height / 2.0f);
        bool found_platform = false;
        if (map->is_solid(position.x, feet_y - 0.1f)) {
            velocity.y = 0.0f;
            found_platform = true;
        } else {
            for (float check_y = feet_y; check_y > feet_y - 5.0f; check_y -= 0.2f) {
                if (map->is_solid(position.x, check_y)) {
                    position.y = check_y + (height / 2.0f) + 0.5f;
                    velocity.y = 0.0f;
                    found_platform = true;
                    break;
                }
            }
        }
        //limit falling
        if (!found_platform && velocity.y < -5.0f) {
            velocity.y = -5.0f; 
        }
    }
    position.x += velocity.x * delta_time;
    //check wall collision
    if (map) {
        if (velocity.x < 0 && map->is_solid(position.x - width/2, position.y)) {
            velocity.x = 0;
        }
        else if (velocity.x > 0 && map->is_solid(position.x + width/2, position.y)) {
            velocity.x = 0;
        }
    }
    
    //update the model matrix
    model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    //visual scaling
    model_matrix = glm::scale(model_matrix, glm::vec3(visual_scale_x, visual_scale_y, 1.0f));
    
    if (direction == 1) {
        model_matrix = glm::scale(model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
}

void Boss2::render(ShaderProgram* program) {
    if (!is_active) return;
    glm::mat4 visual_model_matrix = glm::mat4(1.0f);
    glm::vec3 adjusted_position = position + glm::vec3(0.0f, 3.0f, 0.0f); 
    visual_model_matrix = glm::translate(visual_model_matrix, adjusted_position);
    visual_model_matrix = glm::scale(visual_model_matrix, glm::vec3(visual_scale_x, visual_scale_y, 1.0f));
    if (direction == 1) {
        visual_model_matrix = glm::scale(visual_model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
    program->set_model_matrix(visual_model_matrix);
    
    float u = 0.0f;
    float v = 0.0f;
    float width = 1.0f;
    float height = 1.0f;
    
    float texCoords[] = {
        u, v + height,
        u + width, v,
        u, v,
        u + width, v,
        u, v + height,
        u + width, v + height
    };
    
    float vertices[] = {
        -0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f,
        0.5f, 0.5f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    if (texture_id == 0) {
        if (transformed) {
            texture_id = demon_idle_textures[0];
        } else {
            texture_id = idle_textures[0];
        }
    }
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Boss2::update_animation() {
    float frames_per_second = 10.0f;
    
    switch (state) {
        case BOSS2_TRANSFORM:
            frames_per_second = 12.0f; 
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= 32) {
                    m_animation_index = 0;
                    transformed = true;
                    set_animation(BOSS2_DEMON_WALK);
                }
                texture_id = transform_textures[m_animation_index];
            }
            break;
            
        case BOSS2_DEMON_IDLE:
            set_animation(BOSS2_DEMON_WALK);
            break;
            
        case BOSS2_DEMON_WALK:
            frames_per_second = 10.0f;
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= 6) {
                    m_animation_index = 0;
                }
                texture_id = demon_walk_textures[m_animation_index];
            }
            break;
            
        case BOSS2_DEMON_ATTACK:
            frames_per_second = 12.0f; 
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                if (m_animation_index >= 10 && m_animation_index <= 15) {
                    is_attack_active = true;
                    is_smash_attack = false;
                } else {
                    is_attack_active = false;
                    is_smash_attack = false;
                }
                
                if (m_animation_index >= 15) {
                    m_animation_index = 0;
                    set_animation(BOSS2_DEMON_WALK);
                    attack_duration = 0.0f;
                    is_attack_active = false;
                    is_smash_attack = false;
                }
                texture_id = demon_cleave_textures[m_animation_index];
            }
            break;
            
        case BOSS2_DEMON_SMASH:
            frames_per_second = 12.0f; 
            //faster animation for smash attack
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                //check damage frames
                if (m_animation_index >= 12 && m_animation_index <= 17) {
                    is_attack_active = true;
                    is_smash_attack = true; 
                } else {
                    is_attack_active = false;
                    is_smash_attack = false;
                }
                //return to walk animation
                if (m_animation_index >= 18) {
                    m_animation_index = 0;
                    set_animation(BOSS2_DEMON_WALK);
                    attack_duration = 0.0f;
                    is_attack_active = false;
                    is_smash_attack = false;
                }
                
                texture_id = demon_smash_textures[m_animation_index];
            }
            break;
            
        case BOSS2_DEMON_FIRE_BREATH:
            frames_per_second = 12.0f; 
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                //damage frame, make sure the damage happens when the fire visually touches the player
                if (m_animation_index >= 15 && m_animation_index <= 17) {
                    is_attack_active = true;
                    is_smash_attack = false; 
                    is_fire_breath_attack = true; 
                } else {
                    is_attack_active = false;
                    is_fire_breath_attack = false;
                }
                
                if (m_animation_index >= 21) {
                    m_animation_index = 0;
                    //return to walk after attack completes
                    set_animation(BOSS2_DEMON_WALK);
                    //reset attack cooldown
                    attack_duration = 0.0f;
                    is_attack_active = false;
                    is_fire_breath_attack = false;
                }

                texture_id = demon_fire_breath_textures[m_animation_index];
            }
            break;
            
        case BOSS2_DEMON_DEATH:
            frames_per_second = 8.0f; 
            //death, to next stage
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                //keep the last frame
                if (m_animation_index >= 22) {
                    m_animation_index = 21; 
                    is_death_animation_done = true;
                }
                texture_id = demon_death_textures[m_animation_index];
            }
            break;
            
        case BOSS2_IDLE:
            frames_per_second = 10.0f;
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= 8) {
                    m_animation_index = 0;
                }
                texture_id = idle_textures[m_animation_index];
            }
            break;
            
        case BOSS2_MELEE_ATTACK:
            frames_per_second = 12.0f;
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= 8) {
                    m_animation_index = 0;
                    if (transformed) {
                        set_animation(BOSS2_DEMON_WALK);
                    } else {
                        set_animation(BOSS2_IDLE);
                    }
                }
                
                texture_id = attack_textures[m_animation_index];
            }
            break;
            
        case BOSS2_CHARGING:
            frames_per_second = 10.0f;
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= 6) {
                    m_animation_index = 0;
                }

                texture_id = charge_textures[m_animation_index % 6]; 
            }
            break;
            
        default:
            int max_frames = m_animation_frames;
            if (m_animation_time >= 1.0f / frames_per_second) {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= max_frames) {
                    m_animation_index = 0;
                }
            }
            break;
    }
}

void Boss2::set_animation(Boss2State new_state) {
    //only change state if it's different
    if (state == new_state) return;
    state = new_state;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    
    //set animation frames based on state, after transformation
    switch (state) {
        case BOSS2_IDLE:
            if (transformed) {
                state = BOSS2_DEMON_WALK;
                m_animation_frames = 6;
                texture_id = demon_walk_textures[0];
            } else {
                m_animation_frames = 8;
                texture_id = idle_textures[0];
            }
            break;
            //boss AI
        case BOSS2_MELEE_ATTACK:
            if (transformed) {
                //randomly choose between cleave, smash, and fire breath attacks by giving random numbers
                int attack_type = rand() % 3; 
                
                if (attack_type == 0) {
                    state = BOSS2_DEMON_ATTACK;
                    m_animation_frames = 15;
                    texture_id = demon_cleave_textures[0];
                    //cleave attack can be defended 
                    //but cannot be dodged by jumping
                } else if (attack_type == 1) {
                    state = BOSS2_DEMON_SMASH;
                    m_animation_frames = 18;
                    texture_id = demon_smash_textures[0];
                    //smash attack ignores shield
                    //but can be dodged by jumping
                } else {
                    state = BOSS2_DEMON_FIRE_BREATH;
                    m_animation_frames = 21;
                    texture_id = demon_fire_breath_textures[0];
                    //fire breath attack can be defended against
                    //but the damage frame comes late, so it is a tricky attack if coming after cleave attack
                }
            } else {
                m_animation_frames = 8;
                texture_id = attack_textures[0];
            }
            break;
            
        case BOSS2_CHARGING:
            m_animation_frames = 6;
            texture_id = charge_textures[0];
            break;
            
        case BOSS2_TRANSFORM:
            m_animation_frames = 32;
            texture_id = transform_textures[0];
            break;
            
        case BOSS2_DEMON_IDLE:
            state = BOSS2_DEMON_WALK;
            m_animation_frames = 6;
            texture_id = demon_walk_textures[0];
            break;
            
        case BOSS2_DEMON_WALK:
            m_animation_frames = 6;
            texture_id = demon_walk_textures[0];
            break;
            
        case BOSS2_DEMON_ATTACK:
            m_animation_frames = 15;
            texture_id = demon_cleave_textures[0];
            break;
            
        case BOSS2_DEMON_SMASH:
            m_animation_frames = 18;
            texture_id = demon_smash_textures[0];
            break;
            
        case BOSS2_DEMON_FIRE_BREATH:
            m_animation_frames = 21;
            texture_id = demon_fire_breath_textures[0];
            break;
            
        case BOSS2_DEMON_DEATH:
            m_animation_frames = 22;
            texture_id = demon_death_textures[0];
            break;
    }
}

void Boss2::take_damage(int damage) {
    //only take damage if not transforming
    if (state != BOSS2_TRANSFORM) {
        health -= damage;
        
        //ensure health doesn't go below 0
        if (health < 0) health = 0;
        
        // Check if the boss has died
        if (health <= 0) {
            play_death_animation();
            return;
        }
        action_cooldown = 0.3f;
        float knockback_force = 0.8f;
        float knock_dir = (direction == 0) ? knockback_force : -knockback_force;
        velocity.x += knock_dir;
        velocity.y = 0.0f;
    }
}

bool Boss2::is_dead() const {
    return health <= 0 || state == BOSS2_DEMON_DEATH;
}

void Boss2::reset_health() {
    health = max_health;
    set_animation(BOSS2_IDLE);
}

void Boss2::set_target_position(const glm::vec3& target) {
    target_position = target;
}

void Boss2::initiate_attack() {
    is_attack_active = false;
    is_smash_attack = false;

    if (transformed) {
        set_animation(BOSS2_MELEE_ATTACK);
        //set attack parameters based on which attack was chosen
        if (state == BOSS2_DEMON_ATTACK) {
            // cleave attack parameters
            attack_duration = 1.5f;     
            attack_cooldown = 2.0f;    
        } else if (state == BOSS2_DEMON_SMASH) {
            //smash attack parameters (slightly longer)
            attack_duration = 2.0f;    
            attack_cooldown = 3.0f;   
        } else if (state == BOSS2_DEMON_FIRE_BREATH) {
            //fire breath attack parameters (longest animation)
            attack_duration = 2.5f;  
            attack_cooldown = 3.0f; 
        }
        
        //set action cooldown based on the chosen attack duration
        action_cooldown = attack_duration;
    } else {
        set_animation(BOSS2_MELEE_ATTACK);

        attack_duration = 1.0f;    
        attack_cooldown = 2.0f;     
        action_cooldown = 1.0f; 
    }
    
    //completely stop all movement during attack
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Boss2::transform() {
    set_animation(BOSS2_TRANSFORM);
    
    //transform parameters
    transform_duration = 3.0f; 
    action_cooldown = 3.0f; 
    
    //stop movement during transform
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    //make sure not in the air
    if (map) {
        float search_y = position.y - (height / 2.0f);
        for (float y_check = search_y; y_check > search_y - 5.0f; y_check -= 0.5f) {
            if (map->is_solid(position.x, y_check)) {
                position.y = y_check + (height / 2.0f) + 0.5f; 
                break;
            }
        }
    }
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}
//dead 
void Boss2::play_death_animation() {
    if (state != BOSS2_DEMON_DEATH) {
        set_animation(BOSS2_DEMON_DEATH);
        health = 0;
        velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        attack_cooldown = 999.0f;
        action_cooldown = 999.0f;
        m_animation_index = 0;
        m_animation_time = 0.0f;
        is_death_animation_done = false;
    }
} 