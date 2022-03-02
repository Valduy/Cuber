#pragma once

#include "../ECS/IComponent.h"

enum class PlayerType {
	kLeft,
	kRight
};

class InputComponent : public ecs::IComponent {
public:
	PlayerType player_type;
	bool up;
	bool down;
};