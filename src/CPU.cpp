//----------------------------------------------

#include <CPU.h>
#include <Utils.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>

//----------------------------------------------

CPU::CPU(MemoryBus& mem) : mem(mem)
{
	if (!mem.isInitialised())
		throw std::runtime_error("[ERROR]: Cannot construct CPU with uninitialised Memory Bus.");
	
	std::cout << "[Notice]: CPU started init...\n";
	
	regs.PC = 0x0;
	//regs.PC = 0x00F0;
	FillOpcodeTable();

	std::cout << "[Notice]: CPU initialised.\n";
}

//----------------------------------------------

uint8_t CPU::Step()
{
	//if (regs.PC >= 256) return 0;
	lastPC = regs.PC;
	ss.str("");

	// fetch
	uint8_t byte = FetchU8();
	lastOpcode = byte;

	// decode
	auto instruction = opcode[byte];

	// exec
	uint8_t cycles = (this->*instruction)();
	
	// something about custom instruction cycles????
	history[historyIndex] = currentOpcode;
	historyIndex = (historyIndex + 1) % HISTORY_SIZE;

	return cycles;
}

//----------------------------------------------

void CPU::SkipLoop()
{
	regs.H = 0x80;
	regs.L = 0x01;
}

//----------------------------------------------

uint8_t CPU::FetchU8()
{
	return mem.GetByte(regs.PC++);
}

//----------------------------------------------

void CPU::FillOpcodeTable()
{
	opcode.fill(&CPU::NOP);

	opcode[0x04] = &CPU::INC_B;
	opcode[0x05] = &CPU::DEC_B;
	opcode[0x06] = &CPU::LD_B_u8;
	opcode[0x0C] = &CPU::INC_C;
	opcode[0x0D] = &CPU::DEC_C;
	opcode[0x0E] = &CPU::LD_C_u8;

	opcode[0x11] = &CPU::LD_DE_u16;
	opcode[0x13] = &CPU::INC_DE;
	opcode[0x15] = &CPU::DEC_D;
	opcode[0x17] = &CPU::RLA;
	opcode[0x18] = &CPU::JR_i8;
	opcode[0x1A] = &CPU::LDL_A_DE;
	opcode[0x1D] = &CPU::DEC_E;
	opcode[0x1E] = &CPU::LD_E_u8;

	opcode[0x20] = &CPU::JR_NZ_i8;
	opcode[0x21] = &CPU::LD_HL_u16;
	opcode[0x22] = &CPU::LD_HL_plus_A;
	opcode[0x23] = &CPU::INC_HL;
	opcode[0x24] = &CPU::INC_H;
	opcode[0x28] = &CPU::JR_Z_i8;
	opcode[0x2E] = &CPU::LD_L_u8;

	opcode[0x31] = &CPU::LD_SP_u16;
	opcode[0x32] = &CPU::LD_HL_minus_A;
	opcode[0x3D] = &CPU::DEC_A;
	opcode[0x3E] = &CPU::LD_A_u8;

	opcode[0x4F] = &CPU::LD_C_A;

	opcode[0x57] = &CPU::LD_D_A;

	opcode[0x67] = &CPU::LD_H_A;

	opcode[0x77] = &CPU::LD_HL_A;
	opcode[0x78] = &CPU::LD_A_B;
	opcode[0x7B] = &CPU::LD_A_E;
	opcode[0x7C] = &CPU::LD_A_H;
	opcode[0x7D] = &CPU::LD_A_L;

	opcode[0x86] = &CPU::ADD_A_HL;

	opcode[0x90] = &CPU::SUB_A_B;

	opcode[0xA8] = &CPU::XOR_r;
	opcode[0xA9] = &CPU::XOR_r;
	opcode[0xAA] = &CPU::XOR_r;
	opcode[0xAB] = &CPU::XOR_r;
	opcode[0xAC] = &CPU::XOR_r;
	opcode[0xAD] = &CPU::XOR_r;
	opcode[0xAE] = &CPU::XOR_A_HL;
	opcode[0xAF] = &CPU::XOR_r;

	opcode[0xBE] = &CPU::CP_A_HL;

	opcode[0xC1] = &CPU::POP_BC;
	opcode[0xC5] = &CPU::PUSH_BC;
	opcode[0xC9] = &CPU::RET;
	opcode[0xCB] = &CPU::CB_Prefix;
	opcode[0xCD] = &CPU::CALL_u16;

	opcode[0xE0] = &CPU::LDH_u8_A;
	opcode[0xE2] = &CPU::LDH_C_A;
	opcode[0xEA] = &CPU::LDL_u16_A;

	opcode[0xF0] = &CPU::LDH_A_u8;
	opcode[0xFE] = &CPU::CP_A_u8;
}

//---------------------------------------------------------//
//                       0x0 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::NOP()		// 0x00
{
	currentOpcode = "NOP";

	return 4;
}

//----------------------------------------------

uint8_t CPU::INC_B()	// 0x04
{
	uint8_t oldVal = regs.B;
	regs.C++;

	SetFlag(Flags::Z, regs.B == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, (oldVal & 0x0F) == 0x0F);

	ss << "INC B";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::DEC_B()	// 0x05
{
	uint8_t oldVal = regs.B;
	regs.B--;

	SetFlag(Flags::Z, regs.B == 0);
	SetFlag(Flags::N, true);
	SetFlag(Flags::H, ((oldVal & 0x0F) == 0x00));

	ss << "DEC B";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::LD_B_u8()	// 0x06
{
	uint8_t byte = FetchU8();

	SetReg(Regs::B, byte);

	ss << "LD B, 0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)byte;
	currentOpcode = ss.str();

	return 8;
}

//----------------------------------------------

uint8_t CPU::INC_C()	// 0x0C
{
	uint8_t oldVal = regs.C;
	regs.C++;

	SetFlag(Flags::Z, regs.C == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, (oldVal & 0x0F) == 0x0F);

	ss << "INC C";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::DEC_C()	// 0x0D
{
	uint8_t oldVal = regs.C;
	regs.C--;

	SetFlag(Flags::Z, regs.C == 0);
	SetFlag(Flags::N, true);
	SetFlag(Flags::H, ((oldVal & 0x0F) == 0x00));

	ss << "DEC C";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::LD_C_u8()	// 0x0E
{
	uint8_t byte = FetchU8();

	SetReg(Regs::C, byte);

	ss << "LD C, 0x" << std::hex << std::uppercase << (int)byte;
	currentOpcode = ss.str();

	return 8;
}

//---------------------------------------------------------//
//                       0x1 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::LD_DE_u16() // 0x11
{
	uint8_t low = FetchU8();
	uint8_t high = FetchU8();

	SetReg(Regs16::DE, MergeBytes(high, low));

	ss << "LD DE, 0x" << std::hex << std::uppercase << (int)MergeBytes(high, low);
	currentOpcode = ss.str();

	return 12;
}

//----------------------------------------------

uint8_t CPU::INC_DE()	// 0x13
{
	uint16_t de = GetReg(Regs16::DE);
	SetReg(Regs16::DE, de + 1);

	ss << "INC DE";
	currentOpcode = ss.str();

	return 8;
}

//----------------------------------------------

uint8_t CPU::DEC_D()	// 0x15
{
	uint8_t oldVal = regs.D;
	regs.D--;

	SetFlag(Flags::Z, regs.D == 0);
	SetFlag(Flags::N, true);
	SetFlag(Flags::H, ((oldVal & 0x0F) == 0x00));

	ss << "DEC D";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::RLA()		// 0x17
{
	uint8_t val = GetReg(Regs::A);

	uint8_t oldcarry = GetFlag(Flags::C) ? 1 : 0;
	uint8_t newcarry = val >> 7;

	val = (val << 1) | oldcarry;
	SetReg(Regs::A, val);

	SetFlag(Flags::Z, false);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, false);
	SetFlag(Flags::C, newcarry);

	ss << "RLA";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::JR_i8()	// 0x18
{
	uint8_t byte = FetchU8();
	int8_t offset = (int8_t)byte;

	regs.PC += offset;

	ss << "JR 0x" << std::hex << std::uppercase << (int)(regs.PC + offset);
	currentOpcode = ss.str();

	return 12;
}

//----------------------------------------------

uint8_t CPU::LDL_A_DE()	// 0x1A
{
	uint16_t addr = GetReg(Regs16::DE);
	uint8_t byte = mem.GetByte(addr);
	SetReg(Regs::A, byte);

	ss << "LD A, 0x" << std::hex << std::uppercase << (int)byte;
	currentOpcode = ss.str();

	return 8;
}

//----------------------------------------------

uint8_t CPU::DEC_E()	// 0x1D
{
	uint8_t oldVal = regs.E;
	regs.E--;

	SetFlag(Flags::Z, regs.E == 0);
	SetFlag(Flags::N, true);
	SetFlag(Flags::H, ((oldVal & 0x0F) == 0x00));

	ss << "DEC E";
	currentOpcode = ss.str();

	return 4;
}


//----------------------------------------------

uint8_t CPU::LD_E_u8()	// 0x1E
{
	uint8_t byte = FetchU8();
	SetReg(Regs::E, byte);

	ss << "LD E, 0x" << std::hex << std::uppercase << (int)byte;
	currentOpcode = ss.str();

	return 8;
}

//---------------------------------------------------------//
//                       0x2 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::JR_NZ_i8() // 0x20
{
	// jump rel not zero by signed 8b offset
	uint8_t byte = FetchU8();
	int8_t offset = (int8_t)byte;

	uint8_t cycles = 8;

	if (!GetFlag(Flags::Z))
	{
		regs.PC += offset;
		cycles = 12;
	}

	ss << "JR NZ 0x" << std::hex << std::uppercase << (int)(regs.PC + offset);
	currentOpcode = ss.str();

	return cycles;
}

//----------------------------------------------

uint8_t CPU::LD_HL_u16() // 0x21
{
	uint8_t low = FetchU8();
	uint8_t high = FetchU8();

	SetReg(Regs16::HL, MergeBytes(high, low));

	ss << "LD HL, 0x" << std::hex << std::uppercase << (int)MergeBytes(high, low);
	currentOpcode = ss.str();

	return 12;
}

//----------------------------------------------

uint8_t CPU::LD_HL_plus_A() // 0x22
{
	uint16_t hl = GetReg(Regs16::HL);
	mem.WriteByte(hl, regs.A);
	SetReg(Regs16::HL, hl + 1);

	ss << "LD (HL+), A";
	currentOpcode = ss.str();

	return 8;
}

//----------------------------------------------

uint8_t CPU::INC_HL()	// 0x23
{
	uint16_t hl = GetReg(Regs16::HL);
	SetReg(Regs16::HL, hl + 1);

	ss << "INC HL";
	currentOpcode = ss.str();

	return 8;
}

//----------------------------------------------

uint8_t CPU::INC_H()	// 0x24
{
	uint8_t oldVal = regs.H;
	regs.H++;

	SetFlag(Flags::Z, regs.H == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, (oldVal & 0x0F) == 0x0F);

	ss << "INC H";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::JR_Z_i8()	// 0x28
{
	// jump rel zero by signed 8b offset
	uint8_t byte = FetchU8();
	int8_t offset = (int8_t)byte;

	uint8_t cycles = 8;

	if (GetFlag(Flags::Z))
	{
		regs.PC += offset;
		cycles = 12;
	}

	ss << "JR Z 0x" << std::hex << std::uppercase << (int)(regs.PC + offset);
	currentOpcode = ss.str();

	return cycles;
}

//----------------------------------------------

uint8_t CPU::LD_L_u8()	// 0x2E
{
	uint8_t byte = FetchU8();
	SetReg(Regs::L, byte);

	ss << "LD L, 0x" << std::hex << std::uppercase << (int)byte;
	currentOpcode = ss.str();
	
	return 8;
}

//---------------------------------------------------------//
//                       0x3 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::LD_SP_u16()	// 0x31
{
	// load into SP the next two bytes merged secondfirst
	uint8_t low = FetchU8();
	uint8_t high = FetchU8();

	SetReg(Regs16::SP, MergeBytes(high, low));

	ss << "LD SP, 0x" << std::hex << std::uppercase << (int)MergeBytes(high, low);
	currentOpcode = ss.str();

	return 12;
}

//----------------------------------------------

uint8_t CPU::LD_HL_minus_A()	// 0x32
{
	uint16_t hl = GetReg(Regs16::HL);
	mem.WriteByte(hl, regs.A);
	SetReg(Regs16::HL, hl - 1);

	ss << "LD (HL-), A";
	currentOpcode = ss.str();

	return 8;
}

//----------------------------------------------

uint8_t CPU::DEC_A()		// 0x3D
{
	uint8_t oldVal = regs.A;
	regs.A--;

	SetFlag(Flags::Z, regs.A == 0);
	SetFlag(Flags::N, true);
	SetFlag(Flags::H, ((oldVal & 0x0F) == 0x00));

	ss << "DEC A";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::LD_A_u8()		// 0x3E
{
	// load into A the next byte
	uint8_t byte = FetchU8();
	SetReg(Regs::A, byte);

	ss << "LD A, 0x" << std::hex << std::uppercase << (int)byte;
	currentOpcode = ss.str();
	
	return 8;
}

//---------------------------------------------------------//
//                       0x4 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::LD_C_A()		// 0x4F
{
	SetReg(Regs::C, GetReg(Regs::A));

	ss << "LD C, A";
	currentOpcode = ss.str();

	return 4;
}

//---------------------------------------------------------//
//                       0x5 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::LD_D_A()		// 0x57
{
	SetReg(Regs::D, GetReg(Regs::A));

	ss << "LD D, A";
	currentOpcode = ss.str();

	return 4;
}

//---------------------------------------------------------//
//                       0x6 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::LD_H_A()		// 0x67
{
	SetReg(Regs::H, GetReg(Regs::A));

	ss << "LD H, A";
	currentOpcode = ss.str();

	return 4;
}

//---------------------------------------------------------//
//                       0x7 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::LD_HL_A()		// 0x77
{
	uint16_t hl = GetReg(Regs16::HL);
	mem.WriteByte(hl, regs.A);

	ss << "LD (HL), A";
	currentOpcode = ss.str();

	return 8;
}

//----------------------------------------------

uint8_t CPU::LD_A_B()		// 0x78
{
	regs.A = regs.B;

	ss << "LD A, B";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::LD_A_E()		// 0x7B
{
	regs.A = regs.E;

	ss << "LD A, E";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::LD_A_H()		// 0x7C
{
	regs.A = regs.H;

	ss << "LD A, H";
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::LD_A_L()		// 0x7D
{
	regs.A = regs.L;

	ss << "LD A, L";
	currentOpcode = ss.str();

	return 4;
}

//---------------------------------------------------------//
//                       0x8 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::ADD_A_HL()		// 0x86
{
	uint16_t addr = GetReg(Regs16::HL);
	uint8_t val = mem.GetByte(addr);
	uint8_t oldA = regs.A;

	uint16_t result = oldA + val;
	regs.A = (uint8_t)result;

	SetFlag(Flags::Z, regs.A == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, ((oldA & 0x0F) + (val & 0x0F)) > 0x0F);
	SetFlag(Flags::C, result > 0xFF);

	ss << "ADD A, (HL)";
	currentOpcode = ss.str();

	return 8;
}

//---------------------------------------------------------//
//                       0x9 opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::SUB_A_B()		// 0x90
{
	uint8_t oldA = regs.A;
	uint8_t result = oldA - regs.B;

	SetFlag(Flags::Z, result == 0);
	SetFlag(Flags::N, true);
	SetFlag(Flags::H, (oldA & 0x0F) < (regs.B & 0x0f));
	SetFlag(Flags::C, oldA < regs.B);

	regs.A = result;

	ss << "SUB A, B";
	currentOpcode = ss.str();

	return 4;
}

//---------------------------------------------------------//
//                       0xA opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::XOR_r()		// 0xA8 - 0xAF
{
	uint8_t op = lastOpcode;
	uint8_t reg = op & 0x07;

	regs.A ^= GetReg(reg);
	SetFlag(Flags::Z, regs.A == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, false);
	SetFlag(Flags::C, false);

	ss << "XOR A, " << GetRegName(reg);
	currentOpcode = ss.str();

	return 4;
}

//----------------------------------------------

uint8_t CPU::XOR_A_HL()		// 0xAE
{
	uint16_t addr = GetReg(Regs16::HL);
	uint8_t value = mem.GetByte(addr);

	regs.A ^= value;
	SetFlag(Flags::Z, regs.A == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, false);
	SetFlag(Flags::C, false);

	ss << "XOR A, (HL)";
	currentOpcode = ss.str();

	return 8;
}

//---------------------------------------------------------//
//                       0xB opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::CP_A_HL()		// 0xBE
{
	uint8_t A = regs.A;
	uint16_t addr = GetReg(Regs16::HL);
	uint8_t byte = mem.GetByte(addr);

	SetFlag(Flags::Z, A == byte);
	SetFlag(Flags::N, true);
	SetFlag(Flags::H, (A & 0x0F) < (byte & 0x0F));
	SetFlag(Flags::C, (A < byte));

	ss << "CP A, (HL)";
	currentOpcode = ss.str();

	return 8;
}

//---------------------------------------------------------//
//                       0xC opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::POP_BC()		// 0xC1
{
	uint16_t pop = Pop();
	SetReg(Regs16::BC, pop);

	ss << "POP BC";
	currentOpcode = ss.str();

	return 12;
}

//----------------------------------------------

uint8_t CPU::PUSH_BC()		// 0xC5
{
	uint16_t BC = GetReg(Regs16::BC);
	Push(BC);

	ss << "PUSH BC";
	currentOpcode = ss.str();

	return 16;
}

//----------------------------------------------

uint8_t CPU::RET()			// 0xC9
{
	regs.PC = Pop();

	ss << "RET";
	currentOpcode = ss.str();

	return 16;
}

//----------------------------------------------

uint8_t CPU::CB_Prefix()	// 0xCB
{
	uint8_t op = FetchU8();

	uint8_t cycles = 0;

	if (op < 0x40) // 0x00-0x3F
	{
		uint8_t operation = (op >> 3) & 0x07;
		uint8_t reg = op & 0x07;
		cycles = CB_Execute(operation, reg);
	}
	else if (op < 0x80)
	{
		cycles = CB_BIT(op);
	}
	else if (op < 0xC0)
	{
		cycles = CB_RES(op);
	}
	else
	{
		cycles = CB_SET(op);
	}
	
	return cycles;
}

//----------------------------------------------

uint8_t CPU::CALL_u16()		// 0xCD
{
	uint8_t low = FetchU8();
	uint8_t high = FetchU8(); // at this poit, pc should be next
	uint16_t addr = MergeBytes(high, low);

	Push(regs.PC);
	regs.PC = addr; // then RET can regs.PC = Pop() which is cool

	ss << "CALL 0x" << std::hex << std::uppercase << (int)addr;
	currentOpcode = ss.str();

	return 24;
}

//---------------------------------------------------------//
//                       0xE opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::LDH_u8_A()		// 0xE0
{
	// load A into FF00 + u8 offset
	uint8_t byte = FetchU8();
	uint16_t addr = 0xFF00 + byte;
	mem.WriteByte(addr, regs.A);

	ss << "LDH 0x" << std::hex << std::uppercase << (int)addr <<", A";
	currentOpcode = ss.str();

	return 12;
}

//----------------------------------------------

uint8_t CPU::LDH_C_A()		// 0xE2
{
	uint8_t byte = GetReg(Regs::C);
	uint16_t addr = 0xFF00 + byte;
	mem.WriteByte(addr, regs.A);

	ss << "LDH 0x" << std::hex << std::uppercase << (int)addr << ", A";
	currentOpcode = ss.str();

	return 8;
}

//----------------------------------------------

uint8_t CPU::LDL_u16_A()	// 0xEA
{
	uint8_t low = FetchU8();
	uint8_t high = FetchU8(); // at this poit, pc should be next
	uint16_t addr = MergeBytes(high, low);

	mem.WriteByte(addr, regs.A);

	ss << "LDL 0x" << std::hex << std::uppercase << (int)addr << ", A";
	currentOpcode = ss.str();

	return 16;
}

//---------------------------------------------------------//
//                       0xF opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::LDH_A_u8()		// 0xF0
{
	uint8_t byte = FetchU8();
	uint16_t addr = 0xFF00 + byte;
	SetReg(Regs::A, mem.GetByte(addr));

	ss << "LDH A, 0x" << std::hex << std::uppercase << (int)addr;
	currentOpcode = ss.str();

	return 12;
}

//----------------------------------------------

uint8_t CPU::CP_A_u8()		// 0xFE
{
	uint8_t A = regs.A;
	uint8_t byte = FetchU8();

	SetFlag(Flags::Z, A == byte);
	SetFlag(Flags::N, true);
	SetFlag(Flags::H, (A & 0x0F) < (byte & 0x0F));
	SetFlag(Flags::C, (A < byte));

	ss << "CP A, 0x" << std::hex << std::uppercase << (int)byte;
	currentOpcode = ss.str();

	return 8;
}

//---------------------------------------------------------//
//                        CB opcodes                       //
//---------------------------------------------------------//

uint8_t CPU::CB_Execute(uint8_t op, uint8_t reg)
{
	uint8_t val = 0;

	if (reg == 6) // HL
		val = mem.GetByte(GetReg(Regs16::HL));
	else
		val = GetReg(reg);

	switch (op)
	{
	case 0: val = RLC(val); break;
	case 1: val = RRC(val); break;
	case 2: val = RL(val); break;
	case 3: val = RR(val); break;
	case 4: val = SLA(val); break;
	case 5: val = SRA(val); break;
	case 6: val = SWAP(val); break;
	case 7: val = SRL(val); break;	// shift right and set C to b0
	}

	ss << GetRegName(reg);
	currentOpcode = ss.str();

	if (reg == 6)
		mem.WriteByte(GetReg(Regs16::HL), val);
	else
		SetReg(reg, val);

	return (reg == 6) ? 16 : 8;
}

//----------------------------------------------

uint8_t CPU::RLC(uint8_t val)
{
	currentOpcode = "RLC not implemented";
	return 0;
}

//----------------------------------------------

uint8_t CPU::RRC(uint8_t val)
{
	currentOpcode = "RRC not implemented";
	return 0;
}

//----------------------------------------------

uint8_t CPU::RL(uint8_t val)
{
	uint8_t oldcarry = GetFlag(Flags::C) ? 1 : 0;
	uint8_t newcarry = val >> 7;

	val = (val << 1) | oldcarry;

	SetFlag(Flags::Z, val == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, false);
	SetFlag(Flags::C, newcarry);

	ss << "RL ";

	return val;
}

//----------------------------------------------

uint8_t CPU::RR(uint8_t val)
{
	currentOpcode = "RR not implemented";
	return 0;
}

//----------------------------------------------

uint8_t CPU::SLA(uint8_t val)
{
	currentOpcode = "SLA not implemented";
	return 0;
}

//----------------------------------------------

uint8_t CPU::SRA(uint8_t val)
{
	currentOpcode = "SRA not implemented";
	return 0;
}

//----------------------------------------------

uint8_t CPU::SWAP(uint8_t val)
{
	currentOpcode = "SWAP not implemented";
	return 0;
}

//----------------------------------------------

uint8_t CPU::SRL(uint8_t val)
{
	uint8_t carry = val & 1;
	val >>= 1;

	SetFlag(Flags::Z, val == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, false);
	SetFlag(Flags::C, carry);

	return val;
}

//----------------------------------------------

uint8_t CPU::CB_BIT(uint8_t val)
{
	// check if bit b in register r is 0/1
	uint8_t bit = (val >> 3) & 0x07;
	uint8_t reg = val & 0x07;

	uint8_t value;

	if (reg == 6)
		value = mem.GetByte(GetReg(Regs16::HL));
	else
		value = GetReg(reg);

	SetFlag(Flags::Z, (value & (1 << bit)) == 0);
	SetFlag(Flags::N, false);
	SetFlag(Flags::H, true);

	ss << "BIT " << (int)bit << ", " << (reg == 6 ? "(HL)" : GetRegName(reg));
	currentOpcode = ss.str();

	return (reg == 6) ? 12 : 8;
}

//----------------------------------------------

uint8_t CPU::CB_RES(uint8_t val)
{
	currentOpcode = "RES not implemented";

	return 8;
}

//----------------------------------------------

uint8_t CPU::CB_SET(uint8_t val)
{
	currentOpcode = "SET not implemented";

	return 8;
}

//----------------------------------------------

const Registers& CPU::GetRegisters() const
{
	return regs;
}

//----------------------------------------------

uint8_t CPU::GetReg(Regs r)
{
	switch (r)
	{
	case Regs::A:
		return regs.A;
	case Regs::B:
		return regs.B;
	case Regs::C:
		return regs.C;
	case Regs::D:
		return regs.D;
	case Regs::E:
		return regs.E;
	case Regs::F:
		return regs.F;
	case Regs::H:
		return regs.H;
	case Regs::L:
		return regs.L;
	}
}

//----------------------------------------------

uint8_t CPU::GetReg(uint8_t byte)
{
	switch (byte)
	{
	case 0: return regs.B;
	case 1: return regs.C;
	case 2: return regs.D;
	case 3: return regs.E;
	case 4: return regs.H;
	case 5: return regs.L;
	case 6: return mem.GetByte(MergeBytes(regs.H, regs.L));
	case 7: return regs.A;
	}
}

//----------------------------------------------

void CPU::SetReg(Regs r, uint8_t val)
{
	switch (r)
	{
	case Regs::A:
		regs.A = val;
		break;
	case Regs::B:
		regs.B = val;
		break;
	case Regs::C:
		regs.C = val;
		break;
	case Regs::D:
		regs.D = val;
		break;
	case Regs::E:
		regs.E = val;
		break;
	case Regs::F:
		regs.F = val & 0xF0;
		break;
	case Regs::H:
		regs.H = val;
		break;
	case Regs::L:
		regs.L = val;
		break;
	}
}

//----------------------------------------------

void CPU::SetReg(uint8_t byte, uint8_t val)
{
	switch (byte)
	{
	case 0: regs.B = val; break;
	case 1: regs.C = val; break;
	case 2: regs.D = val; break;
	case 3: regs.E = val; break;
	case 4: regs.H = val; break;
	case 5: regs.L = val; break;
	case 6: mem.WriteByte(MergeBytes(regs.H, regs.L), val); break;
	case 7: regs.A = val; break;
	}
}

//----------------------------------------------

uint16_t CPU::GetReg(Regs16 r)
{
	switch (r)
	{
	case Regs16::AF:
		return (MergeBytes(regs.A, regs.F));
	case Regs16::BC:
		return (MergeBytes(regs.B, regs.C));
	case Regs16::DE:
		return (MergeBytes(regs.D, regs.E));
	case Regs16::HL:
		return (MergeBytes(regs.H, regs.L));
	case Regs16::SP:
		return regs.SP;
	}
}

//----------------------------------------------

void CPU::SetReg(Regs16 r, uint16_t val)
{
	uint8_t h, l;
	SplitBytes(val, h, l);

	switch (r)
	{
	case Regs16::AF:
		SetReg(Regs::A, h);
		SetReg(Regs::F, l);
		break;
	case Regs16::BC:
		SetReg(Regs::B, h);
		SetReg(Regs::C, l);
		break;
	case Regs16::DE:
		SetReg(Regs::D, h);
		SetReg(Regs::E, l);
		break;
	case Regs16::HL:
		SetReg(Regs::H, h);
		SetReg(Regs::L, l);
		break;
	case Regs16::SP:
		regs.SP = val;
		break;
	}
}

//----------------------------------------------

const char* CPU::GetRegName(uint8_t byte)
{
	switch (byte)
	{
	case 0: return "B";
	case 1: return "C";
	case 2: return "D";
	case 3: return "E";
	case 4: return "H";
	case 5: return "L";
	case 6: return "(HL)";
	case 7: return "A";
	default: return "?";
	}
}

//----------------------------------------------

bool CPU::GetFlag(Flags f)
{
	switch (f)
	{
	case Flags::Z:
		return (regs.F & 0b10000000) != 0;
	case Flags::N:
		return (regs.F & 0b01000000) != 0;
	case Flags::H:
		return (regs.F & 0b00100000) != 0;
	case Flags::C:
		return (regs.F & 0b00010000) != 0;
	}
}

//----------------------------------------------

void CPU::SetFlag(Flags f, bool val)
{
	switch (f)
	{
	case Flags::Z:
		regs.F = (val) ? regs.F |= 0b10000000 : regs.F &= 0b01110000;
		break;
	case Flags::N:
		regs.F = (val) ? regs.F |= 0b01000000 : regs.F &= 0b10110000;
		break;
	case Flags::H:
		regs.F = (val) ? regs.F |= 0b00100000 : regs.F &= 0b11010000;
		break;
	case Flags::C:
		regs.F = (val) ? regs.F |= 0b00010000 : regs.F &= 0b11100000;
		break;
	}
}

//----------------------------------------------

void CPU::Push(uint16_t value)
{
	uint8_t high, low;
	SplitBytes(value, high, low);

	mem.WriteByte(--regs.SP, high);
	mem.WriteByte(--regs.SP, low);
}

//----------------------------------------------

uint16_t CPU::Pop()
{
	uint8_t low = mem.GetByte(regs.SP++);
	uint8_t high = mem.GetByte(regs.SP++);
	return (MergeBytes(high, low));
}

//----------------------------------------------