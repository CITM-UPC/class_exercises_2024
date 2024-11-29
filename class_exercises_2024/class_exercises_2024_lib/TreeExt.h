#pragma once

#include <list>
#include <ranges>

template <class T>
class TreeExt {

private:
	T* _parent{ nullptr };
	std::list<T> _children;
	std::ranges::ref_view<std::list<T>> _children_view{ _children };

public:
	bool hasParent() const { return _parent != nullptr; }
	const auto& parent() const { return *_parent; }
	auto& parent() { return *_parent; }
	const auto& children() const { return _children; }
	const auto& children() { return _children_view; }

	bool isRoot() const { return !hasParent(); }
	auto& root() const { return hasParent() ? parent().root() : *this; }
	
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

	TreeExt& operator=(TreeExt&& other) noexcept {
		_children = std::move(other._children);
		for (auto& child : _children) child._parent = static_cast<T*>(this);
		return *this;
	}

	virtual ~TreeExt() = default;

	template <typename ...Args>
	auto& emplaceChild(Args&&... args) {
		auto& child = _children.emplace_back(std::forward<Args>(args)...);
		child._parent = static_cast<T*>(this);
		return child;
	}

	void removeChild(const T& child) { return _children.remove(std::forward(child)); }
};
