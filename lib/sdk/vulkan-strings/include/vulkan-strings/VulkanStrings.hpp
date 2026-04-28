
#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>




namespace vk {

namespace enums {
#ifdef VULKAN_STRINGS_GEN
#include <vulkan-strings/gen/strings-interface.inl>
#endif
}




namespace detail {

template <typename Enum>
struct FlagsTraits {
	static const int size;
	static const std::string_view names[];
};

struct ValueEntry {
	int32_t value;
	std::string_view name;
};

template <typename Enum>
struct ValuesTraits {
	static const int size;
	static const ValueEntry entries[];
};

extern std::string_view findValue(const ValueEntry * entries, int size, int32_t value) noexcept;

} // detail




template <typename Enum>
struct Flags {
	static_assert(std::is_enum_v<Enum>);

	int count() const noexcept
	{
		return detail::FlagsTraits<Enum>::size;
	}

	std::string_view name(int bit) const noexcept
	{
		if (int(bit) >= detail::FlagsTraits<Enum>::size) return {};
		return detail::FlagsTraits<Enum>::names[int(bit)];
	}
};


template <typename Enum>
struct Values {
	using U = std::make_unsigned_t<std::underlying_type_t<Enum>>;
	using Traits = detail::ValuesTraits<Enum>;

	static_assert(std::is_enum_v<Enum>);
	static_assert(std::is_same_v<U, uint32_t>);

	int count() const noexcept
	{
		return detail::ValuesTraits<Enum>::size;
	}

	Enum at(int i) const noexcept
	{
		return static_cast<Enum>(Traits::entries[i].value);
	}

	std::string_view name(U value) const noexcept
	{
		return detail::findValue(Traits::entries, Traits::size, value);
	}
};


} // vk
