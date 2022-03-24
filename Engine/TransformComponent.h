#pragma once

#include <SimpleMath.h>

#include "../ECS/Entity.h"

namespace engine {

class TransformComponent : public ecs::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector3 GetLocalEuler() const {
		return local_euler_;
	}

	void SetLocalEuler(DirectX::SimpleMath::Vector3 local_euler) {
		local_euler_ = local_euler;
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

	DirectX::SimpleMath::Vector3 GetEuler() const {
		return parent_ != nullptr
			? parent_->Get<TransformComponent>().GetEuler() + local_euler_
			: local_euler_;
	}

	void SetEuler(DirectX::SimpleMath::Vector3 rotation) {
		local_euler_ = parent_ != nullptr
			? rotation - parent_->Get<TransformComponent>().GetEuler()
			: rotation;
	}

	DirectX::SimpleMath::Vector3 GetScale() const {
		return parent_ != nullptr
			? parent_->Get<TransformComponent>().GetScale() * local_scale_
			: local_scale_;
	}

	void SetScale(DirectX::SimpleMath::Vector3 scale) {
		local_scale_ = parent_ != nullptr
			? scale / parent_->Get<TransformComponent>().GetScale()
			: scale;
	}

	DirectX::SimpleMath::Vector3 GetPosition() const {
		if (parent_ == nullptr) return local_position_;
		using namespace DirectX::SimpleMath;
		Vector4 offset(local_position_.x, local_position_.y, local_position_.z, 1.0);
		const Matrix to_world = parent_->Get<TransformComponent>().GetModelMatrix();
		offset = Vector4::Transform(offset, to_world);
		return { offset.x, offset.y, offset.z };
	}

	void SetPosition(DirectX::SimpleMath::Vector3 position) {
		if (parent_ != nullptr) {
			using namespace DirectX::SimpleMath;
			const Matrix to_local = parent_->Get<TransformComponent>().GetModelMatrix().Invert();
			Vector4 offset(position.x, position.y, position.z, 1.0f);
			offset *= Vector4::Transform(offset, to_local);
			local_position_ = Vector3(offset.x, offset.y, offset.z);
		}
		else {
			local_position_ = position;
		}
	}

	DirectX::SimpleMath::Matrix GetModelMatrix() const {
		using namespace DirectX::SimpleMath;
		Matrix model = Matrix::Identity;
		model *= Matrix::CreateScale(local_scale_);
		model *= Matrix::CreateFromYawPitchRoll(local_euler_ * DirectX::XM_PI / 180);
		model *= Matrix::CreateTranslation(local_position_);

		if (parent_ != nullptr) {
			model *= parent_->Get<TransformComponent>().GetModelMatrix();
		}

		return model;
	}

	ecs::Entity& GetParent() const {
		assert(parent_ != nullptr && "Entity hasn't parent.");
		return *parent_;
	}

	size_t GetChildrenCount() const {
		return children_.size();
	}

	TransformComponent()
		: parent_(nullptr)
		, local_euler_(0.0f, 0.0f, 0.0f)
		, local_scale_(1.0f, 1.0f, 1.0f)
		, local_position_(0.0f, 0.0f, 0.0f)
	{}

	void Delete() override {
		if (IsHasParent()) {
			parent_->Get<TransformComponent>().RemoveChild(GetOwner());
		}

		for (const auto child : children_) {
			child->Get<TransformComponent>().parent_ = nullptr;
		}

		children_.clear();
	}

	bool IsHasParent() const {
		return parent_ != nullptr;
	}

	// TODO: child of this or child check!!!
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

	ecs::Entity& operator[](size_t index) const {
		return *children_[index];
	}

private:
	ecs::Entity* parent_;
	std::vector<ecs::Entity*> children_{};

	DirectX::SimpleMath::Vector3 local_euler_;
	DirectX::SimpleMath::Vector3 local_scale_;
	DirectX::SimpleMath::Vector3 local_position_;
};
	
} // namespace engine
