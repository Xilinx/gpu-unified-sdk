
#pragma once

#include <type_traits>




template <typename T>
class Scalar {
public:
	template <typename K>
	Scalar(const K & value) noexcept :
		value_(value)
	{
		static_assert(std::is_same_v<T, K>);
	}

	operator T() const noexcept { return value_; }

private:
	T value_;
};
