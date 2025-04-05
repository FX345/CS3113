#pragma once
#include "Scene.h"

class Menu : public Scene {
public:
	GLuint font_texture_id;

	~Menu();

	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;
};