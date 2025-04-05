#pragma once
#include "Scene.h"

class Level3 : public Scene {
public:
	~Level3();
	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;
};