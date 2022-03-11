#pragma once

#include <directxmath.h>
#include <SimpleMath.h>

#include "CameraComponent.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "RenderComponent.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"

struct ConstData {
	DirectX::SimpleMath::Matrix mat;
};

class RenderSystem : public engine::Game::SystemBase {
public:
	RenderSystem(engine::Game& game)
		: shader_(game.GetRenderer(), L"../Shaders/SolarShader.hlsl")
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		shader_.Init();

		GetGame().GetEntityManager().For<
			ShapeComponent>([&](ecs::Entity& e) {
				e.Add<RenderComponent>([&]() {
					ShapeComponent& shape_component = e.Get<ShapeComponent>();
					auto vertices = GetVertices(shape_component.points);

					graph::VertexBuffer vb(
						GetGame().GetRenderer(),
						vertices);
					vb.Init();

					graph::IndexBuffer ib(
						GetGame().GetRenderer(),
						shape_component.indexes);
					ib.Init();

					graph::ConstantBuffer cb(
						GetGame().GetRenderer(),
						sizeof(ConstData)
					);
					cb.Init();

					return new RenderComponent(vertices, vb, ib, cb);
				});				
			});
	}

	void Render() override {
		GetRenderer().BeginRender();
		shader_.SetShader();

		For<CameraComponent>([&](ecs::Entity& camera) {
			CameraComponent& camera_component = camera.Get<CameraComponent>();

			For<RenderComponent, TransformComponent>([&](ecs::Entity& e) {
				RenderComponent& render_component = e.Get<RenderComponent>();
				TransformComponent& transform_component = e.Get<TransformComponent>();

				using namespace DirectX::SimpleMath;

				Matrix model = Matrix::Identity;
				Vector3 euler = transform_component.GetRotation() * DirectX::XM_PI / 180;
				Quaternion rotation = Quaternion::CreateFromYawPitchRoll(euler);
				model *= Matrix::CreateScale(transform_component.GetScale());
				model *= Matrix::CreateFromQuaternion(rotation);
				model *= Matrix::CreateTranslation(transform_component.GetPosition());

				ConstData data{ (model * camera_component.GetCameraMatrix()).Transpose() };

				render_component.vertex_buffer.SetBuffer();
				render_component.index_buffer.SetBuffer();
				render_component.constant_buffer.SetBuffer();
				render_component.constant_buffer.Update(&data);

				GetGame().GetRenderer().GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				GetGame().GetRenderer().GetContext()->DrawIndexed(
					render_component.index_buffer.GetSize(), 0, 0);
			});
		});

		GetRenderer().EndRender();
	}

private:
	graph::Shader shader_;

	std::vector<DirectX::SimpleMath::Vector4> GetVertices(
		const std::vector<DirectX::SimpleMath::Vector3>& points)
	{
		std::vector<DirectX::SimpleMath::Vector4> result;
		int color = 0;

		for (auto p : points) {
			result.push_back({ p.x, p.y, p.z, 1.0f });

			if (color == 0) {
				result.push_back({ 1.0f, 0.0f, 0.0f, 1.0f });
				color += 1;
			}
			else if (color == 1) {
				result.push_back({ 0.0f, 1.0f, 0.0f, 1.0f });
				color += 1;
			}
			else {
				result.push_back({ 0.0f, 0.0f, 1.0f, 1.0f });
				color = 0;
			}
		}

		return result;
	}
};