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
		if (parent_ != nullptr) {
			return parent_->GetRotation() + local_rotation_;
		}

		return local_rotation_;
	}

	void SetRotation(DirectX::SimpleMath::Vector3 rotation) {
		if (parent_ != nullptr) {
			local_rotation_ = rotation - parent_->GetRotation();
		} else {
			local_rotation_ = rotation;
		}
	}

	DirectX::SimpleMath::Vector3 GetScale() {
		if (parent_ != nullptr) {
			return parent_->GetScale() * local_scale_;
		}

		return local_scale_;
	}

	void SetScale(DirectX::SimpleMath::Vector3 scale) {
		if (parent_ != nullptr) {
			local_scale_ = scale / parent_->GetScale();
		} else {
			local_scale_ = scale;
		}
	}

	DirectX::SimpleMath::Vector3 GetPosition() {
		if (parent_ == nullptr) return local_position_;
		using namespace DirectX::SimpleMath;		
		Vector4 offset(local_position_.x, local_position_.y, local_position_.z, 1.0);
		Vector3 euler = parent_->GetRotation() * DirectX::XM_PI / 180;

		Matrix transform = Matrix::Identity;
		transform *= Matrix::CreateFromYawPitchRoll(euler);
		transform *= Matrix::CreateTranslation(parent_->GetPosition());

		offset = Vector4::Transform(offset, transform);
		return { offset.x, offset.y, offset.z };
	}

	void SetPosition(DirectX::SimpleMath::Vector3 position) {
		if (parent_ != nullptr) {
			using namespace DirectX::SimpleMath;
			Vector4 offset(position.x, position.y, position.z, 1.0f);
			Vector3 euler = parent_->GetRotation() * DirectX::XM_PI / 180;

			Matrix transform = Matrix::Identity;
			transform *= Matrix::CreateTranslation(-parent_->GetPosition());
			transform *= Matrix::CreateFromYawPitchRoll(-euler);

			offset = Vector4::Transform(offset, transform);
			local_position_ = Vector3(offset.x, offset.y, offset.z);
		} else {
			local_position_ = position;
		}
	}

	DirectX::SimpleMath::Matrix GetModelMatrix() const {
		using namespace DirectX::SimpleMath;
		
		Matrix model = Matrix::Identity;
		model *= Matrix::CreateScale(local_scale_);
		model *= Matrix::CreateFromYawPitchRoll(local_rotation_ * DirectX::XM_PI / 180);
		model *= Matrix::CreateTranslation(local_position_);

		if (parent_ != nullptr) {
			model *= parent_->GetModelMatrix();
		}

		return model;
	}

	// TODO: fix this madness...
	ecs::Entity& GetParent() {
		assert(owner_ != nullptr && "Entity has not parent.");
		return *owner_;
	}

	size_t GetChildrenCount() {
		return children_.size();
	}

	TransformComponent(ecs::Entity& owner)
		: owner_(&owner)
		, parent_(nullptr)
		, local_rotation_(0.0f, 0.0f, 0.0f)
		, local_scale_(1.0f, 1.0f, 1.0f)
		, local_position_(0.0f, 0.0f, 0.0f)
	{}

	bool IsHasParent() const {
		return parent_ != nullptr;
	}

	void AddChild(ecs::Entity& child) {
		assert(&child != owner_ && "Entity can't contain itself.");
		assert(child.Contain<TransformComponent>() && "Child don't contain transform component.");
		assert(!IsParent(child) && "Child shouldn't be parent.");

		TransformComponent& transform_component = child.Get<TransformComponent>();
		transform_component.parent_ = this;
		children_.push_back(&child);
	}

	bool RemoveChild(ecs::Entity& child) {
		assert(child.Contain<TransformComponent>() && "Child don't contain transform component.");
		TransformComponent& transform_component = child.Get<TransformComponent>();

		if (transform_component.parent_ == this) {
			for (auto it = children_.begin(); it != children_.end(); ++it) {
				if (*it == &child) {
					children_.erase(it);
					transform_component.parent_ = nullptr;
					return true;
				}
			}			
		}

		return false;
	}

	ecs::Entity& operator[](size_t index) {
		return *children_[index];
	}

private:
	ecs::Entity* owner_;
	TransformComponent* parent_;
	std::vector<ecs::Entity*> children_{};

	DirectX::SimpleMath::Vector3 local_rotation_;
	DirectX::SimpleMath::Vector3 local_scale_;
	DirectX::SimpleMath::Vector3 local_position_;

	bool IsParent(ecs::Entity& entity) {
		TransformComponent& transform = entity.Get<TransformComponent>();
		TransformComponent* relative = parent_;

		while (relative != nullptr) {
			if (relative == &transform) {
				return true;
			}

			relative = relative->parent_;
		}

		return false;
	}
};