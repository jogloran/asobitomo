#pragma once

#include "types.h"
#include "mmu.h"
#include "ppu.h"
#include "cpu_macros.h"
#include "cpu_bc_macros.h"
#include <array>
#include <iomanip>

constexpr int NINSTR = 256;

constexpr long ncycles_cb[NINSTR] = {
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
  8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,
};

constexpr long ncycles[NINSTR] = {
  4,  12, 8,  8,  4,  4,  8,  4,  20, 8,  8,  8,  4,  4,  8,  4,
  4,  12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  4,
  12, 12, 8,  8,  4,  4,  8,  4,  12, 8,  8,  8,  4,  4,  8,  4,
  12, 12, 8,  8,  12, 12, 12, 4,  12, 8,  8,  8,  4,  4,  8,  4,
  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
  8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4,
  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
  4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,
  20, 12, 16, 16, 24, 16, 8,  16, 20, 16, 16, 4,  24, 24, 8,  16,
  20, 12, 16, 0,  24, 16, 8,  16, 20, 16, 16, 0,  24, 0,  8,  16,
  12, 12, 8,  0,  0,  16, 8,  16, 16, 4,  16, 0,  0,  0,  8,  16,
  12, 12, 8,  4,  0,  16, 8,  16, 12, 8,  16, 4,  0,  0,  8,  16,
};

class CPU {
public:
  CPU(std::string path): a(0), f(0), b(0), c(0), d(0), e(0), h(0), l(0),
    pc(0x0000), sp(0x0000), cycles(0), mmu(path), ppu(*this), halted(false) {}

  enum Flag {
    Zf = 1 << 7,
    Nf = 1 << 6,
    Hf = 1 << 5,
    Cf = 1 << 4,
  };

  bool Z() { return (f & Zf) == Zf; }
  bool N() { return (f & Nf) == Nf; }
  bool H() { return (f & Hf) == Hf; }
  bool C() { return (f & Cf) == Cf; }

  void unset_flags(int flags) {
    f &= ~flags;
  }

  void set_flags(int flags) {
    f |= flags;
  }

  void toggle_flags(int flags) {
    f ^= flags;
  }

  word get_word() {
    return (mmu[pc + 1] << 8) | mmu[pc];
  }

  word get_word(byte hi, byte lo) {
    return (hi << 8) | lo;
  }

  void halt() {
    halted = true;
  }

  void stop() {
    halted = true;
  }

  void step();
  void inst();

  void enable_interrupts() {}
  void enable_interrupts_next_instruction() {}
  void disable_interrupts_next_instruction() {}

  byte a, f, b, c, d, e, h, l;
  word pc, sp;
  long cycles;

  MMU mmu;
  PPU ppu;
  bool halted;

  static constexpr size_t NINSTR = 256;

  void _handle_cb() {
    byte instr = mmu[pc];
    ++pc;
    cb_ops[instr](*this);
    cycles += ncycles_cb[instr];
  }

  static void handle_cb(CPU& cpu) {
    cpu._handle_cb();
  }

  static std::array<std::function<void(CPU&)>, 256> cb_ops;
  static std::array<std::function<void(CPU&)>, 256> ops;
};
