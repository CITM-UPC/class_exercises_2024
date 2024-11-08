#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "TreeExt.h"
#include "Transform.h"
#include "Texture.h"
#include "Material.h"
#include "BoundingBox.h"
#include "Mesh.h"

class GraphicObject :  public TreeExt<GraphicObject> {
	
	Transform _transform;
	std::shared_ptr<Material> _material_ptr;
	std::shared_ptr<Mesh> _mesh_ptr;

public:
	const auto& transform() const { return _transform; }
	auto& transform() { return _transform; }
	
	const auto& material() const { return *_material_ptr; }
	auto& material() { return *_material_ptr; }
	void setDefaultMaterial() { _material_ptr = std::make_shared<Material>(); }
	void setMaterial(const std::shared_ptr<Material>& material_ptr) { _material_ptr = material_ptr; }
	bool hasMaterial() const { return _material_ptr != nullptr; }

	const auto& mesh() const { return *_mesh_ptr; }
	auto& mesh() { return *_mesh_ptr; }
	void setMesh(const std::shared_ptr<Mesh>& mesh_ptr) { _mesh_ptr = mesh_ptr; }
	bool hasMesh() const { return _mesh_ptr != nullptr; }

	Transform worldTransform() const { return isRoot() ? _transform : parent().worldTransform() * _transform; }

	BoundingBox localBoundingBox() const;
	BoundingBox boundingBox() const { return _transform.mat() * localBoundingBox(); }

	BoundingBox worldBoundingBox() const;

	void draw() const;
};
