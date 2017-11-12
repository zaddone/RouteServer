#include "stdafx.h"
#include "Coordinate.h"

Coordinate::Coordinate(int _x,int _y,int _v ){
	x= _x;
	y = _y;
	v = _v;
}
Coordinate::~Coordinate(){

}
bool Coordinate::MouseClick(const int _v,const int n){
	if (v!= _v ) return false;
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
	return a.x<b.x;
}