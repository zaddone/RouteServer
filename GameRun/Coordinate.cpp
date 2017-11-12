#include "stdafx.h"
#include "Coordinate.h"

Coordinate::Coordinate(TempleImg *t,CvRect r,int o  ){
	this->te = t;
	this->rect = r;
	order = o;
}
Coordinate::~Coordinate(){

}
bool Coordinate::MouseClick(const int _v,const int n){
	if (this->te != NULL)if (this->te->Tag != _v ) return false;
	
	int x = this->rect.x + this->rect.width/2;
	int y = this->rect.y + this->rect.height/2;
	MouseMove(x,y);	
	for (int i=0;i<n;i++){
		Sleep(100);
		MouseLeftDown();
		Sleep(100);
		MouseLeftUp();	
	}
	MouseMove(130,130);
	Sleep(100);
	return true;
}
 
bool compCoordinate(const Coordinate &a,const Coordinate &b)
{
	return a.rect.x<b.rect.x;
}
bool compCoordinateOrder(const Coordinate &a,const Coordinate &b)
{
	return a.order > b.order;
}