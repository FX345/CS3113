#include "Level2.h"
#include "Utility.h"

#define LEVEL2_WIDTH 30
#define LEVEL2_HEIGHT 10
#define ENEMY_COUNT 1

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
const char PLAYER_SPRITE[] = "assets/characters/player.png";
const char TILEMAP_SPRITE[] = "assets/map/blocks.png";
const char ENEMY_SPRITE[] = "assets/characters/enemies.png";
const char FONT_SPRITE[] = "assets/font/font.png";
const char EXIT_SPRITE[] = "assets/map/next_level.png";

unsigned int LEVEL_2_DATA[] =
{
    0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 6,
    0, 6, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 6,
    0, 6, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
};



Level2::~Level2() {
    delete m_game_state.player;
    delete m_game_state.map;
}

void Level2::initialise() {
    //create player
    m_game_state.player = new Entity(PLAYER, glm::vec3(3.0f, -3.0f, 0.0f), 5.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(PLAYER_SPRITE),
        new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 }, new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 }, 8, 0, 0.1f, 8, 1, 0.5f, 0.5f);
    m_game_state.player->set_direction("right");
    m_game_state.player->m_animation_indices = m_game_state.player->m_walking[m_game_state.player->RIGHT];

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    //create AIs
    //jumper-guard
    m_game_state.enemies[0] = Entity(ENEMY, glm::vec3(15.0f, -1.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(ENEMY_SPRITE),
        new int[1] { 1 }, new int[2] { 0, 1 }, 2, 0, 0.1f, 10, 1, 0.8f, 0.8f);
    m_game_state.enemies[0].set_direction("left");
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_jumping_power(10.0f);
    m_game_state.enemies[0].set_ai_state(IDLE);

    //jump power of the player
    m_game_state.player->m_jumping_power = 7.0f;

    //exit
    m_game_state.exit = new Entity(EXIT, glm::vec3(25.0f, -8.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(EXIT_SPRITE),
        new int[3] { 3, 4, 5 }, new int[3] { 6, 7, 8 }, 3, 0, 0.1f, 3, 4, 0.8f, 0.8f);

    GLuint map_texture_id = Utility::load_texture(TILEMAP_SPRITE);
    m_game_state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, LEVEL_2_DATA, map_texture_id, 1.0f, 15, 2);
}

void Level2::update(float delta_time) {
    bool g_game_is_over = false;

    if (!g_game_is_over) {
        m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
        m_game_state.exit->update(delta_time, m_game_state.player, m_game_state.player, ENEMY_COUNT, m_game_state.map);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
        }

    }
}

void Level2::render(ShaderProgram* program) {
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].render(program);
    }
    m_game_state.exit->render(program);
}