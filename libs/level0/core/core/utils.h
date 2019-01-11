/// \file	core\core_utils.h
/// \brief	Declares the core utils class. 
/// \details	
///		just some general helper macros and functions

#pragma once
#ifndef CORE_UTILS_H_
#define CORE_UTILS_H_

#include <type_traits>

/// \brief	Returns the number of elements in a staic array. 
/// \param	array	The array must be static.
/// \todo	Google has a version that detects it is a static array, this just bugs out if not... 
#define NUM_ARRAY_ELEMENTS( array ) ( sizeof( (array) ) / sizeof( (array)[0] ) )

/// \brief	marks the variable unused. 
/// \param	i	The variable to mark as unused. 
#define UNUSED( i ) (void)i

/// \def	MACRO_TEXT(x) #x
/// \brief	Converts parameter in text. 
/// \param	x	The parameter. 
#define MACRO_TEXT(x) #x

/// \brief	returns the text of the value of the parameter. 
/// \param	x	The parameter. 
#define MACRO_VALUE(x) MACRO_TEXT(x)

namespace Core {

/// \brief	return the bit of the number.
/// \param	num	returns the bit representing this index.
template<typename T>
constexpr auto Bit(T num) -> T
{
	static_assert(std::is_integral<T>::value, "Integral required.");
	return (T(1) << (num));
}

/// \brief	Query if 'iNum' is power of two. 
/// \param	iNum	Number to test. 
/// \return	true if pow 2, false if not. 
inline bool isPow2(unsigned int iNum)
{
	return ((iNum & (iNum - 1)) == 0);
}


/// \brief	Gets the next pow 2. 
/// \param	iNum	Number to get next pow 2 off. 
/// \return	The next pow 2 of iNum. 
inline unsigned int nextPow2(unsigned int iNum)
{
	iNum -= 1;
	iNum |= iNum >> 16;
	iNum |= iNum >> 8;
	iNum |= iNum >> 4;
	iNum |= iNum >> 2;
	iNum |= iNum >> 1;

	return iNum + 1;
}


/// \brief	Align input to align val. 
/// \param	k		The value to align. 
/// \param	align	The alignment boundary. 
/// \return	k aligned to align. 
template<typename T, typename T2>
inline T alignTo(T k, T2 align)
{
	return ((k + align - 1) & ~(align - 1));
}

namespace detail {

template<typename T, std::size_t...Is>
std::array<T, sizeof...(Is)> make_array(const T& value, std::index_sequence<Is...>)
{
	return {{(static_cast<void>(Is), value)...}};
}
}

template<std::size_t N, typename T>
std::array<T, N> make_array(const T& value)
{
	return detail::make_array(value, std::make_index_sequence<N>());
}

// is an enum a scoped enum (enum class) or a old fashioned one?
// from StackOverflow https://stackoverflow.com/questions/15586163/c11-type-trait-to-differentiate-between-enum-class-and-regular-enum
template<typename E>
using is_scoped_enum = std::integral_constant<
		bool,
		std::is_enum<E>::value && !std::is_convertible<E, int>::value>;

} // end namespace Core

template<typename E>
constexpr auto is_bitmask_enum(E) -> bool
{
	// normal enums allow the bitwise ops regardless
	// for scoped enums (enum class) only if this function is overridden
	if constexpr (Core::is_scoped_enum<E>::value == false)
	{
		return true;
	} else
	{
		return false;
	}
}

// the enum class bitwise operators are boosted outside the Core namespace
// so that they operator the same as the normal operators do
template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto operator|(E lhs, E rhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		using basetype = typename std::underlying_type<E>::type;
		return static_cast<E>(
				static_cast<basetype>(lhs) | static_cast<basetype>(rhs));
	}
}

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto operator&(E lhs, E rhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		using basetype = typename std::underlying_type<E>::type;
		return static_cast<E>(
				static_cast<basetype>(lhs) & static_cast<basetype>(rhs));
	}
}

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto operator^(E lhs, E rhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		using basetype = typename std::underlying_type<E>::type;
		return static_cast<E>(
				static_cast<basetype>(lhs) ^ static_cast<basetype>(rhs));
	}
}

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto operator~(E lhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		using basetype = typename std::underlying_type<E>::type;
		return static_cast<E>(~static_cast<basetype>(lhs));
	}
}


template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto operator|=(E& lhs, E rhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		using basetype = typename std::underlying_type<E>::type;
		lhs = static_cast<E>(
				static_cast<basetype>(lhs) | static_cast<basetype>(rhs));
		return lhs;
	}
}

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto operator&=(E& lhs, E rhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		using basetype = typename std::underlying_type<E>::type;
		lhs = static_cast<E>(
				static_cast<basetype>(lhs) & static_cast<basetype>(rhs));
		return lhs;
	}
}

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto operator^=(E& lhs, E rhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		using basetype = typename std::underlying_type<E>::type;
		lhs = static_cast<E>(
				static_cast<basetype>(lhs) ^ static_cast<basetype>(rhs));
		return lhs;
	}
}

namespace Core {

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto test_equal(E lhs, E rhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		return (lhs & rhs) == rhs;
	}
}

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto test_any(E lhs, E rhs)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		return bool(lhs & rhs);
	}
}


template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto zero()
{
	return static_cast<E>(0);
}

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto from_uint(typename std::underlying_type<E>::type v)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		return static_cast<E>(v);
	}
}

template<typename E, typename = typename std::enable_if<std::is_enum<E>{}>::type>
constexpr auto to_uint(E e)
{
	if constexpr (is_bitmask_enum(E{}))
	{
		using basetype = typename std::underlying_type<E>::type;
		return static_cast<basetype>(e);
	}
}

}	//namespace Core


#endif
