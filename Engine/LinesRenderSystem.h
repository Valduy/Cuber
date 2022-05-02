#pragma once

#include <SimpleMath.h>

#include "CameraComponent.h"
#include "LinesComponent.h"
#include "LinesRenderComponent.h"
#include "TransformComponent.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/VertexBuffer.h"

namespace engine {

class LinesRendererSystem : public Game::SystemBase {
public:
	struct MatrixData {
		DirectX::SimpleMath::Matrix mat;
	};

	struct ColorData {
		DirectX::SimpleMath::Vector4 color;
	};

	void Init(Game& game) override {
		SystemBase::Init(game);

#ifdef _DEBUG
		shader_.Init(&GetRenderer(), graph::LayoutDescriptor::kPosition3, L"../Engine/Shaders/LineShader.hlsl");
#else
		shader_.Init(&GetRenderer(), graph::LayoutDescriptor::kPosition3, L"Shaders/LineShader.hlsl");
#endif		

		for (auto it = GetIterator<TransformComponent, LinesComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			LinesComponent& lines_component = entity.Get<LinesComponent>();

			graph::VertexBuffer vb(
				lines_component.points.data(),
				sizeof(DirectX::SimpleMath::Vector3) * lines_component.points.size());
			vb.Init(&GetRenderer());

			graph::ConstantBuffer matrix_cb;
			matrix_cb.Init(&GetRenderer(), sizeof(MatrixData));

			graph::ConstantBuffer color_cb;
			color_cb.Init(&GetRenderer(), sizeof(MatrixData));

			entity.Add<LinesRenderComponent>([&] {
				return new LinesRenderComponent(vb, matrix_cb, color_cb);
			});
		}
	}

	void Update(float dt) override {
		auto camera_it = GetIterator<CameraComponent>();
		if (!camera_it.HasCurrent()) return;

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
			MatrixData matrix_data{ transform_matrix.Transpose() };
			ColorData color_data{ lines_component.color };
						
			lines_render_component.matrix_const_buffer.Update(&matrix_data);			
			lines_render_component.color_const_buffer.Update(&color_data);
		}
	}

	void Render() override {
		shader_.SetShader();

		for (auto it = GetIterator<LinesComponent, LinesRenderComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			LinesComponent& lines_component = entity.Get<LinesComponent>();
			LinesRenderComponent& lines_render_component = entity.Get<LinesRenderComponent>();

			lines_render_component.vertex_buffer.SetBuffer(sizeof(DirectX::SimpleMath::Vector3));
			lines_render_component.matrix_const_buffer.VSSetBuffer(0);
			lines_render_component.color_const_buffer.VSSetBuffer(1);

			GetRenderer().GetContext().IASetPrimitiveTopology(lines_component.topology);
			GetRenderer().GetContext().Draw(lines_component.points.size(), 0);
		}
	}

private:
	graph::Shader shader_;
};

} // namespace engine