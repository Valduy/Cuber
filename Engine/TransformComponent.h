#pragma once

#include <SimpleMath.h>

#include "../ECS/Entity.h"

namespace engine {

class TransformComponent : public ash::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Quaternion GetLocalRotation() const {
		return local_rotation_;
	}

	void SetLocalRotation(DirectX::SimpleMath::Quaternion local_rotation) {
		local_rotation_ = local_rotation;
	}

	DirectX::SimpleMath::Vector3 GetLocalEuler() const {
		return local_rotation_.ToEuler() * 180 / DirectX::XM_PI;
	}

	void SetLocalEuler(DirectX::SimpleMath::Vector3 local_euler) {
		using namespace DirectX::SimpleMath;
		local_rotation_ = Quaternion::CreateFromYawPitchRoll(local_euler * DirectX::XM_PI / 180);
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

	DirectX::SimpleMath::Quaternion GetRotation() const {
		return parent_ != nullptr
			? local_rotation_ * parent_->Get<TransformComponent>().GetRotation()
			: local_rotation_;
	}

	void SetRotation(DirectX::SimpleMath::Quaternion rotation) {		
		if (parent_ != nullptr) {
			DirectX::SimpleMath::Quaternion quat;
			parent_->Get<TransformComponent>().GetRotation().Inverse(quat);
			local_rotation_ = quat * rotation;
		}
		else {
			local_rotation_ = rotation;
		}			
	}

	DirectX::SimpleMath::Vector3 GetEuler() const {
		return GetRotation().ToEuler() * 180 / DirectX::XM_PI;
	}

	void SetEuler(DirectX::SimpleMath::Vector3 rotation) {
		using namespace DirectX::SimpleMath;
		SetRotation(Quaternion::CreateFromYawPitchRoll(rotation * DirectX::XM_PI / 180));
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
		return Vector3::Transform(local_position_, to_world);
	}

	void SetPosition(DirectX::SimpleMath::Vector3 position) {
		if (parent_ != nullptr) {
			using namespace DirectX::SimpleMath;
			const Matrix to_local = parent_->Get<TransformComponent>().GetModelMatrix().Invert();
			local_position_ = Vector3::Transform(position, to_local);
		}
		else {
			local_position_ = position;
		}
	}

	DirectX::SimpleMath::Matrix GetModelMatrix() const {
		using namespace DirectX::SimpleMath;
		Matrix model = Matrix::Identity;
		model *= Matrix::CreateScale(local_scale_);
		model *= Matrix::CreateFromQuaternion(local_rotation_);
		model *= Matrix::CreateTranslation(local_position_);

		if (parent_ != nullptr) {
			model *= parent_->Get<TransformComponent>().GetModelMatrix();
		}

		return model;
	}

	ash::Entity& GetParent() const {
		assert(parent_ != nullptr && "Entity hasn't parent.");

		return *parent_;
	}

	void SetParent(ash::Entity& parent) {
		assert(&this->GetOwner() != &parent && "Entity can't be parent for itself.");
		assert(!parent.Get<TransformComponent>().IsChildOf(this->GetOwner()) && 
			"Parent doesn't have to be a child.");
		
		MakeRoot();
		parent_ = &parent;
		parent.Get<TransformComponent>().children_.push_back(&this->GetOwner());
	}

	size_t GetChildrenCount() const {
		return children_.size();
	}

	TransformComponent()
		: parent_(nullptr)
		, local_rotation_(DirectX::SimpleMath::Quaternion::Identity)
		, local_scale_(1.0f, 1.0f, 1.0f)
		, local_position_(0.0f, 0.0f, 0.0f)
	{}

	~TransformComponent() {
		BreakRelations();
	}

	void OnRemove() override {
		BreakRelations();
	}

	bool IsRoot() const {
		return parent_ == nullptr;
	}

	void MakeRoot() {
		if (parent_ != nullptr) {
			auto& parent_transform = parent_->Get<TransformComponent>();
			auto it = std::find(parent_transform.children_.begin(), parent_transform.children_.end(), &this->GetOwner());
			parent_transform.children_.erase(it);
			parent_ = nullptr;
		}
	}

	bool IsChildOf(ash::Entity& entity) const {
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

	ash::Entity& operator[](size_t index) const {
		return *children_[index];
	}

private:
	ash::Entity* parent_;
	std::vector<ash::Entity*> children_{};

	DirectX::SimpleMath::Quaternion local_rotation_;
	DirectX::SimpleMath::Vector3 local_scale_;
	DirectX::SimpleMath::Vector3 local_position_;

	void BreakRelations() {
		MakeRoot();

		for (const auto child : children_) {
			child->Get<TransformComponent>().parent_ = nullptr;
		}

		children_.clear();
	}
};
	
} // namespace engine
