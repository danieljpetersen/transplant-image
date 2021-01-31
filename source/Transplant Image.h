#ifndef TRANSPLANT_IMAGE_H
#define TRANSPLANT_IMAGE_H

#include <SFML/Graphics.hpp>
#include <deque>
#include <list>

class My_Pixel_Container
{
public:
	sf::Color Color;
	int x, y;
};

class Transplant_Image
{
private:
	unsigned int PixelSourceImageWidth, PixelSourceImageHeight;
	std::vector<std::vector<bool>> PixelsProcessed;
	std::deque<sf::Vector2i> PixelQueue;

	std::string BinPath;
	void getBinPath();

	std::list<My_Pixel_Container> SourcePixels;

	double rankClosenessOfColors(sf::Color a, sf::Color b);
	sf::Color findClosestUnusedPixel(sf::Color DesiredColor);
	void addSurroundingPixelsToQueue(int x, int y);

public:
	Transplant_Image()
    {
	};
	
	void process(std::string PixelSourcePath, std::string DesiredResultPath);
};
 
#endif 