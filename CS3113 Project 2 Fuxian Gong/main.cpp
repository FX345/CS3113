/**
* Author: Fuxian Gong
* Assignment: Pong
* Date due: 2025-03-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'


#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>

enum AppStatus { RUNNING, TERMINATED };

// Our window dimensions
constexpr float WINDOW_SIZE_MULT = 2.0f;

constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

// Background color components
constexpr float BG_RED = 1.0f,
BG_BLUE = 1.0f,
BG_GREEN = 1.0f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
//textures
constexpr char PADDLE1_SPRITE_FILEPATH[] = "paddle1.png",
PADDLE2_SPRITE_FILEPATH[] = "paddle2.png",
BALL_SPRITE_FILEPATH[] = "ball.png",
LEFTWINS_SPRITE_FILEPATH[] = "leftWins.png",
RIGHTWINS_SPRITE_FILEPATH[] = "rightWins.png";

//necessary parameters
constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_SCALE_PADDLES = glm::vec3(0.8f, 1.6f, 0.0f),
INIT_POS_PAD1 = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_POS_PAD2 = glm::vec3(0.0f, 0.0f, 0.0f),

INIT_POS_BALL = glm::vec3(0.0f, 0.0f, 0.0f),
INIT_SCALE_BALL = glm::vec3(0.5f, 0.5f, 0.0f);

constexpr glm::vec3 INIT_SCALE_VICTORY = glm::vec3(5.0f, 3.0f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_paddle1_matrix, g_projection_matrix, g_paddle2_matrix, g_ball_matrix[3];
glm::mat4 g_victory_matrix;




GLuint g_paddle1_id;
GLuint g_paddle2_id;
GLuint g_ball_id;
GLuint g_leftWins_id;
GLuint g_rightWins_id;

constexpr float PADDLE_SPEED = 4.0f; 
constexpr float BALL_SPEED = 5.0f; 


float g_previous_ticks = 0.0f;
glm::vec3 g_paddle1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_paddle1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_paddle2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_paddle2_movement = glm::vec3(0.0f, 0.0f, 0.0f);


glm::vec3 g_ball_position[3] = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) };
glm::vec3 g_ball_movement[3] = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) };




int g_number_of_balls = 1; 

bool linkStart = true;
bool singlePlayer = false;
bool moveUp = true;
bool player1Wins = false;
bool player2Wins = false;


void endGame();
void initialise();
void process_input();
void update();
void render();
void shutdown();

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

    LOG("Loading texture: "<< filepath);
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




void initialise()
{
    // HARD INITIALISE 
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Pong",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    if (g_display_window == nullptr)
    {
        std::cerr << "ERROR: SDL Window could not be created.\n";
        g_app_status = TERMINATED;

        SDL_Quit();
        exit(1);
    }

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif



    // SOFT INITIALISE
    // Initialise our camera
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    //initialize the necessary matrices 
    g_paddle2_matrix = glm::mat4(1.0f);
    g_paddle1_matrix = glm::mat4(1.0f);
    for (int i = 0; i < 3; i++) {
        g_ball_matrix[i] = glm::mat4(1.0f);
    }
    
    g_victory_matrix = glm::mat4(1.0f);
    g_victory_matrix = glm::translate(g_victory_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    g_victory_matrix = glm::scale(g_victory_matrix, INIT_SCALE_VICTORY); 
    

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    //load all textures
    g_paddle1_id = load_texture(PADDLE1_SPRITE_FILEPATH);
    g_paddle2_id = load_texture(PADDLE2_SPRITE_FILEPATH);
    g_ball_id = load_texture(BALL_SPRITE_FILEPATH);
    g_leftWins_id = load_texture(LEFTWINS_SPRITE_FILEPATH);
    g_rightWins_id = load_texture(RIGHTWINS_SPRITE_FILEPATH);
    


    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
    
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        //switch mode by detecting key pressed
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_t:
                singlePlayer = true;
                break;
            case SDLK_1: g_number_of_balls = 1;
                break; 
            case SDLK_2: g_number_of_balls = 2;
                break; 
            case SDLK_3: g_number_of_balls = 3;
                break; 
            default: break;
            }
        default:
            break;
        }
    }

    //detect input key when game starts
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (linkStart== true) {
        if (key_state[SDL_SCANCODE_W])
        {
            g_paddle1_movement.y = 1.0f;

        }
        else if (key_state[SDL_SCANCODE_S])
        {
            g_paddle1_movement.y = -1.0f;

        }
        else {
            g_paddle1_movement.y = 0.0f;
        }
        if (singlePlayer == false) {
            if (key_state[SDL_SCANCODE_UP])
            {
                g_paddle2_movement.y = 1.0f;
            }
            else if (key_state[SDL_SCANCODE_DOWN])
            {
                g_paddle2_movement.y = -1.0f;
            }
            else {
                g_paddle2_movement.y = 0.0f;
            }
        }
        else {
            // singleplayer mode, the right side moves automatically
            if (g_paddle2_position.y >= 3.2f || g_paddle2_position.y <= -3.2f) {
                moveUp = !moveUp;
            }
            if (moveUp == true) {
                g_paddle2_movement.y = 1.0f;
            }
            else {
                g_paddle2_movement.y = -1.0f;
            }
        }
    }
}
//collision logic, measures horizontal and vertical distance and change direction
bool collision(glm::vec3 x_coord_a, glm::vec3 x_coord_b, glm::vec3 width_a, glm::vec3 width_b) {
    float x_distance = fabs(x_coord_a.x - x_coord_b.x) - ((width_a.x + width_b.x) / 2.0f);
    float y_distance = fabs(x_coord_a.y - x_coord_b.y) - ((width_a.y + width_b.y) / 2.0f);

    if (x_distance < 0.0f && y_distance < 0.0f) {
        return true;
    }
    return false;
}



//stops all movements when game is ended
void endGame() {
    g_paddle1_movement = { 0.0f, 0.0f, 0.0f };
    g_paddle2_movement = { 0.0f, 0.0f, 0.0f };
    for (int i = 0; i < g_number_of_balls; i++) {
        g_ball_movement[i] = { 0.0f, 0.0f, 0.0f };
    }
}


void update() {
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    
    g_paddle1_position += g_paddle1_movement * PADDLE_SPEED * delta_time;

    g_paddle2_position += g_paddle2_movement * PADDLE_SPEED * delta_time;

    for (int i = 0; i < g_number_of_balls; i++) {
        g_ball_position[i] += g_ball_movement[i] * BALL_SPEED * delta_time;
    }

    //initial position
    g_paddle1_position.x = -4;
    g_paddle2_position.x = 4;

    // translation
    g_paddle2_matrix = glm::mat4(1.0f);
    g_paddle2_matrix = glm::translate(g_paddle2_matrix, g_paddle2_position);

    g_paddle1_matrix = glm::mat4(1.0f);
    g_paddle1_matrix = glm::translate(g_paddle1_matrix, g_paddle1_position);

    
    for (int i = 0; i < g_number_of_balls; i++) {
        g_ball_matrix[i] = glm::mat4(1.0f);
        g_ball_matrix[i] = glm::translate(g_ball_matrix[i], INIT_POS_BALL);
        g_ball_matrix[i] = glm::translate(g_ball_matrix[i], g_ball_position[i]);
        g_ball_matrix[i] = glm::scale(g_ball_matrix[i], INIT_SCALE_BALL);
    }

    // scaling
    g_paddle1_matrix = glm::scale(g_paddle1_matrix, INIT_SCALE_PADDLES);
    g_paddle2_matrix = glm::scale(g_paddle2_matrix, INIT_SCALE_PADDLES);

   
    
    if (glm::length(g_paddle1_movement) > 1.0f)
    {
        g_paddle1_movement = glm::normalize(g_paddle1_movement);
    }
    if (glm::length(g_paddle2_movement) > 1.0f)
    {
        g_paddle2_movement = glm::normalize(g_paddle2_movement);
    }

    //collision
    for (int i = 0; i < g_number_of_balls; i++) {
        if (collision(g_ball_position[i], g_paddle2_position, INIT_SCALE_BALL, INIT_SCALE_PADDLES)) {
            g_ball_movement[i].x = -g_ball_movement[i].x;
            g_ball_movement[i].y = g_ball_position[i].y - g_paddle2_position.y;
        }

        else if (collision(g_ball_position[i], g_paddle1_position, INIT_SCALE_BALL, INIT_SCALE_PADDLES)) {
            g_ball_movement[i].x = -g_ball_movement[i].x;
            g_ball_movement[i].y = g_ball_position[i].y - g_paddle1_position.y;
        }

        if (g_ball_position[i].y > 3.5f) {
            g_ball_movement[i].y = -g_ball_movement[i].y;
        }
        else if (g_ball_position[i].y < -3.5f) {
            g_ball_movement[i].y = -g_ball_movement[i].y;
        }
    }

    // add boarder
    if (g_paddle1_position.y >= 3.2f) {
        g_paddle1_position.y = 3.2f;
    }
    else if (g_paddle1_position.y <= -3.2f) {
        g_paddle1_position.y = -3.2f;
    }
    if (g_paddle2_position.y >= 3.2f) {
        g_paddle2_position.y = 3.2f;
    }
    else if (g_paddle2_position.y <= -3.2f) {
        g_paddle2_position.y = -3.2f;
    }

    // endgame 
    for (int i = 0; i < g_number_of_balls; i++) {
        if (g_ball_position[i].x <= -5.0f) {
            //player 2 wins
            g_ball_position[i].x = -5.0f;
            endGame();
            linkStart = false;
            player2Wins = true;
            
        }
        else if (g_ball_position[i].x >= 5.0f) {
            //player 1 wins
            g_ball_position[i].x = 5.0f;
            endGame();
            linkStart= false;
            player1Wins = true;
            
        }
    }
}   

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_paddle1_matrix, g_paddle1_id);
    draw_object(g_paddle2_matrix, g_paddle2_id);

    for (int i = 0; i < g_number_of_balls; i++) {
        draw_object(g_ball_matrix[i], g_ball_id);
    }

    // show winner image
    if (player1Wins) {
        g_victory_matrix = glm::mat4(1.0f);
        g_victory_matrix = glm::translate(g_victory_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        g_victory_matrix = glm::scale(g_victory_matrix, INIT_SCALE_VICTORY);
        draw_object(g_victory_matrix, g_leftWins_id);
        LOG("Player1 wins");
    }
    else if (player2Wins) {
        g_victory_matrix = glm::mat4(1.0f);
        g_victory_matrix = glm::translate(g_victory_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        g_victory_matrix = glm::scale(g_victory_matrix, INIT_SCALE_VICTORY);
        draw_object(g_victory_matrix, g_rightWins_id);
        LOG("Player2 wins");
    }
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    
    


    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}