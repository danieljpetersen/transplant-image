#include "Transplant Image.h"
#include <math.h>
#include <sstream>
#include <sys/stat.h>
#include <SFML/System.hpp>
#include <zconf.h>

void Transplant_Image::getBinPath()
{
	// get executable path, then remove executable to get the directory path
#define PATH_MAX        4096    /* # chars in a path name including nul */
	char result[ PATH_MAX ];
	ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
	BinPath = std::string( result, (count > 0) ? count : 0 );

	sf::String s = BinPath ;
	int Count = 0, SlashCount = 0, DesiredSlashCount=1;
	for (unsigned int i = s.getSize()-1; i >= 0; i--)
	{
		if (BinPath.at(i) == '/')
		{
			SlashCount++;
			if (SlashCount == DesiredSlashCount)
			{
				break;
			}
		}
		Count++;
	}

	// remove executable name.  we're just trying to get the executable dir
	for (int i = 0; i < Count; i++)
	{
		BinPath .pop_back();
	}
}

void Transplant_Image::process(std::string PixelSourcePath, std::string DesiredResultPath)
{
	getBinPath();
	PixelSourcePath = BinPath + PixelSourcePath;
	DesiredResultPath = BinPath + DesiredResultPath;

	// we're going to take the pixels from this image
	sf::Image PixelSourceImage;
	PixelSourceImage.loadFromFile(PixelSourcePath);
	
	// and rearrange them so that they look as close to this image as possible
	sf::Image DesiredResultImage;
	DesiredResultImage.loadFromFile(DesiredResultPath);

	PixelSourceImageWidth = PixelSourceImage.getSize().x;
	PixelSourceImageHeight = PixelSourceImage.getSize().y;
	
	sf::Image Output;
	Output.create(PixelSourceImageWidth, PixelSourceImageHeight);

	// flag every pixel as unprocessed; add all Source Pixels to a giant list
	PixelsProcessed.resize(PixelSourceImageHeight);
	for (unsigned int y = 0; y < PixelSourceImageHeight; y++)
	{
		PixelsProcessed[y].resize(PixelSourceImageWidth);
		for (unsigned int x = 0; x < PixelSourceImageWidth; x++)
		{
			PixelsProcessed[y][x] = false;

			SourcePixels.push_back(My_Pixel_Container());
			SourcePixels.back().Color = PixelSourceImage.getPixel(x, y);
			SourcePixels.back().x = x;
			SourcePixels.back().y = y;
		}
	}

	//start in center
	int xCenter = PixelSourceImageWidth / 2;
	int yCenter = PixelSourceImageHeight / 2;
	PixelQueue.push_back(sf::Vector2i(xCenter, yCenter));
	PixelsProcessed[xCenter][yCenter] = true;

	int loops = 0;
	while (SourcePixels.size() > 0)
	{
		loops++;
		if (loops > 1000)
		{	
			std::stringstream ss;
			ss << "Still working. . .  There are " << SourcePixels.size() << " pixels left to process." << std::endl;
			printf(ss.str().c_str());
			loops = 0;
		}

		int x = PixelQueue.back().x;
		int y = PixelQueue.back().y;
		sf::Color DesiredColor = DesiredResultImage.getPixel(x, y);
		PixelQueue.pop_back();

		sf::Color ClosestColor = findClosestUnusedPixel(DesiredColor);
		Output.setPixel(x, y, ClosestColor);

		addSurroundingPixelsToQueue(x, y);
	}
	
	Output.saveToFile("output.png");
}

sf::Color Transplant_Image::findClosestUnusedPixel(sf::Color DesiredColor)
{
	double ClosestRank = -1;
	std::list<My_Pixel_Container>::const_iterator ClosestIterator;
	for (std::list<My_Pixel_Container>::const_iterator iter = SourcePixels.begin(), end = SourcePixels.end(); iter != end; ++iter) 
	{
		double Rank = rankClosenessOfColors(DesiredColor, (*iter).Color);
		if ((Rank < ClosestRank) || (ClosestRank == -1))
		{
			ClosestRank = Rank;
			ClosestIterator = iter;
		}
	}	

	sf::Color Color = (*ClosestIterator).Color;
	SourcePixels.erase(ClosestIterator);
	return Color;
}

double Transplant_Image::rankClosenessOfColors(sf::Color e1, sf::Color e2)
{
	long rmean = ((long)e1.r + (long)e2.r) / 2;
	long r = (long)e1.r - (long)e2.r;
	long g = (long)e1.g - (long)e2.g;
	long b = (long)e1.b - (long)e2.b;
	return sqrt((((512 + rmean)*r*r) >> 8) + 4 * g*g + (((767 - rmean)*b*b) >> 8));
}

void Transplant_Image::addSurroundingPixelsToQueue(int x, int y)
{		
	// I have no idea what I was thinking here, but hey, it works
	std::vector<sf::Vector2i> Neighbors;
	Neighbors.push_back(sf::Vector2i(x - 1, y));
	Neighbors.push_back(sf::Vector2i(x - 1, y - 1));
	Neighbors.push_back(sf::Vector2i(x, y - 1));
	Neighbors.push_back(sf::Vector2i(x + 1, y - 1));
	Neighbors.push_back(sf::Vector2i(x + 1, y));
	Neighbors.push_back(sf::Vector2i(x + 1, y + 1));
	Neighbors.push_back(sf::Vector2i(x, y + 1));
	Neighbors.push_back(sf::Vector2i(x - 1, y + 1));
	const int TOLEFT = 0;
	const int TOTOPLEFT = 1;
	const int TOTOP = 2;
	const int TOTOPRIGHT = 3;
	const int TORIGHT = 4;
	const int TOBOTTOMRIGHT = 5;
	const int TOBOTTOM = 6;
	const int TOBOTTOMLEFT = 7;
		
	for (int i = 0; i < 8; i++)
	{
		bool Valid = true;
		if ((i == TOLEFT) || (i == TOTOPLEFT) || (i == TOBOTTOMLEFT))
			if (x == 0)
				Valid = false;

		if ((i == TOTOPLEFT) || (i == TOTOP) || (i == TOTOPRIGHT))
			if (y == 0)
				Valid = false;

		if ((i == TOTOPRIGHT) || (i == TORIGHT) || (i == TOBOTTOMRIGHT))
			if (x == PixelSourceImageWidth - 1)
				Valid = false;

		if ((i == TOBOTTOMLEFT) || (i == TOBOTTOM) || (i == TOBOTTOMRIGHT))
			if (y == PixelSourceImageHeight - 1)
				Valid = false;

		if (Valid)
		{
			int xIndex = Neighbors[i].x;
			int yIndex = Neighbors[i].y;
			
			if (PixelsProcessed[yIndex][xIndex] != true)
			{
				PixelsProcessed[yIndex][xIndex] = true;
				PixelQueue.push_front(Neighbors[i]);
			}
		}
	}
}