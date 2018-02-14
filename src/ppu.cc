#include "ppu.h"
#include "cpu.h"

void
PPU::update_stat_register()  {
  cpu.mmu[0xff41] = static_cast<byte>(mode);
  cpu.mmu[0xff44] = static_cast<byte>(line);
}
