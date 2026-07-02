#pragma once

#include <Cartridge.h>
#include <memory>
#include <array>
#include <cstdint>
#include <string>

class MemoryBus
{
public:
	MemoryBus();
	bool isInitialised() { return true; } // TODO: add check for memory bus
	void LoadBootROM(const std::string& path);

	uint8_t GetByte(uint16_t addr);
	void WriteByte(uint16_t addr, uint8_t data);

	const std::array<uint8_t, 256>& GetBootROM() const;

private:
	// memory that doesn't need to be it's own class
	std::array<uint8_t, 256> bootROM{};		// read boot rom into here
	std::array<uint8_t, 0x7F> HRAM{};		// High RAM :D
	Cartridge cartridge;

};