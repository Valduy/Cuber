#pragma once

#include <SimpleMath.h>
#include <vector>

#include "../ECS/IComponent.h"
#include "../ECS/Entity.h"

class TransformComponent : public ecs::IComponent {
public:
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;

	TransformComponent()
		: position(DirectX::SimpleMath::Vector3::Zero)
		, rotation(DirectX::SimpleMath::Vector3::Zero)
		, parent_(nullptr)
	{}

	ecs::Entity* GetParent() {
		return parent_;
	}
	
	void AddChild(ecs::Entity&) {
		
	}

private:	
	ecs::Entity* parent_;
	std::vector<ecs::Entity*> children_;
};