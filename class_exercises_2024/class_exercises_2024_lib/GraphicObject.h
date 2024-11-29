#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "TreeExt.h"
#include "Cache.h"
#include "Transform.h"
#include "Texture.h"
#include "Material.h"
#include "BoundingBox.h"
#include "PolyList.h"

class GraphicObject :  public TreeExt<GraphicObject> {

public:
	class IComponent {
	public:
		virtual const std::string& name() const = 0;
		virtual const GraphicObject& owner() const = 0;
		virtual GraphicObject& owner() = 0;
	};

	class IDrawable : public IComponent {
	public:
		virtual const BoundingBox& boundingBox() const = 0;
		virtual void draw() const = 0;
	};

private:
	PolyList<IComponent> _components;
	mutable Cache<BoundingBox> _cached_boundingBox;

public:

	GraphicObject();
	GraphicObject(const GraphicObject& other) = delete;
	GraphicObject& operator=(const GraphicObject& other) = delete;
	GraphicObject(GraphicObject&& other) = default;
	GraphicObject& operator=(GraphicObject&& other) = default;
	virtual ~GraphicObject() = default;

	template <class TComponent>
	TComponent& emplaceComponent() {  return _components.emplace<TComponent>(std::ref(*this)); }

	template <class TComponent>
	bool hasComponent() const { return _components.hasType<TComponent>(); }

	template <class TComponent>
	TComponent& getComponent() { return _components.oneOfType<TComponent>(); }

	template <class TComponent>
	const TComponent& getComponent() const { return _components.oneOfType<TComponent>(); }

	const Transform& transform() const;
	Transform& transform();

	Transform worldTransform() const { return isRoot() ? transform() : parent().worldTransform() * transform(); }

	const BoundingBox& localBoundingBox() const;
	BoundingBox boundingBox() const { return transform().mat() * localBoundingBox(); }

	void draw() const;
	void cleanCache() const;
	static std::pair<int, int> cacheHits();

	bool isDirty() const { for(const auto& child : children()) if (child.isDirty()) return true; return  transform().dirty(); }
};
