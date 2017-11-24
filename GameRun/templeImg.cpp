#include "stdafx.h"
#include "templeImg.h"
//#include <time.h>
#include "common.h"
#include <sys/stat.h>  


TempleImg::TempleImg(IplImage * src ,const int tag,int _order ){
	this->img = src;
	Tag = tag;
	this->order = _order;
}
TempleImg::TempleImg(IplImage * src ,CvRect r,const int tag ){
	cvSetImageROI(src, r);	
	this->img=cvCreateImage(cvSize(r.width,r.height),src->depth,src->nChannels);
	cvCopy(src,img);
	cvResetImageROI(src);
	Tag = tag;
	this->order =0;
}
TempleImg::~TempleImg(){
	cvReleaseImage(&this->img);
	//this->img = cvLoadImage(path,0);
}
void TempleImg::SaveTemple(char  *filePath){	
	char file[1024] = {0};		
	//this->GetTemplePath(file);
	CreatDir(filePath);
	sprintf(file,"%s\\%d.bmp\0",filePath,this->Tag);
	int result;
	struct _stat st;
	result = _stat(file, &st);
	if (result == 0){
		clock_t begin = clock();
		while(result==0){
			begin++;
			sprintf(file,"%s\\%d_%d.bmp\0",filePath,this->Tag,begin);
			result = _stat(file, &st);		
		
		}	
	}
	cvSaveImage(file,this->img);
}
bool compTempleImgOrder(TempleImg *a,TempleImg *b)
{
	return a->order > b->order;
}
void TempleImg::ShowImg(){
	cvNamedWindow("contour1");
	cvShowImage("contour1", this->img);  
	cvWaitKey(0);
	cvDestroyWindow("contour1");
	//cvReleaseImage(&dst);
}