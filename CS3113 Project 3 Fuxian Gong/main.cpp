/**
* Author: Fuxian Gong
* Assignment: Lunar Lander
* Date due: 2025-03-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "stb_image.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"               // We'll talk about these later in the course
#include "Entity_Platform.h"
#include "Entity_Rocket.h"
#include <vector>
#include <string>
#define LOG(argument) std::cout << argument << '\n'
enum AppStatus { RUNNING, TERMINATED };


using namespace std;

const int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 960;
SDL_Window* displayWindow;

const int VIEWPORT_X = 0, VIEWPORT_Y = 0, VIEWPORT_WIDTH = WINDOW_WIDTH, VIEWPORT_HEIGHT = WINDOW_HEIGHT;

#define LOG(argument) std::cout << argument << '\n'

// Shader variables
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float BG_RED = 0.1f,
BG_GREEN = 0.1f, 
BG_BLUE = 0.2f, 
BG_OPACITY = 1.0f;


//platform initialization, with bombs 
glm::vec3 bomb_positions[2] = {
    glm::vec3(-3.0f, -5.5f, 0.0f),
    glm::vec3(-6.0f, -6.0f, 0.0f),
};

glm::vec3 g_platform_positions[2] = {
    glm::vec3(-11.0f, -3.0f, 0.0f),
    glm::vec3(-2.5f, -10.5f, 0.0f),
};
    
glm::vec3 g_platform_scale = glm::vec3(1.0f, 1.0f, 0.0f);
// rocket initialization
glm::vec3 g_rocket_scale = glm::vec3(1.2f, 1.2f, 0.0f);
glm::vec3 g_rocket_position = glm::vec3(10.0f, 5.0f, 0.0f);
//initial velocity
float ROCKET_SPEED = 1.0f;
bool gameIsRunning;
bool gameOn = true;

//textures
const char ROCKET_SPRITE_FILEPATH[] = "rocket.png";
const char PLATFORM_SPRITE_FILEPATH[] = "platform.png";
const char BOMB_SPRITE_FILEPATH[] = "bomb.png";
const char FONT_SPRITE_FILEPATH[] = "font.png";
GLuint g_rocket_texture_id;
GLuint g_platform_texture_id;
GLuint g_bomb_texture_id;
GLuint g_font_texture_id;

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

struct GameState {
    Rocket* rocket;
    Platform* platform;
    Platform* bombs;
};

GameState g_game_state;
int PLATFORM_COUNT = 2;
int BOMB_COUNT = 2;
bool winner = false;

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
float g_time_accumulator = 0.0f;
float g_previous_ticks = 0.0f;
const float MILLISECONDS_IN_SECOND = 1000.0f;

//load texture func
GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise() {
    //HARD INITIALIZE
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

    gameIsRunning = true;

#ifdef _WINDOWS
    glewInit();
#endif
    //SOFT INITIALIZE
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-20.0f, 20.0f, -15.0f, 15.0f, -1.0f, 1.0f);

    g_shader_program.set_view_matrix(g_view_matrix);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    g_rocket_texture_id = load_texture(ROCKET_SPRITE_FILEPATH);
    g_platform_texture_id = load_texture(PLATFORM_SPRITE_FILEPATH);
    g_bomb_texture_id = load_texture(BOMB_SPRITE_FILEPATH);
    g_font_texture_id = load_texture(FONT_SPRITE_FILEPATH);

    g_game_state.rocket = new Rocket(g_rocket_position, g_rocket_scale, ROCKET_SPEED, g_rocket_texture_id);
    g_game_state.platform = new Platform[PLATFORM_COUNT];
    g_game_state.bombs = new Platform[BOMB_COUNT];

    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        g_game_state.platform[i].set_text_id(g_platform_texture_id);
        g_game_state.platform[i].set_position(g_platform_positions[i]);
        g_game_state.platform[i].set_scale(g_platform_scale);
    }
    for (int i = 0; i < BOMB_COUNT; ++i) {
        g_game_state.bombs[i].set_text_id(g_bomb_texture_id);
        g_game_state.bombs[i].set_position(bomb_positions[i]);
        g_game_state.bombs[i].set_scale(g_platform_scale);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (gameOn) {
        if (key_state[SDL_SCANCODE_W]) g_game_state.rocket->inc_y();
        if (key_state[SDL_SCANCODE_S]) g_game_state.rocket->dec_y();
        if (key_state[SDL_SCANCODE_A]) g_game_state.rocket->dec_x();
        if (key_state[SDL_SCANCODE_D]) g_game_state.rocket->inc_x();
    }
}

void update() {
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    //boundry and lose condition
    if (g_game_state.rocket->get_position().y < -15.0f) {
        gameOn = false;
        winner = false;
    }
    if (g_game_state.rocket->get_position().x >= 20.0f) {
        gameOn = false;
        winner = false;
    }
    if (g_game_state.rocket->get_position().x <= -20.0f) {
        gameOn = false;
        winner = false;
    }
    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        g_game_state.platform[i].update();
        if (g_game_state.rocket->collision(&g_game_state.platform[i])) {
            gameOn = false;
            // Rocket lands successfully on platform
            if (g_game_state.rocket->get_position().y > g_game_state.platform[i].get_position().y) {
                winner = true;
            }
            // Rocket crashes into platform
            else {
                winner = false;
            }
        }
    }

    for (int i = 0; i < BOMB_COUNT; ++i) {
        g_game_state.bombs[i].update();
        if (g_game_state.rocket->collision(&g_game_state.bombs[i])) {
            gameOn = false;
            winner = false;
        }
    }
    delta_time += g_time_accumulator;
    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }
    while (delta_time >= FIXED_TIMESTEP)
    {
        if (gameOn) {
            g_game_state.rocket->update(FIXED_TIMESTEP);
        }
        delta_time -= FIXED_TIMESTEP;
    }
    g_time_accumulator = delta_time;
}

//for using the font got from lecture
constexpr int FONTBANK_SIZE = 16;
void draw_text(ShaderProgram* program, GLuint g_font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    std::vector<float> vertices;
    std::vector<float> texture_coordinates;


    for (int i = 0; i < text.size(); i++) {
        int spritesheet_index = (int)text[i];  
        float offset = (font_size + spacing) * i;
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

 
        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, g_font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
//draw
void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id) {
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void render() {
    glClear(GL_COLOR_BUFFER_BIT);  
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    }; 
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    g_game_state.rocket->render(&g_shader_program);
    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        g_game_state.platform[i].render(&g_shader_program);
    }
    for (int i = 0; i < BOMB_COUNT; ++i) {
        g_game_state.bombs[i].render(&g_shader_program);
    }

    // win lose using font
    if (gameOn == false) {
        if (winner == true) {
            draw_text(&g_shader_program, g_font_texture_id, "Reached Platform", 1.0f, 0.05f, glm::vec3(-7.0f, 2.0f, 0.0f));
        }
        else {
            draw_text(&g_shader_program, g_font_texture_id, "Crashed", 1.0f, 0.05f, glm::vec3(-7.0f, 2.0f, 0.0f));
        }
    }

    // fuel ui 
    std::string fuel_text = "Fuel: " + std::to_string((int)g_game_state.rocket->get_fuel());
    draw_text(&g_shader_program, g_font_texture_id, fuel_text, 1.0f, 0.05f, glm::vec3(-15.0f, 12.0f, 0.0f));

    SDL_GL_SwapWindow(displayWindow);
}
void shutdown() { SDL_Quit(); }

int main(int argc, char* argv[]) {
    initialise();

    while (gameIsRunning) {
        process_input();

        update();

        render();
    }


    shutdown();
    return 0;
}