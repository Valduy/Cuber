#pragma once

#include <SimpleMath.h>
#include "../Ash/Entity.h"

class CollisionComponent : public ash::Entity::ComponentBase {
public:
	ash::Entity& sphere;
	DirectX::SimpleMath::Vector3 offset;
	float radius;

	CollisionComponent(
		ash::Entity& sphere,
		DirectX::SimpleMath::Vector3 offset,
		float radius)
		: sphere(sphere)
		, offset(offset)
		, radius(radius)
	{}
};