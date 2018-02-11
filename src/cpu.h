#include "types.h"
#include "mmu.h"
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
  CPU(std::string path): a(0), f(0), b(0), c(0), d(0), e(0), h(0), l(0), Z(0), H(0), N(0), C(0),
    pc(0x0000), sp(0x0000), cycles(0), mmu(path), halted(false) {}

  enum Flag {
    Zf = 1 << 4,
    Hf = 1 << 3,
    Nf = 1 << 2,
    Cf = 1 << 1
  };

  void unset_flags(int flags) {
    if (flags & Zf) {
      Z = 0;
    }
    if (flags & Hf) {
      H = 0;
    }
    if (flags & Nf) {
      N = 0;
    }
    if (flags & Cf) {
      C = 0;
    }
  }

  void set_flags(int flags) {
    if (flags & Zf) {
      Z = 1;
    }
    if (flags & Hf) {
      H = 1;
    }
    if (flags & Nf) {
      N = 1;
    }
    if (flags & Cf) {
      C = 1;
    }
  }

  void halt() {
    halted = true;
  }

  void stop() {
    halted = true;
  }

  void step() {
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
      "Z: " << setw(2) << hex << static_cast<int>(Z) << ' ' <<
      "N: " << setw(2) << hex << static_cast<int>(N) << ' ' <<
      "H: " << setw(2) << hex << static_cast<int>(H) << ' ' <<
      "C: " << setw(2) << hex << static_cast<int>(C) << ' ' << endl;

    ++pc;
    ops[instr](*this);
    cycles += ncycles[instr];

    cout << endl;
  }
  void inst();

  void enable_interrupts() {}
  void enable_interrupts_next_instruction() {}
  void disable_interrupts_next_instruction() {}

  byte a, f, b, c, d, e, h, l;
  byte Z, H, N, C;
  word pc, sp;
  long cycles;

  MMU mmu;
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
