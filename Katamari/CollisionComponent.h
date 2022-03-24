#pragma once

#include <SimpleMath.h>
#include "../ECS/Entity.h"

class CollisionComponent : public ecs::Entity::ComponentBase {
public:
	ecs::Entity& sphere;
	DirectX::SimpleMath::Vector3 offset;
	float radius;

	CollisionComponent(
		ecs::Entity& sphere,
		DirectX::SimpleMath::Vector3 offset,
		float radius)
		: sphere(sphere)
		, offset(offset)
		, radius(radius)
	{}
};