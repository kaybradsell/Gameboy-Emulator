#pragma once

#include <cstdint>

uint16_t MergeBytes(uint8_t high, uint8_t low)
{
	return ((uint16_t)high << 8) | (uint16_t)low;
}

void SplitBytes(uint16_t byte, uint8_t& high, uint8_t& low)
{
	high = byte >> 8;
	low = (byte & 0xFF);
}

uint8_t GetHighByte(uint16_t byte)
{
	return (uint8_t)byte >> 8;
}

uint8_t GetLowByte(uint16_t byte)
{
	return (uint8_t)(byte & 0xFF);
}