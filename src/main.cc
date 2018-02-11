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
}
