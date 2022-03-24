#pragma once

#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "CollisionComponent.h"
#include "ItemComponent.h"
#include "KatamariComponent.h"

class StickingSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		using namespace ecs;
		using namespace engine;
		using namespace DirectX::SimpleMath;

		static const auto katamari_sign = 
			Signer::GetSignature<TransformComponent, KatamariComponent, CollisionComponent>();

		static const auto item_sign 	= 
			Signer::GetSignature<TransformComponent, ItemComponent, CollisionComponent>();

		auto katamari_it = GetIterator(katamari_sign);
		if (!katamari_it.HasCurrent()) return;
		Entity& katamari = katamari_it.Get();

		auto& katamari_transform = katamari.Get<TransformComponent>();
		auto& katamari_collision = katamari.Get<CollisionComponent>();

		for (auto it = GetIterator(item_sign); it.HasCurrent(); it.Next()) {
			Entity& item = it.Get();
			auto& item_transform = item.Get<TransformComponent>();
			auto& item_collision = item.Get<CollisionComponent>();
			
			if (IsNearEnough(katamari_transform, katamari_collision, item_transform, item_collision) &&
				IsCollide(katamari_transform, katamari_collision, item_transform, item_collision)) 
			{
				item.Remove<ItemComponent>();
				const Vector3 position = item_transform.GetPosition();
				const Vector3 scale = item_transform.GetScale();
				const Quaternion rotation = item_transform.GetRotation();

				katamari_transform.AddChild(item);
				item_transform.SetPosition(position);
				item_transform.SetScale(scale);
				item_transform.SetRotation(rotation);
				ScaleUp(katamari_transform, katamari_collision, item_transform, item_collision);
			}
		}
	}

private:
	static bool IsNearEnough(
		const engine::TransformComponent& katamari_transform,
		const CollisionComponent& katamari_collision,
		const engine::TransformComponent& item_transform,
		const CollisionComponent& item_collision)
	{
		const float katamari_radius = katamari_collision.radius * GetMaxComponent(katamari_transform.GetScale());
		const float item_radius = item_collision.radius * GetMaxComponent(item_transform.GetScale());
		return katamari_radius >= item_radius;
	}

	static bool IsCollide(
		const engine::TransformComponent& katamari_transform,
		const CollisionComponent& katamari_collision,
		const engine::TransformComponent& item_transform,
		const CollisionComponent& item_collision)
	{
		using namespace DirectX::SimpleMath;
		const Vector3 katamari_offset = Vector3::Transform(
			katamari_collision.offset, 
			katamari_transform.GetModelMatrix());
		const float katamari_radius = katamari_collision.radius * GetMaxComponent(katamari_transform.GetScale());

		const Vector3 item_offset = Vector3::Transform(
			item_collision.offset,
			item_transform.GetModelMatrix());
		const float item_radius = item_collision.radius * GetMaxComponent(item_transform.GetScale());

		const Vector3 difference = item_offset - katamari_offset;
		return difference.Length() < katamari_radius + item_radius;
	}

	static void ScaleUp(
		const engine::TransformComponent& katamari_transform,
		CollisionComponent& katamari_collision,
		const engine::TransformComponent& item_transform,
		const CollisionComponent& item_collision)
	{
		using namespace DirectX::SimpleMath;
		constexpr float scale_factor = 0.1f;
		const float katamari_radius = katamari_collision.radius * GetMaxComponent(katamari_transform.GetScale());
		const float item_radius = item_collision.radius * GetMaxComponent(item_transform.GetScale());
		const float ratio = katamari_radius / item_radius * scale_factor;
		const float scale = 1 + ratio;

		auto& sphere_transform = katamari_collision.sphere.Get<engine::TransformComponent>();
		katamari_collision.radius *= scale;
		sphere_transform.SetLocalScale(sphere_transform.GetLocalScale() * scale);
	}

	static float GetMaxComponent(const DirectX::SimpleMath::Vector3& vector) {
		if (vector.x > vector.y) {
			if (vector.x > vector.z) {
				return vector.x;
			}

			return vector.z;
		}

		if (vector.y > vector.z) {
			return vector.y;
		}

		return vector.z;
	}
};
