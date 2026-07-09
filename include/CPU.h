#pragma once

#include <cstdint>
#include <sstream>
#include <MemoryBus.h>

struct Registers
{
	uint8_t A = 0x00;
	uint8_t B = 0x00;
	uint8_t C = 0x00;
	uint8_t D = 0x00;
	uint8_t E = 0x00;
	uint8_t F = 0x00;
	uint8_t H = 0x00;
	uint8_t L = 0x00;
	uint16_t PC = 0x0000;
	uint16_t SP = 0x0000;
};

enum class Regs {
	A, B, C, D, E, F, H, L
};

enum class Regs16 {
	AF, BC, DE, HL, SP
};

enum class Flags {
	Z, N, H, C
};

class CPU
{
public:
	CPU(MemoryBus& mem);

	uint8_t Step();

	const Registers& GetRegisters() const;
	uint16_t GetLastPC() const { return lastPC; }

	void SkipLoop();
	static const int HISTORY_SIZE = 100;
	const std::array<std::string, HISTORY_SIZE>& GetHistory() const { return history; }
	int GetHistoryIndex() const { return historyIndex; }

private:
	Registers regs;
	MemoryBus& mem;
	uint16_t lastPC;
	uint8_t lastOpcode = 0x0;
	std::string currentOpcode = "";
	std::ostringstream ss;

	std::array<std::string, HISTORY_SIZE> history;
	int historyIndex = 0;

	using Opcode = uint8_t(CPU::*)();
	std::array<Opcode, 256> opcode;
	void FillOpcodeTable();

	uint8_t FetchU8();

	uint8_t GetReg(Regs r);
	uint8_t GetReg(uint8_t byte);
	void SetReg(Regs r, uint8_t val);
	void SetReg(uint8_t byte, uint8_t val);
	uint16_t GetReg(Regs16 r);
	void SetReg(Regs16 r, uint16_t val);
	const char* GetRegName(uint8_t byte);
	bool GetFlag(Flags f);
	void SetFlag(Flags f, bool val);

	void Push(uint16_t value);
	uint16_t Pop();

	uint8_t NOP();				// 0x00
	uint8_t INC_B();			// 0x04
	uint8_t DEC_B();			// 0x05
	uint8_t LD_B_u8();			// 0x06
	uint8_t INC_C();			// 0x0C
	uint8_t DEC_C();			// 0x0D
	uint8_t LD_C_u8();			// 0x0E
	uint8_t LD_DE_u16();		// 0x11
	uint8_t INC_DE();			// 0x13
	uint8_t DEC_D();			// 0x15
	uint8_t RLA();				// 0x17
	uint8_t JR_i8();			// 0x18
	uint8_t LDL_A_DE();			// 0x1A
	uint8_t DEC_E();			// 0x1D
	uint8_t LD_E_u8();			// 0x1E
	uint8_t JR_NZ_i8();			// 0x20
	uint8_t LD_HL_u16();		// 0x21
	uint8_t LD_HL_plus_A();		// 0x22
	uint8_t INC_HL();			// 0x23
	uint8_t INC_H();			// 0x24
	uint8_t JR_Z_i8();			// 0x28
	uint8_t LD_L_u8();			// 0x2E
	uint8_t LD_SP_u16();		// 0x31
	uint8_t LD_HL_minus_A();	// 0x32
	uint8_t DEC_A();			// 0x3D
	uint8_t LD_A_u8();			// 0x3E
	uint8_t LD_C_A();			// 0x4F
	uint8_t LD_D_A();			// 0x57
	uint8_t LD_H_A();			// 0x67
	uint8_t LD_HL_A();			// 0x77
	uint8_t LD_A_E();			// 0x7B
	uint8_t LD_A_H();			// 0x7C
	uint8_t LD_A_L();			// 0x7D
	uint8_t SUB_A_B();			// 0x90
	uint8_t XOR_r();			// 0xA8 - 0xAF
	uint8_t XOR_A_HL();			// 0xAE
	uint8_t CP_A_HL();			// 0xBE
	uint8_t POP_BC();			// 0xC1
	uint8_t PUSH_BC();			// 0xC5
	uint8_t RET();				// 0xC9
	uint8_t CB_Prefix();		// 0xCB
	uint8_t CALL_u16();			// 0xCD
	uint8_t LDH_u8_A();			// 0xE0
	uint8_t LDH_C_A();			// 0xE2
	uint8_t LDL_u16_A();		// 0xEA
	uint8_t LDH_A_u8();			// 0xF0
	uint8_t CP_A_u8();			// 0xFE

	uint8_t CB_Execute(uint8_t op, uint8_t reg);
	uint8_t RLC(uint8_t val);
	uint8_t RRC(uint8_t val);
	uint8_t RL(uint8_t val);
	uint8_t RR(uint8_t val);
	uint8_t SLA(uint8_t val);
	uint8_t SRA(uint8_t val);
	uint8_t SWAP(uint8_t val);
	uint8_t SRL(uint8_t val);
	uint8_t CB_BIT(uint8_t val);
	uint8_t CB_RES(uint8_t val);
	uint8_t CB_SET(uint8_t val);

};