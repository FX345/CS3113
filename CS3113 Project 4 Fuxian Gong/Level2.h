#pragma once
#include "Scene.h"

class Level2 : public Scene {
public:
	~Level2();
	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;
};