#include <GameBoy.h>
#include <iostream>

GameBoy::GameBoy() : ppu(), mem(ppu), cpu(mem)
{
	mem.LoadBootROM(bootROMpath);
}

void GameBoy::Pause(bool pause)
{
	paused = pause;
}

void GameBoy::Cycle()
{
	if (paused) return;

	for (int i = 0; i < 100; i++)
	{
		// idk if i should do the proper timings here but
	// for now who cares?
		Step();
	}

	
}

void GameBoy::Step()
{
	cpu.Step();
	ppu.Step();
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