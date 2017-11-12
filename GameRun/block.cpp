#include "stdafx.h"
#include <fstream>
#include "block.h"
#include <io.h> 
//#include <direct.h>  
#include <sys/stat.h>  


Block::Block(CvRect r,Block * _Par){
	rect = r;
	this->Par = _Par;
	this->ID = -1;
	this->NL = NULL;
	if (_Par != NULL){
		int Sep = _Par->Sep;
		int Coll = _Par->Coll;
		
		this->tag = _Par->tag;
		this->ID = this->Par->Child.size();
		this->Par->Child.push_back(this);
	}
	
}
Block::Block(char *RectData,Block *_Par,int _id,char * GName){
	this->ID = _id;
	this->tag = GName;
	this->Par = _Par;
	this->NL = NULL;
	CvPoint po = {0,0};
	if (_Par != NULL){
		po.x = _Par->rect.x;
		po.y = _Par->rect.y;
		this->Par->Child.push_back(this);
	}
	//printf("%s %s \r\n",RectData,GName);
	char *p ;
	const char *d = " ";
	p = strtok(RectData,d);
	int i=0;
	int re[4];
	while(p){
		if (i<4){
			re[i]=atoi (p);
		}else if (i == 4){
			//printf("%d %d %d %d\r\n",re[0],po.x,re[1], po.y);
			rect = cvRect(re[0]+po.x,re[1] + po.y,re[2],re[3]);
		//	ID = atoi (p);			
		//}else if (i == 5){
		//	IsArr = atoi(p) ;
		//}else if (i == 5){
			this->Coll = atoi(p);
		}else if (i == 5) {
			LoadToTempleList(p);			
		}
		p=strtok(NULL,d);
		i++;  
	}
	char defPath[1024]={0};
	GetTemplePath(defPath);
	//printf("%s\r\n",defPath);
	this->GetChildBlock();
}
Block::~Block(){
	for (vector<Block *>::iterator it = Child.begin(); it != Child.end(); it ++) {
		delete (*it);
	}
	Child.clear();

	for (vector<TempleImg *>::iterator it = TempleList.begin(); it != TempleList.end(); it ++) {
		delete (*it);
	}
	TempleList.clear();
	ClearCoord();
	delete NL;
}
void Block::ClearCoord(){
	//for (vector<Coordinate>::iterator it = Coord.begin(); it != Coord.end(); it ++) {
	//	delete (*it);
	//}
	Coord.clear();
}
void Block::GetChildBlock(){
	char PathID[1024]={0};
	this->GetChildFileName(PathID);
	fstream In(PathID, ios::in|ios::binary);	
	if (In.is_open())   
    {  	
		printf("open ok %s\r\n",PathID);
		char tmpStr[1024];
		int UserArrLen = 0;
		while (!In.eof() ){
			In.getline(tmpStr ,1024,'\n');
			if (strlen(tmpStr)==0){
				break;
			}
			Block *bl = new Block(tmpStr,this,UserArrLen,this->tag);
			//Child.push_back(bl);
			UserArrLen++;
		}
        In.close();	
	}
}
void Block::GetTemplePath(char *TemplePath){
	//printf("%d %s\r\n",this->ID,TemplePath);
	char * newPath = new char[1024];
	if (this->Par==NULL){
		if (this->ID>=0){
			//tmpPaht
			int len = strlen(TemplePath);
			if (len == 0 ) sprintf(newPath,"%s\\%d\0",this->tag  ,this->ID);			
			else sprintf(newPath,"%s\\%d\\%s\0",this->tag ,this->ID,TemplePath);
		}else{
			int len = strlen(TemplePath);
			if (len > 0 ) sprintf(newPath,"%s\0",this->tag  );			
			else sprintf(newPath,"%s\\%s\0",this->tag  ,TemplePath);
		}
	}else{
		if (this->ID>=0){
			int len = strlen(TemplePath);
			if (len == 0 )sprintf(newPath,"%d\0",this->ID);
			else sprintf(newPath,"%d\\%s\0",this->ID,TemplePath);
		}
		this->Par->GetTemplePath(newPath);
	}
	strcpy(TemplePath,newPath);
	delete [] newPath;
}
void Block::GetChildFileName(char *PathID){
	char * newPath = new char[1024];
	if (this->Par==NULL){
		int len = strlen(PathID);
		//printf("%d %d\r\n",this->ID,len);
		if (len == 0 ) sprintf(newPath,"%s_%d.log\0",this->tag  ,this->ID);			
		else sprintf(newPath,"%s_%d_%s.log\0",this->tag  ,this->ID,PathID);
	}else{
		int len = strlen(PathID);
		if (len == 0 )sprintf(newPath,"%d\0",this->ID);
		else sprintf(newPath,"%d_%s\0",this->ID,PathID);
		this->Par->GetChildFileName(newPath);
	}
	strcpy(PathID,newPath);
	delete [] newPath;
}
void Block::LoadToTempleList(char * Filepath ){
	struct _stat st;
	int result;
	char defPath[1024]={0};
    result = _stat(Filepath, &st);	
	if (result!=0){		
		GetTemplePath(defPath);
		result = _stat(defPath, &st);
		if (result==0)this->NL = new NumberList(defPath,NULL,atoi(Filepath));		
	}else if(!(st.st_mode & _S_IFDIR)){
		IplImage * img = cvLoadImage(Filepath,CV_LOAD_IMAGE_UNCHANGED);
		
		if (img != NULL)TempleList.push_back(new TempleImg(img,this->ID>=0?this->ID:0));
		GetTemplePath(defPath);
		result = _stat(defPath, &st);
		if (result==0)LoadTempleForDir(defPath);
	}else{	
		LoadTempleForDir(Filepath);		
		GetTemplePath(defPath);
		result = strcmp(defPath,Filepath);
		if (result!=0) {
			result = _stat(defPath, &st);
			if (result==0)LoadTempleForDir(defPath);
		}
	}

}
void Block::LoadTempleForDir(char * Filepath){
	char searchPath[1024]={0};
	char path[1024]={0};
	char Num[10]={0};
	char * pch;
	int nlen;
	sprintf(searchPath,"%s\\*.bmp",Filepath);
	struct _finddata_t fd;
	intptr_t handle = _findfirst(searchPath, &fd);
	if (handle == -1) {
		return;
	}
	do {
		sprintf(path,"%s\\%s",Filepath,fd.name);
		IplImage * img = cvLoadImage(path,CV_LOAD_IMAGE_ANYDEPTH);
		if (img==NULL)continue;
		pch = strchr(fd.name,'_');
		if (pch == NULL){
			pch = strchr(fd.name,'.');
			if (pch == NULL) nlen = strlen(fd.name);
			nlen = pch-fd.name;
		}else	nlen = pch-fd.name;
		memcpy(Num,fd.name,nlen);
		Num[nlen] = 0;
		//TempleImg *ni = new TempleImg(img,atoi(Num));
		TempleList.push_back(new TempleImg(img,atoi(Num)));

	} while (_findnext(handle, &fd)==0);
	_findclose(handle);
}
vector <TempleImg*>  Block::GetTempleList(){
	if (this->TempleList.empty() && this->Par != NULL)
		return this->Par->GetTempleList();	
	return this->TempleList;
}
NumberList * Block::GetNumblerListClass(){
	if (this->NL == NULL){
		if (this->Par != NULL){
			return this->Par->GetNumblerListClass();
		}
		return NULL;
	}
	return this->NL;
}
int Block::FindNL(IplImage * src ){	
	cvSetImageROI(src, this->rect);
	IplImage * dst=cvCreateImage(cvSize(rect.width,rect.height),src->depth,src->nChannels);
	cvCopy(src,dst);
	cvResetImageROI(src);	
	int n = 0;
	NumberList * _NL = GetNumblerListClass();
	if (_NL != NULL)n = _NL->Know(dst);		
	cvReleaseImage(&dst);
	return n;	
}
void Block::ShowRectImg(IplImage * src){
	cvSetImageROI(src, this->rect);
	IplImage * dst=cvCreateImage(cvSize(rect.width,rect.height),src->depth,src->nChannels);
	cvCopy(src,dst);
	cvResetImageROI(src);
	cvNamedWindow("contour1");
	cvShowImage("contour1", dst);  
	cvWaitKey(0);
	cvDestroyWindow("contour1");
	cvReleaseImage(&dst);
}
Block * Block::GetChildOne(IplImage * src,int id){
	if (id >= int(this->Child.size())){
		int n = this->FindArr(src,true);
		if (id>=n ) return NULL;
	}
	return this->Child[id];
}
int Block::FindOne(IplImage * src){
	CvPoint loc;	
	int n = -1;	
	 
	for (vector<Coordinate>::iterator it = Coord.begin(); it != Coord.end(); it ++){
		if (inSameOne(src,(*it).rect, (*it).te->img,&loc)){
			(*it).order = Coord.front().order+1;
			n = (*it).te->Tag;
			sort(Coord.begin(),Coord.end(),compCoordinateOrder);
			//printf("0 out\r\n");
			return n;
		}
	}
		
 
	vector <TempleImg*> _TempleList = GetTempleList();

	if (!_TempleList.empty()){
		if ( Coord.size() < _TempleList.size() ){
			for (vector<TempleImg*>::iterator it = _TempleList.begin(); it != _TempleList.end(); it ++) {
				if (inSameOne(src,rect, (*it)->img,&loc)){
					Coordinate coo((*it) , cvRect(loc.x,loc.y,(*it)->img->width,(*it)->img->height));	
					//coo.rect = cvRect(loc.x,loc.y,(*it)->img->width,(*it)->img->height);
					//coo.te = (*it);				
					this->Coord.push_back(coo);
					coo.order = Coord.front().order+1;
					sort(Coord.begin(),Coord.end(),compCoordinateOrder);
					n = (*it)->Tag;
					//printf("1 out\r\n");
					return n;
				}
			}
		}
		double max = 0,val;
		CvPoint  l;
		
		TempleImg* Temp =NULL;
		cvSetImageROI(src, rect);
		IplImage * dst=cvCreateImage(cvSize(rect.width,rect.height),src->depth,src->nChannels);
		cvCopy(src,dst);
		cvResetImageROI(src); 
		for (vector<TempleImg*>::iterator it = _TempleList.begin(); it != _TempleList.end(); it ++) {
			val = inSameOneT(dst, (*it)->img,&l);
			if (val < 0.8) {
				continue;
			}
			if (val>max){
				Temp = (*it);
				max = val;
				loc.x = rect.x+l.x;
				loc.y = rect.y+l.y;					 
			}
		}
		cvReleaseImage(&dst);
		if (Temp != NULL){
			n = Temp->Tag;
			CvRect r= cvRect(loc.x, loc.y,Temp->img->width,Temp->img->height);
			Coordinate coo(new TempleImg(src, r,n ),r);
			//coo.rect = cvRect( loc.x, loc.y,Temp->img->width,Temp->img->height);

			//coo.te = new TempleImg(src, coo.rect,n );
			char defPath[1024]={0};
			GetTemplePath(defPath);
			coo.te->SaveTemple(defPath);
			_TempleList.push_back(coo.te);
			this->Coord.push_back(coo);
			coo.order = Coord.front().order+1;
			sort(Coord.begin(),Coord.end(),compCoordinateOrder);
			//printf("2 out\r\n");
			return n;
		}

	}	
	if (!this->Child.empty()){
		for (vector<Block*>::iterator it = Child.begin(); it != Child.end(); it ++) {
			n = (*it)->FindOne(src);
			//(*it)->ShowRectImg(src);
			if (n!= -1) return n;
		}
	}

	return n;
}

int Block::FindOneS(IplImage * src){
	CvPoint loc;
	//cvSetImageROI(src, rect);
	//IplImage * dst=cvCreateImage(cvSize(rect.width,rect.height),src->depth,src->nChannels);
	//cvCopy(src,dst);
	//cvResetImageROI(src); 
	this->ClearCoord();
	int n = -1;
	vector <TempleImg*> _TempleList = GetTempleList();
	if (!_TempleList.empty()){
		//cvSetImageROI(src, rect);
		for (vector<TempleImg*>::iterator it = _TempleList.begin(); it != _TempleList.end(); it ++) {
			if (inSameOne(src,rect, (*it)->img,&loc)){
				//(*it)->order++;
				Coordinate coo((*it),cvRect(loc.x,loc.y,(*it)->img->width,(*it)->img->height));	
				this->Coord.push_back(coo);
				n = (*it)->Tag;
				//sort(_TempleList.begin(),_TempleList.end(),compTempleImgOrder);	
				//printf("order %d %d\r\n",_TempleList.front()->order,_TempleList.size());
				//printf("loc %d %d\r\n",loc.x,loc.y);
				if (loc.x !=this->rect.x || loc.y != this->rect.y){
					this->rect.height = coo.rect.height;
					this->rect.width = coo.rect.width;
					this->rect.x = coo.rect.x;
					this->rect.y = coo.rect.y;
				}
				break;
			}
		}
		//cvResetImageROI(src); 
		
		if (n == -1 && _TempleList.front()->order == 0){
			double max = 0,val;
			CvPoint  l;
			
			//coo.rect = rect;	
			TempleImg* Temp =NULL;

			//vector <TempleImg*> _TempleList = GetTempleList();
			cvSetImageROI(src, rect);
			IplImage * dst=cvCreateImage(cvSize(rect.width,rect.height),src->depth,src->nChannels);
			cvCopy(src,dst);
			cvResetImageROI(src); 
			for (vector<TempleImg*>::iterator it = _TempleList.begin(); it != _TempleList.end(); it ++) {
				val = inSameOneT(dst, (*it)->img,&l);
				if (val < 0.8) {
					continue;
				}
				if (val>max){
					Temp = (*it);
					max = val;
					loc.x = l.x;
					loc.y = l.y;					 
				}
			}
			cvReleaseImage(&dst);
			if (Temp != NULL){
				n = Temp->Tag;
				Coordinate coo(Temp,cvRect(rect.x+loc.x,rect.y+loc.y,Temp->img->width,Temp->img->height));
				//coo.te = Temp;
				//coo.rect = cvRect(rect.x+loc.x,rect.y+loc.y,Temp->img->width,Temp->img->height);
				//coo.rect = cvRect(loc.x,loc.y,(*it)->img->width,(*it)->img->height);
				if (loc.x !=0 || loc.y != 0){
					this->rect.height = coo.rect.height;
					this->rect.width = coo.rect.width;
					this->rect.x = coo.rect.x;
					this->rect.y = coo.rect.y;
				}
				
				cvSetImageROI(src, rect);
				IplImage * temple=cvCreateImage(cvSize(Temp->img->width,Temp->img->height),src->depth,src->nChannels);
				cvCopy(src,temple);
				cvResetImageROI(src); 
				TempleImg* newTemple = new TempleImg(temple,n,1);
				char defPath[1024]={0};
				GetTemplePath(defPath);
				newTemple->SaveTemple(defPath);

				_TempleList.push_back(newTemple);

				this->Coord.push_back(coo);
				//Temp->order++;
				sort(_TempleList.begin(),_TempleList.end(),compTempleImgOrder);	
			}
		}
		//printf("order %d %d\r\n",_TempleList.front()->order,_TempleList.size());
	}else{
		if (!this->Child.empty()){
			for (vector<Block*>::iterator it = Child.begin(); it != Child.end(); it ++) {
				n = (*it)->FindOne(src);
				//(*it)->ShowRectImg(src);
				if (n!= -1) return n;
			}
		}
	}
	//cvResetImageROI(src); 
	//cvReleaseImage(&dst);
	return n;

}
int Block::FindArr(IplImage * src,bool isUpdate){
	this->ClearCoord();
	int count=0;
	int _n = -1;
	if (!this->Child.empty() && !isUpdate){		
		for (vector<Block *>::iterator it = Child.begin(); it != Child.end(); it ++) {
			_n = (*it)->FindOne(src);
			if (_n != -1){
				this->Coord.push_back((*it)->Coord.back());
				count ++ ;
			}
		}
		return count;
	}else{		
		//IplImage * dst = cvCreateImage(cvSize(src->width,src->height),src->depth,src->nChannels);
		//cvCopy(src,dst);
		while(true){
			Block * bl = new Block(this->rect,this);
			_n = bl->FindOne(src);
			if (_n == -1){
				this->Child.pop_back();
				delete bl;
				break;
			}
			bl->Fill(src);
			bl->TempleList.push_back(bl->Coord.back().te);
			this->Coord.push_back(bl->Coord.front());
			count++;
		}
		sort(this->Child.begin(),this->Child.end(),compBlockX);
		sort(this->Child.begin(),this->Child.end(),compBlockY);
		//cvReleaseImage(&dst);
	}
	return count;
}
void Block::Fill(IplImage * src){
	rect = this->Coord.front().rect;
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
bool Block::ClickCoordinate(int _v,int num){
	if (_v == -1){
		Coordinate coor(NULL,this->rect);
		return coor.MouseClick(_v,num);
	}		
	for (vector<Coordinate>::iterator it = Coord.begin(); it != Coord.end(); it ++) {
		if ((*it).MouseClick(_v,num) ){
			Coord.erase(it);
			return true;		
		}
	}
	return false;
}

bool compBlockX(Block *a,Block *b){
	return a->rect.x < b->rect.x;
}
bool compBlockY(Block *a,Block *b){
	return a->rect.y < b->rect.y;
}
bool compImg(IplImage * a,IplImage * b,CvRect rect){
	int height     =rect.height;
	int width      =rect.width;
	if ( width != b->width) return false;
	if ( height != b->height) return false;
	int step       = a->widthStep;
	uchar * data    =  (uchar *) a->imageData;
	int channels   = a->nChannels;
	int nstep       = b->widthStep;
	uchar * ndata    = (uchar *) b->imageData;
	int nchannels   = b->nChannels;
	int i,j;	
	for(i=0; i< height  ; i+=nchannels){
		for(j=0; j< width ; j+=nchannels){
			if (data[(i+rect.y) *step+(j+rect.x)*channels+0] != ndata[i*nstep+j*nchannels+0]){
				return false;
			}
		}
	}
	return true;
}
bool inSameOne(IplImage *img,CvRect rect ,IplImage * num,CvPoint *loc){

	int h = rect.height - num->height;
	int w = rect.width - num->width;
	if (h == 0 && w == 0) {
		return compImg(img,num,rect);
	}
	if(h<0 || w < 0){
		return false;
	}		
	//printf("%d %d\r\n",h,w);
	int height     =rect.y+ rect.height;
	int width      =rect.x+ rect.width;
	int step       = img->widthStep;
	uchar * data    =  (uchar *) img->imageData;
	int channels   = img->nChannels;

	int nheight     = num->height;
	int nwidth      = num->width;
	int nstep       = num->widthStep;
	uchar * ndata    = (uchar *) num->imageData;
	int nchannels   = num->nChannels;
	int i,j,ii,jj;
	bool isS = false;
	//int sum = 0,max=0;
	for(j=rect.x; j< (width-nwidth ); j+=1){
		for(i=rect.y; i< (height-nheight) ; i+=1){
			isS = true;
			//sum = 0;
			for(jj=0; jj<nwidth; jj+=channels){
				for(ii=0; ii<nheight ; ii+=channels){
					if (data[(i+ii)*step+(j+jj)*channels ] != ndata[ii*nstep+jj*nchannels ]){
						isS = false;
						break;
					//}else{
					//	sum+=1;
					}
				}
				if (!isS){
					//if (sum>max)max=sum;
					break;
				}
			}
			if (isS){
				loc->x = j;
				loc->y = i;
				return true;
			}

		}
	}
	return false;
	
}

double inSameOneT(IplImage *img ,IplImage * num,CvPoint *loc){

	int w = img->width - num->width + 1;
	
	int h = img->height - num->height + 1;

	 
	if (w<1 && h<1) {
		return inSameOneT(num,img,loc);
	}else if (w<1 || h<1)return 0;
	 
	//if (w<1 || h<1)return 0;

	IplImage* imgResult = cvCreateImage(cvSize(w,h),IPL_DEPTH_32F,1); 
	//printf("v2>%d %d\r\n",img->depth,num->depth);
	cvMatchTemplate(img,num,imgResult,CV_TM_CCOEFF_NORMED); 	 
    double max_val,min_val; 
	CvPoint minLoc;

	cvMinMaxLoc(imgResult, &min_val, &max_val, &minLoc, loc, NULL);  
	//if (max_val>0.8)printf("%f\r\n",max_val);
	cvReleaseImage(&imgResult);
	//printf("t %f\r\n",max_val);
	return max_val;

}