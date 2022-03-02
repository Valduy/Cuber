#pragma once

#include "../ECS/IComponent.h"

class SpeedComponent : public ecs::IComponent {
public:
	float speed;
};