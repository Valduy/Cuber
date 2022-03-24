#pragma once

#include <SimpleMath.h>
#include "../ECS/Entity.h"

class CollisionComponent : public ecs::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector3 offset;
	float radius;
};