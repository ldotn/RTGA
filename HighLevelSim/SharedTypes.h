#pragma once
#include <systemc.h>
#include "GlobalConstants.h"

using namespace sc_core;

inline uint32_t as_uint24(float x)
{
	assert(x >= 0 && x < (1 << 24));

	uint32_t raw = *(uint32_t*)&x;

	// On verilog this can be done without actual ops
	uint32_t mantissa = raw & ((1 << 23) - 1);
	uint32_t exp = raw & (255 << 22);

	// Bit hackery! Ints up to 2^24 are accurately stored on floats and as such can be easily converted back
	return mantissa >> (23 - exp - 127) | (1 << (exp - 127));
}

// Typedefing all this to centralize the definitions and data widths
typedef sc_uint<kCodeAddressBits> CodeAddress;
typedef sc_uint<kMemoryAddressBits> MemoryAddress;
typedef sc_uint<kConstantBufferIndexBits> CBIndex;

struct Pixel 
{
	sc_uint<8> r, g, b;

	bool operator==(const Pixel&) const = default;
};

inline void sc_trace(sc_trace_file* file, const Pixel& px, const std::string& str)
{
	sc_trace(file, px.r, str + "_r");
	sc_trace(file, px.g, str + "_g");
	sc_trace(file, px.b, str + "_b");
}

inline ostream& operator<<(ostream& os, Pixel const& px)
{
	os << px.r << px.g << px.b;
	return os;
}

struct PixelQuad
{
	Pixel data[4];

	bool operator==(const PixelQuad& rhs) const
	{
		return (data[0] == rhs.data[0]) &&
			   (data[1] == rhs.data[1]) &&
			   (data[2] == rhs.data[2]) &&
			   (data[3] == rhs.data[3]);
	}
};

inline void sc_trace(sc_trace_file* file, const PixelQuad& quad, const std::string& str)
{
	sc_trace(file, quad.data[0], str + "_00");
	sc_trace(file, quad.data[1], str + "_01");
	sc_trace(file, quad.data[2], str + "_10");
	sc_trace(file, quad.data[3], str + "_11");
}

inline ostream& operator<<(ostream& os, PixelQuad const& quad)
{
	os << quad.data[0] 
	   << quad.data[1] 
	   << quad.data[2] 
	   << quad.data[3];
	return os;
}

struct PixelCoord
{
	sc_uint<kPixelCoordBits> x;
	sc_uint<kPixelCoordBits> y;

	bool operator==(const PixelCoord&) const = default;
};

inline void sc_trace(sc_trace_file* file, const PixelCoord& coord, const std::string& str)
{
	sc_trace(file, coord.x, str + "_x");
	sc_trace(file, coord.y, str + "_y");
}

inline ostream& operator<<(ostream& os, PixelCoord const& coord)
{
	os << coord.x << coord.y;
	return os;
}

constexpr int kfloatBitSize = 8 * sizeof(float);
extern sc_trace_file * gTraceFile;