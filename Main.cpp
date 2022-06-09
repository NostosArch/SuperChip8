#include "Chip8.hpp"
#include "Platform.hpp"
#include "Chip8.cpp"
#include "Platform.cpp"
#include <chrono>
#include <iostream>

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cout << "Please provide the correct parameters: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

	Platform platform(video_width * videoScale, video_height * videoScale, video_width, video_height);

	Chip8 chip8;
	chip8.LoadROM(romFilename);

	//SDL stores the video as a row, so this says how many rows (bytes) should be reserved for the video size
	int videoPitch = sizeof(chip8.video[0]) * video_width;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;
	
	while(!quit)
	{
		quit = platform.ProcessInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;
			
			chip8.Cycle();
			
			platform.Update(chip8.video, videoPitch);
		}
	}

	return 0;
}
