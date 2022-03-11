#pragma once

#include <SimpleMath.h>

#include "../ECS/IComponent.h"
#include "../ECS/Entity.h"

class TransformComponent : public ecs::IComponent {
public:
	DirectX::SimpleMath::Vector3 GetLocalRotation() {
		return local_rotation_;
	}

	void SetLocalRotation(DirectX::SimpleMath::Vector3 local_rotation) {
		local_rotation_ = local_rotation;
	}

	DirectX::SimpleMath::Vector3 GetLocalScale() {
		return local_scale_;
	}

	void SetLocalScale(DirectX::SimpleMath::Vector3 local_scale) {
		local_scale_ = local_scale;
	}

	DirectX::SimpleMath::Vector3 GetLocalPosition() {
		return local_position_;
	}

	void SetLocalPosition(DirectX::SimpleMath::Vector3 local_position) {
		local_position_ = local_position;
	}

	DirectX::SimpleMath::Vector3 GetRotation() {
		if (relative_ != nullptr) {
			return relative_->GetRotation() + local_rotation_;
		}

		return local_rotation_;
	}

	void SetRotation(DirectX::SimpleMath::Vector3 rotation) {
		if (relative_ != nullptr) {
			local_rotation_ = relative_->GetRotation() + rotation;
		} else {
			local_rotation_ = rotation;
		}
	}

	DirectX::SimpleMath::Vector3 GetScale() {
		if (relative_ != nullptr) {
			return relative_->GetScale() * local_scale_;
		}

		return local_scale_;
	}

	void SetScale(DirectX::SimpleMath::Vector3 scale) {
		if (relative_ != nullptr) {
			local_scale_ = scale / relative_->GetScale();
		} else {
			local_scale_ = scale;
		}
	}

	DirectX::SimpleMath::Vector3 GetPosition() {
		if (relative_ == nullptr) return local_position_;
		using namespace DirectX::SimpleMath;		
		Vector4 offset(local_position_.x, local_position_.y, local_position_.z, 1.0);

		Vector3 euler = relative_->GetRotation() * DirectX::XM_PI / 180;
		Quaternion rotation = Quaternion::CreateFromYawPitchRoll(euler);

		Matrix transform = Matrix::Identity;
		transform *= Matrix::CreateFromQuaternion(rotation);
		transform *= Matrix::CreateTranslation(relative_->GetPosition());

		offset = Vector4::Transform(offset, transform);
		return { offset.x, offset.y, offset.z };
	}

	void SetPosition(DirectX::SimpleMath::Vector3 position) {
		if (relative_ != nullptr) {
			using namespace DirectX::SimpleMath;
			Vector4 offset(position.x, position.y, position.z, 1.0f);

			Vector3 euler = -relative_->GetRotation() * DirectX::XM_PI / 180;
			Quaternion rotation = Quaternion::CreateFromYawPitchRoll(euler);

			Matrix transform = Matrix::Identity;
			transform *= Matrix::CreateTranslation(-relative_->GetPosition());
			transform *= Matrix::CreateFromQuaternion(rotation);

			offset = Vector4::Transform(offset, transform);
			local_position_ = Vector3(offset.x, offset.y, offset.z);
		} else {
			local_position_ = position;
		}
	}

	TransformComponent(ecs::Entity& owner)
		: owner_(&owner)
		, relative_(nullptr)
		, local_rotation_(0.0f, 0.0f, 0.0f)
		, local_scale_(1.0f, 1.0f, 1.0f)
		, local_position_(0.0f, 0.0f, 0.0f)
	{}
	
	void AddChild(ecs::Entity& child) {
		assert(&child != owner_ && "Entity can't contain itself.");
		assert(child.Contain<TransformComponent>() && "Child don't contain transform component.");
		assert(!IsParent(child) && "Child shouldn't be parent.");

		TransformComponent& transform_component = child.Get<TransformComponent>();
		transform_component.relative_ = this;
		children_.insert(&child);
	}

	void RemoveChild(ecs::Entity& child) {
		assert(child.Contain<TransformComponent>() && "Child don't contain transform component.");

		TransformComponent& transform_component = child.Get<TransformComponent>();

		if (transform_component.relative_ == this) {
			children_.erase(&child);
			transform_component.relative_ = nullptr;
		}
	}

private:
	ecs::Entity* owner_;
	TransformComponent* relative_;
	std::unordered_set<ecs::Entity*> children_;

	DirectX::SimpleMath::Vector3 local_rotation_;
	DirectX::SimpleMath::Vector3 local_scale_;
	DirectX::SimpleMath::Vector3 local_position_;

	bool IsParent(ecs::Entity& entity) {
		TransformComponent& transform = entity.Get<TransformComponent>();
		TransformComponent* relative = relative_;

		while (relative != nullptr) {
			if (relative == &transform) {
				return true;
			}

			relative = relative->relative_;
		}

		return false;
	}
};