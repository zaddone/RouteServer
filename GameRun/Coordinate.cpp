#include "stdafx.h"
#include "Coordinate.h"

Coordinate::Coordinate(TempleImg *t,CvRect r,double o  ){
	this->te = t;
	this->rect = r;
	order = o;
}
Coordinate::~Coordinate(){

}
void Coordinate::Fill(IplImage * src ){
	int i,j;
	int height = rect.y + rect.height;
	int width = rect.x + rect.width;
	int step       = src->widthStep;
	uchar * data    =  (uchar *) src->imageData;
	//int channels   = src->nChannels;
	for (i=this->rect.y;i< height;i++){
		for(j=this->rect.x;j< width;j++){
			data[i *step+j] = 0;
		}
	}
}
void Coordinate::Fill(IplImage * src,CvRect rect_old){
	int i,j;
	int y = (rect.y - rect_old.y );
	int x = (rect.x - rect_old.x );
	int height = y + rect.height;
	int width  = x + rect.width;
	int step       = src->widthStep;
	uchar * data    =  (uchar *) src->imageData;
	//int channels   = src->nChannels;
	//printf("%d %d %d %d\r\n",i,j,height,width);
	for (i=y ;i< height;i++){
		for(j=x ;j< width;j++){
			data[i *step+j] = 0;
		}
	}
	/**
	cvNamedWindow("contour1");
	cvShowImage("contour1", src);  
	cvWaitKey(0);
	cvDestroyWindow("contour1");
	//cvReleaseImage(&dst);
	**/
}
bool Coordinate::Check(const int _v){
	if (this->te != NULL)if (this->te->Tag != _v ) return false;
	return true;
}
void Coordinate::MouseClick(const int n){
	//if (this->te != NULL)if (this->te->Tag != _v ) return false;
	
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
	//return true;
}
 
bool compCoordinate(const Coordinate &a,const Coordinate &b)
{
	return a.rect.x<b.rect.x;
}
bool compCoordinateOrder(const Coordinate &a,const Coordinate &b)
{
	return a.order < b.order;
}