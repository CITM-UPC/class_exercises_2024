#include "GraphicObject.h"
#include "GraphicComponents.h"
#include <GL/glew.h>
using namespace std;

GraphicObject::GraphicObject() {
	emplaceComponent<TransformComponent>();
}

Transform& GraphicObject::transform() { return getComponent<TransformComponent>().transform(); }
const Transform& GraphicObject::transform() const { return getComponent<TransformComponent>().transform(); }


void GraphicObject::draw() const {
	glPushMatrix();
	glMultMatrixd(transform().data());

	for (const auto& drawable : _components.allOfType<IDrawable>()) drawable.draw();
	for (const auto& child : children()) child.draw();

	glPopMatrix();
}

enum ClipSide { Inside=1, Outside=-1, Intersects=0 };

template <typename t>
static auto signOf(t x) { return (x > 0) - (x < 0); }

static inline auto sideOfPlane(const Plane& plane, const Point& point) {
	return signOf(glm::dot(point.p - plane.point.p, plane.normal.v));
}

static inline auto planeAllignedBoundingBox(const Plane& plane, const BoundingBox& bbox) {
	auto alligned_bbox = bbox;
	if (plane.normal.v.x < 0) swap(alligned_bbox.min.x, alligned_bbox.max.x);
	if (plane.normal.v.y < 0) swap(alligned_bbox.min.y, alligned_bbox.max.y);
	if (plane.normal.v.z < 0) swap(alligned_bbox.min.z, alligned_bbox.max.z);
	return alligned_bbox;
}

static inline auto sideOfPlane(const Plane& plane, const BoundingBox& bbox) {
	const auto bbox_points = planeAllignedBoundingBox(plane, bbox);
	const auto s = sideOfPlane(plane, bbox_points.min);
	return (s == sideOfPlane(plane, bbox_points.max)) ? s : ClipSide::Intersects;
}

static inline auto sideOfClipSpace(const BoundingBox& bbox, const list<Plane>& clipPlanes) {
	bool intersects = false;
	for (const auto& clipPlane : clipPlanes) {
		const auto side = sideOfPlane(clipPlane, bbox);
		if (side==ClipSide::Outside) return ClipSide::Outside;
		if (side==ClipSide::Intersects) intersects = true;
	}
	return intersects ? ClipSide::Intersects : ClipSide::Inside;
}

static int cache_hits = 0;
static int cache_misses = 0;

pair<int,int> GraphicObject::cacheHits() {
	return { cache_hits, cache_misses };
}

void GraphicObject::cleanDirtyCaches() const {
	cache_hits = 0;
	cache_misses = 0;
	if (isDirty()) {
		transform().clean();
		_cached_boundingBox.clean();
		for (const auto& child : children()) child.cleanDirtyCaches();
	}
}

const BoundingBox& GraphicObject::localBoundingBox() const {
	if (!_cached_boundingBox.isCached()) {
		cache_misses--;
		if (children().size()) {
			BoundingBox bbox = hasComponent<IDrawable>() ? getComponent<IDrawable>().boundingBox() : children().front().boundingBox();
			for (const auto& child : children()) bbox = bbox + child.boundingBox();
			_cached_boundingBox.setValue(bbox);
		}
		else _cached_boundingBox.setValue(hasComponent<IDrawable>() ? getComponent<IDrawable>().boundingBox() : BoundingBox());
	}
	else {
		cache_hits++;
	}
	return _cached_boundingBox.value();
}
