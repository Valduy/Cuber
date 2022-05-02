#pragma once

#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "../GraphicEngine/ConstantBuffer.h"
#include "../Engine/CameraComponent.h"
#include "ModelComponent.h"
#include "ModelTransformData.h"

class UpdateMeshesSystem : public engine::Game::SystemBase {
public:
	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		using namespace engine;
		using namespace graph;

		for (auto it = GetIterator<ModelComponent>(); it.HasCurrent(); it.Next()) {
			auto& entity = it.Get();
			auto& model_component = entity.Get<ModelComponent>();
			model_component.model_buffers = CreateMeshBuffers(model_component.model);
			model_component.transform_buffer.Init(&GetRenderer(), sizeof(ModelTransformData));
		}
	}

	void Update(float dt) override {
		using namespace engine;

		DirectX::SimpleMath::Matrix camera_matrix;
		if (!TryGetCameraMatrix(&camera_matrix)) return;

		for (auto it = GetIterator<TransformComponent, ModelComponent>(); it.HasCurrent(); it.Next()) {
			auto& model = it.Get();
			auto& transform_component = model.Get<TransformComponent>();
			auto& model_component = model.Get<ModelComponent>();

			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
			DirectX::SimpleMath::Matrix world_view_proj_matrix = model_matrix * camera_matrix;
			ModelTransformData model_transform_data{
				model_matrix.Transpose(),
				world_view_proj_matrix.Transpose(),
				model_matrix.Transpose().Invert().Transpose(),
			};
			model_component.transform_buffer.Update(&model_transform_data);
		}
	}

private:
	std::vector<MeshBuffers> CreateMeshBuffers(const engine::Model& model) {
		std::vector<MeshBuffers> model_buffers;

		for (const engine::Mesh& mesh : model.GetMeshes()) {
			graph::VertexBuffer vb(mesh.vertices.data(), sizeof(engine::Vertex) * mesh.vertices.size());
			vb.Init(&GetRenderer());

			graph::IndexBuffer ib(mesh.indices.data(), mesh.indices.size());
			ib.Init(&GetRenderer());

			model_buffers.push_back({ vb, ib });
		}

		return model_buffers;
	}

	bool TryGetCameraMatrix(DirectX::SimpleMath::Matrix* camera_matrix) {
		using namespace engine;
		using namespace DirectX::SimpleMath;

		auto camera_it = GetIterator<CameraComponent>();
		if (!camera_it.HasCurrent()) return false;

		auto& camera = camera_it.Get();
		auto& camera_component = camera.Get<CameraComponent>();
		*camera_matrix = camera.Get<CameraComponent>().GetCameraMatrix();
		return true;
	}
};
