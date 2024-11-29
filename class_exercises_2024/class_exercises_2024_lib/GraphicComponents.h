#pragma once

#include "GraphicObject.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

class GraphicComponent : public GraphicObject::IComponent {

	GraphicObject& _owner;

protected:
	std::string _name;

public:
	explicit GraphicComponent(GraphicObject& owner) : _owner(owner) {}
	const std::string& name() const override { return _name; }
	const GraphicObject& owner() const override { return _owner; }
	GraphicObject& owner() override { return _owner; }

};

class GraphicDrawable : public GraphicObject::IDrawable {
	GraphicComponent _component;

public:
	explicit GraphicDrawable(GraphicObject& owner) : _component(owner) {}
	const std::string& name() const override { return _component.name(); }
	const GraphicObject& owner() const override { return _component.owner(); }
	GraphicObject& owner() override { return _component.owner(); }
};


class MeshComponent : public GraphicDrawable {
	
	std::shared_ptr<Material> _material = std::make_shared<Material>();
	std::shared_ptr<Mesh> _mesh = std::make_shared<Mesh>();

public:
	explicit MeshComponent(GraphicObject& owner) : GraphicDrawable(owner) {}
	void draw() const override;
	const BoundingBox& boundingBox() const override { return _mesh->boundingBox(); }

	const auto& material() const { return *_material; }
	auto& material() { return *_material; }

	const auto& mesh() const { return *_mesh; }
	auto& mesh() { return *_mesh; }

	void setMaterial(const std::shared_ptr<Material>& material) { _material = material; }
	void setMesh(const std::shared_ptr<Mesh>& mesh) { _mesh = mesh; }
};


class TransformComponent : public GraphicComponent {
	Transform _transform;
public:
	explicit TransformComponent(GraphicObject& owner) : GraphicComponent(owner) {}
	const auto& transform() const { return _transform; }
	auto& transform() { return _transform; }
};