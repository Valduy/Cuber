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

		for (auto& [entity, transform_component, lines_component] : Filter<TransformComponent, LinesComponent>()) {
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
		auto it = Filter<CameraComponent>().GetIterator();
		if (!it.HasCurrent()) return;
		auto& [camera, camera_component] = it.Get();

		for (auto& node : Filter<TransformComponent, LinesComponent, LinesRenderComponent>()) {
			auto& [entity, transform_component, lines_component, lines_render_component] = node;

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

		for (auto& [entity, lines_component, lines_render_component] : Filter<LinesComponent, LinesRenderComponent>()) {
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