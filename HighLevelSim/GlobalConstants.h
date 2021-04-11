#pragma once
#include "Helpers.h"

constexpr int kMaxCodeAddress = 255;
constexpr int kMaxMemoryAddress = 1024 * 1024 * 1024 - 1; // 1GB
constexpr int kMaxConstantBufferIndex = (1024 / 4) - 1; // 1KB, 32 bit entries (255 entries)
constexpr int kMaxPixelChannelValue = 255;
constexpr int kMaxPixelCoord = 2047;
constexpr int kMaxTexelOffset = 64 * 1024 * 1024 - 1; // 64 MB, forced by using 24 bits

struct TextureFormat
{
	enum
	{
		r8g8b8a8_unorm,
		r8g8b8_unorm, 
		r16g16_unorm, 
		f32,

		Count
	};

	static constexpr int kBits = max_to_bits(Count - 1);
};


constexpr int kCodeAddressBits = max_to_bits(kMaxCodeAddress);
constexpr int kMemoryAddressBits = max_to_bits(kMaxMemoryAddress);
constexpr int kConstantBufferIndexBits = max_to_bits(kMaxConstantBufferIndex);
constexpr int kPixelCoordBits = max_to_bits(kMaxPixelCoord);


constexpr int kHaltCodeAddress  = 0; // Moving the IP to this causes execution to halt
constexpr int kFirstCodeAddress = 1; // First valid code address

