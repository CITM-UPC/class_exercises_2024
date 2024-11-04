#pragma once

#include <list>
#include <ranges>

template <class T>
class TreeExt {

private:
	T* _parent = nullptr;
	std::list<T> _children;

	std::list<T*> allItemsPtrs() const {
		std::list<T*> items{ static_cast<T*>(const_cast<TreeExt*>(this)) };
		for (const auto& child : _children) items.splice(items.end(), child.allItemsPtrs());
		return items;
	}

public:
	bool hasParent() const { return _parent != nullptr; }
	auto& parent() const { return *_parent; }
	auto children() const { return const_cast<std::list<T>&>(_children) | std::views::all; }
	auto allItems() const { return allItemsPtrs() | std::views::transform([](T* ptr) -> T& {return *ptr; }); }

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

	TreeExt& operator=(TreeExt&& other) noexcept = delete;

	virtual ~TreeExt() = default;

	template <typename ...Args>
	auto& emplaceChild(Args&&... args) {
		auto& child = _children.emplace_back(std::forward<Args>(args)...);
		child._parent = static_cast<T*>(this);
		return child;
	}

	void removeChild(const T& child) { return _children.remove(std::forward(child)); }
};
