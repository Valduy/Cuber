#pragma once

#include "../ECS/ComponentBase.h"

enum class PlayerType {
	kLeft,
	kRight
};

class InputComponent : public ecs::ComponentBase {
public:
	PlayerType player_type;
	bool up;
	bool down;
};