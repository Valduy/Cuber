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
	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		shader_.Init(&GetRenderer(), L"../Shaders/SolarShader.hlsl");

		auto it = GetIterator<ShapeComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			ShapeComponent& shape_component = entity.Get<ShapeComponent>();
			auto vertices = GetVertices(shape_component.points);

			graph::VertexBuffer vb(vertices.data(), sizeof(DirectX::SimpleMath::Vector4) * vertices.size());
			vb.Init(&GetRenderer());

			graph::IndexBuffer ib(shape_component.indexes.data(), shape_component.indexes.size());
			ib.Init(&GetRenderer());

			graph::ConstantBuffer cb(sizeof(ConstData));
			cb.Init(&GetRenderer());

			entity.Add<RenderComponent>([&] {
				return new RenderComponent(vertices, vb, ib, cb);
			});
		}
	}

	void Render() override {
		GetRenderer().BeginRender();
		shader_.SetShader();

		auto camera_it = GetIterator<CameraComponent>();
		for (; camera_it.HasCurrent(); camera_it.Next()) {
			ecs::Entity& camera = camera_it.Get();
			CameraComponent& camera_component = camera.Get<CameraComponent>();

			auto mesh_it = GetIterator<RenderComponent, TransformComponent>();
			for (; mesh_it.HasCurrent(); mesh_it.Next()) {
				ecs::Entity& mesh = mesh_it.Get();
				RenderComponent& render_component = mesh.Get<RenderComponent>();
				TransformComponent& transform_component = mesh.Get<TransformComponent>();

				DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
				DirectX::SimpleMath::Matrix camera_matrix = camera_component.GetCameraMatrix();
				DirectX::SimpleMath::Matrix transform_matrix = model_matrix * camera_matrix;
				ConstData data{ transform_matrix.Transpose() };

				render_component.vertex_buffer.SetBuffer();
				render_component.index_buffer.SetBuffer();
				render_component.constant_buffer.SetBuffer();
				render_component.constant_buffer.Update(&data);

				//GetRenderer().GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				GetRenderer().GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
				GetRenderer().GetContext()->DrawIndexed(
					render_component.index_buffer.GetSize(), 0, 0);
			}
		}

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
				result.push_back({ 1.0f, 1.0f, 0.0f, 1.0f });
				color += 1;
			}
			else {
				result.push_back({ 1.0f, 0.0f, 1.0f, 1.0f });
				color = 0;
			}
		}

		return result;
	}
};