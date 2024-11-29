#pragma once

#include <list>
#include <memory>
#include <ranges>
#include <stdexcept>


template <class TBase>
class PolyList {

	std::list<std::unique_ptr<TBase>> _list;

public:
	PolyList() = default;
	PolyList(const PolyList& other) = delete;
	PolyList& operator=(const PolyList& other) = delete;
	PolyList(PolyList&& other) = default;
	PolyList& operator=(PolyList&& other) = default;
	virtual ~PolyList() = default;

	template <class TDerived, typename ...Args>
	TDerived& emplace(Args... args) {
		static_assert(std::is_base_of_v<TBase, TDerived>, "TDerived must be derived from TBase");
		auto& uptr = _list.emplace_back(std::make_unique<TDerived>(std::forward<Args>(args)...));
		return static_cast<TDerived&>(*uptr);
	}

	template <class TDerived>
	bool hasType() const {
		for (auto& uptr : _list) if (dynamic_cast<TDerived*>(uptr.get())) return true;
		return false;
	}

	template <class TDerived>
	TDerived& oneOfType() {
		for (auto& uptr : _list) if (dynamic_cast<TDerived*>(uptr.get())) return static_cast<TDerived&>(*uptr);
		throw std::out_of_range("");
	}

	template <class TDerived>
	const TDerived& oneOfType() const {
		for (const auto& uptr : _list) if (dynamic_cast<const TDerived*>(uptr.get())) return static_cast<const TDerived&>(*uptr);
		throw std::out_of_range("");
	}

	template <class TDerived>
	auto allOfType() { return _list
		| std::views::filter([](auto& uptr) { return dynamic_cast<TDerived*>(uptr.get()); })
		| std::views::transform([](auto& uptr) -> TDerived& { return static_cast<TDerived&>(*uptr); });
	}

	template <class TDerived>
	auto allOfType() const {
		return _list
			| std::views::filter([](const auto& uptr) { return dynamic_cast<const TDerived*>(uptr.get()); })
			| std::views::transform([](const auto& uptr) -> const TDerived& { return static_cast<const TDerived&>(*uptr); });
	}

};