#pragma once

#include <list>
#include "readOnlyView.h"

template <class T>
class TreeExt {

private:
	T* _parent = nullptr;
	std::list<T> _children;

public:
	auto& parent() const { return *_parent; }
	auto children() const { return readOnlyListView<T>(_children); }

	auto& root() const { return _parent ? _parent->root() : *this; }
	bool isRoot() const { return !_parent; }

	TreeExt() = default;

	TreeExt(const TreeExt& other) : _children(other._children) {
		for (auto& child : _children) child._parent = static_cast<T*>(this);
	}

	TreeExt& operator=(const TreeExt& other) {
		_children = other._children;
		for (auto& child : _children) child._parent = static_cast<T*>(this);
		return *this;
	}

	TreeExt(TreeExt&& other) noexcept : _parent(other._parent), _children(std::move(other._children)) {
		for (auto& child : _children) child._parent = static_cast<T*>(this);
		other._parent = nullptr;
	}

	TreeExt& operator=(TreeExt&& other) noexcept = delete;

	virtual ~TreeExt() = default;

	template <typename ...Args>
	auto& emplaceChild(Args&&... args) {
		_children.emplace_back(std::forward<Args>(args)...);
		_children.back()._parent = static_cast<T*>(this);
		return _children.back();
	}

	void removeChild(const T& child) { return _children.remove(std::forward(child)); }
};
