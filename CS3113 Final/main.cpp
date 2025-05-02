/**
* Author: Fuxian Gong
* Assignment: Final Project
* Due Date: 2025/5/2
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
#define LEVEL1_WIDTH 50
#define LEVEL1_HEIGHT 10
#define LOG(argument) std::cout << argument << '\n'
#define FONTBANK_SIZE 16
#define LEFT_BOUND -12.0f
#define RIGHT_BOUND 12.0f

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h> 
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include <string>
#include "Map.h"
#include "Utility.h"
#include "Entity.h"
#include "Knight.h"
#include "Slash.h"
#include "Boss.h"
#include "Boss2.h"
#include "Boss3.h"
#include "StartMenu.h"
#include "Effects.h"

using namespace std;

const char GAME_WINDOW_NAME[] = "Final Project";
const int WINDOW_WIDTH = 1024,
WINDOW_HEIGHT = 768;

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
const char FONT_SPRITE_FILEPATH[] = "assets/font/font.png";
const char KNIGHT_IDLE_FILEPATH[] = "assets/Knight_1/Idle.png";
const char KNIGHT_RUN_FILEPATH[] = "assets/Knight_1/Run.png";
const char KNIGHT_ATTACK_FILEPATH[] = "assets/Knight_1/RUN+ATTACK.png";
const char KNIGHT_JUMP_FILEPATH[] = "assets/Knight_1/Jump.png";
const char KNIGHT_DEFEND_FILEPATH[] = "assets/Knight_1/Protect.png";
const char KNIGHT_HURT_FILEPATH[] = "assets/Knight_1/Hurt.png";
const char TILEMAP_SPRITE[] = "assets/final/Tiles.png";
const char BACKGROUND_FILEPATH[] = "assets/final/background_0.png";
const char START_MENU_BACKGROUND[] = "assets/background/start.gif";
const char SLASH_FRAME1_FILEPATH[] = "assets/slash/frame1.png";
const char SLASH_FRAME2_FILEPATH[] = "assets/slash/frame2.png";
const char SLASH_FRAME3_FILEPATH[] = "assets/slash/frame3.png";
const char SLASH_FRAME4_FILEPATH[] = "assets/slash/frame4.png";
const char SLASH_FRAME5_FILEPATH[] = "assets/slash/frame5.png";
const char BOSS_IDLE_FILEPATH[] = "assets/boss/individual sprites/01_idle/idle_1.png";
const char BOSS_ATTACK_FILEPATH[] = "assets/boss/boss_attack.png";
const char BOSS_CHARGE_FILEPATH[] = "assets/boss/boss_charge.png";
const char BOSS_HURT_FILEPATH[] = "assets/boss/boss_hurt.png";
const char LEVEL3_BACKGROUND_FILEPATH[] = "assets/final/dawn/dawn.png";

//delta time
float g_previous_ticks = 0.0f;
const float MILLISECONDS_IN_SECOND = 1000.0f;
float delta_time;
float g_time_accumulator = 0.0f;

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_game_on = true;
bool winner = false;
bool loser = false;

//game state enum and variable to track current state
enum GameMode {
    STATE_MENU,
    STATE_TRANSITION,
    STATE_GAME,
    STATE_GAME_OVER
};
GameMode g_current_mode = STATE_MENU;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix;
glm::mat4 g_projection_matrix;

GLuint g_font_texture_id;
GLuint knight_idle_texture_id;
GLuint knight_run_texture_id;
GLuint knight_attack_texture_id;
GLuint knight_jump_texture_id;
GLuint knight_defend_texture_id;
GLuint knight_hurt_texture_id;
GLuint background_texture_id;
GLuint start_menu_background_id;
GLuint boss_idle_texture_id;
GLuint boss_attack_texture_id;
GLuint boss_charge_texture_id;
GLuint boss_hurt_texture_id;

//for player helth bar 
GLuint g_health_bar_textures[6]; 

//game state
//3 level bosses
struct GameState {
    Knight* player;
    Map* map;
    Slash* slash;
    Boss* boss;       
    Boss2* boss2;     
    Boss3* boss3;     
    bool using_boss2; 
    bool using_boss3; 
    StartMenu* start_menu;
    Effects* effects;
    glm::vec3 camera_position;
};

GameState g_game_state;

unsigned int LEVEL_1_DATA[] =
{
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 6, 2, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 6,
    0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
};

//track current level
int g_current_level = 1;

//global variables
GLuint level3_background_texture_id;
float g_post_boss2_death_timer = 0.0f;
bool g_level3_transition_pending = false;
bool g_boss2_death_triggered = false;

//for audio
Mix_Music* g_game_music = nullptr;
//sound effects
Mix_Chunk* g_knight_hurt_sound = nullptr;
Mix_Chunk* g_slash_hit_sound = nullptr; 
Mix_Chunk* g_slash_attack_sound = nullptr; 
Mix_Chunk* g_defend_sound = nullptr; 
//load texture
GLuint load_texture(const char* filepath) {
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL) {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}
//function prototypes
void initialise();
void process_input();
void update();
void render();
void shutdown();
void setupLevel1();
void setupLevel2(); 
void setupLevel3(); 


void initialise() {
    //audio 
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);  
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // Initialize SDL_mixer with higher quality and more channels
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        std::cout << "ERROR: Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
    }
    
    // Allocate more mixing channels to ensure enough are available
    int allocated_channels = Mix_AllocateChannels(16); // Allocate 16 channels
    
    // Set overall volume
    Mix_Volume(-1, 128); 
    Mix_VolumeMusic(100); 
    
    // Load music
    g_game_music = Mix_LoadMUS("assets/audio/smt1re/09 battle.mp3");
    if (g_game_music == nullptr) {
        std::cout << "ERROR: Failed to load music: " << Mix_GetError() << std::endl;
        std::string full_music_path = "C:/Users/15028/source/repos/SDLSimple/CS3113 Final/assets/audio/smt1re/09 battle.mp3";
        g_game_music = Mix_LoadMUS(full_music_path.c_str());
        if (g_game_music == nullptr) {
            std::cout << "ERROR: Failed to load music with full path: " << Mix_GetError() << std::endl;
        }
    }
    
    // Base path for sound files
    std::string sound_base_path = "C:/Users/15028/source/repos/SDLSimple/CS3113 Final/assets/soundeffect/";
    
    // Load sound effects
    g_knight_hurt_sound = Mix_LoadWAV((sound_base_path + "ow.wav").c_str());
    if (g_knight_hurt_sound == nullptr) {
        std::cout << "ERROR: Failed to load hurt sound effect: " << Mix_GetError() << std::endl;
    } else {
        Mix_VolumeChunk(g_knight_hurt_sound, 128); // Maximum volume
    }
    
    // Load slash sound with absolute path
    g_slash_hit_sound = Mix_LoadWAV((sound_base_path + "slash.wav").c_str());
    if (g_slash_hit_sound == nullptr) {
        g_slash_hit_sound = Mix_LoadWAV((sound_base_path + "slash (2).wav").c_str());
        if (g_slash_hit_sound == nullptr) {
            std::cout << "ERROR: Both slash sound files failed to load: " << Mix_GetError() << std::endl;
        } else {
            Mix_VolumeChunk(g_slash_hit_sound, 128); 
        }
    } else {
        Mix_VolumeChunk(g_slash_hit_sound, 128); 
    }
    
    //defend sound
    g_defend_sound = Mix_LoadWAV((sound_base_path + "defend.wav").c_str());
    if (g_defend_sound == nullptr) {
        std::cout << "ERROR: Failed to load defend sound effect: " << Mix_GetError() << std::endl;
    } else {
        Mix_VolumeChunk(g_defend_sound, 128);
    }
    
    //slash sound
    g_slash_attack_sound = Mix_LoadWAV((sound_base_path + "slash.wav").c_str());
    if (g_slash_attack_sound == nullptr) {
        g_slash_attack_sound = Mix_LoadWAV((sound_base_path + "slash (2).wav").c_str());
        if (g_slash_attack_sound == nullptr) {
            std::cout << "ERROR: Both slash attack sound files failed to load: " << Mix_GetError() << std::endl;
        } else {
            Mix_VolumeChunk(g_slash_attack_sound, 128); 
        }
    } else {
        Mix_VolumeChunk(g_slash_attack_sound, 128); 
    }
    
    //bgm
    if (g_game_music != nullptr) {
        if (Mix_PlayMusic(g_game_music, -1) == -1) {
            std::cout << "ERROR: Failed to play background music: " << Mix_GetError() << std::endl;
        }
    }
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-12.0f, 12.0f, -9.0f, 9.0f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

    //load textures
    g_font_texture_id = Utility::load_texture(FONT_SPRITE_FILEPATH);
    knight_idle_texture_id = Utility::load_texture(KNIGHT_IDLE_FILEPATH);
    knight_run_texture_id = Utility::load_texture(KNIGHT_RUN_FILEPATH);
    knight_attack_texture_id = Utility::load_texture(KNIGHT_ATTACK_FILEPATH);
    knight_jump_texture_id = Utility::load_texture(KNIGHT_JUMP_FILEPATH);
    knight_defend_texture_id = Utility::load_texture(KNIGHT_DEFEND_FILEPATH);
    knight_hurt_texture_id = Utility::load_texture(KNIGHT_HURT_FILEPATH);
    background_texture_id = Utility::load_texture(BACKGROUND_FILEPATH);
    start_menu_background_id = Utility::load_texture(START_MENU_BACKGROUND);
    
    //change background for level 3
    level3_background_texture_id = Utility::load_texture(LEVEL3_BACKGROUND_FILEPATH);

    if (level3_background_texture_id == 0) {
        std::cout << "failed to load Level 3 background" << std::endl;
    }
    //initializations
    g_game_state.map = new Map();
    g_game_state.player = new Knight();
    g_game_state.player->texture_id = knight_idle_texture_id;
    g_game_state.slash = new Slash();
    g_game_state.slash->texture_ids[0] = Utility::load_texture(SLASH_FRAME1_FILEPATH);
    g_game_state.slash->texture_ids[1] = Utility::load_texture(SLASH_FRAME2_FILEPATH);
    g_game_state.slash->texture_ids[2] = Utility::load_texture(SLASH_FRAME3_FILEPATH);
    g_game_state.slash->texture_ids[3] = Utility::load_texture(SLASH_FRAME4_FILEPATH);
    g_game_state.slash->texture_ids[4] = Utility::load_texture(SLASH_FRAME5_FILEPATH);
    g_game_state.camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.start_menu = new StartMenu();
    g_game_state.start_menu->init(start_menu_background_id);
    g_game_state.effects = new Effects(g_projection_matrix, g_view_matrix);
    
    g_game_state.boss = new Boss();
    g_game_state.boss2 = nullptr;  
    g_game_state.boss3 = nullptr;  
    g_game_state.using_boss2 = false; 
    g_game_state.using_boss3 = false; 
    
    g_game_state.boss->idle_textures[0] = Utility::load_texture("assets/boss/individual sprites/01_idle/idle_1.png");
    g_game_state.boss->idle_textures[1] = Utility::load_texture("assets/boss/individual sprites/01_idle/idle_2.png");
    g_game_state.boss->idle_textures[2] = Utility::load_texture("assets/boss/individual sprites/01_idle/idle_3.png");
    g_game_state.boss->idle_textures[3] = Utility::load_texture("assets/boss/individual sprites/01_idle/idle_4.png");
    g_game_state.boss->idle_textures[4] = Utility::load_texture("assets/boss/individual sprites/01_idle/idle_5.png");
    g_game_state.boss->idle_textures[5] = Utility::load_texture("assets/boss/individual sprites/01_idle/idle_6.png");
    
    //check texture loading
    bool textures_loaded_successfully = true;
    for (int i = 0; i < 6; i++) {
        if (g_game_state.boss->idle_textures[i] == 0) {
            LOG("Failed to load boss texture index: " << i);
            textures_loaded_successfully = false;
        }
    }
    
    if (textures_loaded_successfully) {
        LOG("");
    } else {
        LOG("failed to load");
    }
    
    g_game_state.boss->texture_id = g_game_state.boss->idle_textures[0];
    
    for (int i = 0; i < 6; i++) {
        g_game_state.boss->attack_textures[i] = g_game_state.boss->idle_textures[i];
        g_game_state.boss->charge_textures[i] = g_game_state.boss->idle_textures[i];
        g_game_state.boss->hurt_textures[i] = g_game_state.boss->idle_textures[i];
    }
    
    //knight position and facing
    g_game_state.player->position = glm::vec3(0.0f, 0.0f, 0.0f); 
    g_game_state.player->set_direction("right");
    
    g_game_state.boss->position = glm::vec3(10.0f, 1.0f, 0.0f);
    g_game_state.boss->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.boss->acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
    //hit box and visual size
    g_game_state.boss->width = 2.0f;  
    g_game_state.boss->height = 3.0f; 
    
    g_game_state.boss->visual_scale_x = 7.0f;  
    g_game_state.boss->visual_scale_y = 3.3f;  
    
    g_game_state.boss->state = BOSS_IDLE;
    g_game_state.boss->direction = 0; 

    g_game_state.boss->set_map(g_game_state.map);
   
    //health bar textures
    g_health_bar_textures[0] = load_texture("assets/healthbar/health1.png");
    g_health_bar_textures[1] = load_texture("assets/healthbar/health2.png");
    g_health_bar_textures[2] = load_texture("assets/healthbar/health3.png");
    g_health_bar_textures[3] = load_texture("assets/healthbar/health4.png");
    g_health_bar_textures[4] = load_texture("assets/healthbar/health5.png");
    g_health_bar_textures[5] = load_texture("assets/healthbar/health6.png");
    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //slash sound effect
    g_slash_attack_sound = Mix_LoadWAV("assets/soundeffect/slashcopy.wav");
    if (g_slash_attack_sound == nullptr) {
   
        g_slash_attack_sound = Mix_LoadWAV("assets/soundeffect/slash.wav");
        if (g_slash_attack_sound == nullptr) {
            std::cout << "failed to load" << Mix_GetError() << std::endl;
        } else {
            Mix_VolumeChunk(g_slash_attack_sound, 128); 
        }
    } else {
        std::cout << "Successfully loaded slashcopy.wav for attack" << std::endl;
        Mix_VolumeChunk(g_slash_attack_sound, 128); 
    }
}
//process input
void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            g_game_is_running = false;
        }
        //check if in menu mode
        if (g_current_mode == STATE_MENU) {
            if (g_game_state.start_menu->process_input(event)) {
                g_current_mode = STATE_TRANSITION;
                //reset effect, start fading
                g_game_state.effects->m_view_offset = glm::vec3(0.0f); 
                g_game_state.effects->start(FADEIN);
            }
            continue; 
        }
        
        if (g_current_mode == STATE_GAME_OVER || winner) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
                if (winner) {
                    winner = false;
                    // Reset game state
                    loser = false;
                    g_game_on = false;
                    if (g_game_state.start_menu != nullptr) {
                        delete g_game_state.start_menu; // Delete existing start menu
                    }
                    g_game_state.start_menu = new StartMenu();
                    g_game_state.start_menu->init(start_menu_background_id);
                    g_current_mode = STATE_MENU;
                    
                    g_game_state.using_boss3 = false;
                    g_current_level = 1; 
                    //reset to level 1
                    
                    g_game_state.player->reset_health();
                    g_game_state.player->position = glm::vec3(0.0f, 0.0f, 0.0f);
                    g_game_state.player->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
                    g_game_state.player->set_animation(IDLE);
                    g_game_state.player->texture_id = knight_idle_texture_id;
                    
                    //reset camera position
                    g_game_state.camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
                    
                    //reset all boss states
                    if (g_game_state.boss3 != nullptr) {
                        g_game_state.boss3->is_active = false;
                    }
                    if (g_game_state.boss2 != nullptr) {
                        g_game_state.boss2->is_active = false;
                    }
                    if (g_game_state.boss != nullptr) {
                        g_game_state.boss->is_active = false;
                    }
                    
                    g_shader_program.set_colour(1.0f, 1.0f, 1.0f, 1.0f);
                    std::cout << "Successfully forced transition to start menu" << std::endl;
                    
                    continue;
                }
                else {
                
                //reset player health and position
                g_game_state.player->reset_health();
                g_game_state.player->position = glm::vec3(0.0f, 0.0f, 0.0f);
                g_game_state.player->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
                g_game_state.player->set_animation(IDLE);
                g_game_state.player->texture_id = knight_idle_texture_id;
                
                    //reset boss 
                    if (g_current_level == 3 && g_game_state.using_boss3 && g_game_state.boss3 != nullptr) {

                        g_game_state.boss3->set_idle_before_cast();
                        g_game_state.boss3->health = g_game_state.boss3->max_health; 
                        g_game_state.boss3->is_active = true;  
                        
                        //clear existing fire effects and monsters
                        g_game_state.boss3->fire_effects.clear();
                        g_game_state.boss3->fire_effects_active = false;
                        g_game_state.boss3->monsters_active = false;
                        
                        //reposition the boss
                        g_game_state.boss3->position = glm::vec3(5.0f, 1.0f, 0.0f);
                        float platform_y = -999.0f;
                        for (float y = 0.0f; y > -5.0f; y -= 0.1f) {
                            if (g_game_state.map->is_solid(g_game_state.boss3->position.x, y)) {
                                platform_y = y + 0.1f;
                                break;
                            }
                        }
                        if (platform_y != -999.0f) {
                            g_game_state.boss3->position.y = platform_y + g_game_state.boss3->height/2 + 1.0f;
                        }
                    } else if (g_game_state.boss) {
                    g_game_state.boss->position = glm::vec3(10.0f, 1.0f, 0.0f);
                    g_game_state.boss->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
                }
                
                //switch back to game state
                g_current_mode = STATE_GAME;
                g_game_on = true;
                loser = false;
                    g_shader_program.set_colour(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
            continue; 
        }
        
        if (g_current_mode == STATE_TRANSITION) {
            continue;
        }
        
        //key press 
        else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    //jump 
                    if (g_game_on && !g_game_state.player->is_defending) {
                        g_game_state.player->jump();
                        if (g_knight_hurt_sound != nullptr) {
                            //jump sound effect
                            Mix_HaltChannel(2);
                            Mix_PlayChannel(2, g_knight_hurt_sound, 0);
                        }
                    }
                    break;
                    
                case SDLK_f:
                    // Attack - only allow if grounded (not jumping or falling) and not defending
                    if (g_game_on && !g_game_state.player->is_attacking && g_game_state.player->is_grounded && !g_game_state.player->is_defending) {
                        // Play slash sound effect first (to ensure it plays) - use channel 1 specifically
                        if (g_slash_attack_sound != nullptr) {
                            // Stop any sound on this channel first
                            Mix_HaltChannel(1);
                            // Use channel 1 specifically for slash sounds to avoid conflicts
                            Mix_PlayChannel(1, g_slash_attack_sound, 0);
                        } else if (g_slash_hit_sound != nullptr) {
                            // Fallback to slash_hit_sound if needed
                            Mix_PlayChannel(1, g_slash_hit_sound, 0);
                        }
                        
                        g_game_state.player->attack();
                        g_game_state.player->texture_id = knight_attack_texture_id;
                        
                        // Activate slash effect
                        bool facing_right = (g_game_state.player->direction == RIGHT);
                        g_game_state.slash->activate(g_game_state.player->position, facing_right);
                    }
                    break;
                    
                case SDLK_LSHIFT:
                    // Defend - only allow if grounded and not attacking
                    if (g_game_on && !g_game_state.player->is_defending && 
                        !g_game_state.player->is_attacking && g_game_state.player->is_grounded) {
                        g_game_state.player->defend();
                        g_game_state.player->texture_id = knight_defend_texture_id;
                        
                        // Play defend sound effect - use channel 3 specifically
                        if (g_defend_sound != nullptr) {
                            // Stop any sound on this channel first
                            Mix_HaltChannel(3);
                            // Use channel 3 specifically for defend sounds to avoid conflicts
                            Mix_PlayChannel(3, g_defend_sound, 0);
                        }
                    }
                    break;
                    
                case SDLK_k:
                    // Test camera shake
                    if (g_game_on && g_game_state.effects) {
                        std::cout << "DEBUG: Testing camera shake with K key" << std::endl;
                        g_game_state.effects->start(SHAKE_H, 3.0f, 0.5f);
                    }
                    break;
            }
        }
        // Handle defend release - end defending when left shift is released
        else if (event.type == SDL_KEYUP) {
            // LSHIFT key-release handling removed since defend now has a fixed duration
            
            // Keep this section for other key-release handlers in the future
        }
    }
    
    // Process knight movement
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (g_game_on) {
        // Process knight horizontal movement only - vertical is controlled by physics
        g_game_state.player->move_left = key_state[SDL_SCANCODE_A];
        g_game_state.player->move_right = key_state[SDL_SCANCODE_D];
        
        // No longer use up/down keys for movement in a platformer
        g_game_state.player->move_up = false;
        g_game_state.player->move_down = false;
        
        // Update direction
        if (g_game_state.player->move_left) {
            g_game_state.player->set_direction("left");
        } else if (g_game_state.player->move_right) {
            g_game_state.player->set_direction("right");
        }
    }
}

// Helper function for hitbox calculations
float get_distance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Simplified slash-boss collision detection without debug output
bool check_slash_boss_collision(Slash* slash, Boss* boss, Knight* knight) {
    if (!slash->is_active || !boss->is_active) {
        return false;
    }
    
    // Adjust the slash size for more accurate hitbox detection
    float slash_effective_width = slash->width * 0.8f;  // 80% of actual width
    float slash_effective_height = slash->height * 0.8f;  // 80% of actual height
    
    // Calculate the hitbox for the slash with adjusted size
    float slash_left = slash->position.x - (slash_effective_width / 2.0f);
    float slash_right = slash->position.x + (slash_effective_width / 2.0f);
    float slash_top = slash->position.y + (slash_effective_height / 2.0f);
    float slash_bottom = slash->position.y - (slash_effective_height / 2.0f);
    
    // The boss hitbox should use the reduced size set in initialise()
    float boss_left = boss->position.x - (boss->width / 2.0f);
    float boss_right = boss->position.x + (boss->width / 2.0f);
    float boss_top = boss->position.y + (boss->height / 2.0f);
    float boss_bottom = boss->position.y - (boss->height / 2.0f);
    
    // Check for overlap between hitboxes
    bool x_overlap = !(slash_right < boss_left || slash_left > boss_right);
    bool y_overlap = !(slash_top < boss_bottom || slash_bottom > boss_top);
    
    return x_overlap && y_overlap;
}

void update() {
    // Update elapsed time
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    // Game update logic
    delta_time += g_time_accumulator;
    if (delta_time < FIXED_TIMESTEP) {
        g_time_accumulator = delta_time;
        return;
    }
    
    // If in menu mode, no need to update game elements
    if (g_current_mode == STATE_MENU) {
        g_time_accumulator = 0.0f;
        return;
    }
    
    if (g_current_mode == STATE_GAME_OVER) {
        g_time_accumulator = 0.0f;
        return;
    }
    
    // Handle transition 
    if (g_current_mode == STATE_TRANSITION) {
        // For transition
        while (delta_time >= FIXED_TIMESTEP) {
            g_game_state.effects->update(FIXED_TIMESTEP);
            //set up levels
            if (g_game_state.effects->is_done()) {
                if (g_current_level == 1) {
                    setupLevel1();
                    g_current_mode = STATE_GAME;
                }
                else if (g_current_level == 2) {
                    setupLevel2();
                    g_current_mode = STATE_GAME;
                }
                else if (g_current_level == 3) {
   
                    setupLevel3();
                    g_current_mode = STATE_GAME;
                }
                break;
            }
            
            delta_time -= FIXED_TIMESTEP;
        }
        
        g_time_accumulator = delta_time;
        return;
    }
    
    //track slash
    static int current_slash_id = 0;
    static int last_damaging_slash_id = -1;
    
    while (delta_time >= FIXED_TIMESTEP) {
        if (g_game_on) {
            if (g_game_state.player->health <= 0) {
                g_game_on = false;
                loser = true;
                g_current_mode = STATE_GAME_OVER;
                break;
            }

            g_game_state.effects->update(FIXED_TIMESTEP);
            
            if (g_level3_transition_pending) {
              
                g_post_boss2_death_timer += FIXED_TIMESTEP;
                if (g_post_boss2_death_timer >= 3.1f) {
                    g_level3_transition_pending = false;
                    if (g_current_level != 3) {
                        setupLevel3();
                    }
                }
            }
            
            //boss interactions
            if (g_game_state.using_boss3) {
           
                if (g_game_state.boss3->is_active) {
                    g_game_state.boss3->set_target_position(g_game_state.player->position);
                    glm::vec3 original_boss_position = g_game_state.boss3->position;
                    g_game_state.boss3->update(FIXED_TIMESTEP);
                    
                    if (g_game_state.boss3->fire_effects_active) {
                        g_game_state.boss3->check_knight_fire_collision(g_game_state.player);
                    }
                    
                    //check for collision between knight and monsters
                    if (g_game_state.boss3->monsters_active) {
                        g_game_state.boss3->check_monster_knight_collision(g_game_state.player);
                    }

                    if (g_game_state.boss3->is_attack_active) {
                        bool knight_is_jumping = g_game_state.player->state == JUMPING ||  !g_game_state.player->is_grounded;

                        float dist_x = abs(g_game_state.boss3->position.x - g_game_state.player->position.x);
                        if (dist_x < g_game_state.boss3->attack_radius) {
                            if (g_game_state.boss3->is_smash_attack) {
                                // Smash attack
                                if (knight_is_jumping) {
                                    // Knight is jumping, smash attack misses
                                } else if (g_game_state.player->is_hurt) {
                                    // Knight is already in hurt animation - don't deal additional damage
                                } else {
                                    // Knight is on ground and not already hurt, smash attack hits EVEN IF knight is defending/invincible
                                    if (g_knight_hurt_sound != nullptr) {
                                        int channel = Mix_PlayChannel(-1, g_knight_hurt_sound, 0);
                                    }
                                    
                                    bool was_invincible = g_game_state.player->is_invincible;
                                    g_game_state.player->take_damage(1); 

                                    g_game_state.player->is_hurt = true;
                                    g_game_state.player->hurt_timer = 0.5f;
                                    g_game_state.player->set_animation(HURT);
                                    g_game_state.player->texture_id = knight_hurt_texture_id;
    
                                    g_game_state.player->is_invincible = was_invincible;
                                }
                            } else if (g_game_state.boss3->is_fire_breath_attack) {
                                // Fire breath attack: can be defended, cannot be dodged by jumping
                                if (g_game_state.player->is_hurt) {
                                    // Knight is already in hurt animation - don't deal additional damage
                                } else if (g_game_state.player->is_invincible || g_game_state.player->is_defending) {
                                    // Knight is defending, fire breath is blocked
                                    // No damage when defending
                                } else {
                                    // Knight is not defending, fire breath deals damage
                                    std::cout << "DEBUG: Knight hit by boss fire breath attack!" << std::endl;
                                    
                                    // Play hurt sound effect
                                    if (g_knight_hurt_sound != nullptr) {
                                        int channel = Mix_PlayChannel(-1, g_knight_hurt_sound, 0);
                                    }
                                    
                                    g_game_state.player->hurt();
                                }
                            } else {
                                // Cleave attack: can be defended, but can't be dodged by jumping
                                if (g_game_state.player->is_invincible) {
                                    // Knight is defending, cleave attack is blocked
                                    std::cout << "Knight blocked cleave attack with defense" << std::endl;
                                } else {
                                    // Knight is not defending, cleave attack hits EVEN IF knight is jumping
                                    std::cout << "Knight hit by boss cleave attack" << std::endl;
                                    
                                    // Play hurt sound effect
                                    if (g_knight_hurt_sound != nullptr) {
                                        int channel = Mix_PlayChannel(-1, g_knight_hurt_sound, 0);

                                    }
                                    
                                    g_game_state.player->hurt();
                                }
                            }
                        }
                    }
                    
                    //check if slash is colliding with boss
                    if (g_game_state.slash->is_active) {
                        static int last_hit_slash_id = -1;
    
                        //only check for collision if this slash hasn't already hit the boss
                        if (last_hit_slash_id != current_slash_id) {
                            float slash_left = g_game_state.slash->position.x - g_game_state.slash->width/2;
                            float slash_right = g_game_state.slash->position.x + g_game_state.slash->width/2;
                            float slash_top = g_game_state.slash->position.y + g_game_state.slash->height/2;
                            float slash_bottom = g_game_state.slash->position.y - g_game_state.slash->height/2;
                            
                            float boss_left = g_game_state.boss3->position.x - g_game_state.boss3->width/2;
                            float boss_right = g_game_state.boss3->position.x + g_game_state.boss3->width/2;
                            float boss_top = g_game_state.boss3->position.y + g_game_state.boss3->height/2;
                            float boss_bottom = g_game_state.boss3->position.y - g_game_state.boss3->height/2;
                            
                            bool collision = !(slash_bottom > boss_top || slash_top < boss_bottom || 
                                               slash_left > boss_right || slash_right < boss_left);
                            
                            if (collision) {
                                // Update boss health
                                g_game_state.boss3->take_damage(1);
                                
                                // Play slash hit sound effect
                                if (g_slash_hit_sound != nullptr) {
                                    int channel = Mix_PlayChannel(-1, g_slash_hit_sound, 0);
                                }
                                
                                // Mark this slash as having hit the boss
                                last_hit_slash_id = current_slash_id;
                            }
                        }
                    }
                    
                    //check if boss is dead
                    if (g_game_state.boss3->is_dead()) {
                        //play death animation
                        if (g_game_state.boss3->is_death_animation_done) {
                            std::cout << "Boss3 defeated" << std::endl;
                            g_game_state.boss3->is_active = false;
                            winner = true;
                            g_game_on = false;
                            g_current_mode = STATE_GAME_OVER; 
                            const Uint8* key_state = SDL_GetKeyboardState(NULL);
                            if (key_state[SDL_SCANCODE_Q]) {

                                //reset everything back to start menu
                                winner = false;
                                loser = false;
                                g_game_on = false;
                                g_current_mode = STATE_MENU;
                                g_current_level = 1;
                                if (g_game_state.start_menu != nullptr) {
                                    delete g_game_state.start_menu;
                                }
                                g_game_state.start_menu = new StartMenu();
                                g_game_state.start_menu->init(start_menu_background_id);
                                
                                //reset player and camera
                                g_game_state.player->reset_health();
                                g_game_state.player->position = glm::vec3(0.0f, 0.0f, 0.0f);
                                g_game_state.camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
                                if (g_game_state.boss3 != nullptr) g_game_state.boss3->is_active = false;
                                if (g_game_state.boss2 != nullptr) g_game_state.boss2->is_active = false;
                                if (g_game_state.boss != nullptr) g_game_state.boss->is_active = false;
                                
                                g_game_state.using_boss3 = false;
                                g_game_state.using_boss2 = false;
                                
                            }
                        }
                    }
                }
            } else if (g_game_state.using_boss2) {
                //Boss2 logic
                if (g_game_state.boss2->is_active) {
                    //set the player position as target for the boss
                    g_game_state.boss2->set_target_position(g_game_state.player->position);
                    glm::vec3 original_boss_position = g_game_state.boss2->position;
                    g_game_state.boss2->update(FIXED_TIMESTEP);
                    
                    //check if boss has hit the ground
                    float boss_left = g_game_state.boss2->position.x - (g_game_state.boss2->width * 0.5f);
                    float boss_right = g_game_state.boss2->position.x + (g_game_state.boss2->width * 0.5f);
                    float boss_top = g_game_state.boss2->position.y + (g_game_state.boss2->height * 0.5f);
                    float boss_bottom = g_game_state.boss2->position.y - (g_game_state.boss2->height * 0.5f);
                    
                    float boss_y_correction = g_game_state.map->check_point_y_collision(
                        g_game_state.boss2->position.x,
                        g_game_state.boss2->position.y - (g_game_state.boss2->height / 2.0f), 
                        original_boss_position.y - (g_game_state.boss2->height / 2.0f)
                    );
                    
                    if (boss_y_correction != -999.0f) {
                        g_game_state.boss2->position.y = boss_y_correction + (g_game_state.boss2->height / 2.0f); 
                        g_game_state.boss2->velocity.y = 0;
                    }
                    if (g_game_state.slash->is_active) {
                        static int last_hit_slash_id = -1;
                        if (last_hit_slash_id != current_slash_id) {
                        float slash_left = g_game_state.slash->position.x - g_game_state.slash->width/2;
                        float slash_right = g_game_state.slash->position.x + g_game_state.slash->width/2;
                        float slash_top = g_game_state.slash->position.y + g_game_state.slash->height/2;
                        float slash_bottom = g_game_state.slash->position.y - g_game_state.slash->height/2;
                        
                        bool collision = !(slash_bottom > boss_top || slash_top < boss_bottom || 
                                           slash_left > boss_right || slash_right < boss_left);
                        
                        if (collision) {
                            //update boss health
                            g_game_state.boss2->take_damage(1);
                                
                                //play slash hit sound effect
                                if (g_slash_hit_sound != nullptr) {
                                    int channel = Mix_PlayChannel(-1, g_slash_hit_sound, 0);
                                    if (channel == -1) {
                                        std::cout << "Failed to play slash sound" << Mix_GetError() << std::endl;
                                    } else {
                                        std::cout << "" << channel << std::endl;
                                    }
                                }
                                
     
                                last_hit_slash_id = current_slash_id;
                            }
                        }
                    }
                    
                    if (g_game_state.boss2->is_attack_active) {
                        bool knight_is_jumping = g_game_state.player->state == JUMPING || 
                                               !g_game_state.player->is_grounded;

                        if (g_game_state.boss2->is_smash_attack) {
                            //smash attack: cannot be defended, but can be dodged by jumping
                            if (knight_is_jumping) {
                                //knight is jumping, smash attack misses
                                std::cout << "dodged smash attack by jumping" << std::endl;
                            } else if (g_game_state.player->is_hurt) {
                                //knight is already in hurt animation - don't deal additional damage
                                std::cout << "already hurt, smash attack cannot deal additional damage" << std::endl;
                            } else {
                                //knight is on ground and not already hurt, smash attack hits EVEN IF knight is defending/invincible
                                std::cout << "hit by boss smash attack" << std::endl;
                                //hurt sound 
                                if (g_knight_hurt_sound != nullptr) {
                                    int channel = Mix_PlayChannel(-1, g_knight_hurt_sound, 0);
                                    if (channel == -1) {
                                        std::cout << "Failed to play hurt sound on smash hit: " << Mix_GetError() << std::endl;
                                    } else {
                                        std::cout << "Playing hurt sound on smash hit on channel " << channel << std::endl;
                                    }
                                }

                                bool was_invincible = g_game_state.player->is_invincible;
                                g_game_state.player->take_damage(1);  

                                g_game_state.player->is_hurt = true;
                                g_game_state.player->hurt_timer = 0.5f;
                                g_game_state.player->set_animation(HURT);
                                g_game_state.player->texture_id = knight_hurt_texture_id;
                                
                                //restore invincibility state after attack
                                g_game_state.player->is_invincible = was_invincible;
                            }
                        } else if (g_game_state.boss2->is_fire_breath_attack) {
                            //fire breath attack: can be defended, cannot be dodged by jumping
                            if (g_game_state.player->is_hurt) {
                                //knight is already in hurt animation - don't deal additional damage
                                std::cout << "already hurt, fire breath cannot deal additional damage!" << std::endl;
                            } else if (g_game_state.player->is_invincible || g_game_state.player->is_defending) {
                                //knight is defending, fire breath is blocked
                                std::cout << "blocked fire breath attack with defense!" << std::endl;
                                //no damage when defending
                            } else {
                                //knight is not defending, fire breath deals damage
                                std::cout << "hit by boss fire breath attack!" << std::endl;

                                if (g_knight_hurt_sound != nullptr) {
                                    int channel = Mix_PlayChannel(-1, g_knight_hurt_sound, 0);
                                    if (channel == -1) {
                                        std::cout << "Failed to play hurt sound on fire breath hit: " << Mix_GetError() << std::endl;
                                    } else {
                                        std::cout << "" << channel << std::endl;
                                    }
                                }
                                
                                g_game_state.player->hurt();
                            }
                        } else {
                            //cleave attack: can be defended, but can't be dodged by jumping
                            if (g_game_state.player->is_invincible) {
                                //knight is defending, cleave attack is blocked
                                std::cout << "blocked cleave attack with defense!" << std::endl;
                            } else {
                                //knight is not defending, cleave attack hits EVEN IF knight is jumping
                                std::cout << "hit by boss cleave attack!" << std::endl;
                                if (g_knight_hurt_sound != nullptr) {
                                    int channel = Mix_PlayChannel(-1, g_knight_hurt_sound, 0);
                                    if (channel == -1) {
                                        std::cout << "Failed to play hurt sound on cleave hit: " << Mix_GetError() << std::endl;
                                    } else {
                                        std::cout << "" << channel << std::endl;
                                    }
                                }
                                
                                g_game_state.player->hurt();
                            }
                        }
                    }
                    
                    //check if boss is dead
                    if (g_game_state.boss2->is_dead()) {
                        // If the boss is dead, let the death animation play out
                        if (g_game_state.boss2->is_death_animation_done) {
                            g_game_state.boss2->is_active = false;
                            
                            //only trigger camera shake once when boss is first detected as dead
                            if (!g_boss2_death_triggered) {
                                //start horizontal camera shake for 3 seconds with intensity 0.5
                                g_game_state.effects->start(SHAKE_H, 3.0f, 0.5f);
                                g_boss2_death_triggered = true;
                                g_level3_transition_pending = true; 
                                g_post_boss2_death_timer = 0.0f; 
                            }
                        }
                    }
                }
            } else {
                if (g_game_state.boss->is_active) {
                    bool was_hurt = g_game_state.player->check_projectile_collision(g_game_state.boss->projectiles);
                    if (was_hurt) {
   
                        if (g_knight_hurt_sound != nullptr) {
                            Mix_PlayChannel(-1, g_knight_hurt_sound, 0);
                        }
                    }

                    g_game_state.boss->target_position = g_game_state.player->position;
                    glm::vec3 original_boss_position = g_game_state.boss->position;
                    g_game_state.boss->update(FIXED_TIMESTEP);
                    float boss_left = g_game_state.boss->position.x - (g_game_state.boss->width * 0.5f);
                    float boss_right = g_game_state.boss->position.x + (g_game_state.boss->width * 0.5f);
                    float boss_top = g_game_state.boss->position.y + (g_game_state.boss->height * 0.5f);
                    float boss_bottom = g_game_state.boss->position.y - (g_game_state.boss->height * 0.5f);
                    
                    float boss_y_correction = g_game_state.map->check_point_y_collision(
                        g_game_state.boss->position.x,
                        g_game_state.boss->position.y - (g_game_state.boss->height / 2.0f), 
                        original_boss_position.y - (g_game_state.boss->height / 2.0f)
                    );
                    
                    if (boss_y_correction != -999.0f) {
                        g_game_state.boss->position.y = boss_y_correction + (g_game_state.boss->height / 2.0f); 
                        g_game_state.boss->velocity.y = 0;
                    }
                    
                    //boss defeat
                    if (g_game_state.boss->health <= 0) {
                        g_game_state.boss->is_active = false;
                        g_current_level = 2;
                        setupLevel2();
                    }
                }
            }
            
            //set state and texture based on movement and attack state
            if (g_game_state.player->is_hurt) {
                g_game_state.player->texture_id = knight_hurt_texture_id;
            }
            else if (g_game_state.player->is_defending) {
                g_game_state.player->texture_id = knight_defend_texture_id;
            }
            else if (g_game_state.player->is_attacking) {
                g_game_state.player->texture_id = knight_attack_texture_id;
            }
            else if (g_game_state.player->state == JUMPING || !g_game_state.player->is_grounded) {
                g_game_state.player->texture_id = knight_jump_texture_id;
                if (g_game_state.player->state != JUMPING && g_game_state.player->state != FALLING) {
                    if (g_game_state.player->velocity.y > 0) {
                        g_game_state.player->set_animation(JUMPING);
                    } else {
                        g_game_state.player->set_animation(FALLING);
                    }
                }
            }
            else if (g_game_state.player->move_left || g_game_state.player->move_right) {
                g_game_state.player->texture_id = knight_run_texture_id;
                g_game_state.player->set_animation(RUNNING);
            }
            else {
                g_game_state.player->texture_id = knight_idle_texture_id;
                g_game_state.player->set_animation(IDLE);
            }
            
            glm::vec3 original_position = g_game_state.player->position;
            g_game_state.player->update(FIXED_TIMESTEP);
            
            if (g_game_state.boss->is_active) {
                bool was_hurt = g_game_state.player->check_projectile_collision(g_game_state.boss->projectiles);
                if (was_hurt) {
                    if (g_knight_hurt_sound != nullptr) {
                        int channel = Mix_PlayChannel(-1, g_knight_hurt_sound, 0);
                        if (channel == -1) {
                            std::cout << "Failed to play hurt sound on projectile hit " << Mix_GetError() << std::endl;
                        } else {
                            std::cout << "" << channel << std::endl;
                        }
                    }
                }
            }
            
            //check for wall collisions after horizontal movement
            float knight_left = g_game_state.player->position.x - (g_game_state.player->width * 0.45f);
            float knight_right = g_game_state.player->position.x + (g_game_state.player->width * 0.45f);
            float knight_top = g_game_state.player->position.y + (g_game_state.player->height * 0.5f);
            float knight_bottom = g_game_state.player->position.y - (g_game_state.player->height * 0.5f);
            
            bool wall_collision = false;
            //knight wall collision
            for (float check_y = knight_bottom + 0.2f; check_y <= knight_top - 0.2f; check_y += 0.4f) {
                if (g_game_state.map->is_solid(knight_left, check_y)) {
                    wall_collision = true;
                    g_game_state.player->position.x = original_position.x + 0.05f; 
                    g_game_state.player->velocity.x = 0;
                    break;
                }
                if (g_game_state.map->is_solid(knight_right, check_y)) {
                    wall_collision = true;
                    g_game_state.player->position.x = original_position.x - 0.05f;
                    g_game_state.player->velocity.x = 0;
                    break;
                }
            }
            
            //platform collision
            float y_correction = 0.0f;
            if (g_game_state.map->check_platform_collision(
                g_game_state.player->position.x, 
                g_game_state.player->position.y - (g_game_state.player->height / 2.0f),
                &y_correction)) {
                
                // Player has landed on a platform
                g_game_state.player->position.y = y_correction + (g_game_state.player->height / 2.0f); 
                g_game_state.player->velocity.y = 0;
                g_game_state.player->is_grounded = true;
                
                // If was jumping or falling, switch to idle
                if (g_game_state.player->state == JUMPING || g_game_state.player->state == FALLING) {
                    g_game_state.player->is_jumping = false;
                    g_game_state.player->set_animation(IDLE);
                }
            } else {
    
                g_game_state.player->is_grounded = false;
            }
            
            float map_left_bound = LEFT_BOUND * 2;  
            float map_right_bound = (LEVEL1_WIDTH * 2) - (RIGHT_BOUND * 2);  
            
            if (g_game_state.player->position.x < map_left_bound) {
                g_game_state.camera_position.x = map_left_bound;
            } 
            else if (g_game_state.player->position.x > map_right_bound) {
                g_game_state.camera_position.x = map_right_bound;
            }
            else {
                g_game_state.camera_position.x = g_game_state.player->position.x;
            }
            
            //camera follow
            g_game_state.camera_position.y = g_game_state.player->position.y + 3.0f;
            
            //shake
            g_game_state.camera_position += g_game_state.effects->m_view_offset;
            
            //update view matrix
            g_view_matrix = glm::mat4(1.0f);
            g_view_matrix = glm::translate(g_view_matrix, -g_game_state.camera_position);
            g_shader_program.set_view_matrix(g_view_matrix);
            
            //slash effect
            g_game_state.slash->update(FIXED_TIMESTEP);
            
            // Check for slash activation and deactivation
            static bool was_slash_active = false;
            
            //new slash
            if (g_game_state.slash->is_active && !was_slash_active) {
                current_slash_id++; 
            }
            
            //track
            was_slash_active = g_game_state.slash->is_active;
            
            //slash collision with boss
            if (g_game_state.slash->is_active && g_game_state.boss->is_active) {
                //only apply damage if this slash hasn't damaged the boss yet
                if (last_damaging_slash_id != current_slash_id) {
                    if (check_slash_boss_collision(g_game_state.slash, g_game_state.boss, g_game_state.player)) {
                        
                        //directly modify the boss health
                        g_game_state.boss->health -= 1;
                        
                        // play slash hit sound effect
                        if (g_slash_hit_sound != nullptr) {
                            int channel = Mix_PlayChannel(-1, g_slash_hit_sound, 0);
                            if (channel == -1) {
                                std::cout << "Failed to play slash sound on boss1 hit: " << Mix_GetError() << std::endl;
                            } else {
                                std::cout << "" << channel << std::endl;
                            }
                        }
                        last_damaging_slash_id = current_slash_id;
                    }
                }
            }
        }
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_time_accumulator = delta_time;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (g_current_mode == STATE_MENU) {
        if (g_game_state.start_menu == nullptr) {
            g_game_state.start_menu = new StartMenu();
            g_game_state.start_menu->init(start_menu_background_id);
        }
        
        g_game_state.start_menu->render(&g_shader_program, g_font_texture_id);
        
        SDL_GL_SwapWindow(g_display_window);
        return;
    }
    
    //render both game and effect
    if (g_current_mode == STATE_TRANSITION) {

        glm::mat4 bg_model_matrix = glm::mat4(1.0f);
        //for camera speed
        float parallax_factor = 0.3f; 
        glm::vec3 bg_position = g_game_state.camera_position * parallax_factor;

        glm::mat4 original_view_matrix = g_view_matrix;
        
        // Create a special view matrix for the background with reduced movement
        glm::mat4 bg_view_matrix = glm::mat4(1.0f);
        bg_view_matrix = glm::translate(bg_view_matrix, -bg_position);
        g_shader_program.set_view_matrix(bg_view_matrix);
        
        // Scale background to cover more than the screen
        float bg_width = 100.0f;  
        float bg_height = 20.0f;  
        bg_model_matrix = glm::scale(bg_model_matrix, glm::vec3(bg_width, bg_height, 1.0f));
        
        g_shader_program.set_model_matrix(bg_model_matrix);
        
        // Draw background
        float bg_vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
        };
        
        static int last_level = 0;
        if (last_level != g_current_level) {
            last_level = g_current_level;
        }
        
        if (g_current_level == 3) {

            if (level3_background_texture_id == 0) {
            glBindTexture(GL_TEXTURE_2D, background_texture_id);
            } else {
                //dawn for level 3
                glBindTexture(GL_TEXTURE_2D, level3_background_texture_id); 
              
            }

            static bool first_level3_render = true;
            if (first_level3_render) {
                first_level3_render = false;
            }
        } else {
            //default background 
            glBindTexture(GL_TEXTURE_2D, background_texture_id); 
        }

        glVertexAttribPointer(g_shader_program.get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), bg_vertices);
        glEnableVertexAttribArray(g_shader_program.get_position_attribute());
        
        glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &bg_vertices[3]);
        glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Restore original view matrix for the rest of rendering
        g_shader_program.set_view_matrix(original_view_matrix);
        
        // Render map
        g_game_state.map->render(&g_shader_program);
        
        // Render player
        g_game_state.player->render(&g_shader_program);
        
        // Render slash effect if active
        g_game_state.slash->render(&g_shader_program);
        
        // Render Boss
        if (g_game_state.using_boss3 && g_game_state.boss3 != nullptr && g_game_state.boss3->is_active) {
            g_game_state.boss3->render(&g_shader_program);
        } else if (g_game_state.using_boss2 && g_game_state.boss2->is_active) {
            g_game_state.boss2->render(&g_shader_program);
        } else if (g_game_state.boss->is_active) {
            g_game_state.boss->render(&g_shader_program);
        }
        
        // Disable attribute arrays before rendering the effect
        glDisableVertexAttribArray(g_shader_program.get_position_attribute());
        glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
        
        // Then render the fade effect using the effects system
        g_game_state.effects->render();
        
        SDL_GL_SwapWindow(g_display_window);
        return;
    }
    
    // Render background with parallax effect
    glm::mat4 bg_model_matrix = glm::mat4(1.0f);
    // Apply a parallax effect by moving the background at a fraction of the camera speed
    float parallax_factor = 0.3f; 
    glm::vec3 bg_position = g_game_state.camera_position * parallax_factor;
    
    // Save current view matrix
    glm::mat4 original_view_matrix = g_view_matrix;
    
    // Create a special view matrix for the background with reduced movement
    glm::mat4 bg_view_matrix = glm::mat4(1.0f);
    bg_view_matrix = glm::translate(bg_view_matrix, -bg_position);
    g_shader_program.set_view_matrix(bg_view_matrix);
    
    // Scale background to cover more than the screen
    float bg_width = 100.0f;  // Make it wide enough for the extended level
    float bg_height = 20.0f;  // Make it tall enough
    bg_model_matrix = glm::scale(bg_model_matrix, glm::vec3(bg_width, bg_height, 1.0f));
    
    g_shader_program.set_model_matrix(bg_model_matrix);
    
    // Draw background
    float bg_vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
    };
    
    // Select the appropriate background texture based on the current level
    static int last_level = 0;
    if (last_level != g_current_level) {
       
        last_level = g_current_level;
    }
    
    if (g_current_level == 3) {
        
        if (level3_background_texture_id == 0) {

            glBindTexture(GL_TEXTURE_2D, background_texture_id);
           
        } else {
            glBindTexture(GL_TEXTURE_2D, level3_background_texture_id); 
        }

        static bool first_level3_render = true;
        if (first_level3_render) {
           
            first_level3_render = false;
        }
    } else {
        glBindTexture(GL_TEXTURE_2D, background_texture_id); 
    }
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 3, GL_FLOAT, false, 5 * sizeof(float), bg_vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 5 * sizeof(float), &bg_vertices[3]);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    g_shader_program.set_view_matrix(original_view_matrix);
    g_game_state.map->render(&g_shader_program);
    g_game_state.player->render(&g_shader_program);
    g_game_state.slash->render(&g_shader_program);
    if (g_game_state.using_boss3 && g_game_state.boss3 != nullptr && g_game_state.boss3->is_active) {
        g_game_state.boss3->render(&g_shader_program);
    } else if (g_game_state.using_boss2 && g_game_state.boss2->is_active) {
        g_game_state.boss2->render(&g_shader_program);
    } else if (g_game_state.boss->is_active) {
        g_game_state.boss->render(&g_shader_program);
    }
    //UI
    if (g_current_mode == STATE_GAME) {
    
        glm::mat4 original_view_matrix = g_view_matrix;
        g_shader_program.set_view_matrix(glm::mat4(1.0f));
        //knight's current health
        int current_health = g_game_state.player->health;
        if (current_health < 0) current_health = 0;
        if (current_health > g_game_state.player->max_health) current_health = g_game_state.player->max_health;
        int health_texture_index = current_health - 1;
        if (health_texture_index < 0) health_texture_index = 0;
        
        //model for health bar
        glm::mat4 health_model_matrix = glm::mat4(1.0f);
        health_model_matrix = glm::translate(health_model_matrix, glm::vec3(-9.5f, 8.0f, 0.0f));
        health_model_matrix = glm::scale(health_model_matrix, glm::vec3(4.0f, 1.0f, 1.0f)); 
        
        //render health bar
        g_shader_program.set_model_matrix(health_model_matrix);
        
        float health_tex_coords[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f
        };

        float health_vertices[] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f
        };
        
        glBindTexture(GL_TEXTURE_2D, g_health_bar_textures[health_texture_index]);
        glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, health_vertices);
        glEnableVertexAttribArray(g_shader_program.get_position_attribute());
        
        glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, health_tex_coords);
        glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        glDisableVertexAttribArray(g_shader_program.get_position_attribute());
        glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
        
        //debug
        if (g_game_state.using_boss3 && g_game_state.boss3 != nullptr && g_game_state.boss3->is_active) {
            std::string boss_health_text = "BOSS3 HEALTH: " + std::to_string(g_game_state.boss3->health) + 
                                        "/" + std::to_string(g_game_state.boss3->max_health);
            
            Utility::draw_text(&g_shader_program, g_font_texture_id, boss_health_text, 
                              0.4f, 0.0f, glm::vec3(0.0f, 8.0f, 0.0f));
        }
        else if (g_game_state.using_boss2 && g_game_state.boss2->is_active) {
            std::string boss_health_text = "BOSS2 HEALTH: " + std::to_string(g_game_state.boss2->health) + 
                                        "/" + std::to_string(g_game_state.boss2->max_health);
            
            //health
            Utility::draw_text(&g_shader_program, g_font_texture_id, boss_health_text, 
                              0.4f, 0.0f, glm::vec3(0.0f, 8.0f, 0.0f));
        }
        else if (g_game_state.boss->is_active) {
            std::string boss_health_text = "BOSS HEALTH: " + std::to_string(g_game_state.boss->health) + 
                                        "/" + std::to_string(g_game_state.boss->max_health);
            Utility::draw_text(&g_shader_program, g_font_texture_id, boss_health_text, 
                              0.4f, 0.0f, glm::vec3(0.0f, 8.0f, 0.0f));
        }
        
        g_shader_program.set_view_matrix(original_view_matrix);
    }
    
    if (g_game_on) {
    } else if (winner) {
        //win message
        glm::mat4 original_view_matrix = g_view_matrix;
        g_shader_program.set_view_matrix(glm::mat4(1.0f));
        g_shader_program.set_colour(1.0f, 0.84f, 0.0f, 1.0f); 
        Utility::draw_text(&g_shader_program, g_font_texture_id, "boss defeated", 0.8f, -0.4f, glm::vec3(0.0f, 3.0f, 0.0f));
        g_shader_program.set_colour(1.0f, 1.0f, 1.0f, 1.0f); // Reset to white
        Utility::draw_text(&g_shader_program, g_font_texture_id, "Press Q to return to menu", 0.4f, -0.2f, glm::vec3(0.0f, 1.5f, 0.0f));
        
        // Restore the original view matrix
        g_shader_program.set_view_matrix(original_view_matrix);
    } else if (loser || g_current_mode == STATE_GAME_OVER) {
        // Save the current view matrix for game over text (keeping it in screen space)
        glm::mat4 original_view_matrix = g_view_matrix;
        
        // Set the view matrix to identity to ensure text appears in screen space
        g_shader_program.set_view_matrix(glm::mat4(1.0f));
        
        // Draw game over text centered on screen
        Utility::draw_text(&g_shader_program, g_font_texture_id, "Game Over", 1.0f, -0.5f, glm::vec3(0.0f, 3.0f, 0.0f));
        
        // Draw restart instruction
        Utility::draw_text(&g_shader_program, g_font_texture_id, "Press Q to restart", 0.5f, -0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Restore the original view matrix
        g_shader_program.set_view_matrix(original_view_matrix);
    }
    
    // Disable vertex attribute arrays
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() {
    delete g_game_state.player;
    delete g_game_state.map;
    delete g_game_state.slash;
    delete g_game_state.boss;
    if (g_game_state.boss2 != nullptr) {
        delete g_game_state.boss2;
    }
    if (g_game_state.boss3 != nullptr) {
        delete g_game_state.boss3;
    }
    delete g_game_state.start_menu;
    delete g_game_state.effects;
    
    Mix_HaltChannel(-1);
    

    if (g_game_music != nullptr) {
        Mix_FreeMusic(g_game_music);
        g_game_music = nullptr;
    }

    if (g_knight_hurt_sound != nullptr) {
        Mix_FreeChunk(g_knight_hurt_sound);
        g_knight_hurt_sound = nullptr;
    }
    
    if (g_slash_hit_sound != nullptr) {
        Mix_FreeChunk(g_slash_hit_sound);
        g_slash_hit_sound = nullptr;
    }
    
    if (g_defend_sound != nullptr) {
        Mix_FreeChunk(g_defend_sound);
        g_defend_sound = nullptr;
    }
    
    if (g_slash_attack_sound != nullptr) {
        Mix_FreeChunk(g_slash_attack_sound);
        g_slash_attack_sound = nullptr;
    }
    
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void setupLevel2() {
    //resets
    g_game_state.player->position = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.player->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.player->reset_health();
    g_game_state.player->set_animation(IDLE);
    g_game_state.player->texture_id = knight_idle_texture_id;
    g_game_state.player->set_direction("right");
    
    g_game_state.camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
    
    if (g_game_state.boss != nullptr) {
        g_game_state.boss->is_active = false;
    }

    if (g_game_state.boss2 != nullptr) {
        delete g_game_state.boss2; 
    }
    
    g_game_state.boss2 = new Boss2();
    g_game_state.using_boss2 = true;
    
    g_game_state.boss2->set_map(g_game_state.map);
    g_game_state.boss2->width = 2.0f;
    g_game_state.boss2->height = 3.0f;
    g_game_state.boss2->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.boss2->acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
    
    g_game_state.boss2->position = glm::vec3(5.0f, 1.0f, 0.0f);

    float platform_y = -999.0f;
    for (float y = 0.0f; y > -5.0f; y -= 0.1f) {
        if (g_game_state.map->is_solid(g_game_state.boss2->position.x, y)) {
            platform_y = y + 0.1f; 
            break;
        }
    }
    
    if (platform_y != -999.0f) {
        g_game_state.boss2->position.y = platform_y + g_game_state.boss2->height/2 + 0.5f; 
    }
    
    g_game_state.boss2->visual_scale_x = 18.0f;
    g_game_state.boss2->visual_scale_y = 9.0f;
    
    g_game_state.boss2->is_active = true;
    //load texture
    for (int i = 1; i <= 32; i++) {
        std::string filepath = "assets/boss/individual sprites/04_transform/transform_" + std::to_string(i) + ".png";
        g_game_state.boss2->transform_textures[i-1] = Utility::load_texture(filepath.c_str());
    }
    
    for (int i = 1; i <= 6; i++) {
        std::string filepath = "assets/boss/individual sprites/05_demon_idle/demon_idle_" + std::to_string(i) + ".png";
        g_game_state.boss2->demon_idle_textures[i-1] = Utility::load_texture(filepath.c_str());
    }
    
    //start transformation
    g_game_state.boss2->transform();
    
    //reset game state
    g_game_on = true;
    winner = false;
    loser = false;
    
    std::cout << "level 2 setup complete with transform boss at position: (" 
        << g_game_state.boss2->position.x << ", " 
        << g_game_state.boss2->position.y << ")" << std::endl;
}

void setupLevel3() {
    
    g_game_state.player->position = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.player->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.player->reset_health();
    g_game_state.player->set_animation(IDLE);
    g_game_state.player->texture_id = knight_idle_texture_id;
    g_game_state.player->set_direction("right");
    
    //reset camera position
    g_game_state.camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
    if (g_game_state.boss2 != nullptr) {
        g_game_state.boss2->is_active = false;
    }
    
    int old_level = g_current_level;
    g_current_level = 3;
    

    level3_background_texture_id = Utility::load_texture(LEVEL3_BACKGROUND_FILEPATH);
    
    
    //create and setup Boss3
    if (g_game_state.boss3 != nullptr) {
        delete g_game_state.boss3; 
    }
    
    g_game_state.boss3 = new Boss3();
    g_game_state.using_boss3 = true;
    
    //hitbox and physics
    g_game_state.boss3->set_map(g_game_state.map);
    g_game_state.boss3->width = 2.0f;
    g_game_state.boss3->height = 3.0f;
    g_game_state.boss3->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.boss3->acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
    
    g_game_state.boss3->position = glm::vec3(5.0f, 1.0f, 0.0f);
    //find platform
    float platform_y = -999.0f;
    for (float y = 0.0f; y > -5.0f; y -= 0.1f) {
        if (g_game_state.map->is_solid(g_game_state.boss3->position.x, y)) {
            platform_y = y + 0.1f; 
            break;
        }
    }
    
    //set final boss position
    if (platform_y != -999.0f) {
        g_game_state.boss3->position.y = platform_y + g_game_state.boss3->height/2 + 0.5f; 
        g_game_state.boss3->position.y += 0.5f; 
    }
    
    //visual scale
    g_game_state.boss3->visual_scale_x = 18.0f;
    g_game_state.boss3->visual_scale_y = 9.0f;
    
    g_game_state.boss3->is_active = true;
    
    g_game_state.boss3->acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
    
    //load cast spell textures
    for (int i = 1; i <= 6; i++) {
        std::string filepath = "assets/boss/individual sprites/10_demon_cast_spell/demon_cast_spell_" + std::to_string(i) + ".png";
        g_game_state.boss3->cast_spell_textures[i-1] = Utility::load_texture(filepath.c_str());

    }
    
    //load demon idle textures
    for (int i = 1; i <= 6; i++) {
        std::string filepath = "assets/boss/individual sprites/05_demon_idle/demon_idle_" + std::to_string(i) + ".png";
        g_game_state.boss3->demon_idle_textures[i-1] = Utility::load_texture(filepath.c_str());

    }
    
    //load demon walk textures
    for (int i = 1; i <= 6; i++) {
        std::string filepath = "assets/boss/individual sprites/06_demon_walk/demon_walk_" + std::to_string(i) + ".png";
        g_game_state.boss3->demon_walk_textures[i-1] = Utility::load_texture(filepath.c_str());
       
    }
    
    //load demon cleave attack textures
    for (int i = 1; i <= 15; i++) {
        std::string filepath = "assets/boss/individual sprites/07_demon_cleave/demon_cleave_" + std::to_string(i) + ".png";
        g_game_state.boss3->demon_cleave_textures[i-1] = Utility::load_texture(filepath.c_str());
    }
    
    //load demon fake move smash attack textures
    for (int i = 1; i <= 27; i++) {
        std::string filepath = "assets/boss/individual sprites/fakemove/demon_";
        //for frames 1-9, use cleave images
        if (i <= 9) {
            filepath += "cleave_" + std::to_string(i) + ".png";
        } 
        //for frames 10-27, use smash images (1-18)
        else {
            filepath += "smash_" + std::to_string(i - 9) + ".png";
        }
        g_game_state.boss3->demon_smash_textures[i-1] = Utility::load_texture(filepath.c_str());
        std::cout << "Loaded fakemove texture " << i << ": " << g_game_state.boss3->demon_smash_textures[i-1] << std::endl;
    }
    
    //load demon fire breath attack textures
    for (int i = 1; i <= 21; i++) {
        std::string filepath = "assets/boss/individual sprites/09_demon_fire_breath/demon_fire_breath_" + std::to_string(i) + ".png";
        g_game_state.boss3->demon_fire_breath_textures[i-1] = Utility::load_texture(filepath.c_str());
        std::cout << "Loaded demon fire breath texture " << i << ": " << g_game_state.boss3->demon_fire_breath_textures[i-1] << std::endl;
    }
    
    //load demon death animation textures
    for (int i = 1; i <= 22; i++) {
        std::string filepath = "assets/boss/individual sprites/12_demon_death/demon_death_" + std::to_string(i) + ".png";
        g_game_state.boss3->demon_death_textures[i-1] = Utility::load_texture(filepath.c_str());
        std::cout << "Loaded demon death texture " << i << ": " << g_game_state.boss3->demon_death_textures[i-1] << std::endl;
    }

    g_game_state.boss3->set_idle_before_cast();
    g_level3_transition_pending = false;
    g_boss2_death_triggered = false;
    //reset gamestate
    g_game_on = true;

}

// Add function to setup level 1 (initial level)
void setupLevel1() {

    //reset player position and health
    g_game_state.player->position = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.player->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    g_game_state.player->reset_health();
    g_game_state.player->set_animation(IDLE);
    g_game_state.player->texture_id = knight_idle_texture_id;
    g_game_state.player->set_direction("right");

    g_game_state.camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
    
    //set current level to 1
    g_current_level = 1;
    
    if (g_game_state.boss2 != nullptr) {
        g_game_state.boss2->is_active = false;
    }
    if (g_game_state.boss3 != nullptr) {
        g_game_state.boss3->is_active = false;
    }
    
    //ensure Boss1 is active
    g_game_state.using_boss2 = false;
    g_game_state.using_boss3 = false;
    
    // Position Boss1
    if (g_game_state.boss != nullptr) {
        g_game_state.boss->position = glm::vec3(10.0f, 1.0f, 0.0f);
        g_game_state.boss->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        g_game_state.boss->reset_health();
        g_game_state.boss->is_active = true;
        
        //ensure Boss1 has its textures loaded, do reloads
        if (g_game_state.boss->idle_textures[0] == 0) {
            for (int i = 1; i <= 6; i++) {
                std::string path = "assets/boss/individual sprites/01_idle/idle_" + std::to_string(i) + ".png";
                g_game_state.boss->idle_textures[i-1] = Utility::load_texture(path.c_str());
            }
            
            for (int i = 1; i <= 6; i++) {
                std::string path = "assets/boss/individual sprites/02_move/move_" + std::to_string(i) + ".png";
                g_game_state.boss->move_textures[i-1] = Utility::load_texture(path.c_str());
            }
            
            for (int i = 1; i <= 6; i++) {
                std::string path = "assets/boss/individual sprites/03_flame/flame_" + std::to_string(i) + ".png";
                g_game_state.boss->flame_textures[i-1] = Utility::load_texture(path.c_str());
            }
        }
    } else {

        g_game_state.boss = new Boss();
        g_game_state.boss->position = glm::vec3(10.0f, 1.0f, 0.0f);
        g_game_state.boss->is_active = true;

        for (int i = 1; i <= 6; i++) {
            std::string path = "assets/boss/individual sprites/01_idle/idle_" + std::to_string(i) + ".png";
            g_game_state.boss->idle_textures[i-1] = Utility::load_texture(path.c_str());
        }
        
        for (int i = 1; i <= 6; i++) {
            std::string path = "assets/boss/individual sprites/02_move/move_" + std::to_string(i) + ".png";
            g_game_state.boss->move_textures[i-1] = Utility::load_texture(path.c_str());
        }
        
        for (int i = 1; i <= 6; i++) {
            std::string path = "assets/boss/individual sprites/03_flame/flame_" + std::to_string(i) + ".png";
            g_game_state.boss->flame_textures[i-1] = Utility::load_texture(path.c_str());
        }
    }
    
    //reset game state
    g_game_on = true;
    winner = false;
    loser = false;

}

int main(int argc, char* argv[]) {
    initialise();

    while (g_game_is_running) {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}

