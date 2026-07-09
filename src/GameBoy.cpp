#include <GameBoy.h>
#include <iostream>

GameBoy::GameBoy() : ppu(), mem(ppu), cpu(mem)
{
	mem.LoadBootROM(bootROMpath);
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

	for (int i = 0; i < 10000; i++)
	{
		if (cpu.GetLastPC() == 0xf1)
		{
			paused = true;
			break;
		}

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