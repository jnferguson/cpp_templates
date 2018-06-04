#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include <algorithm>

template< typename T >
std::string
to_string(T i, uint8_t base)
{
	std::stringstream ss;

	ss << std::setfill('0') << std::setw(sizeof(T)*2);

	if (8 == base)
		ss << std::oct;
	else if (10 == base)
		ss << std::dec;
	else
		ss << std::hex;

	// uint8_t is an unsigned char
	// which causes this to work incorrectly
	// so the + causes an integer promotion...
	if (1 == sizeof(T)) 
		ss << +i;
	else 
		ss << i;

	return ss.str();
}

template< typename T >
std::string
to_hex_string(T i)
{

	return to_string(i, 16);
}

template< typename T >
std::string
to_dec_string(T i)
{
	return to_string(i, 10);
}

template< typename T >
std::string
to_oct_string(T i)
{
	return to_string(i, 8);
}

template< typename T, typename std::enable_if< std::is_integral< T >::value, bool >::type = true >
static T
convert_to_integral(const std::string& val)
{
	T				retval(0);
	std::string		fmt("%");
	signed int		rv(0);
	bool			sgn(std::is_signed< T >::value);

	if (typeid(T) == typeid(bool)) {
		std::string tmp(val);
	
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);

		if (! tmp.compare("1") || ! tmp.compare("yes")  || ! tmp.compare("true") || ! tmp.compare("on"))
			retval = true;
		else
			retval = false;

		return retval;
	}

	switch (sizeof(T)) {
		case sizeof(int8_t):
			fmt +=  "hh";
			break;

		case sizeof(int16_t):
			fmt += "h";
			break;

		case sizeof(int32_t):
			break;

		case sizeof(int64_t):
			fmt += "ll";
			break;

		default:
			throw std::runtime_error("Unknown/unsupported size integer specified!");
			break;
	}

	if (true == std::is_signed< T >::value)
		fmt += "i";
	else
		fmt += "u";

	rv = std::sscanf(val.c_str(), fmt.c_str(), &retval);

	if (1 != rv || EOF == rv)
		throw std::runtime_error("Error while calling std::sscanf()!");

	return retval;
}


template< typename T, typename std::enable_if< std::is_integral< T >::value, bool >::type = true >
static T
convert_to_integral(const char* val)
{
	std::string v(val);

	return convert_to_integral< T >(v);
}
