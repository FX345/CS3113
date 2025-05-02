#include "Boss.h"
#include "Utility.h"
#include <cmath>
#include <iostream>

Boss::Boss() : 
    //initialzations of parameters
    m_animation_frames(6),
    m_animation_index(0),
    m_animation_time(0.0f),
    state(BOSS_IDLE),
    direction(0), 
    health(10),
    action_cooldown(0.0f),
    attack_duration(0.0f),
    detection_radius(7.0f),   
    attack_radius(2.0f),      
    flame_attack_radius(7.0f), 
    attack_damage(2.0f),
    flame_cooldown(0.0f),
    projectile_speed(4.0f), 
    projectile_cooldown(0.0f), 
    max_projectile_distance(30.0f), 
    max_health(10), 
    visual_scale_x(1.0f),
    visual_scale_y(1.0f)
{
    //boss size and speed
    width = 1.0f;      
    height = 1.0f;    
    speed = 1.5f; 
    is_active = true;
    
    //default pos
    position = glm::vec3(0.0f);
    
    //gravity
    acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
    velocity = glm::vec3(0.0f);
    
    //projectile initialzation
    projectiles.reserve(10);
    
    //load idle
    for (int i = 0; i < 6; i++) {
        std::string path = "assets/boss/individual sprites/01_idle/idle_" + std::to_string(i + 1) + ".png";
        idle_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load move 
    for (int i = 0; i < 8; i++) {
        std::string path = "assets/boss/individual sprites/02_move/move_" + std::to_string(i + 1) + ".png";
        move_textures[i] = Utility::load_texture(path.c_str());
    }
    
    //load projectile
    for (int i = 0; i < 3; i++) {
        std::string path = "assets/boss/individual sprites/13_projectile_idle/projectile_idle_" + std::to_string(i + 1) + ".png";
        flame_textures[i] = Utility::load_texture(path.c_str());
    }
    
    for (int i = 0; i < 6; i++) {
        attack_textures[i] = idle_textures[i];
        charge_textures[i] = idle_textures[i];
        hurt_textures[i] = idle_textures[i];
    }
    
    //use idle as initial texture
    set_animation(BOSS_IDLE);
    texture_id = idle_textures[0];
}

Boss::~Boss() {
   
}

void Boss::update(float delta_time) {
    //check is active
    if (!is_active) return;
    
    //update animation timer
    m_animation_time += delta_time;
    //attack cooldown 
    if (action_cooldown > 0) {
        action_cooldown -= delta_time;
    }
    
    if (attack_duration > 0) {
        attack_duration -= delta_time;
       
        if (attack_duration <= 0 && state == BOSS_ATTACKING) {
            set_animation(BOSS_IDLE);
        }
    }
    
    if (flame_cooldown > 0) {
        flame_cooldown -= delta_time;
    }
    
    //calculate distance to player
    float x_distance = target_position.x - position.x;
    float y_distance = target_position.y - position.y;
    float distance = glm::sqrt(x_distance * x_distance + y_distance * y_distance);

    //logic: run from player if player is near, shoot projectiles if player is far
    if (distance < detection_radius) {
        //facing of the boss
        if (x_distance > 0) {
            direction = 1; 
        }
        else {
            direction = 0; 
        }
    } else {
        
        if (x_distance > 0) {
            direction = 0; 
        }
        else {
            direction = 1; 
        }
    }

    //AI, distance dectection
    if (distance < detection_radius) {
        
        float x_dir = x_distance / (distance + 0.01f); 
        velocity.x = -x_dir * speed * 1.2f; 

        if (state != BOSS_MOVING) {
            set_animation(BOSS_MOVING);
        }

        projectile_cooldown = 1.5f; 
    }
    else {
    
        velocity.x = 0; 
        

        if (projectile_cooldown <= 0) {
        
            fire_projectile();
            projectile_cooldown = 1.5f; 
 
            if (state != BOSS_ATTACKING) {
                set_animation(BOSS_ATTACKING);
            }
        }
        else {
            if (state != BOSS_IDLE && state != BOSS_ATTACKING) {
                set_animation(BOSS_IDLE);
            }
        }
    }
    projectile_cooldown -= delta_time;
    update_projectiles(delta_time);
    velocity.y += acceleration.y * delta_time;
    glm::vec3 previous_position = position;

    position.x += velocity.x * delta_time;
    position.y += velocity.y * delta_time;
    
    //wall collision of boss
    float boss_right_edge = position.x + (width / 2.0f);
    float boss_left_edge = position.x - (width / 2.0f);
    
    if (map != nullptr) {
        if (map->is_solid(boss_right_edge, position.y)) {
            position.x = previous_position.x;
            velocity.x = 0; 
        }
        if (map->is_solid(boss_left_edge, position.y)) {
            position.x = previous_position.x;
            velocity.x = 0; 
        }
    }

    update_animation();
    
    //update the model matrix
    model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    if (direction == 0) { 
        model_matrix = glm::scale(model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
    
    model_matrix = glm::scale(model_matrix, glm::vec3(width, height, 1.0f));
}
//textures frame rate
void Boss::update_animation() {
    float frame_rate;
    if (state == BOSS_ATTACKING) {
        frame_rate = 0.1f; 
    } else if (state == BOSS_CHARGING) {
        frame_rate = 0.08f; 
    } else if (state == BOSS_HURT) {
        frame_rate = 0.12f; 
    } else if (state == BOSS_MOVING) {
        frame_rate = 0.1f; 
    } else if (state == BOSS_FLAME_ATTACK) {
        frame_rate = 0.15f; 
    } else {
        frame_rate = 0.15f;
    }
  
    if (m_animation_time >= frame_rate) {
        m_animation_time = 0.0f;

        if (state == BOSS_MOVING) {
            m_animation_index = (m_animation_index + 1) % 8; 
        } else {
            m_animation_index = (m_animation_index + 1) % 6; 
        }
        //boss states
        switch (state) {
            case BOSS_IDLE:
                if (m_animation_index < 6 && idle_textures[m_animation_index] != 0) {
                    texture_id = idle_textures[m_animation_index];
                } else {
                    m_animation_index = 0;
                    texture_id = idle_textures[0];
                }
                break;
                
            case BOSS_MOVING:
                if (m_animation_index < 8 && move_textures[m_animation_index] != 0) {
                    texture_id = move_textures[m_animation_index];
                } else {
                    m_animation_index = 0;
                    texture_id = move_textures[0];
                }
                break;
                
            case BOSS_ATTACKING:
                if (m_animation_index < 6 && attack_textures[m_animation_index] != 0) {
                    texture_id = attack_textures[m_animation_index];
                } else {
                    m_animation_index = 0;
                    texture_id = attack_textures[0];
                }
                break;
                
            case BOSS_CHARGING:
                if (m_animation_index < 6 && charge_textures[m_animation_index] != 0) {
                    texture_id = charge_textures[m_animation_index];
                } else {
                    m_animation_index = 0;
                    texture_id = charge_textures[0];
                }
                break;
                
            case BOSS_HURT:
                if (m_animation_index < 6 && hurt_textures[m_animation_index] != 0) {
                    texture_id = hurt_textures[m_animation_index];
                } else {
                    m_animation_index = 0;
                    texture_id = hurt_textures[0];
                }
                break;
                
            case BOSS_FLAME_ATTACK:
                int idle_frame = m_animation_index % 6;
                if (idle_frame < 6 && idle_textures[idle_frame] != 0) {
                    texture_id = idle_textures[idle_frame];
                } else {
                    texture_id = idle_textures[0];
                }
                break;
        }
    }
}

void Boss::render(ShaderProgram* program) {
    if (!is_active) return;
    update_animation();
    model_matrix = glm::mat4(1.0f);
    glm::vec3 visual_position = position + glm::vec3(0.0f, 3.3f, 0.0f);
    model_matrix = glm::translate(model_matrix, visual_position);
    if (direction == 0) { // LEFT
        model_matrix = glm::scale(model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }

    model_matrix = glm::scale(model_matrix, glm::vec3(width * visual_scale_x, height * visual_scale_y, 1.0f));
    program->set_model_matrix(model_matrix);
    
    // Draw with texture
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
    };
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    // Render projectiles
    render_projectiles(program);
    
}

void Boss::set_animation(BossState new_state) {
    // Only change animation if state changed
    if (state == new_state) return;
    
    state = new_state;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    
    //set animation frames based on new state
    switch (state) {
        case BOSS_IDLE:
            m_animation_frames = 6; 
            texture_id = idle_textures[0];
            break;
        case BOSS_MOVING:
            m_animation_frames = 8; 
            texture_id = move_textures[0];
            break;
        case BOSS_ATTACKING:
            m_animation_frames = 6; 
            texture_id = attack_textures[0];
            break;
        case BOSS_CHARGING:
            m_animation_frames = 6; 
            texture_id = charge_textures[0];
            break;
        case BOSS_HURT:
            m_animation_frames = 6; 
            texture_id = hurt_textures[0];
            break;
        case BOSS_FLAME_ATTACK:
            m_animation_frames = 6;
            texture_id = idle_textures[0];
            break;
        default:
            m_animation_frames = 6;
            texture_id = idle_textures[0];
            break;
    }
}

void Boss::set_target_position(const glm::vec3& target) {
    target_position = target;
}

void Boss::take_damage(int damage) {
    if (state == BOSS_HURT) return; 
    
    health -= damage;
    
    //ensure health doesn't go below 0
    if (health < 0) health = 0;
    
    //set hurt state and animation
    state = BOSS_HURT;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    //won't take excessive dmg
    action_cooldown = 0.5f;
    

}

bool Boss::is_dead() const {
    return health <= 0;
}

//reset healthg
void Boss::reset_health() {
    health = max_health;
}

//fire projectile 
void Boss::fire_projectile() {
    Projectile new_projectile;
    glm::vec4 worldPos = model_matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float y_offset = -4.0f; 
    //set projectile position
    new_projectile.position = glm::vec3(worldPos.x, worldPos.y + y_offset, worldPos.z);
    //won't shoot if player is in 7 units
    glm::vec3 projectile_direction;
    float x_distance = target_position.x - position.x;
    //right or left shoot
    if (x_distance > 0) {
        projectile_direction = glm::vec3(1.0f, 0.0f, 0.0f); 
    } else {
        projectile_direction = glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    // projectile velocity
    new_projectile.velocity = projectile_direction * projectile_speed;
    
    //projectile size
    new_projectile.width = 0.8f;
    new_projectile.height = 0.8f;
    new_projectile.is_active = true;
    new_projectile.animation_index = 0;
    new_projectile.animation_time = 0.0f;
    
    //update model matrix
    new_projectile.model_matrix = glm::mat4(1.0f);
    new_projectile.model_matrix = glm::translate(new_projectile.model_matrix, new_projectile.position);
    new_projectile.model_matrix = glm::scale(new_projectile.model_matrix, glm::vec3(new_projectile.width, new_projectile.height, 1.0f));
    
    //add to vector
    projectiles.push_back(new_projectile);
}

// update all active projectiles
void Boss::update_projectiles(float delta_time) {
    for (size_t i = 0; i < projectiles.size(); ++i) {
        Projectile& projectile = projectiles[i];  
        if (!projectile.is_active) continue;
        projectile.position += projectile.velocity * delta_time;
        //check if projectile has gone too far
        float distance = glm::distance(position, projectile.position);
        if (distance > max_projectile_distance) {
            projectile.is_active = false;
            continue;
        }
        
        //update animation
        projectile.animation_time += delta_time;
        if (projectile.animation_time >= 0.15f) {
            projectile.animation_time = 0.0f;
            projectile.animation_index = (projectile.animation_index + 1) % 3;
        }
        
        //update model matrix
        projectile.model_matrix = glm::mat4(1.0f);
        projectile.model_matrix = glm::translate(projectile.model_matrix, projectile.position);
        projectile.model_matrix = glm::scale(projectile.model_matrix, glm::vec3(projectile.width, projectile.height, 1.0f));
    }
    
    //remove useless projectiles
    projectiles.erase(
        std::remove_if(
            projectiles.begin(), 
            projectiles.end(),
            [](const Projectile& p) { return !p.is_active; }
        ),
        projectiles.end()
    );
}

//render all active projectiles
void Boss::render_projectiles(ShaderProgram* program) {
    for (const Projectile& projectile : projectiles) {
        if (!projectile.is_active) continue;
        program->set_model_matrix(projectile.model_matrix);
        GLuint texture_id = flame_textures[projectile.animation_index];
        float tex_coords[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f
        };
        
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, tex_coords[0], tex_coords[1],
             0.5f, -0.5f, 0.0f, tex_coords[2], tex_coords[3],
             0.5f,  0.5f, 0.0f, tex_coords[4], tex_coords[5],
            -0.5f,  0.5f, 0.0f, tex_coords[6], tex_coords[7]
        };
        if (texture_id != 0) {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            
            glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
            glEnableVertexAttribArray(program->get_position_attribute());
            
            glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
            glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
            
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            
            glDisableVertexAttribArray(program->get_position_attribute());
            glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
        }
    }
} 