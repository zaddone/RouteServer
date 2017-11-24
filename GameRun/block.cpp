#include "stdafx.h"
#include <fstream>
#include "block.h"
#include <io.h> 
//#include <direct.h>  
#include <sys/stat.h>  

TrainTemple::TrainTemple(char * _path,char * _toPath){
	this->img = cvLoadImage(_path,CV_LOAD_IMAGE_ANYDEPTH);
	//this->img = _img;
	//if (_path != NULL){
	//Path = new char[strlen(_path)];
	strcpy(this->Path , _path);
	if (_toPath != NULL){
		//this->toPath =  new char[strlen(_toPath)];
		strcpy(this->toPath , _toPath);
	}else{
		toPath[0] = 0;
	}
}
TrainTemple::~TrainTemple(){
	//printf("run delete toPath\r\n");
	//if (this->toPath != NULL) delete [] this->toPath;
	//printf("delete img\r\n");
	if (img != NULL) {
		//printf("delete img 0\r\n");
		cvReleaseImage(&img);
		
	}
	//printf("delete path %s\r\n",Path);
	//if (this->Path != NULL) delete [] this->Path;
	//printf("run delete over \r\n");
}
void TrainTemple::Remove(){
	remove(this->Path);
}
void TrainTemple::MoveTo(char * _path,char * name){
	SaveTempleName(_path,name,img);
}
bool TrainTemple::IsSame(TrainTemple * t){
	CvPoint loc;
	return inSameOne(this->img,cvRect(0,0,img->width,img->height),t->img,&loc);
}
bool TrainTemple::Show(){
	
	printf("%s\r\n",this->Path);
	cvNamedWindow("contour1");
	cvShowImage("contour1", img);  
	char ValStr;
	char newFile[5]={0};
	int len = 0;
	do {
		ValStr=cvWaitKey(0);
		if (ValStr == 27){
			len= 0;
			break;
		}else if (ValStr>47 && ValStr<58){
			newFile[len] = ValStr;
			len++;
		}else{
			break;
		}
	}while(true);
	if (len>0){
		//char fileName[1024];
		//sprintf(fileName,"%s\\%s.bmp",strlen(this->toPath)== 0 ? path:toPath,newFile);
		this->MoveTo(strlen(this->toPath)== 0 ? Path:toPath,newFile);
	}

	cvDestroyWindow("contour1");
	if (ValStr == 27){
		return false;
	}
	
	return true;
}

Block::Block(CvRect r,Block * _Par){
	rect = r;
	this->Par = _Par;
	this->ID = -1;
	this->NL = NULL;
	this->MaxVal = 0.8;
	if (_Par != NULL){
		//int Sep = _Par->Sep;
		this->Coll = _Par->Coll;
		
		this->tag = _Par->tag;
		this->ID = this->Par->Child.size();
		//this->Par->Child.push_back(this);
		this->MaxVal  =_Par->MaxVal;
	}
	Nothing = NULL;
	CollPath = NULL;
}
Block::Block(char *RectData,Block *_Par,int _id,char * GName){
	this->ID = _id;
	this->tag = GName;
	this->Par = _Par;
	this->NL = NULL;
	CollPath = NULL;
	Nothing = NULL;
	MaxVal = 0.8;
	CvPoint po = {0,0};
	if (_Par != NULL){
		po.x = _Par->rect.x;
		po.y = _Par->rect.y;
		//this->Par->Child.push_back(this);
		MaxVal = _Par->MaxVal;
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
		}else if (i == 6) {
			MaxVal = atoi(p);
			while(MaxVal>=1){
				MaxVal *= 0.1;
			}
			if (MaxVal == 0)MaxVal = 0.8;
		}
		p=strtok(NULL,d);
		i++;  
	}
	char defPath[1024]={0};
	GetTemplePath(defPath);
	//printf("%f %d\r\n",MaxVal,Sep);
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
	if (CollPath != NULL) delete [] CollPath;
	for (vector<TrainTemple *>::iterator it = TrainList.begin(); it != TrainList.end(); it ++) {
		if ((*it)!= NULL )delete (*it);
	}
	
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
			Child.push_back(bl);
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
		//result = _stat(defPath, &st);
		//if (result==0)
		int sep = atoi(Filepath);
		if (sep >0 && sep < 255)
			this->NL = new NumberList(defPath,this->Coll,sep);		
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

void Block::ErgodicBlock(CallBackTask _f ){
	_f(this);
	for (vector<Block *>::iterator it = Child.begin(); it != Child.end(); it ++) {
		(*it)->ErgodicBlock(_f);
	}
}
void trainNL(LPVOID lpParamter){
	Block * bl = (Block *)lpParamter;
	if (bl->NL == NULL) {
		
	}else{
		bl->NL->ReadDirImg(NULL);
		bl->NL->ReadListImg();
	}
}
void Block::AddTrainTemple(char *path){
	TrainTemple *t = new TrainTemple(path);
	if (t->img == NULL){
		//printf("delete 0 %s\r\n",t->Path);
		delete t;
		return;
	}
	for (vector<TrainTemple*>::iterator it = this->TrainList.begin(); it != this->TrainList.end(); it ++) {
		if ((*it) == NULL) continue;
		if ((*it)->IsSame(t)){	
			//printf("delete 1 %s\r\n",t->Path);
			t->Remove();
			delete t;
			t = NULL;
			break;
		}else  if (t->IsSame((*it))){				
			//printf("delete 2 %s\r\n",(*it)->Path);
			(*it)->Remove();
			delete (*it);
			(*it) =NULL;
			TrainList.erase(it);
			//t = NULL;
			break;
		}
	}
	if (t != NULL){
		this->TrainList.push_back(t);
	}
}
void train(LPVOID lpParamter){
	Block * bl = (Block *)lpParamter;
	if (bl->NL != NULL) {
		bl->NL->ReadDirImg(NULL);
		bl->NL->ReadListImg();
		return;
	}
	if (bl->Coll != 1) return;
	if (bl->CollPath == NULL){
		char defPath[1024]={0};
		bl->GetTemplePath(defPath);
		bl->CollPath = new char [1024];
		sprintf(bl->CollPath,"coll\\%s\0",defPath);
	}
	printf("%s\r\n",bl->CollPath);
	bl->ReadDirImg(bl->CollPath);
	
	for (vector<TrainTemple*>::iterator it = bl->TrainList.begin(); it != bl->TrainList.end(); it ++) {
		//if (!(*it)->Show()){
		delete (*it);
		//}
	}
	bl->TrainList.clear();
}
void Block::ReadDirImg(const char * path ){
	if (path== NULL) return;	
	struct _stat st;
	if (0 != _stat(path, &st)) return;

	struct _finddata_t fd;
	char searchPath[1024];
	char pathFile[1024];
	sprintf(searchPath,"%s\\*",path);
	size_t nlen =0;
	intptr_t handle = _findfirst(searchPath, &fd);
	if (handle != -1) {
		do   {
			if (fd.attrib & _A_SUBDIR)    
			{
				if( (strcmp(fd.name,".") != 0 ) &&(strcmp(fd.name,"..") != 0))   
				{	
					sprintf(pathFile,"%s\\%s",path,fd.name);
					this->ReadDirImg(pathFile );
				}
			}else{
				sprintf(pathFile,"%s\\%s",path,fd.name);	
				this->AddTrainTemple(pathFile);
			}

		}while (_findnext(handle, &fd)==0);
		_findclose(handle);
	}
}
int Block::FindNL(IplImage * src ){	
	
	if (this->Nothing != NULL){
		//CvPoint loc;
		if (compImg(src,Nothing->img,rect)){
			return this->Nothing->Tag;
		}else{
			delete Nothing;
			Nothing = NULL;
		}
	}
	Nothing = new TempleImg(src, rect,0 );
	NumberList * _NL = GetNumblerListClass();
	if (_NL != NULL)Nothing->Tag = _NL->Know(Nothing->img);
	return Nothing->Tag;

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
	int n = -1;	
	if (!this->Child.empty()){
		for (vector<Block*>::iterator it = Child.begin(); it != Child.end(); it ++) {
			n = (*it)->FindOne(src);
			//this->Coord.push_back((*it)->Coord.back());
			//(*it)->ShowRectImg(src);
			if (n != -1) return n;
		}
		return n;
	}
	CvPoint loc;
	if (this->Nothing!= NULL){
		if (inSameOne(src,rect, Nothing->img,&loc)){
			return Nothing->Tag;
		}else{
			delete Nothing;
			Nothing = NULL;
		}
	}	
	
	for (vector<Coordinate>::reverse_iterator it = Coord.rbegin(); it != Coord.rend(); ++ it ){	
	//for (vector<Coordinate>::iterator it = Coord.begin(); it != Coord.end(); it ++){
		if (compImg(src,(*it).te->img,(*it).rect )){
			//Coordinate c = (*it);
			n = (*it).te->Tag;
			//Coord.cbegin()
			swap((*it),Coord.back());
			//Coord.push_back(c);
			return  n;
		}
	}	

	vector <TempleImg*> _TempleList = GetTempleList();
	if (_TempleList.empty())return n;
	/**
	for (vector<TempleImg*>::iterator it = _TempleList.begin(); it != _TempleList.end(); it ++) {
		if (inSameOne(src,this->rect, (*it)->img,&loc)){
			Coordinate coo((*it),cvRect(loc.x, loc.y,(*it)->img->width,(*it)->img->height));
			this->Coord.push_back(coo);
			return (*it)->Tag;			
		}
	}
	**/
	
	double max = 0,val;
	CvPoint  l;
	
	TempleImg* Temp =NULL;
	cvSetImageROI(src, rect);
	IplImage * dst=cvCreateImage(cvSize(rect.width,rect.height),src->depth,src->nChannels);
	cvCopy(src,dst);
	cvResetImageROI(src); 
	for (vector<TempleImg*>::iterator it = _TempleList.begin(); it != _TempleList.end(); it ++) {
		/**
		if (inSameOne(src,this->rect, (*it)->img,&loc)){
			Coordinate coo((*it),cvRect(loc.x, loc.y,(*it)->img->width,(*it)->img->height));
			this->Coord.push_back(coo);
			return (*it)->Tag;			
		}
		**/
		val = inSameOneT(dst, (*it)->img,&l);
		if (val < MaxVal) {
			continue;
		}
		if (val>max){
			Temp = (*it);
			max = val;
			//Temp->order = val;
			loc.x = rect.x+l.x;
			loc.y = rect.y+l.y;					 
		}
	}
	cvReleaseImage(&dst);
	if (Temp != NULL){
		n = Temp->Tag;
		CvRect r= cvRect(loc.x, loc.y,Temp->img->width,Temp->img->height);
		Coordinate coo(new TempleImg(src, r,n ),r);
		if (this->Coll==1){
			if (CollPath == NULL){
				char defPath[1024]={0};
				GetTemplePath(defPath);
				CollPath = new char [1024];
				sprintf(CollPath,"coll\\%s\0",defPath);
			}
			coo.te->SaveTemple(CollPath);
		}
		_TempleList.push_back(coo.te);
		//_TempleList.push_back(coo.te);
		this->Coord.push_back(coo);
		return n;
	}
	if (n == -1){
		if (Nothing != NULL) delete Nothing;
		Nothing = new TempleImg(src, rect,n );
	}
	return n;
}
bool Block::ClickFindOne(IplImage * src,int tag){
	/**
	//Child.rbegin();
	vector<int>::reverse_iterator r_iter;  
for (r_iter = vec.rbegin(); // binds r_iter to last element  
      r_iter != vec.rend(); // rend refers 1 before 1st element  
      ++r_iter) // decrements iterator one element  
    cout << *r_iter << endl; // prints 9,8,7,...0  
	**/

	//for (vector<Coordinate>::iterator it = Coord.end(); it !=  Coord.begin(); it --){
	//printf("ClickFindOne begin %d\r\n",tag);
	for (vector<Coordinate>::reverse_iterator it = Coord.rbegin(); it != Coord.rend(); ++ it ){
		
		if ( (*it).te->Tag != tag ) continue;
		//printf("Click %d %d\r\n",(*it).te->Tag ,tag);
		if (compImg(src, (*it).te->img,(*it).rect)){
			//if (!Coord.empty())(*it).order = Coord.back().order+1;
			Coordinate c = (*it);
			if (Coord.size()>1){

				//swap((*it),Coord.back());
				 
				//Coord.push_back(c);				
				(*it).order = Coord.back().order+1;
				sort(Coord.begin(),Coord.end(),compCoordinateOrder);
			}
			c.MouseClick(1);
			//printf("click 0\r\n");
			return true;
		}
	}

	/**
	if (!this->Child.empty()){
		//this->ClearCoord();
		for (vector<Block*>::iterator it = Child.begin(); it != Child.end(); it ++) {

			if ((*it)->ClickFindOne(src,tag)){
				//printf("click 1\r\n");
				//this->Coord.push_back((*it)->Coord.back());
				return true;
			}
		}
		//return n;
	}
	//Coordinate coo;
	**/
	vector <TempleImg*> _TempleList = GetTempleList();
	if (_TempleList.empty())return NULL;
	TempleImg* Temp =NULL;

	//vector <TempleImg*> _TempleTmp;
	for (vector<TempleImg*>::iterator it = _TempleList.begin(); it != _TempleList.end(); it ++) {
		if ((*it)->Tag == tag){
			Temp=(*it);
			break;
		}
	}
	if (Temp == NULL) return false;


	CvPoint loc;
	cvSetImageROI(src, rect);
	IplImage * dst=cvCreateImage(cvSize(rect.width,rect.height),src->depth,src->nChannels);
	cvCopy(src,dst);
	cvResetImageROI(src); 

	double val = inSameOneT(dst, Temp->img,&loc);
	cvReleaseImage(&dst);
	if (val > MaxVal) {
		loc.x += rect.x;
		loc.y += rect.y;
		Coordinate coo(Temp,cvRect(loc.x,loc.y,Temp->img->width,Temp->img->height));
		//if (!this->Coord.empty())coo.order = Coord.front().order+1;
		this->Coord.push_back(coo);
		//printf("click 2\r\n");
		coo.MouseClick(1);
		//if (Coord.size()>1)sort(Coord.begin(),Coord.end(),compCoordinateOrder);
		return true;
	}
	return false;
}
bool Block::MergeWithCoordinate(Coordinate coor){
	int MaxX1 = this->rect.x + this->rect.width;
	int MaxY1 = this->rect.y + this->rect.height;
	int MaxX2 = coor.rect.x + coor.rect.width;
	int MaxY2 = coor.rect.y + coor.rect.height;

	if (coor.rect.x > MaxX1 ||  this->rect.x > MaxX2 || coor.rect.y > MaxY1 ||  this->rect.y > MaxY2 ) return false;
	//return true;
	
	this->rect.x = this->rect.x < coor.rect.x ? this->rect.x : coor.rect.x;
	this->rect.y = this->rect.y < coor.rect.y ? this->rect.y : coor.rect.y;
	this->rect.width = MaxX1 > MaxX2 ? MaxX1 - this->rect.x  : MaxX2 - this->rect.x;
	this->rect.height =  MaxY1 > MaxY2 ?  MaxY1 - this->rect.y  : MaxY2 - this->rect.y;
	this->Coord.push_back(coor);

	vector <TempleImg*> _TempleList = GetTempleList();
	//_TempleList.insert(_TempleList.end(),bl->TempleList.begin(),bl->TempleList.end());
	//if (!_TempleList.empty()){
	_TempleList.push_back(coor.te);
	//}
	return true;
}
bool Block::Merge(Block *bl){
	//bool check = false;
	int MaxX1 = this->rect.x + this->rect.width;
	int MaxY1 = this->rect.y + this->rect.height;
	int MaxX2 = bl->rect.x + bl->rect.width;
	int MaxY2 = bl->rect.y + bl->rect.height;

	if (bl->rect.x > MaxX1 ||  this->rect.x > MaxX2 || bl->rect.y > MaxY1 ||  this->rect.y > MaxY2 ) return false;
	
	this->rect.x = this->rect.x < bl->rect.x ? this->rect.x : bl->rect.x;
	this->rect.y = this->rect.y < bl->rect.y ? this->rect.y : bl->rect.y;
	this->rect.width = MaxX1 > MaxX2 ? MaxX1 - this->rect.x  : MaxX2 - this->rect.x;
	this->rect.height =  MaxY1 > MaxY2 ?  MaxY1 - this->rect.y  : MaxY2 - this->rect.y;

	
	if (!bl->TempleList.empty()){
		vector <TempleImg*> _TempleList = GetTempleList();
		_TempleList.insert(_TempleList.end(),bl->TempleList.begin(),bl->TempleList.end());
		bl->TempleList.clear();
	}
	if (!bl->Coord.empty()){
		this->Coord.insert(this->Coord.end(),bl->Coord.begin(),bl->Coord.end());
		bl->Coord.clear();
	}
	return true;

}

char* Block::FindArr(IplImage * src,int & size){
	this->ClearCoord();
	int n = -1,count=0;
	int chileLen = Child.size();
	if (size>chileLen)chileLen = size;
	char* val = new char[ chileLen];

	for (vector<Block *>::reverse_iterator it = Child.rbegin(); it != Child.rend(); ++ it ){
	//for (vector<Block *>::iterator it = Child.begin(); it != Child.end(); it ++) {
		n = (*it)->FindOne(src);
		if (n != -1){
			this->Coord.push_back((*it)->Coord.back());
			val[count] = n;
			count ++ ;
			if (count == size){
				return val;
			}
			(*it)->Fill(src);
		}
	}
	int len = size - count;	
	if (len>0){
		CvPoint loc;		
		IplImage * dst=cvCreateImage(cvSize(rect.width,rect.height),src->depth,src->nChannels);
		//cvCopy(src,dst);
		double k;
		vector <Block *> TmpBl;	
		//TmpBl.reserve(size);
		bool isAdd;

		vector <TempleImg*> _TempleList = GetTempleList();
		for (vector<TempleImg*>::iterator it = _TempleList.begin(); it != _TempleList.end(); it ++) {

			cvSetImageROI(src, rect);
			cvCopy(src,dst);	
			cvResetImageROI(src); 
			while(true){
				k = inSameOneT(dst, (*it)->img,&loc);
				if (k < MaxVal)break;
				//printf("find %d %f\r\n",(*it)->Tag,k);
				CvRect r = cvRect(loc.x+rect.x,loc.y+rect.y, (*it)->img->width, (*it)->img->height);
				Coordinate coo(new TempleImg(src, r,(*it)->Tag ),r,k);
				isAdd = false;
				for (vector<Block *>::iterator _it = TmpBl.begin(); _it != TmpBl.end(); _it ++) {
					if ((*_it)->MergeWithCoordinate(coo)){
						isAdd = true;
						break;
					}
				}
				if (!isAdd){
					Block * bl = new Block(coo.rect,NULL);
					if (!bl->MergeWithCoordinate(coo)){
						printf("add err bl merge coor................");
						exit(0);
					}
					//printf("add new tmpbl\r\n");
					TmpBl.push_back(bl);
				}
				//printf("find ok\r\n");
				coo.Fill(dst,this->rect);
			}
		}		
		cvReleaseImage(&dst);
		for (vector<Block *>::iterator it = TmpBl.begin(); it != TmpBl.end(); it ++) {

			sort((*it)->Coord.begin(),(*it)->Coord.end(),compCoordinateOrder);
			Coordinate Coor = (*it)->Coord.back();

			vector <TempleImg*> _T = (*it)->GetTempleList();
			for (vector<TempleImg*>::iterator _t = _T.begin(); _t != _T.end(); _t ++) {
				if ((*_t) == Coor.te){
					_T.erase(_t);
					break;
				}
			}
			//(*it)->Coord.pop_back();
			//printf("coor %d\r\n",Coor.te->Tag);

			if (this->Coll==1){
				if (CollPath == NULL){
					char defPath[1024]={0};
					GetTemplePath(defPath);
					CollPath = new char [1024];
					sprintf(CollPath,"coll\\%s\0",defPath);
				}
				Coor.te->SaveTemple(CollPath);
			}

			this->Coord.push_back(Coor);
			val[count] = Coor.te->Tag;
			count++;

			isAdd = false;
			for (vector<Block *>::iterator _it = Child.begin(); _it != Child.end(); _it ++) {
				if ( (*_it)->MergeWithCoordinate(Coor) ){				 
					isAdd = true;
					break;
				}
			}
			if (!isAdd){
				Block * bl = new Block((*it)->rect,this);		
				if (!bl->MergeWithCoordinate(Coor)){
					printf("add err bl merge coor 1................");
					exit(0);
				}
				this->Child.push_back(bl);		
			}
			delete (*it);		
		}
		TmpBl.clear();
		sort(this->Child.begin(),this->Child.end(),compBlockX);
	}
	//for (int i=count;i<chileLen;i++){
	//	val[i] = -1;
	//}
	size= count;
	return val;
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
				//this->Child.pop_back();
				delete bl;
				break;
			}
			this->Child.push_back(bl);
			Coordinate Coor = bl->Coord.back();
			bl->rect = Coor.rect;
			bl->Fill(src);
			bl->TempleList.push_back(Coor.te);
			this->Coord.push_back(Coor);
			count++;
		}
		sort(this->Child.begin(),this->Child.end(),compBlockX);
		sort(this->Child.begin(),this->Child.end(),compBlockY);
		//cvReleaseImage(&dst);
	}
	return count;
}
void Block::Fill(IplImage * src){
	//rect = this->Coord.front().rect;
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
bool Block::ClickCoordinate(int _v,int num,bool isDel){
	if (_v == -1){
		if (this->Coord.empty()){
			Coordinate coor(NULL,this->rect);		
			coor.MouseClick(num);
		}else{
			this->Coord.back().MouseClick(num);
		}
		return true;
	}		
	for (vector<Coordinate>::iterator it = Coord.begin(); it != Coord.end(); it ++) {
		if ((*it).Check(_v)){
			(*it).MouseClick(num);
			if (isDel){
				Coord.erase(it);
			}
			return true;		
		}
	}
	return false;
}
bool Block::ClickCoordinateCheck(int _v,int num,IplImage *Img){
	
	for (vector<Coordinate>::iterator it = Coord.begin(); it != Coord.end(); it ++) {
		if (!(*it).Check(_v)) continue;

		if (compImg(Img,(*it).te->img,(*it).rect)){
			(*it).MouseClick(num);
		}
		Coord.erase(it);
		return true;		
		
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
		loc->x = 0;
		loc->y = 0;
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
/**
void inSameT(IplImage * img ,TempleImg * NI,int* Arr,CvPoint *loc,int & N){
		//CvSize sizeResult = cvSize(img->width - NI->img->width + 1,img->height - NI->img->height + 1);   
	IplImage* imgResult = cvCreateImage(cvSize(img->width - NI->img->width + 1,img->height - NI->img->height + 1),IPL_DEPTH_32F,1);   
	cvMatchTemplate(img,NI->img,imgResult,CV_TM_CCOEFF_NORMED); 
	 
    double max_val,min_val;  
	CvPoint minLoc;

	cvMinMaxLoc(imgResult, &min_val, &max_val, &minLoc, &loc[N], NULL);  
	while (max_val >0.8){
		Arr[N]=NI->Tag;
		//printf("%d %d %d %d\r\n",N,NI->num,loc[N].x,loc[N].y);
		//loc[N] = max_loc;
		N++;
		if (N==14)break;
		//CvPoint next_loc;
		max_val = getNextMaxLoc(imgResult , NI->img->width,NI->img->height,min_val , loc,N);
		//N++;
		
	}
	cvReleaseImage(&imgResult);

}
double getNextMaxLoc(IplImage* result , int templatWidth,int templatHeight,double minValIn , CvPoint *lastLoc,int  n)
{
	int startX = lastLoc[n-1].x ;  
    int startY = lastLoc[n-1].y ;  
    int endX = lastLoc[n-1].x + templatWidth;  
    int endY = lastLoc[n-1].y + templatHeight;  
    if(startX < 0 || startY < 0)  
    {  
        startX = 0;  
        startY = 0;  
    }  
    if(endX > result->width - 1 || endY > result->height - 1)  
    {  
        endX = result->width - 1;  
        endY = result->height - 1;  
    }  
    int y, x;  
    for(y = startY; y < endY; y++)  
    {  
        for(x = startX; x < endX; x++)  
        {  
            cvSetReal2D(result, y, x, minValIn);  
        }  
    }  
	//n++;
	 

         double minVal,maxVal;
        CvPoint minLoc;        //查找result中的最小值 及其所在坐标
        cvMinMaxLoc(result,&minVal,&maxVal,&minLoc,&lastLoc[n],NULL);
        return maxVal; 
} 

**/