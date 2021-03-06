#include "stdafx.h"
#include "readNumber.h"
#include "common.h"
#include <vector>
using namespace std;
bool compRectNum(CvRect a,CvRect b)
{
	return a.x<b.x;
}
void SaveTempleName(char  *filePath,char * name,IplImage *img){	
	char file[1024] = {0};		
	CreatDir(filePath);
	clock_t begin = clock();
	sprintf(file,"%s\\%s_%d.bmp\0",filePath,name,begin);
	//printf("%s\r\n",file);
	int result;
	struct _stat st;
	result = _stat(file, &st);
	while(result==0){
		begin++;
		sprintf(file,"%s\\%s_%d.bmp\0",filePath,name,begin);
		result = _stat(file, &st);				
	}
	int r = cvSaveImage(file,img);
	printf("%s %d\r\n",file,r);
}
void SaveTemple(char  *filePath,IplImage *img){	
	char file[1024] = {0};		
	CreatDir(filePath);
	clock_t begin = clock();
	sprintf(file,"%s\\%d.bmp\0",filePath,begin);
	int result;
	struct _stat st;
	result = _stat(file, &st);
	while(result==0){
		begin++;
		sprintf(file,"%s\\%d.bmp\0",filePath,begin);
		result = _stat(file, &st);				
	}
	cvSaveImage(file,img);
}

Number::Number(int i,char *path,IplImage * temp){
	this->Num = i;
	this->temple = temp;

	this->TPath = new char[1024];
	strcpy(TPath,path);
	//sprintf(TPath,"coll\\%s",TemplePath);
	//this->TPath = path;
}
Number::~Number(){
	if (this->temple!= NULL){
		SaveImg();
		cvReleaseImage(&temple);
	}
	if (TPath != NULL) delete [] TPath;
}
void Number::SaveImg(){
	
	if (TPath == NULL)return;
	char file[100]={0};
		//char file[100]="templebak";
	CreatDir(TPath);
	sprintf(file,"%s\\%d.bmp",this->TPath,this->Num);
	if (!cvSaveImage(file,temple))printf("%s save Err\r\n",file);

}
bool Number::Matching(IplImage * img){
	if (this->temple == NULL ) return false;
	IplImage *MaxImgWidth = this->temple->width >= img->width ? this->temple  :img ;
	IplImage *MaxImgHeight = this->temple->height >= img->height ? this->temple  :img ;
	if (MaxImgWidth!=MaxImgHeight || MaxImgWidth != this->temple || MaxImgHeight != this->temple) return false;
	return MatchingIplImage(this->temple,img);
}
bool MatchingIplImage(const IplImage * dst,const IplImage * src){
	int beginHeight = (dst->height - src->height)/2;
	int beginWidth = (dst->width - src->width)/2;
	int i,j;
	int srcStep       = src->widthStep;
	uchar * srcData    =  (uchar *) src->imageData;
	//int srcNchannels   = src->nChannels;

	int dstStep       = dst->widthStep;
	uchar * dstData    =  (uchar *) dst->imageData;
	//int dstNchannels   = dst->nChannels;
	int Count=0;
	for(i=0; i < src->height; i++){
		for(j=0; j < src->width ; j++){
			if (srcData[i*srcStep+j] > 0 ){
				Count++;
				if (dstData[(i+beginHeight)*dstStep + (j+beginWidth)] == 0) {
					return false;
				}
			}
		}
	}
	if (Count==0)return false;
	return true;
}
void Number::Merge(IplImage  *img){
	printf("merge num %d\r\n",this->Num);
	if (this->temple == NULL){
		this->temple =cvCreateImage(cvSize(img->width,img->height),img->depth,img->nChannels) ;
		cvCopy(img,temple);
	}else{	
		IplImage *MaxImgWidth = this->temple->width > img->width ? this->temple  :img ;
		IplImage *MaxImgHeight = this->temple->height > img->height ? this->temple  :img ;
		//IplImage * MaxImg;
		if (MaxImgWidth != MaxImgWidth){
			IplImage * temp = this->temple;
			this->temple = cvCreateImage(cvSize((temp->width > img->width ? temp->width :img->width), (temp->height > img->height ? temp->height :img->height)), img->depth,img->nChannels);
			MergeIplImage(this->temple,img);
			MergeIplImage(this->temple,temp);
			cvReleaseImage(&temp);
		}else if (MaxImgWidth == this->temple){
			MergeIplImage(MaxImgWidth,img);
		}else{
			MaxImgWidth = this->temple;
			this->temple =cvCreateImage(cvSize(img->width,img->height),img->depth,img->nChannels) ;
			cvCopy(img,temple);
			MergeIplImage(this->temple,MaxImgWidth);  
			cvReleaseImage(&MaxImgWidth);
		}	
	}
	SaveImg();
	//cvReleaseImage(&img);
}
void MergeIplImage(IplImage * dst,const IplImage * src){
	int beginHeight = (dst->height - src->height)/2;
	int beginWidth = (dst->width - src->width)/2;
	int i,j;
	int srcStep       = src->widthStep;
	uchar * srcData    =  (uchar *) src->imageData;
	//int srcNchannels   = src->nChannels;

	int dstStep       = dst->widthStep;
	uchar * dstData    =  (uchar *) dst->imageData;
	//int dstNchannels   = dst->nChannels;

	uchar val;
	for(i=0; i < src->height; i++){
		for(j=0; j < src->width ; j++){
			val = srcData[i*srcStep+j];
			if ( val > 0 ){
				dstData[(i+beginHeight)*dstStep + (j+beginWidth)] = val;
			}
		}
	}
}
NumberList::~NumberList(){
	for (int i=0;i<10;i++){
		if (Nums[i]!=NULL){
			delete Nums[i];
		}
	}
	delete [] templebak;
}
NumberList::NumberList(char * TemplePath,int coll,int sep){
//Number ** Nums = new (Number**)[10]; 
	
	this->templebak = NULL;
	int i;
	Sep=sep;
	if (TemplePath != NULL){
		for (i=0;i<10;i++){		 
			Nums[i] = new Number(i,TemplePath);
		}

		if (coll!=0){
			this->templebak = new char[1024];
			sprintf(templebak,"coll\\%s",TemplePath);
			//this->templebak = bak;
		}

	}
	if (TemplePath== NULL) return;
	struct _finddata_t fd;
	char searchPath[100];
	char path[100];
	char Num[10]={0};
	char * pch;
	if (-1 == _access(TemplePath,0)){
		_mkdir(TemplePath);
	}
	sprintf(searchPath,"%s\\*.bmp",TemplePath);
	size_t nlen =0;
	intptr_t handle = _findfirst(searchPath, &fd);
	if (handle != -1) {
		do   {
			sprintf(path,"%s\\%s",TemplePath,fd.name);
			IplImage * img = cvLoadImage(path,CV_LOAD_IMAGE_ANYDEPTH);
			
			if (img==NULL)continue;
				
			pch = strchr(fd.name,'.');
			if (pch == NULL) nlen = 1;
			else	nlen = pch-fd.name;
			memcpy(Num,fd.name,nlen);
			Num[nlen] = 0;
			this->SetNums(atoi(Num),img);
			//i = atoi(Num);
			//Nums[i]->temple = img ;

		}  while (_findnext(handle, &fd)==0);
		_findclose(handle);
	}
}
NumberList::NumberList(char * TemplePath,char * bak,int sep){
	
	//Number ** Nums = new (Number**)[10]; 
	if (bak!= NULL){
		this->templebak = new char[1024];
		strcpy(this->templebak , bak);
	}
	int i;
	Sep=sep;
	if (TemplePath != NULL){
		for (i=0;i<10;i++){		 
			Nums[i] = new Number(i,TemplePath);
		}
	}
	if (TemplePath== NULL) return;
	struct _finddata_t fd;
	char searchPath[100];
	char path[100];
	char Num[10]={0};
	char * pch;
	if (-1 == _access(TemplePath,0)){
		_mkdir(TemplePath);
	}
	sprintf(searchPath,"%s\\*.bmp",TemplePath);
	size_t nlen =0;
	intptr_t handle = _findfirst(searchPath, &fd);
	if (handle != -1) {
		do   {
			sprintf(path,"%s\\%s",TemplePath,fd.name);
			IplImage * img = cvLoadImage(path,CV_LOAD_IMAGE_ANYDEPTH);
			
			if (img==NULL)continue;
				
			pch = strchr(fd.name,'.');
			if (pch == NULL) nlen = 1;
			else	nlen = pch-fd.name;
			memcpy(Num,fd.name,nlen);
			Num[nlen] = 0;
			this->SetNums(atoi(Num),img);
			//i = atoi(Num);
			//Nums[i]->temple = img ;

		}  while (_findnext(handle, &fd)==0);
		_findclose(handle);
	}

}
void NumberList::SetNums(int i,IplImage * src){
	this->Nums[i]->temple = src;
}
void NumberList::ReadDirImg(const char * path ){
	struct _finddata_t fd;
	char searchPath[1024];
	char FilePath[1024];
	if (path== NULL)path = this->templebak;
	sprintf(searchPath,"%s\\*",path);
	size_t nlen =0;
	intptr_t handle = _findfirst(searchPath, &fd);
	if (handle != -1) {
		do   {
			if (fd.attrib & _A_SUBDIR)    
			{
				//这个语句很重要
				if( (strcmp(fd.name,".") != 0 ) &&(strcmp(fd.name,"..") != 0))   
				{	
					sprintf(FilePath,"%s\\%s",path,fd.name);
					this->ReadDirImg(FilePath );
				}
			}else{
				sprintf(FilePath,"%s\\%s",path,fd.name);				
				IplImage * img = cvLoadImage(FilePath,CV_LOAD_IMAGE_ANYDEPTH);
				if (img == NULL)continue;
				//printf("%s %d %d\r\n",searchPath,img->depth,img->nChannels);
				cvThreshold( img,img, this->Sep , 255, CV_THRESH_BINARY  );
				this->TrainImg(img);
				//remove(searchPath);
				cvReleaseImage(&img);
				
			}

		}while (_findnext(handle, &fd)==0);
		_findclose(handle);
	}
}
double inSame(IplImage *img ,IplImage * num,CvPoint *loc){
	int w = img->width - num->width + 1;	
	int h = img->height - num->height + 1;	 
	if (w<1 && h<1) {
		return inSame(num,img,loc);
	}else if (w<1 || h<1)return 0;	 
	IplImage* imgResult = cvCreateImage(cvSize(w,h),IPL_DEPTH_32F,1); 
	cvMatchTemplate(img,num,imgResult,CV_TM_CCOEFF_NORMED); 	 
    double max_val,min_val; 
	CvPoint minLoc;
	cvMinMaxLoc(imgResult, &min_val, &max_val, &minLoc, loc, NULL);  
	//if (max_val>0.8)printf("%f\r\n",max_val);
	cvReleaseImage(&imgResult);
	//printf("t %f\r\n",max_val);
	return max_val;

}
Number * NumberList::CheckImg( IplImage * src){
	int i,Len = 0;
	Number ** likeNum = new Number * [10] ;
	Number * n =NULL;
	for (i=0;i<10;i++){
		if (this->Nums[i]->Matching(src)){
			likeNum[Len] = this->Nums[i];
			Len++;
			//printf("l %d %d\r\n",l,i);
		}
	}
	if (Len == 1){
		n = likeNum[0];
	}else if (Len > 1){
		double MaxVal = 0,tmpVal;
		int iu=0;
		for (i = 0;i<Len;i++){
			tmpVal = inSame(src,likeNum[i]->temple,NULL);
			if (tmpVal>MaxVal){
				iu = i;
				MaxVal = tmpVal;
			}
		}
		n = likeNum[iu];
		likeNum[iu]->Merge(src);
	}
	delete [] likeNum;
	return n;
}
void NumberList::TrainImg(IplImage * img){
	//NumberList *n = (NumberList *)handle;
	int l=0,i;
	Number * likeNum[10] ;
	
	for (i=0;i<10;i++){
		//printf("i %d\r\n",i);
		//Number *Num = n->Nums[i];
		if (this->Nums[i]->Matching(img)){
			likeNum[l] = this->Nums[i];
			l++;
			printf("l %d %d\r\n",l,i);
		}
	}
	if (l == 1) return;

	int WindowsWidth =0,WindowsHeight=0;
	for (i=0;i<l;i++){
		WindowsWidth+=likeNum[i]->temple->width;
		if (likeNum[i]->temple->height > WindowsHeight) WindowsHeight = likeNum[i]->temple->height;
	}
	if (WindowsWidth<img->width){
		WindowsWidth = img->width;
	}
	WindowsHeight+=img->height;

	IplImage * WindowsImg = cvCreateImage(cvSize(WindowsWidth,WindowsHeight), img->depth,img->nChannels);
	CvRect rect;
	int beginX=0;
	for (i=0;i<l;i++){
		rect = cvRect(beginX,0,likeNum[i]->temple->width,likeNum[i]->temple->height);
		cvSetImageROI(WindowsImg, rect);
		cvCopy(likeNum[i]->temple,WindowsImg);
		cvResetImageROI(WindowsImg); 
		beginX+=likeNum[i]->temple->width;
	}
	rect= cvRect(0,WindowsHeight - img->height,img->width,img->height );
	cvSetImageROI(WindowsImg, rect);
	cvCopy(img,WindowsImg);
	cvResetImageROI(WindowsImg);
	cvNamedWindow("contour1");
	cvShowImage("contour1", WindowsImg);  
	
	char ValStr = 0;
	ValStr=cvWaitKey(0);
	//i = atoi(ValStr);
	printf("ValStr  %d\r\n",ValStr);



	if (ValStr>47 && ValStr<58){
		this->Nums[ValStr-48]->Merge(img);
	}else {
		if (ValStr==13){
			int maxLen = 12;
			char * key = new char[maxLen];
			for (int i = 0;i<maxLen;i++){
				ValStr = cvWaitKey(0);
				printf("ValStr  %d\r\n",ValStr);
				if (ValStr>47 && ValStr<58){
					key[i]= ValStr;
				}else{
					break;
				}
			}

			//SaveTemple(templebak,img);
		//}else if (ValStr==32){			
		}
	}

	cvDestroyWindow("contour1");
	cvReleaseImage(&WindowsImg);
}
int NumberList::Know(IplImage * _src,int sep){
	/**
	IplImage * src = NULL;
	if (this->templebak == NULL ){
		src= _src;
	}else{
		src = cvCreateImage(cvSize(_src->width,_src->height), _src->depth,_src->nChannels);
		
	}
	**/
	IplImage *src = cvCreateImage(cvSize(_src->width,_src->height), _src->depth,_src->nChannels);
	if (sep>0)this->Sep = sep;
	cvThreshold( _src,src,Sep , 255, CV_THRESH_BINARY  );

	vector <CvRect> templeRect;
	CvSeq* contours = NULL;
	CvMemStorage* storage = cvCreateMemStorage(0);		
	int count = cvFindContours(src, storage, &contours,sizeof(CvContour),CV_RETR_EXTERNAL); 

	//int tempCount=0;
	for (CvSeq* c = contours; c != NULL; c = c->h_next) {  
		templeRect.push_back(cvBoundingRect(c,0));
	}
	sort(templeRect.begin(),templeRect.end(),compRectNum);
	int j = 0;
	Number ** likeNum = new Number*[10] ;
	int l = 0,x;
	for (vector<CvRect>::iterator it = templeRect.begin(); it != templeRect.end(); it ++) {
		cvSetImageROI(src, (*it));
		IplImage * dst=cvCreateImage(cvSize((*it).width,(*it).height),src->depth,src->nChannels);
		cvCopy(src,dst);
		cvResetImageROI(src);

		Number *n = this->CheckImg(dst);
		if (n == NULL){			
			x = this->splitTemple(likeNum,l,dst);
			if (dst->width != x){					
				if (this->templebak != NULL ){
					CvRect r =  cvRect((*it).x+x,(*it).y,(*it).width-x,(*it).height);
					IplImage * dst1=cvCreateImage(cvSize(r.width,r.height),src->depth,src->nChannels);
					cvSetImageROI(_src, r);
					cvCopy(_src,dst1);
					cvResetImageROI(_src);
					/**
					cvSetImageROI(_src, (*it));
					IplImage * dst1=cvCreateImage(cvSize((*it).width,(*it).height),src->depth,src->nChannels);
					cvCopy(src,dst1);
					cvResetImageROI(_src);
					**/
					SaveTemple(templebak,dst1);
					cvReleaseImage(&dst1);
				}
					
				//return 0;
			}
		}else{
			likeNum[l] = n;
			l++;
		}

		cvReleaseImage(&dst);

	}
	cvReleaseMemStorage(&storage);

	//if (this->templebak != NULL )	cvReleaseImage(&src);
	cvReleaseImage(&src);

	int _Num=0;
	for (int i = 0;i<l;i++){
		_Num*=10;
		_Num+=likeNum[i]->Num;
	}
	return _Num;
}
void NumberList::ReadListImg(){
	struct _finddata_t fd;
	char searchPath[100];
	char path[100];
	Number ** likeNum = new Number*[10] ;
	//char Num[10]={0};
	//char * pch;
	//if (-1 == _access(this->templebak,0)){
	//	_mkdir(this->templebak);
	//}
	sprintf(searchPath,"%s\\*.bmp",this->templebak);
	size_t nlen =0;
	intptr_t handle = _findfirst(searchPath, &fd);
	if (handle != -1) {
		do   {
			sprintf(path,"%s\\%s",this->templebak,fd.name);
			IplImage * img = cvLoadImage(path,CV_LOAD_IMAGE_ANYDEPTH);			
			if (img==NULL)continue;
			//bool isFind;
			cvThreshold( img,img,Sep , 255, CV_THRESH_BINARY  );
			int l=0,x=0;
			x = this->splitTemple(likeNum,l,img);
			
			int wid_x = img->width - x;
			printf("%d %d\r\n",img->width ,x);
			/**
			if (wid_x > 0){
				IplImage * Remainder = cvCreateImage(cvSize(wid_x,img->height), img->depth,img->nChannels);
				cvSetImageROI(img,cvRect(x,0,wid_x,img->height));
				cvCopy(img,Remainder);
				this->cutIplImage(Remainder);
				cvResetImageROI(img);
				cvReleaseImage(&Remainder);
			}else{	
			**/
			if (wid_x > 0){
				IplImage * WindowsImg = ShowNumsListImg(likeNum,l,img,x);			 
				cvNamedWindow("contour1");
				cvShowImage("contour1", WindowsImg);
				//char ValStr[1] = {0};
				char ValStr  = (char)cvWaitKey(0);
				
			//	if (ValStr>47 && ValStr<58){


			//		this->Nums[ValStr-48]->Merge(img);
			//	}else {
					if ( ValStr==13  ){
						remove(path);
					}
			//	}

				cvDestroyWindow("contour1");
				 
				cvReleaseImage(&WindowsImg);
			}
		//	}
			cvReleaseImage(&img);
			
		}  while (_findnext(handle, &fd)==0);
		_findclose(handle);
	}
	delete [] likeNum;
}
IplImage * ShowNumsListImg(Number **likeNum ,const int Len,IplImage * img,int x){
	int tempWindowsWidth=0;
	int tempMaxWindowsHeight=0;
	int tempWindowsHeight=0;
	int i;
	for(i=0;i<Len;i++){
		tempWindowsWidth  += likeNum[i]->temple->width;
		if (likeNum[i]->temple->height>tempMaxWindowsHeight){
			tempMaxWindowsHeight  = likeNum[i]->temple->height ;
		}
		//printf("%d %d\r\n",likeNum[i]->temple->width,likeNum[i]->temple->height);
	}
	if (tempWindowsWidth < img->width)tempWindowsWidth = img->width;
	tempWindowsHeight =tempMaxWindowsHeight+ img->height;
	if (img->width > x){
		tempWindowsHeight += img->height;
	}
	//printf("windows %d %d\r\n",tempWindowsWidth,tempWindowsHeight);
	IplImage * WindowsImg = cvCreateImage(cvSize(tempWindowsWidth,tempWindowsHeight), img->depth,img->nChannels);
	//CvRect rect;
	int beginX=0;
	for (i=0;i<Len;i++){
		//rect = cvRect(beginX,0,likeNum[i]->temple->width,likeNum[i]->temple->height);
		cvSetImageROI(WindowsImg, cvRect(beginX,0,likeNum[i]->temple->width,likeNum[i]->temple->height));
		cvCopy(likeNum[i]->temple,WindowsImg);
		cvResetImageROI(WindowsImg); 
		beginX+=likeNum[i]->temple->width;
	}
	cvSetImageROI(WindowsImg, cvRect(0,tempMaxWindowsHeight,img->width,img->height));
	cvCopy(img,WindowsImg);
	cvResetImageROI(WindowsImg); 
	if (img->width > x){
		cvSetImageROI(img,cvRect(x,0,img->width - x,img->height));
		cvSetImageROI(WindowsImg, cvRect(0,tempMaxWindowsHeight+ img->height,img->width-x,img->height));
		cvCopy(img,WindowsImg);
		cvResetImageROI(img);
		cvResetImageROI(WindowsImg); 
	}
	return WindowsImg;
}

void NumberList::cutIplImage( IplImage * src){

	vector <CvRect> templeRect;
	CvSeq* contours = NULL;
	CvMemStorage* storage = cvCreateMemStorage(0);		
	int count = cvFindContours(src, storage, &contours,sizeof(CvContour),CV_RETR_EXTERNAL); 

	//int tempCount=0;
	for (CvSeq* c = contours; c != NULL; c = c->h_next) {  
		templeRect.push_back(cvBoundingRect(c,0));
	}
	//printf("temple %d\r\n",templeRect.size());
	sort(templeRect.begin(),templeRect.end(),compRectNum);
	for (vector<CvRect>::iterator it = templeRect.begin(); it != templeRect.end(); it ++) {
		cvSetImageROI(src,*it);
		this->TrainImg(src);
		cvResetImageROI(src);
	}
	templeRect.clear();
	cvReleaseMemStorage(&storage);
}

int NumberList::splitTemple(Number ** likeNum,int & Len,IplImage * src ){
	int x=0;
	bool isFind;
	while(true){
		isFind = false;
		for (int i=0;i<10;i++){		
			if (this->Nums[i]->temple == NULL) continue;
			splitIplImage(src,this->Nums[i]->temple,x,isFind,0);	
			if (isFind){
				likeNum[Len] = this->Nums[i];
				Len++;
				break;
			}
		}
		//printf("x %d\r\n",x);
		if (!isFind || x >= src->width){
			break;
		}				
	}
	return x;
}
void splitIplImage(IplImage * dst,const IplImage * src ,int &x,bool &isFind,int src_x){
	//if (src->height < dst->height) return;
	//int beginHeight = ( src->height - dst->height)/2;	

	int srcStep       = src->widthStep;
	uchar * srcData    =  (uchar *) src->imageData;

	int dstStep       = dst->widthStep;
	uchar * dstData    =  (uchar *) dst->imageData;


	//CvRect r;// = cvRect();
	int i,j,j1;
	int src_x_2=src_x/2;
	int src_x_1 = src_x%2 +src_x_2;
	//r.width = (src->width-src_x_1);
	int endx = x +  src->width - src_x_1;
	if (endx > dst->width) endx = dst->width;

	int isV=0;
	int _i=0;
	for(i=0,_i=0; i < dst->height; i++){
		for(j=x,j1=src_x_2; j <  endx; j++,j1++){
		
			if (dstData[i*dstStep+j]>0){
				isV++;
				//printf("src i %d j %d\r\n",i+beginHeight,j1);
				if (srcData[_i*srcStep + (j1+src_x_2)] == 0 ){
					src_x++;
					if (src_x < src->width/3 ) {
						splitIplImage(dst,src,x,isFind,src_x);
					}
					return;
				}
			}
		}
		if (isV > 0){
			_i++;
			if (_i >= src->height)break;
		}
	}
	if (isV==0){
		isFind = false;
		return;
	}
	x=endx;
	isFind = true;
	return;

}