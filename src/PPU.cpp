#include <PPU.h>
#include <stdexcept>
#include <iostream>

PPU::PPU()
{
	vram.fill(0);
}

void PPU::Step(uint8_t cycles)
{
    cycleCounter += (int)cycles;

    while (cycleCounter >= 456)
    {
        cycleCounter -= 456;
        regs.LY++;

        if (regs.LY > 153)
            regs.LY = 0;
    }

    // calc time, and if it's been 16.74ms ish, then draw
}

const uint8_t PPU::GetByte(uint16_t addr) const
{
	if (addr < 0x2000)
		return vram[addr];
	else
		throw new std::runtime_error("[ERROR]: Trying to access outside of Cartridge memory.");
}

void PPU::WriteByte(uint16_t addr, uint8_t data)
{
	if (addr < 0x2000)
		vram[addr] = data;
	else
		throw new std::runtime_error("[ERROR]: Trying to access outside of Cartridge memory.");
}

const PPU::Tile& PPU::GetTile(uint16_t index) const
{
    Tile tile{};
    // get the actual address in the VRAM
    uint16_t addr = index * 16;

    // from addr to addr + 16, where addr += 2 to account for two bytes
    for (int y = 0; y < 8; y++)
    {
        uint8_t low = GetByte(addr + y * 2);
        uint8_t high = GetByte(addr + y * 2 + 1);

        // merge each pixel to be high-low so like 01 or 11 or 10 or 00
        for (int x = 0; x < 8; x++)
        {
            uint8_t lowBit = (low >> (7 - x)) & 1;
            uint8_t highBit = (high >> (7 - x)) & 1;

            tile.pixels[y][x] = (highBit << 1) | lowBit;
        }
    }

    return tile;
}

uint8_t PPU::GetIOReg(uint16_t addr)
{
    switch (addr)
    {
    case 0xFF40: return regs.LCDC;
    case 0xFF41: return regs.STAT;
    case 0xFF42: return regs.SCY;
    case 0xFF43: return regs.SCX;
    case 0xFF44: return regs.LY;
    case 0xFF45: return regs.LYC;
    case 0xFF46: return regs.DMA;
    case 0xFF47: return regs.BGP;
    case 0xFF48: return regs.OBP0;
    case 0xFF49: return regs.OBP1;
    case 0xFF4A: return regs.WY;
    case 0xFF4B: return regs.WX;
    default:     return 0xFF;
    }
}

void PPU::WriteIOReg(uint16_t addr, uint8_t data)
{
    switch (addr)
    {
    case 0xFF40: regs.LCDC = data; break;
    case 0xFF41: regs.STAT = data; break;
    case 0xFF42: regs.SCY = data; break;
    case 0xFF43: regs.SCX = data; break;
    case 0xFF44: regs.LY = data; break;
    case 0xFF45: regs.LYC = data; break;
    case 0xFF46: regs.DMA = data; break;
    case 0xFF47: regs.BGP = data; break;
    case 0xFF48: regs.OBP0 = data; break;
    case 0xFF49: regs.OBP1 = data; break;
    case 0xFF4A: regs.WY = data; break;
    case 0xFF4B: regs.WX = data; break;
    }
}

const std::array<uint8_t, 0x2000>& PPU::GetVram() const
{
	return vram;
}

const PPU::LCDRegs& PPU::GetRegs() const
{
	return regs;
}