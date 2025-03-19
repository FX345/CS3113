class Rocket{
public:
    // constructor
    Rocket(glm::vec3 position, glm::vec3 scale, float speed, GLuint texture_id);
    

    // getter and setters
    glm::vec3 const get_position() const { return m_position; };
    glm::vec3 const get_scale() const { return m_position; }
    float const get_speed() const { return m_speed; }
    glm::mat4 const get_model_matrix() const { return m_model_matrix; };
    GLuint const get_texture_id() const { return m_texture_id; };
    glm::vec3 const get_velocity() const { return m_velocity; };
    glm::vec3 const get_acceleration() const { return m_acceleration; };
    float const get_fuel() const { return m_fuel; }
    void const set_position(glm::vec3 pos) { m_position = pos; };
    void const set_velocity(glm::vec3 v) { m_velocity = v; };
    void const set_acceleration(glm::vec3 pos) { m_acceleration = pos; };
    

    void update(float delta_time);
    void render(ShaderProgram* program);
    void inc_y();
    void dec_y();
    void inc_x();
    void dec_x();
    bool collision(Platform* platform);

    
private:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    glm::vec3 m_scale;
    float m_speed;
    glm::mat4 m_model_matrix;
    GLuint m_texture_id;
    float m_fuel;
};