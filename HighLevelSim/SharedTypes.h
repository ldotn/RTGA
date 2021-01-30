#pragma once
#include <systemc.h>
#include "GlobalConstants.h"

using namespace sc_core;

// Typedefing all this to centralize the definitions and data widths
typedef sc_int<kCodeAddressBits> CodeAddress;
typedef sc_int<kMemoryAddressBits> MemoryAddress;

struct Pixel 
{
	sc_int<kPixelChannelBits> r, g, b;

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
	sc_int<kPixelCoordBits> x;
	sc_int<kPixelCoordBits> y;

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

typedef float Real; // So I can easily test float vs fixed point later on

typedef int Instruction; // TODO : Change this into a real struct