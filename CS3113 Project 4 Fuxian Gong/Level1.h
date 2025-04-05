#pragma once
#include "Scene.h"

class Level1 : public Scene {
public:
	~Level1();
	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;
};