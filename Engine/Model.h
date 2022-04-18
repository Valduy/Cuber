#pragma once

#include <winerror.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>

#include "Mesh.h"

namespace engine {

class Model {
public:
	const std::vector<Mesh>& GetMeshes() const {
		return meshes_;
	}

	Model()
		: meshes_{}
	{}

	Model(std::vector<Mesh> meshes)
		: meshes_(meshes)
	{}

	static HRESULT Load(Model& model, const std::string& path) {
		model.meshes_.clear();
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FlipWindingOrder);

		if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
			std::cout << importer.GetErrorString() << std::endl;
			return E_FAIL;
		}

		ProcessNode(model, scene, scene->mRootNode);
		return S_OK;
	}

private:
	std::vector<Mesh> meshes_;

	static void ProcessNode(Model& model, const aiScene* scene, const aiNode* node) {
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			model.meshes_.push_back(ProcessMesh(scene, mesh));
		}
		
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(model, scene, node->mChildren[i]);
		}
	}

	static Mesh ProcessMesh(const aiScene* scene, const aiMesh* mesh)
	{
		using namespace DirectX::SimpleMath;
		std::vector<Vertex> vertices;
		std::vector<int> indices;

		for (size_t i = 0; i < mesh->mNumVertices; ++i) {
			Vertex vertex;
			vertex.position = Vector3 {
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			};

			if (mesh->HasNormals()) {
				vertex.normal = Vector3{
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};
			}

			if (mesh->mTextureCoords[0] != nullptr) {
				vertex.texture_coords = Vector2 {
					 mesh->mTextureCoords[0][i].x,
					 mesh->mTextureCoords[0][i].y,
				};
			} else {
				vertex.texture_coords = Vector2 { 0.0f, 0.0f };
			}

			vertices.push_back(vertex);
		}

		for (size_t i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace face = mesh->mFaces[i];

			for (size_t j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}
		
		// TODO: now I load only diffuse texture, but i should load all necessary textures in future.
		return Mesh(vertices, indices);
	}
};

} // namespace engine