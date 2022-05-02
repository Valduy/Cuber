#pragma once

#include "MaterialData.h"
#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "../GraphicEngine/ConstantBuffer.h"
#include "../GraphicEngine/Texture.h"
#include "ModelComponent.h"
#include "ModelTransformData.h"
#include "RenderComponent.h"

class RenderPreparationSystem : public engine::Game::SystemBase {
public:
	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		using namespace engine;
		using namespace graph;

		for (auto it = GetIterator<TransformComponent, ModelComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			auto& model_component = entity.Get<ModelComponent>();

			entity.Add<RenderComponent>([&] {
				return new RenderComponent(
					CreateMeshBuffers(model_component.model),
					CreateConstantBuffer<sizeof(ModelTransformData)>(),
					CreateConstantBuffer<sizeof(MaterialData)>(),
					CreateTexture(model_component.diffuse),
					CreateTexture(model_component.normal),
					CreateTexture(model_component.specular));
				});
		}
	}

	void Update(float dt) override {
		using namespace engine;
		using namespace DirectX::SimpleMath;

		auto camera_it = GetIterator<CameraComponent>();
		if (!camera_it.HasCurrent()) return;

		auto& camera = camera_it.Get();
		auto& camera_component = camera.Get<CameraComponent>();

		auto it = GetIterator<RenderComponent, TransformComponent, ModelComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& model = it.Get();
			auto& render_component = model.Get<RenderComponent>();
			auto& transform_component = model.Get<TransformComponent>();
			auto& model_component = model.Get<ModelComponent>();

			Matrix model_matrix = transform_component.GetModelMatrix();
			Matrix camera_matrix = camera_component.GetCameraMatrix();
			Matrix world_view_proj_matrix = model_matrix * camera_matrix;
			ModelTransformData model_transform_data{
				model_matrix.Transpose(),
				world_view_proj_matrix.Transpose(),
				model_matrix.Transpose().Invert().Transpose(),
			};
			render_component.transform_buffer.Update(&model_transform_data);

			MaterialData material_data{
				model_component.material.ambient,
				model_component.material.shininess,
				model_component.material.specular
			};
			render_component.material_buffer.Update(&material_data);
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

	template<size_t sizemem>
	graph::ConstantBuffer CreateConstantBuffer() {
		graph::ConstantBuffer material_buffer(sizemem);
		material_buffer.Init(&GetRenderer());
		return material_buffer;
	}

	graph::Texture CreateTexture(DirectX::ScratchImage& image) {
		graph::Texture texture;
		texture.Init(&GetRenderer(), image);
		return texture;
	}
};