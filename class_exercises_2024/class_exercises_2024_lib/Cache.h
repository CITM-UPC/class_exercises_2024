#pragma once

template <class T>
class Cache {
	T _value;
	bool _cached = false;

public:
	const T& value() const { return _value; }
	void setValue(const T& value) { _value = value; _cached = true; }
	void clean() { _cached = false; }
	bool isCached() const { return _cached; }
};
