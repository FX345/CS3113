/**
* Author: Fuxian Gong
* Assignment: Project 4: Rise of the AI
* Date due: 2025/4/5, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <Windows.h>

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 1
#define LEVEL1_WIDTH 30
#define LEVEL1_HEIGHT 10
#define LOG(argument) std::cout << argument << '\n'
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Menu.h"

using namespace std;

const char GAME_WINDOW_NAME[] = "Project 4";
const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

float BG_RED = 0.2f,
BG_BLUE = 0.1f,
BG_GREEN = 0.15f,
BG_OPACITY = 1.0f;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
const char FONT_SPRITE[] = "assets/font/font.png";

//delta time
float previous_ticks = 0.0f;
const float MILLISECONDS_IN_SECOND = 1000.0f;
float delta_time;
float g_accumulator = 0.0f;

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_on = false;
bool winner;
bool loser;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

GLuint font_texture_id;
GLuint player_texture_id;
GLuint enemy_texture_id;

Utility g_utility;

int levelNum = 1;
int lives = 3;

Scene* g_current_scene;
Level1* g_level_1;
Level2* g_level_2;
Level3* g_level_3;
Menu* g_menu;


// music
constexpr int CD_QUAL_FREQ = 44100,  
AUDIO_CHAN_AMT = 2,      
AUDIO_BUFF_SIZE = 4096;

// SFX
constexpr int PLAY_ONCE = 0,
NEXT_CHNL = -1,  
MUTE_VOL = 0,
MILS_IN_SEC = 1000,
ALL_SFX_CHN = -1;

constexpr char BGM_FILEPATH[] = "assets/audio/music.mp3",
BOUNCING_SFX_FILEPATH[] = "assets/audio/bounce.wav",
WINNING_SFX_FILEPATH[] = "assets/audio/win.wav",
LOSING_SFX_FILEPATH[] = "assets/audio/lose.wav";


//for bgm
constexpr int  LOOP_FOREVER = -1; 

Mix_Music* g_music;
Mix_Chunk* g_bouncing_sfx;
Mix_Chunk* g_losing_sfx;
Mix_Chunk* g_winning_sfx;

//change scene
void switch_to_scene(Scene* scene) {
    g_current_scene = scene;
    g_current_scene->initialise();
}

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
//load texture
GLuint load_texture(const char* filepath)
{
	int width, height, number_of_components;
	unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

	if (image == NULL)
	{
		LOG("Unable to load image. Make sure the path is correct.");
		assert(false);
	}

	GLuint texture_id;
	glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(image);

	return texture_id;
}


void initialise() {
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-10.0f, 10.0f, -7.0f, 7.0f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    
    //initialise texture_ids
    font_texture_id = Utility::load_texture(FONT_SPRITE);
    
    g_level_1 = new Level1();
    g_level_2 = new Level2();
    g_level_3 = new Level3();
    

    g_menu = new Menu();

    switch_to_scene(g_menu);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // start audio
    Mix_OpenAudio(
        CD_QUAL_FREQ,        
        MIX_DEFAULT_FORMAT, 
        AUDIO_CHAN_AMT,     
        AUDIO_BUFF_SIZE   
    );

    
    g_music = Mix_LoadMUS(BGM_FILEPATH);
    g_bouncing_sfx = Mix_LoadWAV(BOUNCING_SFX_FILEPATH);
    g_winning_sfx = Mix_LoadWAV(WINNING_SFX_FILEPATH);
    g_losing_sfx= Mix_LoadWAV(LOSING_SFX_FILEPATH);
    //bgm
    Mix_PlayMusic(g_music, LOOP_FOREVER);
    Mix_VolumeMusic(MIX_MAX_VOLUME/2);
    //sound effects
    Mix_VolumeChunk(
        g_bouncing_sfx,     
        MIX_MAX_VOLUME  
    );
}

void process_input()
{

    
    if (g_game_on) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                //double jump to get rid of enem
                if (g_game_on) {
                    if (g_current_scene->get_state().player->m_jumps >= 1)
                    {
                        g_current_scene->get_state().player->m_is_jumping = true;
                        Mix_PlayChannel(
                            NEXT_CHNL,       // using the first channel that is not currently in use...
                            g_bouncing_sfx,  // ...play this chunk of audio...
                            PLAY_ONCE        
                        );
                    }
                }
                break;

            case SDLK_RETURN:
                if (g_game_on == false) {
                    Mix_PlayChannel(
                        NEXT_CHNL,     
                        g_winning_sfx,  
                        PLAY_ONCE
                    );
                    switch_to_scene(g_level_1);
                    g_game_on = true;
                }
            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (g_game_on) {
        if (key_state[SDL_SCANCODE_LEFT])
        {
            g_current_scene->get_state().player->move_left();
            g_current_scene->get_state().player->m_animation_indices = g_current_scene->get_state().player->m_walking[g_current_scene->get_state().player->LEFT];
        }
        else if (key_state[SDL_SCANCODE_RIGHT])
        {
            g_current_scene->get_state().player->move_right();
            g_current_scene->get_state().player->m_animation_indices = g_current_scene->get_state().player->m_walking[g_current_scene->get_state().player->RIGHT];
        }

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
        {
            g_current_scene->get_state().player->set_movement(glm::normalize(g_current_scene->get_state().player->get_movement()));
        }
    }
    
}

void update()
{

    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - previous_ticks;
    previous_ticks = ticks;
    
    //decrement number of enemies alive
    int enemies_alive = ENEMY_COUNT;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (!g_current_scene->get_state().enemies[i].get_active()) {
            enemies_alive -= 1;
        }
    }
    winner = false;
    loser = false;
    if (g_game_on) {

        //lose
        if (lives <= 0) {
            g_game_on == false;
            loser = true;
            
            return;
        }
        //if reaches exit, go next level
        if (g_current_scene->get_state().exit->m_collided_exit == true && levelNum == 1) {
            switch_to_scene(g_level_2);
            levelNum++;
            return;
            
        }
        if (g_current_scene->get_state().exit->m_collided_exit == true && levelNum == 2) {
            switch_to_scene(g_level_3);
            levelNum++;
            return;
        }
        //win
        if (g_current_scene->get_state().exit->m_collided_exit == true && levelNum == 3) {
            g_game_on == false;
            winner = true;
            return;
        }
        

        //if player is dead, restart at the same level
        if (!g_current_scene->get_state().player->get_active()) {
            Mix_PlayChannel(
                NEXT_CHNL,
                g_losing_sfx,
                PLAY_ONCE
            );
            if (levelNum == 1 && g_game_on == true) {
                lives--;
                switch_to_scene(g_level_1);
                return;
            }
            else if (levelNum == 2 && g_game_on == true) {
                lives--;
                switch_to_scene(g_level_2);
                return;
            }
            else if (levelNum == 3 && g_game_on == true) {
                lives--;
                switch_to_scene(g_level_3);
            }

        }
    }
    

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    //update player and enemies
    while (delta_time >= FIXED_TIMESTEP)
    {
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, -g_current_scene->get_state().player->get_position().y, 0.0f));
    
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);
    

    g_current_scene->render(&g_shader_program);

    if (g_game_on) {
        draw_text(&g_shader_program, font_texture_id, "Lives:" + to_string(lives), 0.8, 0.1, g_current_scene->get_state().player->get_position() + glm::vec3(-9.0, 6.5, 0.0));
        if (winner == true) {
            draw_text(&g_shader_program, font_texture_id, "You Win!", 1, 0.1, g_current_scene->get_state().player->get_position() + glm::vec3(0.0, 1.0, 0.0));
        }
        if (loser == true) {
            draw_text(&g_shader_program, font_texture_id, "You Lose!", 1, 0.1, g_current_scene->get_state().enemies->get_position());
        }
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete g_level_1;
    delete g_level_2;
    delete g_level_3;
    delete g_menu;
    Mix_FreeChunk(g_bouncing_sfx);
    Mix_FreeMusic(g_music);
    Mix_HaltMusic();
    
}


int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}