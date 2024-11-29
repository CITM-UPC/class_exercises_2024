#pragma once

#include "types.h"

class Transform {

	mat4 _mat;
	mutable bool _dirty{ false };

public:
	const auto inv() const { return Transform(glm::inverse(_mat)); }
	const auto& mat() const { return _mat; }
	const auto& left() const { return reinterpret_cast<const vec3&>(_mat[0]); }
	const auto& up() const { return reinterpret_cast<const vec3&>(_mat[1]); }
	const auto& fwd() const { return reinterpret_cast<const vec3&>(_mat[2]); }
	const auto& pos() const { return reinterpret_cast<const vec3&>(_mat[3]); }
	auto& pos() { _dirty = true;  return reinterpret_cast<vec3&>(_mat[3]); }
	const auto* data() const { return &_mat[0][0]; }

	Transform() : _mat(1.0) {}
	Transform(const mat4& mat) : _mat(mat) {}

	void translate(const vec3& v);
	void rotate(double rads, const vec3& v);

	Transform operator*(const mat4& other) const { return Transform(_mat * other); }
	Transform operator*(const Transform& other) const { return Transform(_mat * other._mat); }

	bool dirty() const { return _dirty; }
	void clean() const { _dirty = false; }

};