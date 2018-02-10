// This program is for weak perspective projection model
// 
//  
//
#include <iostream>
#include <cmath>

#define pi 3.141592654


int main(int argc, char** argv)
{
	// unit: mm
	double f = 50;

	// unit: mm
	double Z = 50 * 1000;

	double alpha = (60 / 180)*pi;

	// magnification 
	double m = f / Z;

	// assume point coordiantes
	double XA = 10, YA = 20, ZA = 30;
	double XB = 5, YB = 20, ZB = 30;

	// get projection value through weak perspective projection model
	double xa = m*(XA - (ZA - Z)*std::tan((60 / 180)*pi));
	double ya = m*(YA - (ZA - Z)*std::tan((60 / 180)*pi));
	double za = Z;

	double xb = m*(XB - (ZB - Z)*std::tan((60 / 180)*pi));
	double yb = m*(YB - (ZB - Z)*std::tan((60 / 180)*pi));
	double zb = Z;

	std::cout << "xa: " << xa << "\n" <<
		"ya: " << ya << "\n" <<
		"za: " << Z << "\n" <<
		"xb: " << xb << "\n" <<
		"yb: " << yb << "\n" <<
		"zb: " << Z << std::endl;



	return 0;
}


