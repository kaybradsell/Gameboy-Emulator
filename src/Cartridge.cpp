#include <Cartridge.h>
#include <stdexcept>
#include <fstream>
#include <iostream>

void Cartridge::LoadROM(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::binary);

	if (!file)
		throw std::runtime_error("[ERROR]: Cannot open " + filePath + " for Cartridge ROM.");

	rom = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
}

uint8_t Cartridge::GetByte(uint16_t addr)
{
	return rom[addr];
}

void Cartridge::WriteByte(uint16_t addr, uint8_t data)
{
	std::cout << "[NOTICE]: Cannot write to Cartridge ROM.\n";
}