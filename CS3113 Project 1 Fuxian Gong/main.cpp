/**
* Author: Fuxian Gong
* Assignment: Simple 2D Scene
* Date due: 2025-02-15, 11:59pm
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

enum AppStatus { RUNNING, TERMINATED };

// Our window dimensions
constexpr float WINDOW_SIZE_MULT = 2.0f;

constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

// Background color components
constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
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

constexpr char TEXTURE1_SPRITE_FILEPATH[] = "texture1.png",
TEXTURE2_SPRITE_FILEPATH[] = "texture2.png";

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_SCALE_TEXTURE2 = glm::vec3(1.0f, 1.0f, 0.0f),
INIT_POS_TEXTURE2 = glm::vec3(-3.0f, 2.0f, 0.0f),
INIT_SCALE_TEXTURE1 = glm::vec3(3.0f, 3.0f, 0.0f),
INIT_POS_TEXTURE1 = glm::vec3(3.0f, -2.0f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_texture1_matrix, g_projection_matrix, g_texture2_matrix;



GLuint g_texture1_id;
GLuint g_texture2_id;

//parameters
float g_previous_ticks = 0.0f;
glm::vec3 g_position = glm::vec3(0.0f, 0.0f, 0.0f);
float g_radius = 2;
float g_frames = 0;

float rotation_angle = 0.0f;
float rotation_speed = 1.0f;



glm::vec3 g_texture1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_texture1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_texture2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_texture2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_texture2_scale = glm::vec3(0.0f, 0.0f, 0.0f);  // scale trigger vector
glm::vec3 g_texture2_size = glm::vec3(1.0f, 1.0f, 0.0f);  // scale accumulator vector

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

    LOG("Loading texture£º", filepath);
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
    // HARD INITIALISE ——————————————————————————————————————————————————————————————————?
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Simple 2D scene",
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

    // ——————————————————————————————————————————————————————————————————————————————————?

    // SOFT INITIALISE ——————————————————————————————————————————————————————————————————?
    // Initialise our camera
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_texture2_matrix = glm::mat4(1.0f);
    g_texture1_matrix = glm::mat4(1.0f);
    g_texture1_matrix = glm::translate(g_texture1_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    g_texture1_position += g_texture1_movement;

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_texture1_id = load_texture(TEXTURE1_SPRITE_FILEPATH);
    g_texture2_id = load_texture(TEXTURE2_SPRITE_FILEPATH);
    

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}

void update() {
    float ticks = (float)SDL_GetTicks()/MILLISECONDS_IN_SECOND;//current ticks
    float delta_time = ticks - g_previous_ticks; //last frame's ticks difference 
    g_previous_ticks = ticks;
    g_frames++;

    g_position.x = g_radius * cos(g_frames / 60);
    g_position.y = g_radius * sin(g_frames / 60);

     /* Reset the Model Matrix */
    g_texture1_matrix = glm::mat4(1.0f);

    /* Translate */
    g_texture1_matrix = glm::translate(g_texture1_matrix, g_position);
    //change scale, making texture1 shrink and stretch
    g_texture1_matrix = glm::scale(g_texture1_matrix, glm::vec3(0.5f * (sin(g_frames / 30.0f)+1.8f), 1.0f, 1.0f));
   

    g_texture2_matrix = glm::mat4(1.0f);
    g_texture2_matrix = glm::scale(g_texture2_matrix, INIT_SCALE_TEXTURE2);
    //rotation
    rotation_angle += rotation_speed * delta_time;
    g_texture2_matrix = glm::rotate(g_texture2_matrix, rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
    //wave movement
    g_texture2_matrix = glm::translate(g_texture2_matrix, glm::vec3(sin(g_frames / 60.0f), cos(g_frames / 40.0f), 0.0f));
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
    draw_object(g_texture1_matrix, g_texture1_id);
    draw_object(g_texture2_matrix, g_texture2_id);

    // We disable two attribute arrays now
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