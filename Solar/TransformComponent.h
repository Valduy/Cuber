#pragma once

#include <SimpleMath.h>

#include "../ECS/Entity.h"

class TransformComponent : public ecs::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector3 GetLocalRotation() const {
		return local_rotation_;
	}

	void SetLocalRotation(DirectX::SimpleMath::Vector3 local_rotation) {
		local_rotation_ = local_rotation;
	}

	DirectX::SimpleMath::Vector3 GetLocalScale() const {
		return local_scale_;
	}

	void SetLocalScale(DirectX::SimpleMath::Vector3 local_scale) {
		local_scale_ = local_scale;
	}

	DirectX::SimpleMath::Vector3 GetLocalPosition() const {
		return local_position_;
	}

	void SetLocalPosition(DirectX::SimpleMath::Vector3 local_position) {
		local_position_ = local_position;
	}

	DirectX::SimpleMath::Vector3 GetRotation() const {
		return parent_ != nullptr
			? parent_->Get<TransformComponent>().GetRotation() + local_rotation_
			: local_rotation_;
	}

	void SetRotation(DirectX::SimpleMath::Vector3 rotation) {
		parent_ != nullptr
			? local_rotation_ = rotation - parent_->Get<TransformComponent>().GetRotation()
			: local_rotation_ = rotation;
	}

	DirectX::SimpleMath::Vector3 GetScale() const {
		return parent_ != nullptr
			? parent_->Get<TransformComponent>().GetScale() * local_scale_
			: local_scale_;
	}

	void SetScale(DirectX::SimpleMath::Vector3 scale) {
		parent_ != nullptr
			? local_scale_ = scale / parent_->Get<TransformComponent>().GetScale()
			: local_scale_ = scale;
	}

	DirectX::SimpleMath::Vector3 GetPosition() const {
		if (parent_ == nullptr) return local_position_;

		using namespace DirectX::SimpleMath;
		TransformComponent& parent_transform = parent_->Get<TransformComponent>();
		Vector4 offset(local_position_.x, local_position_.y, local_position_.z, 1.0);
		Vector3 euler = parent_transform.GetRotation() * DirectX::XM_PI / 180;

		Matrix transform = Matrix::Identity;
		transform *= Matrix::CreateFromYawPitchRoll(euler);
		transform *= Matrix::CreateTranslation(parent_transform.GetPosition());

		offset = Vector4::Transform(offset, transform);
		return { offset.x, offset.y, offset.z };
	}

	void SetPosition(DirectX::SimpleMath::Vector3 position) {
		if (parent_ != nullptr) {
			using namespace DirectX::SimpleMath;
			TransformComponent& parent_transform = parent_->Get<TransformComponent>();
			Vector4 offset(position.x, position.y, position.z, 1.0f);
			Vector3 euler = parent_transform.GetRotation() * DirectX::XM_PI / 180;

			Matrix transform = Matrix::Identity;
			transform *= Matrix::CreateTranslation(-parent_transform.GetPosition());
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
			model *= parent_->Get<TransformComponent>().GetModelMatrix();
		}

		return model;
	}

	ecs::Entity& GetParent() {
		assert(parent_ != nullptr && "Entity hasn't parent.");
		return *parent_;
	}

	size_t GetChildrenCount() {
		return children_.size();
	}

	TransformComponent()
		: parent_(nullptr)
		, local_rotation_(0.0f, 0.0f, 0.0f)
		, local_scale_(1.0f, 1.0f, 1.0f)
		, local_position_(0.0f, 0.0f, 0.0f)
	{}

	void Delete() override {
		if (IsHasParent()) {
			parent_->Get<TransformComponent>().RemoveChild(GetOwner());

			for (auto child : children_) {
				child->Get<TransformComponent>().parent_ = nullptr;
			}

			children_.clear();
		}
	}

	bool IsHasParent() const {
		return parent_ != nullptr;
	}

	void AddChild(ecs::Entity& child) {
		assert(&child != &GetOwner() && "Entity can't contain itself.");
		assert(child.Contain<TransformComponent>() && "Child don't contain transform component.");
		assert(!IsChildOf(child) && "Child shouldn't be parent.");

		TransformComponent& transform_component = child.Get<TransformComponent>();
		transform_component.parent_ = &GetOwner();
		children_.push_back(&child);
	}

	bool RemoveChild(ecs::Entity& child) {
		assert(child.Contain<TransformComponent>() && "Child don't contain transform component.");
		TransformComponent& transform_component = child.Get<TransformComponent>();
		if (transform_component.parent_ != &GetOwner()) return false;

		for (auto it = children_.begin(); it != children_.end(); ++it) {
			if (*it == &child) {
				children_.erase(it);
				transform_component.parent_ = nullptr;
				break;
			}
		}

		return true;
	}

	bool IsChildOf(ecs::Entity& entity) const {
		const TransformComponent* temp = this;

		do {
			if (temp->parent_ == nullptr) {
				return false;
			}

			if (temp->parent_ == &entity) {
				return true;
			}

			temp = &temp->parent_->Get<TransformComponent>();
		} while (true);
	}

	ecs::Entity& operator[](size_t index) {
		return *children_[index];
	}

private:
	ecs::Entity* parent_;
	std::vector<ecs::Entity*> children_{};

	DirectX::SimpleMath::Vector3 local_rotation_;
	DirectX::SimpleMath::Vector3 local_scale_;
	DirectX::SimpleMath::Vector3 local_position_;
};