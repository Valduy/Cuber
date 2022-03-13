#pragma once

#include <SimpleMath.h>

#include "CameraComponent.h"
#include "LinesComponent.h"
#include "LinesRenderComponent.h"
#include "TransformComponent.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/VertexBuffer.h"

class LinesRendererSystem : public engine::Game::SystemBase {
public:
	struct MatrixData {
		DirectX::SimpleMath::Matrix mat;
	};

	struct ColorData {
		DirectX::SimpleMath::Vector4 color;
	};

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		shader_.Init(&GetRenderer(), graph::LayoutDescriptor::kPosition3, L"../Shaders/LineShader.hlsl");

		for (auto it = GetIterator<TransformComponent, LinesComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			LinesComponent& lines_component = entity.Get<LinesComponent>();

			graph::VertexBuffer vb(
				lines_component.points.data(), 
				sizeof(DirectX::SimpleMath::Vector3) * lines_component.points.size());
			vb.Init(&GetRenderer());

			graph::ConstantBuffer matrix_cb(sizeof(MatrixData));
			matrix_cb.Init(&GetRenderer());

			graph::ConstantBuffer color_cb(sizeof(MatrixData));
			color_cb.Init(&GetRenderer());

			entity.Add<LinesRenderComponent>([&] {
				return new LinesRenderComponent(vb, matrix_cb, color_cb);
			});
		}
	}

	void Render() override {
		shader_.SetShader();

		for (auto camera_it = GetIterator<CameraComponent>(); camera_it.HasCurrent(); camera_it.Next()) {
			ecs::Entity& camera = camera_it.Get();
			CameraComponent& camera_component = camera.Get<CameraComponent>();

			auto it = GetIterator<TransformComponent, LinesComponent, LinesRenderComponent>();
			for (; it.HasCurrent(); it.Next()) {
				ecs::Entity& entity = it.Get();
				TransformComponent& transform_component = entity.Get<TransformComponent>();
				LinesComponent& lines_component = entity.Get<LinesComponent>();
				LinesRenderComponent& lines_render_component = entity.Get<LinesRenderComponent>();

				DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
				DirectX::SimpleMath::Matrix camera_matrix = camera_component.GetCameraMatrix();
				DirectX::SimpleMath::Matrix transform_matrix = model_matrix * camera_matrix;
				MatrixData matrix_data { transform_matrix.Transpose() };
				ColorData color_data{ lines_component.color };

				lines_render_component.vertex_buffer.SetBuffer(sizeof(DirectX::SimpleMath::Vector3));
				lines_render_component.matrix_const_buffer.SetBuffer(0);
				lines_render_component.matrix_const_buffer.Update(&matrix_data);
				lines_render_component.color_const_buffer.SetBuffer(1);
				lines_render_component.color_const_buffer.Update(&color_data);

				GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
				GetRenderer().GetContext().Draw(lines_component.points.size(), 0);
			}
		}
	}

private:
	graph::Shader shader_;
};