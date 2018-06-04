#include "byteswap.hpp"

uint16_t
byte_swap(uint16_t v)
{
	return static_cast< uint16_t >( ( v << 8 ) | ( v >> 8 ) );
}

int16_t
byte_swap(int16_t v)
{
	return static_cast< int16_t >( ( v << 8 ) | ( ( v >> 8 ) & 0xFF ) );
}

uint32_t
byte_swap(uint32_t v)
{
	uint32_t val(( ( v << 8 ) & 0xFF00FF00 ) | ( ( v >> 8 ) & 0x00FF00FF ));
	return static_cast< uint32_t >( ( val << 16 ) | ( val >> 16 ) );
}

int32_t
byte_swap(int32_t v)
{
	int32_t val(( ( v << 8 ) & 0xFF00FF00 ) | ( ( v >> 8 ) & 0x00FF00FF ));
	return static_cast< int32_t >( ( val << 16 ) | ( ( val >> 16 ) & 0xFFFF ) );
}

uint64_t
byte_swap(uint64_t v)
{
	uint64_t val(( ( v << 8 ) & 0xFF00FF00FF00FF00ULL ) | ( ( v >> 8 ) & 0x00FF00FF00FF00FFULL ));

	val = ( ( val << 16 ) & 0xFFFF0000FFFF0000ULL ) | ( ( val >> 16 ) & 0x0000FFFF0000FFFFULL );
	return static_cast< uint64_t >( ( val << 32 ) | ( ( val >> 32 ) ) );
}

int64_t
byte_swap(int64_t v)
{
	int64_t val(( ( v << 8 ) & 0xFF00FF00FF00FF00ULL ) | ( ( v >> 8 ) & 0x00FF00FF00FF00FFULL ));
	val = ( ( val << 16 ) & 0xFFFF0000FFFF0000ULL ) | ( ( val >> 16 ) & 0x0000FFFF0000FFFFULL );
	return static_cast< int64_t >( ( val << 32 ) | ( ( val >> 32 ) & 0x00000000FFFFFFFFULL ) );
}
