#include <GameBoy.h>
#include <iostream>

GameBoy::GameBoy() : cart(), ppu(), mem(ppu, cart), cpu(mem)
{
	mem.LoadBootROM(bootROMpath);
	cart.LoadROM("ROMS/tetris.gb");
	paused = true;
}

void GameBoy::Pause(bool pause)
{
	paused = pause;
}

void GameBoy::Cycle()
{
	//paused = true;
	if (paused) return;

	for (int i = 0; i < speed; i++)
	{
		Step();
	}
	
}

void GameBoy::Step()
{
	uint8_t cycles = cpu.Step();
	ppu.Step(cycles);
}

const CPU& GameBoy::GetCPU() const
{
	return cpu;
}

const MemoryBus& GameBoy::GetMemoryBus() const
{
	return mem;
}

const PPU& GameBoy::GetPPU() const
{
	return ppu;
}

void GameBoy::SkipLoop()
{
	cpu.SkipLoop();
}