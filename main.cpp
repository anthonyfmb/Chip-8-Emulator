#include <chrono>
#include <iostream>

#include "Chip8Emulator.hpp"
#include "Platform.hpp"

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::exit(EXIT_FAILURE);
  }

  int video_scale = std::stoi(argv[1]);
  int cycle_delay = std::stoi(argv[2]);
  char const *rom_filename = argv[3];

  Platform platform("CHIP-8 Emulator", GFX_WIDTH * video_scale, GFX_HEIGHT * video_scale,
      GFX_WIDTH, GFX_HEIGHT);

  Chip8Emulator chip8;
  chip8.LoadROM(rom_filename);

  int video_pitch = sizeof(chip8.video[0]) * GFX_WIDTH;

  auto last_cycle_time = std::chrono::high_resolution_clock::now();
  bool quit = false;

  while (!quit) {
    quit = platform.ProcessInput(chip8.keypad);

    auto current_time = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>
      (current_time - last_cycle_time).count();

    if (dt > cycle_delay) {
      last_cycle_time = current_time;
      chip8.Cycle();
      platform.Update(chip8.video, video_pitch);
    }
  }

  return 0;
}
