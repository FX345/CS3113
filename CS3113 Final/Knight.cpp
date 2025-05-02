#include "Knight.h"
#include "Map.h"

Knight::Knight() {
    position = glm::vec3(0.0f, 3.0f, 0.0f); 
    speed = 6.0f;
    //size
    width = 1.8f;  
    height = 2.2f; 
    //health based on health bar
    max_health = 6;
    health = max_health;
    //animation for all movements
    m_animation_cols = 10; 
    m_animation_rows = 1;  
    m_walking[LEFT] = new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 };
    m_walking[RIGHT] = new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 };
    
    m_running[LEFT] = new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 };
    m_running[RIGHT] = new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 };
    
    m_idle[LEFT] = new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 };
    m_idle[RIGHT] = new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 };
    
    m_jumping[LEFT] = new int[6] { 0, 1, 2, 3, 4, 5 };
    m_jumping[RIGHT] = new int[6] { 0, 1, 2, 3, 4, 5 };
    
    m_falling[LEFT] = new int[6] { 0, 1, 2, 3, 4, 5 }; 
    m_falling[RIGHT] = new int[6] { 0, 1, 2, 3, 4, 5 };
    
    m_attacking[LEFT] = new int[6] { 0, 1, 2, 3, 4, 5 };
    m_attacking[RIGHT] = new int[6] { 0, 1, 2, 3, 4, 5 };

    m_defending[LEFT] = new int[4] { 0, 1, 2, 3 };
    m_defending[RIGHT] = new int[4] { 0, 1, 2, 3 };

    m_hurt[LEFT] = new int[4] { 0, 1, 2, 3 };
    m_hurt[RIGHT] = new int[4] { 0, 1, 2, 3 };
        
    //default
    m_animation_frames = 8;
    m_animation_index = 0;
    m_animation_time = 0.0f;
    state = IDLE;
    direction = RIGHT;
    m_animation_indices = m_idle[RIGHT];
    move_left = false;
    move_right = false;
    move_up = false;
    move_down = false;
    is_attacking = false;
    is_jumping = false;
    is_defending = false;
    is_grounded = false;
    is_hurt = false;
    is_invincible = false;
    hurt_timer = 0.0f;
    attack_cooldown = 0.0f;
    defend_cooldown = 0.0f;
    defend_duration = 0.0f;
    jump_power = 9.0f; 
    can_jump = true;
    
    //initialize acceleration with gravity
    acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
}

void Knight::set_direction(const std::string& new_direction) {
    if (new_direction == "left") {
        direction = LEFT;
    } else if (new_direction == "right") {
        direction = RIGHT;
    }
}

void Knight::attack() {
    if (!is_attacking && attack_cooldown <= 0 && !is_hurt && !is_defending) {
        is_attacking = true;
        set_animation(ATTACKING);
        attack_cooldown = 0.5f; 
    }
}

void Knight::defend() {
    if (!is_defending && !is_attacking && defend_cooldown <= 0 && is_grounded && !is_hurt) {
        is_defending = true;
        is_invincible = true; 
        set_animation(DEFENDING);
        //defend dutation
        defend_duration = 0.8f;
    }
}

void Knight::jump() {
    //only jump if on the ground, if can jump, not hurt, and not defending
    if (is_grounded && can_jump && !is_jumping && !is_hurt && !is_defending) {
        velocity.y = jump_power;
        is_jumping = true;
        is_grounded = false;
        set_animation(JUMPING);
    }
}

void Knight::update(float delta_time) {
    if (attack_cooldown > 0) {
        attack_cooldown -= delta_time;
    }
    if (defend_cooldown > 0) {
        defend_cooldown -= delta_time;
    }
    if (is_defending) {
        defend_duration -= delta_time;
        if (defend_duration <= 0) {
            //end defending after duration expires
            is_defending = false;
            is_invincible = false;
            
            // defend cool down
            defend_cooldown = 0.5f; 
            //return to appropriate animation
            if (velocity.x != 0) {
                set_animation(RUNNING);
            } else {
                set_animation(IDLE);
            }
        }

        velocity.x = 0;
        velocity.y = 0;
        

        update_animation();
        return;
    }
    
    //hurt timer
    if (is_hurt) {
        hurt_timer -= delta_time;
        if (hurt_timer <= 0) {
            is_hurt = false;
            // Keep invincible for the full 0.5 seconds
            if (velocity.x != 0) {
                set_animation(RUNNING);
            } else if (!is_jumping && !is_grounded) {
                set_animation(FALLING);
            } else if (is_grounded) {
                set_animation(IDLE);
            }
        }
    }
    

    if (is_invincible && !is_hurt && !is_defending) {
        hurt_timer -= delta_time;
        if (hurt_timer <= 0) {
            is_invincible = false;
        }
    }
    
    velocity.x = 0;
    
    //cannot move while defending, attacking or hurt
    if (!is_attacking && !is_defending && !is_hurt) {
        if (move_left) {
            velocity.x = -speed;
            direction = LEFT;
        } 
        if (move_right) {
            velocity.x = speed;
            direction = RIGHT;
        }
    } else if (is_defending) {
        velocity.x = 0;
    }

    update_animation();
    
    //reset attack state if animation complete
    if (is_attacking && attack_cooldown <= 0) {
        is_attacking = false;
        if (velocity.x != 0) {
            set_animation(RUNNING);
        } else if (!is_jumping && !is_grounded) {
            set_animation(FALLING);
        } else if (is_grounded) {
            set_animation(IDLE);
        }
    }
    
    //reset defend state and invincibility if no longer defending
    if (!is_defending && is_invincible && hurt_timer <= 0) {
        is_invincible = false;
        if (velocity.x != 0) {
            set_animation(RUNNING);
        } else if (!is_jumping && !is_grounded) {
            set_animation(FALLING);
        } else if (is_grounded) {
            set_animation(IDLE);
        }
    }
    //gravity
    if (!is_grounded) {
        velocity.y += acceleration.y * delta_time;
        
        //falling animation
        if (velocity.y < 0 && state != FALLING && !is_attacking && !is_defending && !is_hurt) {
            set_animation(FALLING);
        }
    }
    

    Entity::update(delta_time);
    
    //check if landed
    if (is_jumping && velocity.y <= 0) {
        is_jumping = false;
        if (!is_grounded && !is_hurt) {
            set_animation(FALLING);
        }
    }
}

void Knight::update_animation() {
    m_animation_time += 1.0f / 60.0f;
    //adjust animation speed based on state
    float frame_rate;
    if (state == RUNNING) {
        frame_rate = 0.08f;  
    } else if (state == ATTACKING) {
        frame_rate = 0.067f; 
    } else if (state == JUMPING || state == FALLING) {
        frame_rate = 0.15f;  
    } else if (state == DEFENDING) {
        frame_rate = 0.1f;  
    } else if (state == HURT) {
        frame_rate = 0.125f;
    } else {
        frame_rate = 0.2f; 
    }
    //frame count
    if (m_animation_time >= frame_rate) {
        m_animation_time = 0.0f;
        int max_frames;
        if (state == ATTACKING) {
            max_frames = 6;
        } else if (state == JUMPING || state == FALLING) {
            max_frames = 6;  
        } else if (state == DEFENDING) {
            max_frames = 4;  

            m_animation_index = 0;
        } else if (state == HURT) {
            max_frames = 4;  
        } else {
            max_frames = 8;
        }
        m_animation_index = (m_animation_index + 1) % max_frames;
    }
}

void Knight::set_animation(KnightState new_state) {
    //only change animation if state changed
    if (state == new_state) return;

    state = new_state;
    m_animation_index = 0;
    m_animation_time = 0.0f;

    switch (state) {
        case IDLE:
            m_animation_indices = m_idle[direction];
            break;
        case WALKING:
            m_animation_indices = m_walking[direction];
            break;
        case RUNNING:
            m_animation_indices = m_running[direction];
            break;
        case JUMPING:
            m_animation_indices = m_jumping[direction];
            m_animation_index = 0;
            break;
        case FALLING:
            m_animation_indices = m_falling[direction];
            m_animation_index = 3;
            break;
        case ATTACKING:
            m_animation_indices = m_attacking[direction];
            break;
        case DEFENDING:
            m_animation_indices = m_defending[direction];
            break;
        case HURT:
            m_animation_indices = m_hurt[direction];
            break;
        default:
            m_animation_indices = m_idle[direction];
            break;
    }
}

void Knight::render(ShaderProgram* program) {
    // Position and resize the knight
    model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);
    
    
    if (direction == LEFT) {
        model_matrix = glm::scale(model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
    
    if (is_invincible && !is_hurt && !is_defending) {
        int frame_count = (int)(hurt_timer * 60.0f); 
        if (frame_count % 4 < 2) { 
            model_matrix = glm::scale(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
    
    model_matrix = glm::scale(model_matrix, glm::vec3(width, height, 1.0f));
    
    program->set_model_matrix(model_matrix);
    
    float vertices[] = {
        -0.5f, -0.5f, 
        0.5f, -0.5f,  
        0.5f, 0.5f,    
        -0.5f, 0.5f   
    };
    
    //adjust texture coordinates based on the state and animation frame
    float tex_coords[8];
    
    if (state == IDLE) {
        tex_coords[0] = 0.0f; tex_coords[1] = 1.0f;    
        tex_coords[2] = 0.25f; tex_coords[3] = 1.0f;  
        tex_coords[4] = 0.25f; tex_coords[5] = 0.0f;   
        tex_coords[6] = 0.0f; tex_coords[7] = 0.0f; 
    }
    else if (state == RUNNING) {
        int frame = m_animation_index % 4;
        float frame_width = 1.0f / 7.0f;
        
        float start_x = frame * frame_width;
        float end_x = start_x + frame_width;
        
        tex_coords[0] = start_x; tex_coords[1] = 1.0f;   
        tex_coords[2] = end_x; tex_coords[3] = 1.0f;  
        tex_coords[4] = end_x; tex_coords[5] = 0.0f;   
        tex_coords[6] = start_x; tex_coords[7] = 0.0f;   
    }
    //for jumping and falling animations
    else if (state == JUMPING || state == FALLING) {
        float frame_width = 1.0f / 6.0f;
        int frame;
        if (state == JUMPING) {
            frame = m_animation_index % 3;
        } else {
            frame = 3 + (m_animation_index % 3);
        }
        
        //calculate the starting and ending texture coordinates for this frame
        float start_x = frame * frame_width;
        float end_x = start_x + frame_width;
        
        tex_coords[0] = start_x; tex_coords[1] = 1.0f; 
        tex_coords[2] = end_x; tex_coords[3] = 1.0f; 
        tex_coords[4] = end_x; tex_coords[5] = 0.0f;    
        tex_coords[6] = start_x; tex_coords[7] = 0.0f;   
    }

    else if (state == ATTACKING) {
        float frame_width = 1.0f / 6.0f;
        float start_x = m_animation_index * frame_width;
        float end_x = start_x + frame_width;
        
        if (direction == RIGHT) {
            start_x -= 0.145f * frame_width;
            end_x -= 0.145f * frame_width;
        } else {
            start_x += -0.15f * frame_width;
            end_x += -0.15f * frame_width;
        }
        
        tex_coords[0] = start_x; tex_coords[1] = 1.0f;   
        tex_coords[2] = end_x; tex_coords[3] = 1.0f;    
        tex_coords[4] = end_x; tex_coords[5] = 0.0f;      
        tex_coords[6] = start_x; tex_coords[7] = 0.0f;    
    }
    else if (state == DEFENDING) {
        tex_coords[0] = 0.0f; tex_coords[1] = 1.0f;   
        tex_coords[2] = 1.0f; tex_coords[3] = 1.0f;   
        tex_coords[4] = 1.0f; tex_coords[5] = 0.0f;    
        tex_coords[6] = 0.0f; tex_coords[7] = 0.0f;    
    }

    else if (state == HURT) {
        tex_coords[0] = 0.0f; tex_coords[1] = 1.0f;  
        tex_coords[2] = 1.0f; tex_coords[3] = 1.0f;  
        tex_coords[4] = 1.0f; tex_coords[5] = 0.0f;   
        tex_coords[6] = 0.0f; tex_coords[7] = 0.0f;  
    }
    else {
        tex_coords[0] = 0.0f; tex_coords[1] = 1.0f;
        tex_coords[2] = 1.0f; tex_coords[3] = 1.0f;
        tex_coords[4] = 1.0f; tex_coords[5] = 0.0f;
        tex_coords[6] = 0.0f; tex_coords[7] = 0.0f;
    }
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
//each time -1 
void Knight::hurt() {
    if (!is_hurt && !is_invincible) {
        is_hurt = true;
        is_invincible = true;
        hurt_timer = 0.5f;
        set_animation(HURT);
        take_damage(1);
    }
}

void Knight::take_damage(int damage) {
    health -= damage;

    //ensure health doesn't go below 0
    if (health < 0) health = 0;
    
    //check for death
    if (health <= 0) {
        set_animation(DEAD);
    }
}

// Check if knight is dead
bool Knight::is_dead() const {
    return health <= 0;
}

//reset health to max
void Knight::reset_health() {
    health = max_health;
}
//hit box
bool Knight::check_projectile_collision(const std::vector<Projectile>& projectiles) {
    bool collision_happened = false;
    float collision_width = is_defending ? width * 0.5f : width * 0.7f;
    
    for (Projectile& projectile : const_cast<std::vector<Projectile>&>(projectiles)) {
        if (!projectile.is_active) continue;

        float x_distance = fabs(position.x - projectile.position.x);
        float y_distance = fabs(position.y - projectile.position.y);

        float x_overlap = (collision_width + projectile.width) / 2.0f;
        float y_overlap = (height + projectile.height) / 2.0f;
        
        if (x_distance < x_overlap && y_distance < y_overlap) {
            projectile.is_active = false;
            if (!is_invincible) {
                hurt();
                collision_happened = true;
            }
        }
    }
    return collision_happened;
} 