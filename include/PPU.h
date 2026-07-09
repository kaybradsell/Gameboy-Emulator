#pragma once

#include <array>
#include <cstdint>

class PPU
{
public:
    struct LCDRegs
    {
        uint8_t LCDC = 0;   // 0xFF40
        uint8_t STAT = 0;   // 0xFF41
        uint8_t SCY = 0;    // 0xFF42
        uint8_t SCX = 0;    // 0xFF43
        uint8_t LY = 0;     // 0xFF44
        uint8_t LYC = 0;    // 0xFF45
        uint8_t DMA = 0;    // 0xFF46
        uint8_t BGP = 0;    // 0xFF47
        uint8_t OBP0 = 0;   // 0xFF48
        uint8_t OBP1 = 0;   // 0xFF49
        uint8_t WY = 0;     // 0xFF4A
        uint8_t WX = 0;     // 0xFF4B
    };

    struct Tile
    {
        uint8_t pixels[8][8];
    };

	PPU();
    void Step(uint8_t cycles);

	const uint8_t GetByte(uint16_t addr) const;
	void WriteByte(uint16_t addr, uint8_t data);

    uint8_t GetIOReg(uint16_t addr);
    void WriteIOReg(uint16_t addr, uint8_t data);

    const LCDRegs& GetRegs() const;
	const std::array<uint8_t, 0x2000>& GetVram() const;

    const Tile& GetTile(uint16_t index) const;

private:
	std::array<uint8_t, 0x2000> vram{};
    LCDRegs regs;
    int cycleCounter = 0;
};