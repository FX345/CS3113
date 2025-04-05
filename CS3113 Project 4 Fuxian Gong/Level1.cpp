#include "Level1.h"
#include "Utility.h"

#define LEVEL1_WIDTH 30
#define LEVEL1_HEIGHT 10
#define ENEMY_COUNT 1

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";
const char PLAYER_SPRITE[] = "assets/characters/player.png";
const char TILEMAP_SPRITE[] = "assets/map/blocks.png";
const char ENEMY_SPRITE[] = "assets/characters/enemies.png";
const char FONT_SPRITE[] = "assets/font/font.png";
const char EXIT_SPRITE[] = "assets/map/next_level.png";

unsigned int LEVEL_1_DATA[] =
{
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 6, 6, //
    6, 6, 2, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 6,
    6, 0, 6, 6, 6, 6, 6, 6, 6, 6, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
};

bool g_game_is_over = false;
int enemies_alive = ENEMY_COUNT;

Level1::~Level1() {
    delete m_game_state.player;
    delete m_game_state.map;
}

void Level1::initialise() {
    //create player
    m_game_state.player = new Entity(PLAYER, glm::vec3(3.0f, -3.0f, 0.0f), 5.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(PLAYER_SPRITE),
        new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 }, new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 }, 8, 0, 0.1f, 8, 1, 0.5f, 0.5f);
    m_game_state.player->set_direction("right");
    m_game_state.player->m_animation_indices = m_game_state.player->m_walking[m_game_state.player->RIGHT];

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    //create AIs
    //walker
    m_game_state.enemies[0] = Entity(ENEMY, glm::vec3(8.0f, -4.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(ENEMY_SPRITE),
        new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 }, new int[8] { 0, 1, 2, 3, 4, 5, 6, 7 }, 8, 0, 0.1f, 8, 1, 0.5f, 0.5f);
    m_game_state.enemies[0].set_direction("right");
    m_game_state.enemies[0].set_ai_type(WALKER);

    //jump power of the player
    m_game_state.player->m_jumping_power = 7.0f;

    //exit
    m_game_state.exit = new Entity(EXIT, glm::vec3(25.0f, -1.0f, 0.0f), 2.0f, glm::vec3(0.0f, -20.0f, 0.0f), Utility::load_texture(EXIT_SPRITE),
        new int[3] { 3, 4, 5 }, new int[3] { 6, 7, 8 }, 3, 0, 0.1f, 3, 4, 0.8f, 0.8f);

    GLuint map_texture_id = Utility::load_texture(TILEMAP_SPRITE);
    m_game_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 15, 2);
}

void Level1::update(float delta_time) {
    if (!g_game_is_over) {
        m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, enemies_alive, m_game_state.map);
        m_game_state.exit->update(delta_time, m_game_state.player, m_game_state.player, enemies_alive, m_game_state.map);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, 0, m_game_state.map);
        }
    }
}

void Level1::render(ShaderProgram* program) {
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].render(program);
    }
    m_game_state.exit->render(program);

}