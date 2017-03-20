/*
 * levelRNG - Helper file for rigel
 * Automatically generates all the levels in the game using C++'s pseudo-random number generator
 * Author: Yohan, Bangalore
 */
#include <fstream>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

int blockCount = 100;
int heightValue = 25;
const int widthValue = 100;

int powerupCount = 2;
int powerupRadius = 35;

string level = "data/level";
string levelNum = "0";
string background;

template <typename T>
string to_string(T value)
{
    ostringstream oss;
    oss << value;
    return oss.str();
}

void otherObjectGenerator(string levelNumber)
{
	string file = level + levelNumber + ".data";
	ofstream out2(file.c_str(), ios::binary| ios::trunc);
	out2 << blockCount << endl;
	out2 << powerupCount;
	out2.close();
}
struct powerupData
{
	int radius;
	int posY;
	int posX;
	int type;
};
struct rectangleData
{
	int height; 
	int width;
	int posY;
	int posX;
};
struct rectangleColumn
{
	rectangleData top;
	rectangleData bottom;
};

void powerupGenerator(string levelNumber)
{
	powerupData lvl1;
	if(levelNumber == "0")
	{
		lvl1.type = 0;
		lvl1.posY = 360;
		lvl1.radius = powerupRadius;
		lvl1.posX = 850;
	}
	powerupData x[powerupCount];
	string file = level + levelNumber + ".pow";
	ofstream output(file.c_str(), ios::binary);
	output.write((char*)&lvl1, sizeof(lvl1));
	srand(time(NULL));
	int i;
	for(i = 0; i < powerupCount; i++)
	{
		x[i].type = 0;
		x[i].posY = 360;
		x[i].radius = powerupRadius;
		x[i].posX = rand() % 100 * blockCount;
		output.write((char*)&x[i], sizeof(x[i]));
	}
	output.close();
}
void blockGenerator(string levelNumber)
{
	string file = level + levelNumber + ".map";
	ofstream output(file.c_str(), ios::binary);
	srand(time(NULL));
	rectangleColumn x[blockCount];

	for(int i = 0; i < blockCount; i++)
	{
		x[i].top.posY = 0;
		x[i].top.width = widthValue;
		x[i].top.height = rand() % heightValue + 25;  
		x[i].bottom.height = rand() % heightValue + 25;
		x[i].bottom.posY = 575 - x[i].bottom.height;
		x[i].bottom.width = widthValue;
		output.write((char*)&x[i], sizeof(x[i]));
	}
	output.close();
}
int main()
{
	string levelNumber = "0";
	float i;
	for(i = 0; i < 10; i++)
	{
		levelNumber = to_string(i);
		background = "background" + levelNumber + ".jpg";
		otherObjectGenerator(levelNumber);
		powerupGenerator(levelNumber);
		blockGenerator(levelNumber);
		if(blockCount <= 350)
		{	
			blockCount += 50;
		}
		if(heightValue <= 125)
		{
			heightValue += 15;
		}
		powerupCount++;
	}	
	cout << i * 3 << " files successfully created";
	return 0;
}
