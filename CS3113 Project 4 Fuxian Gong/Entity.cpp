#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"


void Entity::ai_activate(Entity* player)
{
    switch (m_ai_type)
    {
    case WALKER:
        ai_walk();
        break;

    case JUMPER:
        ai_jump();
        break;

    case GUARD:
        ai_guard(player);
        break;
    case FLYING:
        ai_flyer();
    default:
        break;
    }
}

void Entity::ai_walk()
{
    //keep walking and change direction if hit wall
    if (m_facing_direction == "right") {
        move_right();
    }
    else {
        move_left();
    }
}

void Entity::ai_jump() {
    //jump around until hits wall then turn around
    if (m_collided_top || m_collided_bottom) {
        if (m_facing_direction == "right") {
            move_right();
        }
        else {
            move_left();
        }
        m_is_jumping = true;
    }
}
//follows the player and hunts him down
void Entity::ai_guard(Entity* player)
{
    switch (m_ai_state) {
    case IDLE:
        if (m_collided_top || m_collided_bottom) {
            m_is_jumping = true;
        }
        if (glm::distance(m_position, player->get_position()) < 20.0f) m_ai_state = WALKING;
        break;

    case WALKING:
        if (m_position.x > player->get_position().x) {
            m_facing_direction = "left";
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        else {
            m_facing_direction = "right";
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        if (m_collided_top || m_collided_bottom) {
            m_is_jumping = true;
        }

        break;

    default:
        break;
    }
}

void Entity::ai_flyer() {
    // simple up and down oscillation, but failed, it just wiggle there
    float amplitude = 1.0f;
    float frequency = 2.0f;
    m_velocity.y = sin(SDL_GetTicks() / 250.0f * frequency) * amplitude;

    m_velocity.x = m_speed * ((m_facing_direction == "right") ? 1 : -1);

    if (m_position.x > 8.0f) {
        m_facing_direction = "left";
    }
    else if (m_position.x < 5.0f) {
        m_facing_direction = "right";
    }
}

Entity::Entity()
{
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);

    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);

    m_collided_exit = false;
}

Entity::Entity(EntityType type, glm::vec3 position, float speed, glm::vec3 acceleration, GLuint texture_id,
    int* left_anim, int* right_anim, int animation_frames, int animation_index, float time, int animation_cols, int animation_rows, float height, float width) {
    m_entity_type = type;
    m_position = position;
    m_speed = speed;
    m_velocity = glm::vec3(0.0f);
    m_acceleration = acceleration;
    m_texture_id = texture_id;
    m_movement = glm::vec3(0.0f);

    m_animation_indices = right_anim;
    m_walking[LEFT] = left_anim;
    m_walking[RIGHT] = right_anim;
    m_animation_frames = animation_frames;
    m_animation_index = animation_index;
    m_animation_time = time;
    m_animation_cols = animation_cols;
    m_animation_rows = animation_rows;
    m_height = height;
    m_width = width;
    m_model_matrix = glm::mat4(1.0f);

}

Entity::~Entity()
{
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_walking;
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{

    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;


    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

  
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}



void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map)
{
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / FRAMES_PER_SECOND;

            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
        else {
            m_animation_index = 0;
        }
    }

    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;

    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);

    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);

    if (m_entity_type == ENEMY) { ai_activate(player); }

    //update jump
    if (m_is_jumping)
    {
        m_jumps -= 1;
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    //reset jumps
    if (m_collided_bottom) {
        m_jumps = 1;
    }

    
    //change enemy direction when hit the wall
    if (m_entity_type == ENEMY) {
        if (m_collided_right) {
            m_facing_direction = "left";
        }
        else if (m_collided_left) {
            m_facing_direction = "right";
        }
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}

bool const Entity::check_collision(Entity* other) const
{
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count) {
    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity)) {
          
            //if player hits exit
            if (m_entity_type == EXIT && collidable_entity->m_entity_type == PLAYER) {
                m_collided_exit = true;
            }

            //kill player
            if (m_entity_type == PLAYER && collidable_entity->m_entity_type == ENEMY) {
                m_is_active = false;
            }
            else {
                float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
                float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
                //upward
                if (m_velocity.y > 0) { 
                    m_position.y -= y_overlap;
                    m_velocity.y = 0;
                    m_collided_top = true;
                }
                //downward
                else if (m_velocity.y < 0) {  
                    m_position.y += y_overlap;
                    m_velocity.y = 0;
                    m_collided_bottom = true;
                }
            }
        }
    }
}



void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            //if player hits exit
            if (m_entity_type == EXIT && collidable_entity->m_entity_type == PLAYER) {
                m_collided_exit = true;
            }

            //kill player
            if (m_entity_type == PLAYER && collidable_entity->m_entity_type == ENEMY) {
                m_is_active = false;
            }
            
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}


void const Entity::check_collision_y(Map* map)
{

    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = -m_velocity.x;

        m_collided_right = true;
    }
    
}



void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) { return; }

    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


