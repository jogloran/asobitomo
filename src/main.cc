#include <iostream>
#include <iomanip>
#include <fstream>
#include "types.h"
#include "cpu.h"

using namespace std;

int main() {
  CPU cpu("Tetris.gb");

  copy(cpu.mmu.rom.begin(), cpu.mmu.rom.end(), cpu.mmu.mem.begin());
  cpu.mmu.mem[0xff44] = 0x90;

  while (!cpu.halted && cpu.pc != 0x100) {
    cpu.step();
  }
  //
  int i = 0;
  while (i++ < 1500000) {
    cpu.step();
  }

  std::ofstream out("vram");
  copy(cpu.mmu.mem.begin() + 0x8000, cpu.mmu.mem.begin() + 0x9000, ostream_iterator<unsigned int>(out, " "));
  out.close();

  out.open("oam");
  copy(cpu.mmu.mem.begin() + 0xfe00, cpu.mmu.mem.begin() + 0xfea0, ostream_iterator<unsigned int>(out, " "));
  out.close();

  out.open("btt");
  copy(cpu.mmu.mem.begin() + 0x9800, cpu.mmu.mem.begin() + 0x9c00, ostream_iterator<unsigned int>(out, " "));
  out.close();
}
