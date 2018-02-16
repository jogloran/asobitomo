#include "cpu.h"

#include "cpu_macros.h"
#include "cpu_bc_macros.h"

std::array<std::function<void(CPU&)>, 256> CPU::cb_ops  {
  RLC8(),
  RRC8(),
  RL8(),
  RR8(),
  SLA8(),
  SRA8(),
  SWAP8(),
  SRL8(),
  BIT_GEN64(),
  RES_GEN64(),
  SET_GEN64(),
};

std::array<std::function<void(CPU&)>, 256> CPU::ops {
 NOOP,
 LD_WORD_d16(b, c),
 LD_ADDR_REG(b, c, a),
 INC_WORD(b, c),
 INC_REG(b),
 DEC_REG(b),
 LD_REG_d8(b),
 RLCA(),
 LD_LOC_SP(),
 ADD_WORD_WORD(h, l, b, c),
 LD_REG_LOC(a, b, c),
 DEC_WORD(b, c),
 INC_REG(c),
 DEC_REG(c),
 LD_REG_d8(c),
 RRCA(),

 STOP(),
 LD_WORD_d16(d, e),
 LD_ADDR_REG(d, e, a),
 INC_WORD(d, e),
 INC_REG(d),
 DEC_REG(d),
 LD_REG_d8(d),
 RLA(),
 JR_COND_r8((true)),
 ADD_WORD_WORD(h, l, d, e),
 LD_REG_LOC(a, d, e),
 DEC_WORD(d, e),
 INC_REG(e),
 DEC_REG(e),
 LD_REG_d8(e),
 RRA(),

 JR_COND_r8((cpu.Z() == 0)) /* JR NZ */,
 LD_WORD_d16(h, l),
 LD_LOC_REG_AUG(+, h, l, a) /* LD (HL+), A todo */,
 INC_WORD(h, l),
 INC_REG(h),
 DEC_REG(h),
 LD_REG_d8(h),
 UNIMPL() /* TODO: DAA */,
 JR_COND_r8((cpu.Z() != 0)) /* JR Z */,
 ADD_WORD_WORD(h, l, h, l),
 LD_REG_LOC_AUG(a, +, h, l) /* LD A, (HL+) */,
 DEC_WORD(h, l),
 INC_REG(l),
 DEC_REG(l),
 LD_REG_d8(l),
 CPL() /* CPL */,

 JR_COND_r8((cpu.C() == 0)) /* todo */,
 LD_WWORD_d16(sp),
 LD_LOC_REG_AUG(-, h, l, a) /* LD (HL-),A todo */,
 INC_WWORD(sp),
 INC_ADDR(h, l),
 DEC_ADDR(h, l),
 LD_ADDR_d8(h, l),
 SCF() /* SCF */,
 JR_COND_r8((cpu.C() != 0)) /* JR C */,
 ADD_WORD_WWORD(h, l, sp),
 LD_REG_LOC_AUG(a, -, h, l) /* LD A,(HL-) */,
 DEC_WWORD(sp),
 INC_REG(a),
 DEC_REG(a),
 LD_REG_d8(a),
 CCF() /* CCF */,

 LD_REG_REG8(b), /*bcdehl (hl) a*/
 LD_REG_REG8(c), /*bcdehl (hl) a*/
 LD_REG_REG8(d), /*bcdehl (hl) a*/
 LD_REG_REG8(e), /*bcdehl (hl) a*/
 LD_REG_REG8(h), /*bcdehl (hl) a*/
 LD_REG_REG8(l), /*bcdehl (hl) a*/
 LD_HL_SPECIAL(),/*  LD (HL), ... and HALT */
 LD_REG_REG8(a), /*bcdehl (hl) a*/
 ADD_A8(),
 ADC_A8(),
 GEN8(-),
 SBC_A8(),
 GEN8(&),
 GEN8(^),
 GEN8(|),
 CP8(),

 RET_COND((cpu.Z() == 0)),
 POP_WORD(b, c),
 JP_COND_a16((cpu.Z() == 0)),
 JP_COND_a16((true)),
 CALL_COND_a16((cpu.Z() == 0)),
 PUSH_WORD(b, c),
 ADD_A_d8(),
 RST(0x00),
 RET_COND((cpu.Z() != 0)),
 RET_COND((true)),
 JP_COND_a16((cpu.Z() != 0)),
 CPU::handle_cb /* prefix CB */,
 CALL_COND_a16((cpu.Z() != 0)),
 CALL_COND_a16((true)),
 ADC_A_d8(),
 RST(0x08),

 RET_COND((cpu.C() == 0)),
 POP_WORD(d, e),
 JP_COND_a16((cpu.C() == 0)),
 INVALID(),
 CALL_COND_a16((cpu.C() == 0)),
 PUSH_WORD(d, e),
 SUB_A_d8(),
 RST(0x10),
 RET_COND((cpu.C() != 0)),
 RETI(),
 JP_COND_a16((cpu.C() != 0)),
 INVALID(),
 CALL_COND_a16((cpu.C() != 0)),
 INVALID(),
 SBC_A_d8(),
 RST(0x18),

 LDH_a8_A() /* LDH (a8), A */,
 POP_WORD(h, l),
 LDH_ADDR_A(c) /* LD (C), A */,
 INVALID(),
 INVALID(),
 PUSH_WORD(h, l),
 OP_d8(&),
 RST(0x20),
 ADD_SP_r8(),
 JP_HL(),
 LD_a16_A(),
 INVALID(),
 INVALID(),
 INVALID(),
 OP_d8(^),
 RST(0x28),

 LDH_A_a8() /* LDH A, (a8) */,
 POP_WORD(a, f),
 LDH_A_ADDR(c) /* LD A, (C) */,
 DI() /* DI */,
 INVALID(),
 PUSH_WORD(a, f),
 OP_d8(|),
 RST(0x30),
 LD_HL_SP_plus_r8(),
 LD_SP_HL(),
 LD_A_a16(),
 EI() /* EI */,
 INVALID(),
 INVALID(),
 CP_d8(),
 RST(0x38),
};


void CPU::step()  {
  if (halted) {
    throw std::runtime_error("cpu halted");
  }

  byte instr = mmu[pc];
  cout << setfill('0') <<
    "pc: 0x" << setw(4) << hex << pc << ' ' <<
    "sp: 0x" <<                   sp << ' ' <<
    "op: 0x" << setw(2) << hex << static_cast<int>(instr) << ' ' << endl;
  cout <<
    "a: " << setw(2) << hex << static_cast<int>(a) << ' ' <<
    "f: " << setw(2) << hex << static_cast<int>(f) << ' ' <<
    "b: " << setw(2) << hex << static_cast<int>(b) << ' ' <<
    "c: " << setw(2) << hex << static_cast<int>(c) << ' ' << endl;
  cout <<
    "d: " << setw(2) << hex << static_cast<int>(d) << ' ' <<
    "e: " << setw(2) << hex << static_cast<int>(e) << ' ' <<
    "h: " << setw(2) << hex << static_cast<int>(h) << ' ' <<
    "l: " << setw(2) << hex << static_cast<int>(l) << ' ' << endl;
  cout <<
    "Z: " << setw(2) << hex << static_cast<int>(Z()) << ' ' <<
    "N: " << setw(2) << hex << static_cast<int>(N()) << ' ' <<
    "H: " << setw(2) << hex << static_cast<int>(H()) << ' ' <<
    "C: " << setw(2) << hex << static_cast<int>(C()) << ' ' << endl;

  ++pc;

  long old_cycles = cycles;
  ops[instr](*this);
  cycles += ncycles[instr];

  ppu.step(cycles - old_cycles);

  cout << endl;
}
