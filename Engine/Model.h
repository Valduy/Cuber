#pragma once

#include "Mesh.h"
#include "CoreErrors.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

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

	static std::error_code LoadObj(const std::string& path, Model* model) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FlipWindingOrder | aiProcess_CalcTangentSpace);

		if (scene == nullptr) {
			return CoreErrors::kFailToLoadResource;
		}
		if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
			return CoreErrors::kResourceIsInvalid;
		}

		ProcessNode(*model, scene, scene->mRootNode);
		return CoreErrors::kSuccess;
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
			vertex.position = ToVector3(mesh->mVertices[i]);

			if (mesh->HasNormals()) {
				vertex.normal = ToVector3(mesh->mNormals[i]);
			}

			if (mesh->mTextureCoords[0] != nullptr) {
				vertex.texture_coords = ToVector2(mesh->mTextureCoords[0][i]);
				vertex.tangent = ToVector3(mesh->mTangents[i]);
				vertex.binormal = ToVector3(mesh->mBitangents[i]);
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

		return Mesh(vertices, indices);
	}

	static DirectX::SimpleMath::Vector2 ToVector2(aiVector3D vector) {
		return { vector.x, vector.y };
	}

	static DirectX::SimpleMath::Vector3 ToVector3(aiVector3D vector) {
		return { vector.x, vector.y, vector.z };
	}
};

} // namespace engine