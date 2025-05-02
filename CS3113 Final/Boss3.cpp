#include "Boss3.h"
#include <iostream>
#include "Utility.h"
#include <cstdlib> 
#include "Knight.h" 

Boss3::Boss3() {
    width = 2.0f;
    height = 3.0f;

    state = BOSS3_INIT;
    animation_time = 0.0f;
    animation_index = 0;
    animation_frames = 6;
    position = glm::vec3(0.0f);
    animation_fps = 5.0f;
    
    //set health
    health = 15;
    max_health = 15;
    
    //set texture
    for (int i = 0; i < 6; i++) {
        cast_spell_textures[i] = 0;
        demon_idle_textures[i] = 0;
        demon_walk_textures[i] = 0;
    }
    
    for (int i = 0; i < 15; i++) {
        if (i < 15) demon_cleave_textures[i] = 0;
        if (i < 27) demon_smash_textures[i] = 0;
        if (i < 21) demon_fire_breath_textures[i] = 0;
    }
    
    //fire in map as obstacles
    fire_effect_texture = Utility::load_texture("assets/effects/Spritesheets/Fire+Sparks-Sheet.png");
}

Boss3::~Boss3() {
}

void Boss3::update(float delta_time) {
    if (!is_active) return;
    animation_time += delta_time;
    
    if (attack_cooldown > 0) attack_cooldown -= delta_time;
    if (action_cooldown > 0) action_cooldown -= delta_time;
    
    //update direction to face the player
    if (target_position.x > position.x) {
        direction = 0; 
    }
    else {
        direction = 1;
    }
    
    //states
    switch (state) {
        case BOSS3_INIT:
            //initial state, start with idle animation before casting
            set_idle_before_cast();
            break;
            
        case BOSS3_IDLE_BEFORE_CAST:
            //cast spell to change map
            animation_index = static_cast<int>((animation_time * animation_fps)) % animation_frames;
            texture_id = demon_idle_textures[animation_index];
            if (animation_time >= 1.0f) {
                cast_spell();
            }
            break;
            
        case BOSS3_CAST_SPELL:
            //cast for 3 sec
            if (animation_time >= 3.0f) {
                //create fire effects when transitioning to idle
                if (!fire_effects_active) {
                    create_fire_effects();
                    fire_effects_active = true;
                    
                    //summons
                    create_monsters();
                    monsters_active = true;
                }
                set_demon_walk(); 
            } else {
                animation_index = static_cast<int>((animation_time * animation_fps)) % animation_frames;
                texture_id = cast_spell_textures[animation_index];
                std::cout << "Cast spell animation frame: " << animation_index + 1 << std::endl;
            }
            break;
            
        case BOSS3_DEMON_IDLE:
            animation_index = static_cast<int>((animation_time * animation_fps)) % animation_frames;
            texture_id = demon_idle_textures[animation_index];
            if (animation_time >= 2.0f) {
                set_demon_walk();
            }
            break;
            
        case BOSS3_DEMON_WALK:
            animation_index = static_cast<int>((animation_time * animation_fps)) % animation_frames;
            texture_id = demon_walk_textures[animation_index];
            //if no active cooldown, try to attack if in range
            if (action_cooldown <= 0 && attack_cooldown <= 0) {
                //distance check
                float dist_x = target_position.x - position.x;
                float dist_y = target_position.y - position.y;
                float distance_to_target = glm::sqrt(dist_x * dist_x + dist_y * dist_y);
                
                //if within attack range and out of cooldown, initiate attack
                if (distance_to_target <= attack_radius) {
                    initiate_attack();
                } else if (distance_to_target > 5.0f) {
                    //move towards target if player is more than 5.0 units away
                    move_toward_target(delta_time);
                } else {
                    //stop moving when within 5.0 units of player but outside attack range
                    velocity.x = 0.0f;
                }
            } else {
                float dist_x = target_position.x - position.x;
                float dist_y = target_position.y - position.y;
                float distance_to_target = glm::sqrt(dist_x * dist_x + dist_y * dist_y);
                if (distance_to_target > 5.0f) {
                    move_toward_target(delta_time);
                } else {
                    velocity.x = 0.0f;
                }
            }
            break;
            
        case BOSS3_DEMON_ATTACK:
            //cleave attack animation
            animation_index = static_cast<int>((animation_time * 10.0f)) % animation_frames;
            texture_id = demon_cleave_textures[animation_index];
            //damage frame
            is_attack_active = (animation_index >= 10 && animation_index <= 15);
            if (animation_time >= attack_duration) {
                is_attack_active = false;
                set_demon_walk();
            }
            break;
            
        case BOSS3_DEMON_SMASH:
            //fakemove attack animation, fake cleave
            animation_index = static_cast<int>((animation_time * 9.0f)) % animation_frames;
            texture_id = demon_smash_textures[animation_index];    
            //damage frame move back
            is_attack_active = (animation_index >= 20 && animation_index <= 25);
            //reset 
            if (animation_time >= attack_duration) {
                is_attack_active = false;
                is_smash_attack = false;
                set_demon_walk();
            }
            break;
            //fire breath remains the same
        case BOSS3_DEMON_FIRE_BREATH:
            animation_index = static_cast<int>((animation_time * 8.0f)) % animation_frames; 
            texture_id = demon_fire_breath_textures[animation_index];
            is_attack_active = (animation_index >= 15 && animation_index <= 17);
            if (animation_time >= attack_duration) {
                is_attack_active = false;
                is_fire_breath_attack = false;
                set_demon_walk();
            }
            break;
            //same death animation
        case BOSS3_DIE:
            animation_index = (static_cast<int>(animation_time * 7.0f)); 
            if (animation_index >= animation_frames) {
                animation_index = animation_frames - 1;
                is_death_animation_done = true;
            }
            texture_id = demon_death_textures[animation_index];
            float fade = 1.0f;
            if (animation_index > 15) {
                fade = 1.0f - ((animation_index - 15) / 7.0f);
                if (fade < 0.0f) fade = 0.0f;
            }
            render_color = glm::vec4(1.0f, 0.7f, 0.7f, fade);
            if (animation_index >= animation_frames - 1 || animation_time >= 3.5f) {
                is_death_animation_done = true;
            }
            break;
    }

    if (fire_effects_active) {
        update_fire_effects(delta_time);
    }

    if (monsters_active) {
        update_monsters(delta_time);
    }

    if (map) {
        bool on_ground = false;
        float check_y = position.y - (height / 2.0f) - 0.1f; 
        if (map->is_solid(position.x, check_y)) {
            on_ground = true;
            velocity.y = 0; 
        }
        if (!on_ground) {
            velocity.y += map->gravity * delta_time;
            position.y += velocity.y * delta_time;
        }
    }
}

void Boss3::render(ShaderProgram* program) {
    if (!is_active) return;
    

    glm::mat4 model_matrix = glm::mat4(1.0f);
    
    // Apply a vertical offset to position the boss visually on the platform
    glm::vec3 adjusted_position = position + glm::vec3(0.0f, 3.0f, 0.0f); // Add vertical offset
    model_matrix = glm::translate(model_matrix, adjusted_position);
    
    // Apply visual scaling
    model_matrix = glm::scale(model_matrix, glm::vec3(visual_scale_x, visual_scale_y, 1.0f));
    
    // Apply horizontal flipping based on direction
    // Direction: 1 = right, 0 = left (reversed)
    if (direction == 0) {
        model_matrix = glm::scale(model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
    
    // Set the model matrix and render
    program->set_model_matrix(model_matrix);
    
    // Draw the boss
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
    };
    
    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    // Apply render color using the proper set_colour method
    program->set_colour(render_color.r, render_color.g, render_color.b, render_color.a);
    
    // Set up attributes
    glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    // Draw
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Reset color to default after drawing
    program->set_colour(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Disable attributes
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    // Render fire effects if active
    if (fire_effects_active) {
        render_fire_effects(program);
    }
    
    // Render monsters if active
    if (monsters_active) {
        render_monsters(program);
    }
}

void Boss3::cast_spell() {
    std::cout << "Boss3 casting spell..." << std::endl;
    state = BOSS3_CAST_SPELL;
    animation_time = 0.0f;
    animation_index = 0;
    animation_frames = 6;
    texture_id = cast_spell_textures[0];
}

void Boss3::set_idle_before_cast() {
    std::cout << "Boss3 starting with idle animation for 1 second..." << std::endl;
    state = BOSS3_IDLE_BEFORE_CAST;
    animation_time = 0.0f;
    animation_index = 0;
    animation_frames = 6;
    texture_id = demon_idle_textures[0];
}

void Boss3::set_demon_idle() {
    std::cout << "Boss3 transitioning to demon idle..." << std::endl;
    state = BOSS3_DEMON_IDLE;
    animation_time = 0.0f;
    animation_index = 0;
    animation_frames = 6;
    texture_id = demon_idle_textures[0];
}

void Boss3::set_demon_walk() {
    std::cout << "Boss3 transitioning to demon walk..." << std::endl;
    state = BOSS3_DEMON_WALK;
    animation_time = 0.0f;
    animation_index = 0;
    animation_frames = 6;
    texture_id = demon_walk_textures[0];
}

void Boss3::die() {
    if (state != BOSS3_DIE) {
        std::cout << "Boss3 dying..." << std::endl;
        state = BOSS3_DIE;
        animation_time = 0.0f;
        is_death_animation_done = false;
        
        // Stop movement
        velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

bool Boss3::is_dead() const {
    return health <= 0 || state == BOSS3_DIE;
}

void Boss3::set_target_position(const glm::vec3& position) {
    target_position = position;
}

void Boss3::take_damage(int damage) {
    health -= damage;
    
    if (health < 0) {
        health = 0;
    }

    if (health == 0 && state != BOSS3_DIE) {
        play_death_animation();
    }
}

//death animation
void Boss3::play_death_animation() {
    if (state != BOSS3_DIE) {
        state = BOSS3_DIE;
        animation_time = 0.0f;
        animation_index = 0;
        animation_frames = 22;
        texture_id = demon_death_textures[0];
        is_death_animation_done = false;
        velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        attack_cooldown = 999.0f;
        action_cooldown = 999.0f;
    }
}

void Boss3::set_map(Map* map_param) {
    map = map_param;
}
//chasing logic
void Boss3::move_toward_target(float delta_time) {
    if (!is_active) return;

    float dist_x = target_position.x - position.x;
    float dist_y = target_position.y - position.y;
    float distance = glm::sqrt(dist_x * dist_x + dist_y * dist_y);
    
    if (distance > 5.0f) {
        float x_dir = dist_x / (distance + 0.01f); 
        float move_speed = 1.5f; 
        velocity.x = x_dir * move_speed;
        position.x += velocity.x * delta_time;
    } else {
        velocity.x = 0.0f;
    }
}

//implement attack initiation
void Boss3::initiate_attack() {
    std::cout << "Boss3 initiating attack..." << std::endl;
    
    is_attack_active = false;
    is_smash_attack = false;
    is_fire_breath_attack = false;
    
    //random attack logic
    int attack_type = rand() % 3; 
    // 0 = cleave, 1 = smash, 2 = fire breath
    
    //set animation based on chosen attack
    switch (attack_type) {
        case 0: 
            set_animation(BOSS3_DEMON_ATTACK);
            attack_duration = 1.5f;
            attack_cooldown = 1.7f;
            break;
            
        case 1: 
            set_animation(BOSS3_DEMON_SMASH);
            is_smash_attack = true;
            attack_duration = 2.7f;
            attack_cooldown = 3.5f;
            break;
            
        case 2: 
            set_animation(BOSS3_DEMON_FIRE_BREATH);
            is_fire_breath_attack = true;
            attack_duration = 2.5f;
            attack_cooldown = 3.0f;
            break;
    }
    
    //action cooldown based on attack duration
    action_cooldown = attack_duration;
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

//set animation based on state, default is idle
void Boss3::set_animation(Boss3State new_state) {
    animation_time = 0.0f;
    animation_index = 0;
    state = new_state;
    switch (state) {
        case BOSS3_IDLE_BEFORE_CAST:
            animation_frames = 6;
            texture_id = demon_idle_textures[0];
            break;
            
        case BOSS3_CAST_SPELL:
            animation_frames = 6;
            texture_id = cast_spell_textures[0];
            break;
            
        case BOSS3_DEMON_IDLE:
            animation_frames = 6;
            texture_id = demon_idle_textures[0];
            break;
            
        case BOSS3_DEMON_WALK:
            animation_frames = 6;
            texture_id = demon_walk_textures[0];
            break;
            
        case BOSS3_DEMON_ATTACK:
            animation_frames = 15;
            texture_id = demon_cleave_textures[0];
            break;
            
        case BOSS3_DEMON_SMASH:
            animation_frames = 27; 
            texture_id = demon_smash_textures[0];
            break;
            
        case BOSS3_DEMON_FIRE_BREATH:
            animation_frames = 21;
            texture_id = demon_fire_breath_textures[0];
            break;
            
        case BOSS3_DIE:
            animation_frames = 22;
            texture_id = demon_death_textures[0]; 
            break;
            
        default:
            animation_frames = 6;
            texture_id = demon_idle_textures[0];
            break;
    }
}
//create fire obstacles in map
void Boss3::create_fire_effects() {
    fire_effects.clear();
    
    float start_x = -42.0f;
    float effect_y = -2.2f;  
    float spacing = 5.0f;  
    int num_effects = 25;
    int created_count = 0;
    
    for (int i = 0; i < num_effects; i++) {
        float x_pos = start_x + (i * spacing);
        if (x_pos < -50.0f || x_pos > 50.0f) {
            continue;  
        }
        
        FireEffect effect;
        
        //set position
        effect.position = glm::vec3(x_pos, effect_y, 0.0f);
        effect.width = 2.5f;  
        effect.height = 4.0f; 

        effect.animation_index = rand() % 4; 
        effect.animation_time = 0.0f;
        effect.is_active = true;
        
        // Set collision properties
        effect.is_obstacle = true;
        effect.collision_width = 1.5f; 
        effect.collision_height = 2.0f;
        
        // Set model matrix
        effect.model_matrix = glm::mat4(1.0f);
        effect.model_matrix = glm::translate(effect.model_matrix, effect.position);
        effect.model_matrix = glm::scale(effect.model_matrix, glm::vec3(effect.width, effect.height, 1.0f));
        
        // Add to vector
        fire_effects.push_back(effect);
        created_count++;
    }
    
}

void Boss3::update_fire_effects(float delta_time) {
    const float FRAME_DURATION = 0.25f; 
    const int NUM_FRAMES = 4;  
    
    for (auto& effect : fire_effects) {
        if (!effect.is_active) continue;
        effect.animation_time += delta_time;
        if (effect.animation_time >= FRAME_DURATION) {
            effect.animation_index = (effect.animation_index + 1) % NUM_FRAMES;
            effect.animation_time = 0.0f;
        }
        
        //update model matrix with current position 
        effect.model_matrix = glm::mat4(1.0f);
        effect.model_matrix = glm::translate(effect.model_matrix, effect.position);
        effect.model_matrix = glm::scale(effect.model_matrix, glm::vec3(effect.width, effect.height, 1.0f));
    }
}

void Boss3::render_fire_effects(ShaderProgram* program) {
    const int NUM_FRAMES = 4;
    for (const auto& effect : fire_effects) {
        if (!effect.is_active) continue;   
        // Set model matrix 
        program->set_model_matrix(effect.model_matrix);
        
        // Bind the fire+sparks texture
        glBindTexture(GL_TEXTURE_2D, fire_effect_texture);
        float u_width = 1.0f / (float)NUM_FRAMES;
        float u_offset = (float)effect.animation_index * u_width;
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, u_offset, 1.0f,
             0.5f, -0.5f, 0.0f, u_offset + u_width, 1.0f,
             0.5f,  0.5f, 0.0f, u_offset + u_width, 0.0f,
            
            -0.5f, -0.5f, 0.0f, u_offset, 1.0f,
             0.5f,  0.5f, 0.0f, u_offset + u_width, 0.0f,
            -0.5f,  0.5f, 0.0f, u_offset, 0.0f
        };
        
        // Set up attributes
        glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        // Draw
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Disable attributes
        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
}
//fire disappears when colliding
bool Boss3::check_fire_collision(const glm::vec3& entity_position, float entity_width, float entity_height) const {
    //if fire effects aren't active, no collision
    if (!fire_effects_active) return false;
    
    float entity_half_width = entity_width / 2.0f;
    float entity_half_height = entity_height / 2.0f;
    
    for (const auto& effect : fire_effects) {
        if (!effect.is_active || !effect.is_obstacle) continue;
        
        float effect_half_width = effect.collision_width / 2.0f;
        float effect_half_height = effect.collision_height / 2.0f;

        if (entity_position.x - entity_half_width < effect.position.x + effect_half_width &&
            entity_position.x + entity_half_width > effect.position.x - effect_half_width &&
            entity_position.y - entity_half_height < effect.position.y + effect_half_height &&
            entity_position.y + entity_half_height > effect.position.y - effect_half_height) {
            return true;
        }
    }

    return false;
}

//seperate check for knight
void Boss3::check_knight_fire_collision(Knight* knight) {
    if (!knight || !fire_effects_active) return;
    const glm::vec3& knight_position = knight->position;
    float knight_width = knight->width;
    float knight_height = knight->height;
    //knight won't be hurt if jumping
    if (knight->state == JUMPING || knight->state == FALLING || !knight->is_grounded) {
        return; 
    }
    //hitbox calculation
    float knight_half_width = knight_width / 2.0f;
    float knight_half_height = knight_height / 2.0f;
    for (auto& effect : fire_effects) {
        if (!effect.is_active || !effect.is_obstacle) continue;
        float effect_half_width = effect.collision_width / 2.0f;
        float effect_half_height = effect.collision_height / 2.0f;
        if (knight_position.x - knight_half_width < effect.position.x + effect_half_width &&
            knight_position.x + knight_half_width > effect.position.x - effect_half_width &&
            knight_position.y - knight_half_height < effect.position.y + effect_half_height &&
            knight_position.y + knight_half_height > effect.position.y - effect_half_height) {
            //detected
            knight->take_damage(1);
            knight->hurt();
            
            effect.is_active = false;

        }
    }
}

//monsters summoned, two from right upper cornor
void Boss3::create_monsters() {
    const glm::vec3& player_target = target_position;
    for (int i = 0; i < NUM_MONSTERS; i++) {
        glm::vec3 monster_pos;
        switch (i) {
            case 0: 
                monster_pos = glm::vec3(player_target.x + 15.0f, player_target.y + 15.0f, 0.0f);
                break;
            case 1: 
                monster_pos = glm::vec3(player_target.x + 15.0f, player_target.y + 10.0f, 0.0f);
                break;
        }

        monsters[i].set_position(monster_pos);
        monsters[i].set_target(player_target);
        monsters[i].set_active(true);
    }
}

// Update monsters
void Boss3::update_monsters(float delta_time) {
    if (!monsters_active) return;
    
    // Update each monster
    for (int i = 0; i < NUM_MONSTERS; i++) {
        if (monsters[i].is_active()) {
            // Update target to current player position
            monsters[i].set_target(target_position);
            
            // Update monster movement
            monsters[i].update(delta_time);
        }
    }
    
    // Check for collisions with fire effects
    check_monster_fire_collision();
}

// Render monsters
void Boss3::render_monsters(ShaderProgram* program) {
    if (!monsters_active) return;
    
    // Render each active monster
    for (int i = 0; i < NUM_MONSTERS; i++) {
        if (monsters[i].is_active()) {
            monsters[i].render(program);
        }
    }
}

//check collisions between monsters and knight
void Boss3::check_monster_knight_collision(Knight* knight) {
    if (!knight || !monsters_active) return;
    
    for (int i = 0; i < NUM_MONSTERS; i++) {
        if (monsters[i].is_active() && monsters[i].check_collision_with_knight(knight)) {
            knight->take_damage(1);
            knight->hurt();

            monsters[i].set_active(false);

        }
    }
}

//check collisions between monsters and fire effects
void Boss3::check_monster_fire_collision() {
    if (!monsters_active || !fire_effects_active) return;
    for (int i = 0; i < NUM_MONSTERS; i++) {
        if (!monsters[i].is_active()) continue;
        for (auto& effect : fire_effects) {
            if (!effect.is_active || !effect.is_obstacle) continue;
            if (monsters[i].check_collision_with_fire(effect.position, effect.collision_width, effect.collision_height)) {
                monsters[i].set_active(false);
                effect.is_active = false;
                //break out of the inner loop since this monster hit a fire
                break;
            }
        }
    }
} 