#include "Map.h"

enum EntityType { PLATFORM, PLAYER, ENEMY, ATTACK, ENEMY_ATTACK, EXIT };
enum AIType { WALKER, JUMPER, GUARD, FLYING };
enum AIState { WALKING, IDLE, ATTACKING };

class Entity {
private:
	bool m_is_active = true;

	//animation
	int* m_animation_right = NULL;
	int* m_animation_left = NULL;

	//physics
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;

	//transformations
	float m_speed;
	glm::vec3 m_movement;
	glm::mat4 m_model_matrix;

	EntityType m_entity_type;

	//AI
	AIType m_ai_type;
	AIState m_ai_state;

	string m_facing_direction;

	float m_width = 1;
	float m_height = 1;

public:
	//statics variables
	static const int FRAMES_PER_SECOND = 4;
	static const int LEFT = 0, RIGHT = 1;

	//animation
	int** m_walking = new int* [2] {
		m_animation_left, m_animation_right
	};

	int m_animation_frames = 0,
		m_animation_index = 0,
		m_animation_cols = 0,
		m_animation_rows = 0;

	int* m_animation_indices = NULL;
	float m_animation_time = 0.0f;

	//jumping
	bool m_can_jump = true;
	bool m_is_jumping = false;
	float m_jumping_power = 0;
	int m_jumps = 2;

	//collisions
	bool m_collided_top = false;
	bool m_collided_bottom = false;
	bool m_collided_right = false;
	bool m_collided_left = false;
	bool m_collided_exit;

	GLuint m_texture_id;

	//methods
	Entity();
	//parameterized constructor
	Entity(EntityType type, glm::vec3 position, float speed, glm::vec3 acceleration, GLuint texture_id,
		int* left_anim, int* right_anim, int animation_frames, int animation_index, float time, int animation_cols, int animation_rows, float height, float width);
	~Entity();

	void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
	void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map); 
	void render(ShaderProgram* program);

	bool const check_collision(Entity* other) const;
	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
	void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);


	void const check_collision_y(Map* map);
	void const check_collision_x(Map* map);

	void move_left() {
		m_movement.x = -1.0f;
		m_facing_direction = "left";
	};
	void move_right() {
		m_movement.x = 1.0f;
		m_facing_direction = "right";
	};
	void move_up() { m_movement.y = 1.0f; };
	void move_down() { m_movement.y = -1.0f; };

	//ai activities
	void ai_activate(Entity* player);
	void ai_walk();
	void ai_jump();
	void ai_guard(Entity* player);
	void ai_flyer();

	void activate() { m_is_active = true; };
	void deactivate() { m_is_active = false; };

	//getters and setters
	EntityType const get_entity_type()    const { return m_entity_type; };
	AIType     const get_ai_type()        const { return m_ai_type; };
	AIState    const get_ai_state()       const { return m_ai_state; };
	glm::vec3  const get_position()       const { return m_position; };
	glm::vec3  const get_movement()       const { return m_movement; };
	glm::vec3  const get_velocity()       const { return m_velocity; };
	glm::vec3  const get_acceleration()   const { return m_acceleration; };
	float      const get_jumping_power()  const { return m_jumping_power; };
	float      const get_speed()          const { return m_speed; };
	int        const get_width()          const { return m_width; };
	int        const get_height()         const { return m_height; };
	string	   const get_direction()	  const { return m_facing_direction; };
	bool	   const get_active()		  const { return m_is_active; };

	
	void const set_entity_type(EntityType new_entity_type) { m_entity_type = new_entity_type; };
	void const set_ai_type(AIType new_ai_type) { m_ai_type = new_ai_type; };
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
	void const set_position(glm::vec3 new_position) { m_position = new_position; };
	void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
	void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
	void const set_speed(float new_speed) { m_speed = new_speed; };
	void const set_jumping_power(float new_jumping_power) { m_jumping_power = new_jumping_power; };
	void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; };
	void const set_width(float new_width) { m_width = new_width; };
	void const set_height(float new_height) { m_height = new_height; };
	void const set_direction(string direction) { m_facing_direction = direction; };
	void const kill() { m_is_active = false; };
};
