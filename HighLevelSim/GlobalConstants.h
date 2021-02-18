#pragma once
#include "Helpers.h"

constexpr int kMaxCodeAddress = 255;
constexpr int kMaxMemoryAddress = 1024 * 1024 * 1024 - 1; // 1GB
constexpr int kMaxConstantBufferIndex = (1024 * 1024 / 32) - 1; // 1KB, 32 bit entries (32768 entries)
constexpr int kMaxPixelChannelValue = 255;
constexpr int kMaxPixelCoord = 2047;
constexpr int kMaxTexelCoord = 255;
constexpr int kMaxTexelOffset = (kMaxTexelCoord + 1)*(kMaxTexelCoord + 1) - 1;
constexpr int kMaxTextureIndex = 31;

constexpr int kCodeAddressBits = max_to_bits(kMaxCodeAddress);
constexpr int kMemoryAddressBits = max_to_bits(kMaxMemoryAddress);
constexpr int kConstantBufferIndexBits = max_to_bits(kMaxConstantBufferIndex);
constexpr int kPixelChannelBits = max_to_bits(kMaxPixelChannelValue);
constexpr int kPixelCoordBits = max_to_bits(kMaxPixelCoord);
constexpr int kTexelCoordBits = max_to_bits(kMaxTexelCoord);
constexpr int kTexelOffsetBits = max_to_bits(kMaxTexelOffset);
constexpr int kBytesPerPixel = 3 * kPixelChannelBits / 8; // All textures are the same as the backbuffer (rgb8)
constexpr int kTextureIndexBits = max_to_bits(kMaxTextureIndex);

constexpr int kHaltCodeAddress  = 0; // Moving the IP to this causes execution to halt
constexpr int kFirstCodeAddress = 1; // First valid code address

