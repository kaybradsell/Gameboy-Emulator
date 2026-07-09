#include <Debugger.h>

Debugger::Debugger(GameBoy* gb)
{
	this->gb = gb;

	SetTraceLogLevel(LOG_ERROR);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "Chip-8 Emulator");
	SetTargetFPS(60);
	rlImGuiSetup(true);

	ImGui::StyleColorsDarkPink();

	history.fill("");

	tileImage = GenImageColor(TILEMAXX * TILESIZE, TILEMAXY * TILESIZE, BLACK);
	tileTexture = LoadTextureFromImage(tileImage);
}

bool Debugger::Running()
{
	return !WindowShouldClose();
}

void Debugger::Display()
{
	BeginDrawing();
	ClearBackground(BLACK);

	rlImGuiBegin();

	DrawRegisters();
	DrawControls();
	DrawMemory();
	DrawHistory();
	DrawTileViewer();

	rlImGuiEnd();
	EndDrawing();
}

void Debugger::DrawControls()
{
	ImGui::Begin("Controls");

	static bool paused = false;
	ImGui::Checkbox("Pause", &paused);
	gb->Pause(paused);

	if (ImGui::Button("Step"))
		gb->Step();

	ImGui::SameLine();
	if (ImGui::Button("+5"))
	{
		for (int i = 0; i < 5; i++)
			gb->Step();
	}

	ImGui::SliderInt("Speed", &(gb->speed), 0, 10000);

	ImGui::End();
}

void Debugger::DrawRegisters()
{
	ImGui::Begin("Registers");

	if (ImGui::BeginTabBar("Register Tabs"))
	{
		if (ImGui::BeginTabItem("CPU"))
		{
			const Registers r = gb->GetCPU().GetRegisters();

			ImGui::Text("A: 0x%02X", r.A);
			ImGui::Text("B: 0x%02X", r.B);
			ImGui::Text("C: 0x%02X", r.C);
			ImGui::Text("D: 0x%02X", r.D);
			ImGui::Text("E: 0x%02X", r.E);
			ImGui::Text("F: 0x%02X", r.F);
			ImGui::Text("H: 0x%02X", r.H);
			ImGui::Text("L: 0x%02X", r.L);

			ImGui::Text("PC: 0x%04X", r.PC);
			ImGui::Text("SP: 0x%04X", r.SP);

			ImGui::Separator();

			ImGui::Text("Z: %d", (r.F & 0x80) != 0);
			ImGui::Text("N: %d", (r.F & 0x40) != 0);
			ImGui::Text("H: %d", (r.F & 0x20) != 0);
			ImGui::Text("C: %d", (r.F & 0x10) != 0);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("PPU"))
		{
			const PPU::LCDRegs& r = gb->GetPPU().GetRegs();

			ImGui::Text("LCDC: 0x%02X", r.LCDC);
			ImGui::Text("STAT: 0x%02X", r.STAT);
			ImGui::Text("SCY : 0x%02X", r.SCY);
			ImGui::Text("SCX : 0x%02X", r.SCX);
			ImGui::Text("LY  : 0x%02X", r.LY);
			ImGui::Text("LYC : 0x%02X", r.LYC);
			ImGui::Text("DMA : 0x%02X", r.DMA);
			ImGui::Text("BGP : 0x%02X", r.BGP);
			ImGui::Text("OBP0: 0x%02X", r.OBP0);
			ImGui::Text("OBP1: 0x%02X", r.OBP1);
			ImGui::Text("WY  : 0x%02X", r.WY);
			ImGui::Text("WX  : 0x%02X", r.WX);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	

	ImGui::End();
}

void Debugger::DrawMemory()
{
	ImGui::Begin("Boot ROM");

	if (ImGui::BeginTabBar("Memory Tabs"))
	{
		if (ImGui::BeginTabItem("Boot ROM"))
		{
			ImGui::BeginChild("MemoryScrollRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
			const std::array<uint8_t, 256>& rom = gb->GetMemoryBus().GetBootROM();

			uint16_t pc = gb->GetCPU().GetRegisters().PC;
			uint16_t lastpc = gb->GetCPU().GetLastPC();
			uint16_t start = lastpc;
			uint16_t end = pc - 1;

			for (int row = 0; row < 16; row++)
			{
				int base = row * 16;
				ImGui::Text("%04X:", base);
				ImGui::SameLine();

				for (int i = 0; i < 16; i++)
				{
					uint16_t addr = base + i;
					ImGui::SameLine();

					if (addr >= start && addr <= end)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(209, 16, 100, 255));
						ImGui::Text("%02X", rom[base + i]);
						ImGui::PopStyleColor();
					}
					else
					{
						ImGui::Text("%02X", rom[base + i]);
					}
				}

				ImGui::SameLine();

				ImGui::SameLine();
				char ascii[17];

				for (int i = 0; i < 16; i++)
				{
					uint8_t v = rom[base + i];
					ascii[i] = (v >= 32 && v <= 126) ? (char)v : '.';
				}

				ascii[16] = '\0';

				ImGui::Text("%s", ascii);
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("VRAM"))
		{
			ImGui::BeginChild("MemoryScrollRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
			const std::array<uint8_t, 0x2000>& vram = gb->GetPPU().GetVram();

			for (int row = 0; row < (0x2000 / 16); row++)
			{
				int base = row * 16;
				int address = base + 0x8000;

				ImGui::Text("%04X:", address);
				ImGui::SameLine();

				for (int i = 0; i < 16; i++)
				{
					ImGui::SameLine();

					ImGui::Text("%02X", vram[base + i]);
				}

				ImGui::SameLine();

				ImGui::SameLine();
				char ascii[17];

				for (int i = 0; i < 16; i++)
				{
					uint8_t v = vram[base + i];
					ascii[i] = (v >= 32 && v <= 126) ? (char)v : '.';
				}

				ascii[16] = '\0';

				ImGui::Text("%s", ascii);
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();

}

void Debugger::DrawHistory()
{
	ImGui::Begin("History");

	const std::array<std::string, HISTORY_SIZE>& history = gb->GetCPU().GetHistory();
	int historyIndex = gb->GetCPU().GetHistoryIndex();

	for (int i = 0; i < HISTORY_SIZE; i++)
	{
		int idx = (historyIndex - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
		ImGui::Text(history[idx].c_str());
	}

	ImGui::End();
}

void Debugger::DrawTileViewer()
{
	// draws tiles to vbuffer
	int tileIndex = 0;

	for (int y = 0; y < TILEMAXY; y++)
	{
		for (int x = 0; x < TILEMAXX; x++)
		{
			DrawTile(gb->GetPPU().GetTile(tileIndex++), x * TILESIZE, y * TILESIZE);
		}
	}

	UpdateTexture(tileTexture, vbuffer);

	ImGui::Begin("Tile Viewer");

	const static float w = 25 * TILEMAXX;
	const static float h = 25 * TILEMAXY;

	ImGui::Image((ImTextureID)tileTexture.id, ImVec2(w, h));

	// grid overlay :)
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();

	float tileW = w / TILEMAXX;
	float tileH = h / TILEMAXY;

	ImU32 gridColour = IM_COL32(0, 0, 0, 128);

	for (int x = 0; x <= TILEMAXX; x++)
	{
		float px = min.x + x * tileW;
		drawList->AddLine(ImVec2(px, min.y), ImVec2(px, max.y), gridColour);
	}

	for (int y = 0; y <= TILEMAXY; y++)
	{
		float py = min.y + y * tileH;
		drawList->AddLine(ImVec2(min.x, py), ImVec2(max.x, py), gridColour);
	}

	ImGui::End();
}

void Debugger::DrawTile(const PPU::Tile& tile, int startX, int startY)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			Color c = colours[tile.pixels[y][x]];
			vbuffer[(startY + y) * (TILEMAXX * TILESIZE) + (startX + x)] = c;
		}
	}
}