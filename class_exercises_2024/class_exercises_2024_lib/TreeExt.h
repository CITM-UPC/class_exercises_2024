#pragma once

#include <list>
#include <ranges>

template <class T>
class TreeExt {

public:
	using IDType = uintptr_t;

private:
	T* _parent{ nullptr };
	std::list<T> _children;
	std::ranges::ref_view<std::list<T>> _children_view{ _children };

public:
	IDType id() const { return reinterpret_cast<IDType>(this); }

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

	auto itr() { auto iter = _parent->_children.begin();  for ( ; iter != _parent->_children.end() && iter->id() != this->id(); ++iter); return iter; }
	auto itr() const { auto iter = _parent->_children.cbegin();  for (;  iter != _parent->_children.cend() && iter->id() != this->id(); ++iter); return iter; }

	auto removeChild(IDType child_id) { return _children.remove_if([child_id](const T& child) { return child.id() == child_id; }); }
	auto removeChild(const T & child_to_remove) { return removeChild(child_to_remove.id()); }

	auto moveTo(T& new_parent) {
		auto iter = itr();
		auto old_parent = _parent;
		_parent = &new_parent;
		return new_parent._children.splice(new_parent._children.end(), old_parent->_children, iter);
	}
	
};
