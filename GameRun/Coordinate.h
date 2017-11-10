#ifndef Coor_H_
#define Coor_H_

#include "mouse.h"
#include "templeImg.h"
class  Coordinate{
public:
	Coordinate(int _x,int _y,int _v );
	~Coordinate();
	int x;
	int y;
	int v;
	TempleImg *te;
	bool MouseClick(const int _v,const int n=1);
	CvRect rect;
}; 
bool compCoordinate(const Coordinate &a,const Coordinate &b);
#endif