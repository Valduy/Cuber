#pragma once

#include "../ECS/Entity.h"

enum class PlayerType {
	kLeft,
	kRight
};

class InputComponent : public ecs::Entity::ComponentBase {
public:
	PlayerType player_type;
	bool up;
	bool down;
};