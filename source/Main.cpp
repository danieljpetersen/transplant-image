#include <SFML/Graphics.hpp>
#include "Transplant Image.h"

int main()
{	
	Transplant_Image TransplantImage = Transplant_Image();
	TransplantImage.process("pixel-source.png", "desired-result.png");

	return 0;
}		
