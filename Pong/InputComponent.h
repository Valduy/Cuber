#pragma once

#include "../Ash/Entity.h"

enum class PlayerType {
	kLeft,
	kRight
};

class InputComponent : public ash::Entity::ComponentBase {
public:
	PlayerType player_type;
	bool up;
	bool down;
};