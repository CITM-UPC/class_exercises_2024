#include "ModelImporter.h"
#include <map>
#include <filesystem>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include "ImageImporter.h"
#include "GraphicComponents.h"
using namespace std;
namespace fs = std::filesystem;

static mat4 aiMat4ToMat4(const aiMatrix4x4& aiMat) {
	mat4 mat;
	for (int c = 0; c < 4; ++c) for (int r = 0; r < 4; ++r) mat[c][r] = aiMat[r][c];
	return mat;
}

GraphicObject graphicObjectFromNode(const aiScene& scene, const aiNode& node, const vector<shared_ptr<Mesh>>& meshes, const vector<shared_ptr<Material>>& materials) {
	GraphicObject obj;

	obj.transform() = aiMat4ToMat4(node.mTransformation);

	for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
		const auto meshIndex = node.mMeshes[i];
		const auto materialIndex = scene.mMeshes[meshIndex]->mMaterialIndex;

		auto& child = i==0 ? obj : obj.emplaceChild();
		auto& meshComponent = child.emplaceComponent<MeshComponent>();
		meshComponent.setMesh(meshes[meshIndex]);
		meshComponent.setMaterial(materials[materialIndex]);
	}

	for (unsigned int i = 0; i < node.mNumChildren; ++i) obj.emplaceChild(graphicObjectFromNode(scene, *node.mChildren[i], meshes, materials));

	return obj;
}

static vector<shared_ptr<Mesh>> createMeshesFromFBX(const aiScene& scene) {
	vector<shared_ptr<Mesh>> meshes;
	for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
		const aiMesh* fbx_mesh = scene.mMeshes[i];
		auto mesh_ptr = make_shared<Mesh>();

		vector<unsigned int> indices(fbx_mesh->mNumFaces * 3);
		for (unsigned int j = 0; j < fbx_mesh->mNumFaces; ++j) {
			indices[j * 3 + 0] = fbx_mesh->mFaces[j].mIndices[0];
			indices[j * 3 + 1] = fbx_mesh->mFaces[j].mIndices[1];
			indices[j * 3 + 2] = fbx_mesh->mFaces[j].mIndices[2];
		}

		mesh_ptr->load(reinterpret_cast<const glm::vec3*>(fbx_mesh->mVertices), fbx_mesh->mNumVertices, indices.data(), indices.size());
		if (fbx_mesh->HasTextureCoords(0)) {
			vector<glm::vec2> texCoords(fbx_mesh->mNumVertices);
			for (unsigned int j = 0; j < fbx_mesh->mNumVertices; ++j) texCoords[j] = glm::vec2(fbx_mesh->mTextureCoords[0][j].x, fbx_mesh->mTextureCoords[0][j].y);
			mesh_ptr->loadTexCoords(texCoords.data(), texCoords.size());
		}
		if (fbx_mesh->HasNormals()) mesh_ptr->loadNormals(reinterpret_cast<glm::vec3*>(fbx_mesh->mNormals), fbx_mesh->mNumVertices);
		if (fbx_mesh->HasVertexColors(0)) {
			vector<glm::u8vec3> colors(fbx_mesh->mNumVertices);
			for (unsigned int j = 0; j < fbx_mesh->mNumVertices; ++j) colors[j] = glm::u8vec3(fbx_mesh->mColors[0][j].r * 255, fbx_mesh->mColors[0][j].g * 255, fbx_mesh->mColors[0][j].b * 255);
			mesh_ptr->loadColors(colors.data(), colors.size());
		}
		meshes.push_back(mesh_ptr);
	}
	return meshes;
}

static vector<shared_ptr<Material>> createMaterialsFromFBX(const aiScene& scene, const fs::path& basePath) {

	vector<shared_ptr<Material>> materials;
	map<string, shared_ptr<Image>> images;

	for (unsigned int i = 0; i < scene.mNumMaterials; ++i) {
		const auto* fbx_material = scene.mMaterials[i];
		auto material = make_shared<Material>();

		if (fbx_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString texturePath;
			fbx_material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			const string textureFileName = fs::path(texturePath.C_Str()).filename().string();
			const auto image_itr = images.find(textureFileName);
			if (image_itr != images.end()) material->texture.setImage(image_itr->second);
			else {
				auto image = ImageImporter::loadFromFile( (basePath/textureFileName).string() );
				images.insert({ textureFileName, image });
				material->texture.setImage(image);
			}

			auto uWrapMode = aiTextureMapMode_Wrap;
			auto vWrapMode = aiTextureMapMode_Wrap;
			fbx_material->Get(AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0), uWrapMode);
			fbx_material->Get(AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0), vWrapMode);
			assert(uWrapMode == aiTextureMapMode_Wrap);
			assert(vWrapMode == aiTextureMapMode_Wrap);

			unsigned int flags = 0;
			fbx_material->Get(AI_MATKEY_TEXFLAGS_DIFFUSE(0), flags);
			assert(flags == 0);
		}

		aiColor4D color;
		fbx_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->color = color4(color.r * 255, color.g * 255, color.b * 255, color.a * 255);

		materials.push_back(material);
	}
	return materials;
}

GraphicObject ModelImporter::loadFromFile(const std::string& path) {
	const aiScene* fbx_scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_FlipUVs);
	const auto meshes = createMeshesFromFBX(*fbx_scene);
	const auto materials = createMaterialsFromFBX(*fbx_scene, fs::absolute(path).parent_path());
	GraphicObject fbx_obj = graphicObjectFromNode(*fbx_scene, *fbx_scene->mRootNode, meshes, materials);
	aiReleaseImport(fbx_scene);
	return fbx_obj;
}

