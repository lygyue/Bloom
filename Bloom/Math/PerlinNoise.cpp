/***********************************************
 * File: PerlinNoise.cpp
 *
 * Author: LYG
 * Date: Ò»ÔÂ 2019
 *
 * Purpose:
 *
 * 
 **********************************************/
#include "PerlinNoise.h"

void BuildPerlinTexture(unsigned char* Data, int Width, int Height, unsigned int seed)
{
	double frequency;
	frequency = 8.0;

	int octaves;
	octaves = 8;

	const PerlinNoise perlin(seed);
	const double fx = double(Width) / frequency;
	const double fy = double(Height) / frequency;

	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			double Value = perlin.octaveNoise0_1(x / fx, y / fy, octaves);
			int pos = x + y * Width;
			unsigned char r = Value >= 1.0 ? 255 : Value <= 0.0 ? 0 : static_cast<std::uint8_t>(Value * 255.0 + 0.5);
			Data[pos] = r;
		}
	}
}