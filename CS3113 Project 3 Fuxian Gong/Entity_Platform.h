
class Platform{
public:
    //default constructor
    Platform();
    //constructor
    Platform(glm::vec3 position, glm::vec3 scale, GLuint texture_id);

    //getters and setters
    glm::vec3 get_position() { return m_position; };
    glm::vec3 get_scale() { return m_scale; };
    void set_position(glm::vec3 pos) { m_position = pos; };
    void set_text_id(GLuint texture_id) { m_texture_id = texture_id; };
    void set_scale(glm::vec3 new_scale) { m_scale = new_scale; };

    void update();

    void render(ShaderProgram* program);

private:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::mat4 m_model_matrix;
    GLuint m_texture_id;
   
};
