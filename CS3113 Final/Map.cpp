#include "Map.h"
#include "Utility.h"
#include <iostream>

//add an external declaration for the current level
extern int g_current_level;

Map::Map() {
    //default platform dimensions 
    platform_width = 48.0f; 
    platform_height = 1.0f; 
    

    platforms.clear(); 
    
    float platform_x = 0.0f; 
    float platform_y = -4.0f; 
    platforms.push_back(glm::vec3(platform_x, platform_y, 0.0f));
    

    left_wall_height = 12.0f; 
    left_wall_x = platform_x - (platform_width / 2.0f) + 0.5f;  
    left_wall_bottom_y = platform_y + 0.5f; 
    
    //right wall
    right_wall_height = 12.0f; 
    right_wall_x = platform_x + (platform_width / 2.0f) - 0.5f;  
    right_wall_bottom_y = platform_y + 0.5f;  
    
    load_textures();
}
//destructor
Map::~Map() {

    glDeleteTextures(1, &background_texture_id);
    glDeleteTextures(1, &platform_texture_id);
}
//load background and the blocks for platforms
void Map::load_textures() {
    background_texture_id = Utility::load_texture("assets/Final/Background_1.png");
    platform_texture_id = Utility::load_texture("assets/final/singleblock.png");
}

void Map::render(ShaderProgram* program) {
    glm::mat4 black_model_matrix;
    float black_vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
        
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
    };
    
    if (black_texture_id == 0) {
        unsigned char black_pixel[] = { 0, 0, 0, 255 }; 
        glGenTextures(1, &black_texture_id);
        glBindTexture(GL_TEXTURE_2D, black_texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, black_pixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    glBindTexture(GL_TEXTURE_2D, black_texture_id);
    
    //left and right black part
    black_model_matrix = glm::mat4(1.0f);
    black_model_matrix = glm::translate(black_model_matrix, glm::vec3(left_wall_x - 24.0f, 0.0f, 0.0f));
    black_model_matrix = glm::scale(black_model_matrix, glm::vec3(48.0f, 36.0f, 1.0f));
    program->set_model_matrix(black_model_matrix);
    
    glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), black_vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &black_vertices[3]);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    black_model_matrix = glm::mat4(1.0f);
    black_model_matrix = glm::translate(black_model_matrix, glm::vec3(right_wall_x + 24.0f, 0.0f, 0.0f));
    black_model_matrix = glm::scale(black_model_matrix, glm::vec3(48.0f, 36.0f, 1.0f));
    program->set_model_matrix(black_model_matrix);
    
    glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), black_vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &black_vertices[3]);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    if (g_current_level != 3) {
        glm::mat4 bg_model_matrix = glm::mat4(1.0f);
        bg_model_matrix = glm::scale(bg_model_matrix, glm::vec3(48.0f, 36.0f, 1.0f));
        
        program->set_model_matrix(bg_model_matrix);
        
        float bg_vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
        };
        
        glBindTexture(GL_TEXTURE_2D, background_texture_id);
        
        glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), bg_vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &bg_vertices[3]);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
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
        
        -0.5f, -0.5f, 0.0f, tex_coords[0], tex_coords[1],
         0.5f,  0.5f, 0.0f, tex_coords[4], tex_coords[5],
        -0.5f,  0.5f, 0.0f, tex_coords[6], tex_coords[7]
    };
    
    glBindTexture(GL_TEXTURE_2D, platform_texture_id);

    float block_width = 1.0f;
    float block_height = 1.0f;

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, platforms[0]);
    
    //count blocks
    int block_count = static_cast<int>(platform_width / block_width);
    
    //draw each block individually to create the platform
    for (int i = 0; i < block_count; i++) {
        float block_x = (platforms[0].x - platform_width/2.0f) + (i * block_width) + (block_width/2.0f);
        float block_y = platforms[0].y;
        
        glm::mat4 block_matrix = glm::mat4(1.0f);
        block_matrix = glm::translate(block_matrix, glm::vec3(block_x, block_y, 0.0f));
        block_matrix = glm::scale(block_matrix, glm::vec3(block_width, block_height, 1.0f));
        
        program->set_model_matrix(block_matrix);
        
        glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    //left wall render
    int wall_height_blocks = static_cast<int>(left_wall_height / block_height);
    for (int i = 0; i < wall_height_blocks; i++) {
        float block_x = left_wall_x;
        float block_y = left_wall_bottom_y + (i * block_height) + (block_height/2.0f);
        
        glm::mat4 block_matrix = glm::mat4(1.0f);
        block_matrix = glm::translate(block_matrix, glm::vec3(block_x, block_y, 0.0f));
        block_matrix = glm::scale(block_matrix, glm::vec3(block_width, block_height, 1.0f));
        
        program->set_model_matrix(block_matrix);
        
        glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    //right wall render
    for (int i = 0; i < wall_height_blocks; i++) {
        float block_x = right_wall_x;
        float block_y = right_wall_bottom_y + (i * block_height) + (block_height/2.0f);
        
        glm::mat4 block_matrix = glm::mat4(1.0f);
        block_matrix = glm::translate(block_matrix, glm::vec3(block_x, block_y, 0.0f));
        block_matrix = glm::scale(block_matrix, glm::vec3(block_width, block_height, 1.0f));
        
        program->set_model_matrix(block_matrix);
        
        glVertexAttribPointer(program->get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &vertices[3]);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool Map::is_solid(float x, float y) {
    //if the position is within the bounds of our single large platform
    const glm::vec3& platform = platforms[0];
    float platform_left = platform.x - (platform_width / 2.0f);
    float platform_right = platform.x + (platform_width / 2.0f);
    float platform_top = platform.y + (platform_height / 2.0f);
    float platform_bottom = platform.y - (platform_height / 2.0f);
    
    //boudary check
    if (x >= platform_left && x <= platform_right && 
        y >= platform_bottom && y <= platform_top) {
        return true;
    }
    
    float left_wall_left = left_wall_x - 0.5f;
    float left_wall_right = left_wall_x + 0.5f;
    float left_wall_top = left_wall_bottom_y + left_wall_height;
    float left_wall_bottom = left_wall_bottom_y - 0.5f;
    
    if (x >= left_wall_left && x <= left_wall_right && 
        y >= left_wall_bottom && y <= left_wall_top) {
        return true;
    }
    
    float right_wall_left = right_wall_x - 0.5f;
    float right_wall_right = right_wall_x + 0.5f;
    float right_wall_top = right_wall_bottom_y + right_wall_height;
    float right_wall_bottom = right_wall_bottom_y - 0.5f;
    
    if (x >= right_wall_left && x <= right_wall_right && 
        y >= right_wall_bottom && y <= right_wall_top) {
        return true;
    }
    
    return false;
}

bool Map::check_platform_collision(float x, float y, float* y_correction) {
    //collision check, y axis
    const glm::vec3& platform = platforms[0];
    float platform_left = platform.x - (platform_width / 2.0f);
    float platform_right = platform.x + (platform_width / 2.0f);
    float platform_top = platform.y + (platform_height / 2.0f);
    
    if (x >= platform_left && x <= platform_right) {
        float threshold = 0.3f;  
        if (y <= platform_top && y >= platform_top - threshold) {
            if (y_correction != nullptr) {
                *y_correction = platform_top;
            }
            return true;
        }
    }
    //wall collision check
    float left_wall_left = left_wall_x - 0.5f;
    float left_wall_right = left_wall_x + 0.5f;
    float left_wall_top = left_wall_bottom_y + left_wall_height;
    
    if (x >= left_wall_left && x <= left_wall_right) {
        float threshold = 0.3f;
        if (y <= left_wall_top && y >= left_wall_top - threshold) {
            if (y_correction != nullptr) {
                *y_correction = left_wall_top;
            }
            return true;
        }
    }

    float right_wall_left = right_wall_x - 0.5f;
    float right_wall_right = right_wall_x + 0.5f;
    float right_wall_top = right_wall_bottom_y + right_wall_height;
    
    if (x >= right_wall_left && x <= right_wall_right) {
        float threshold = 0.3f;
        if (y <= right_wall_top && y >= right_wall_top - threshold) {
            if (y_correction != nullptr) {
                *y_correction = right_wall_top;
            }
            return true;
        }
    }
    
    return false;
}
//should not fall below
float Map::check_point_y_collision(float x, float y, float prev_y) {
    const glm::vec3& platform = platforms[0];
    float platform_left = platform.x - (platform_width / 2.0f);
    float platform_right = platform.x + (platform_width / 2.0f);
    float platform_top = platform.y + (platform_height / 2.0f);
    //wall top collision
    if (x >= platform_left && x <= platform_right) {
        if (prev_y >= platform_top && y < platform_top) {
            return platform_top;
        }
    }

    float left_wall_left = left_wall_x - 0.5f;
    float left_wall_right = left_wall_x + 0.5f;
    float left_wall_top = left_wall_bottom_y + left_wall_height;
    
    if (x >= left_wall_left && x <= left_wall_right) {
        if (prev_y >= left_wall_top && y < left_wall_top) {
            return left_wall_top;
        }
    }

    float right_wall_left = right_wall_x - 0.5f;
    float right_wall_right = right_wall_x + 0.5f;
    float right_wall_top = right_wall_bottom_y + right_wall_height;
    
    if (x >= right_wall_left && x <= right_wall_right) {
        if (prev_y >= right_wall_top && y < right_wall_top) {
            return right_wall_top;
        }
    }

    return -999.0f;
} 