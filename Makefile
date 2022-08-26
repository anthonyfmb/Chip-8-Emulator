
emulator:
	g++ -std=c++11 main.cpp Platform.cpp Chip8Emulator.cpp -o chip8 -I include -L lib -l SDL2	
