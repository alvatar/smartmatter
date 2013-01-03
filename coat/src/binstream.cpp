#include "voreen/modules/coat/binstream.h"

#include <cassert>

using namespace std;

DWORD FILE_ReadBinStream::make_magic(const string magic) {
    int l = magic.size();
    int i;
    DWORD c, M = 0;
    for(i = l - 1; i >= 0; i--)
    {
        c = magic[i];
        M += c << (8 * (3 - i));
    }
    return M;
}

FILE_ReadBinStream::FILE_ReadBinStream(const char* Name)
{
    fSize=0;
    F=INVALID_HANDLE_VALUE;
    Pos=0;
    TempSize=65536*128;
    TempBuffer=new BYTE[TempSize];
    TempCurrSize=0;
    TempPos=0;
    ReadFromFile(Name);
}

FILE_ReadBinStream::~FILE_ReadBinStream()
{
    if(TempBuffer) delete []TempBuffer;
    if(F!=INVALID_HANDLE_VALUE) RClose(F);
}

void FILE_ReadBinStream::ReadFromFile(const string filename)
{
	if(F!=INVALID_HANDLE_VALUE) RClose(F);
	fSize=0;
	F=INVALID_HANDLE_VALUE;
	Pos=0;
	if(Name)
    {
		//F=RReset(filename);
		if(F)
        {
			fSize=RFileSize(F);			
		}
	}
}

/*
typedef HANDLE ResFile;
ResFile RReset(LPCSTR lpFileName)
{
	return CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,0,NULL);
}
ResFile RRewrite(LPCSTR lpFileName)
{
	return CreateFile(lpFileName,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
		                         CREATE_ALWAYS,0,NULL);
}
DWORD RFileSize(HANDLE hFile)
{
	SetLastError(0);
	return GetFileSize(hFile,NULL);
}
DWORD RSeek(ResFile hFile,int pos)
{
	SetLastError(0);
	return SetFilePointer(hFile,pos,NULL,FILE_BEGIN);
}

DWORD RSeekFromCurrent(ResFile hFile,int nBytes)
{
	SetLastError(0);
	return SetFilePointer(hFile,nBytes,NULL,FILE_CURRENT);
}

DWORD RBlockRead(ResFile hFile,LPVOID lpBuffer,DWORD BytesToRead)
{
	SetLastError(0);
	DWORD readBytes;
	ReadFile(hFile,lpBuffer,BytesToRead,&readBytes,NULL);
	return readBytes;
}
DWORD RBlockWrite(ResFile hFile,LPVOID lpBuffer,DWORD BytesToWrite)
{
	SetLastError(0);
	DWORD writeBytes;
	WriteFile(hFile,lpBuffer,BytesToWrite,&writeBytes,NULL);
	return writeBytes;
}
void RClose(ResFile hFile)
{
	SetLastError(0);
	if(!CloseHandle(hFile))
		MessageBox(NULL,"Failed to close file!","Strange error",0);
}
bool FILE_ReadBinStream::Valid(){
	return F!=INVALID_HANDLE_VALUE;
}
void FILE_ReadBinStream::Write(void* data,FsType size){	
	assert(1);
}
FsType FILE_ReadBinStream::Read(void* Data,FsType size){	
	BYTE* data=(BYTE*)Data;
	int ads=0;
	if(TempPos+size>TempCurrSize){
		FsType rem=TempCurrSize-TempPos;
		if(rem){
			memcpy(data,TempBuffer+TempPos,rem);			
			data+=rem;
			size-=rem;
			Pos+=rem;
		}
		if(size>TempSize){
            if(F!=INVALID_HANDLE_VALUE)size=RBlockRead(F,data,size);
			TempCurrSize=0;
			TempPos=0;
			Pos+=size;
			return size+rem;
		}else{
			DWORD tsz=0;
			if(F!=INVALID_HANDLE_VALUE)tsz=RBlockRead(F,TempBuffer,TempSize);
			if(size>tsz)size=tsz;
			TempCurrSize=tsz;//TempSize;
			TempPos=0;
			ads=rem;
		}
	}
	memcpy(data,TempBuffer+TempPos,size);
	TempPos+=size;
	Pos+=size;
	return size+ads;
}
FsType FILE_ReadBinStream::Skip(FsType size){	
	if(TempPos+size>TempCurrSize){
		FsType rem=TempCurrSize-TempPos;
		if(rem){
			size-=rem;
			Pos+=rem;
		}
		if(size>TempSize){
			TempCurrSize=0;
			TempPos=0;
			Pos+=size;
			return size;
		}else{
			RBlockRead(F,TempBuffer,TempSize);
			TempCurrSize=TempSize;
			TempPos=0;
		}
	}	
	TempPos+=size;
	Pos+=size;
	return size;
}
void FILE_ReadBinStream::WriteToFile(const char* Name){
	assert(1);
}
FsType FILE_ReadBinStream::Size(){
	return fSize;
}
void FILE_ReadBinStream::SetReadPos(FsType pos){
	assert(1);
}
void FILE_ReadBinStream::SetWritePos(FsType pos){
	assert(1);
}
FsType FILE_ReadBinStream::GetReadPos(){
	return Pos;
}
FsType FILE_ReadBinStream::GetWritePos(){
	assert(1);
	return 0;
}
BYTE* FILE_ReadBinStream::GetData(){
	assert(1);
	return NULL;
}
void FILE_ReadBinStream::Clear(){
	if(TempBuffer)delete[]TempBuffer;
	if(F!=INVALID_HANDLE_VALUE)RClose(F);
	F=INVALID_HANDLE_VALUE;
	TempBuffer=NULL;
	Pos=0;
	fSize=0;
}
BYTE  BinStream::ReadBYTE(){
    BYTE b=0;
	Read(&b,1);
	return b;
}
WORD  BinStream::ReadWORD(){
	WORD w=0;
	Read(&w,2);
	return w;
}
DWORD BinStream::ReadDWORD(){
	DWORD D=0;
	Read(&D,4);
	return D;
}
FileWriteBinStream::FileWriteBinStream(const char* Name){
	Pos=0;
    F=INVALID_HANDLE_VALUE;
	if(Name)F=RRewrite(Name);
	else F=RRewrite("temp.dat");	
}
FileWriteBinStream::~FileWriteBinStream(){
	Clear();
}
bool FileWriteBinStream::Valid(){
	return F!=INVALID_HANDLE_VALUE;
}
void FileWriteBinStream::Write(void* data,FsType size){	
	if(F!=INVALID_HANDLE_VALUE){
        RBlockWrite(F,data,size);
	}
	Pos+=size;
}
FsType FileWriteBinStream::Read(void* data,FsType size){
	assert(1);
	return 0;
}
void FileWriteBinStream::WriteToFile(const char* Name){
	assert(1);
}
void FileWriteBinStream::ReadFromFile(const char* Name){
	assert(1);
}
FsType FileWriteBinStream::Size(){
    return Pos;
}
void FileWriteBinStream::SetReadPos(FsType pos){
	assert(1);
}
void FileWriteBinStream::SetWritePos(FsType pos){
	assert(1);
}
FsType FileWriteBinStream::GetReadPos(){
	assert(1);
	return 0;
}
FsType FileWriteBinStream::Skip(FsType size){
	assert(1);
	return 0;
}
FsType FileWriteBinStream::GetWritePos(){
	return Pos;
}
BYTE* FileWriteBinStream::GetData(){
	assert(1);
	return NULL;
}
void FileWriteBinStream::Clear(){
	if(F!=INVALID_HANDLE_VALUE)RClose(F);	
    F=INVALID_HANDLE_VALUE;
	Pos=0;
}
MemoryBinStream::MemoryBinStream(){
	Data=NULL;
	_Size=0;
	_MaxSize=0;
	_WritePos=0;
	_ReadPos=0;
}
MemoryBinStream::~MemoryBinStream(){
	Clear();
}
void  MemoryBinStream::CheckSize(FsType size){
	if(size>_MaxSize){
		size=size+_MaxSize/2;
		BYTE* D1=new BYTE[size];
		if(Data){
			memcpy(D1,Data,_MaxSize);
			delete[]Data;
		}
        Data=D1;
		_MaxSize=size;
	}
}
void  MemoryBinStream::Write(void* data,FsType size){
    CheckSize(_WritePos+size);
	memcpy(Data+_WritePos,data,size);
	_WritePos+=size;
	_Size=max(_Size,_WritePos);
}
FsType  MemoryBinStream::Read(void* data,FsType size){
	int rsize=_Size-_ReadPos;
	rsize=min(rsize,int(size));
	if(rsize>0)memcpy(data,Data+_ReadPos,size);
    rsize=max(rsize,0);
	_ReadPos+=rsize;
	return rsize;
}
void  MemoryBinStream::WriteToFile(const char* Name){
	ResFile F=RRewrite((char*)Name);
	if(F!=INVALID_HANDLE_VALUE){
		RBlockWrite(F,Data,_Size);
		RClose(F);
	}
}
void  MemoryBinStream::ReadFromFile(const char* Name){
	Clear();
	ResFile F=RReset((char*)Name);
	if(F!=INVALID_HANDLE_VALUE){
		FsType sz=RFileSize(F);
        CheckSize(sz);
		RBlockRead(F,Data,sz);
		RClose(F);
		_Size=sz;
	}
}
FsType   MemoryBinStream::Size(){
	return _Size;
}
void  MemoryBinStream::SetReadPos(FsType pos){
	_ReadPos=pos;//max(pos,0);
}
void  MemoryBinStream::SetWritePos(FsType pos){
	_WritePos=pos;//max(pos,0);
}
FsType   MemoryBinStream::GetReadPos(){
	return _ReadPos;
}
FsType   MemoryBinStream::GetWritePos(){
	return _WritePos;
}
BYTE* MemoryBinStream::GetData(){
	return Data;
}
void  MemoryBinStream::Clear(){
	if(Data)delete[]Data;
	Data=NULL;
	_Size=0;
	_MaxSize=0;
	_WritePos=0;
	_ReadPos=0;
}
void  BinStream::WriteBYTE(BYTE c){
    Write(&c,1);
}
void  BinStream::WriteWORD(WORD w){
	Write(&w,2);
}
void  BinStream::WriteDWORD(DWORD D){
	Write(&D,4);
}
FILE_WriteBinStream::FILE_WriteBinStream(const char* Name){
	Pos=0;
	F=INVALID_HANDLE_VALUE;
	bool SkipAlloc=false;
	if(Name){
		F=RRewrite(Name);
	}else F=RRewrite("temp.dat");
	TempSize=65536*128;
	if(SkipAlloc)TempBuffer=NULL;
	else TempBuffer=new BYTE[TempSize];
	TempCurrSize=0;
	TempPos=0;
}
FILE_WriteBinStream::~FILE_WriteBinStream(){
	Clear();
}
bool FILE_WriteBinStream::Valid(){
	return F!=INVALID_HANDLE_VALUE;
}
void StartTimer(int i);
void EndTimer(const char* Msg,int i);
void FILE_WriteBinStream::Write(void* data,FsType size){	
	if(F!=INVALID_HANDLE_VALUE){
		if(TempCurrSize+size>=TempSize){
			RBlockWrite(F,TempBuffer,TempCurrSize);
			TempCurrSize=0;		
		}
		if(size>TempSize){
			RBlockWrite(F,data,size);
		}else{
			memcpy(TempBuffer+TempCurrSize,data,size);
			TempCurrSize+=size;
		}
	}
	Pos+=size;
}
FsType FILE_WriteBinStream::Read(void* data,FsType size){
	assert(1);
	return 0;
}
void FILE_WriteBinStream::MoveStructure(FILE_WriteBinStream* Dest){
	Dest->F=F;    
	Dest->Pos=Pos;
	Dest->TempBuffer=TempBuffer;
	Dest->TempSize=TempSize;
	Dest->TempCurrSize=TempCurrSize;
	Dest->TempPos=TempPos;
	F=INVALID_HANDLE_VALUE;
	Pos=0;
	TempBuffer=NULL;
	TempCurrSize=0;
	TempPos=0;
}
void FILE_WriteBinStream::WriteToFile(const char* Name){
	assert(1);
}
void FILE_WriteBinStream::ReadFromFile(const char* Name){
	assert(1);
}
FsType FILE_WriteBinStream::Size(){
    return Pos;
}
void FILE_WriteBinStream::SetReadPos(FsType pos){
	assert(1);
}
void FILE_WriteBinStream::SetWritePos(FsType pos){
	assert(1);
}
FsType FILE_WriteBinStream::GetReadPos(){
	assert(1);
	return 0;
}
FsType FILE_WriteBinStream::GetWritePos(){
	return Pos;
}
BYTE* FILE_WriteBinStream::GetData(){
	assert(1);
	return NULL;
}
void FILE_WriteBinStream::Clear(){
	if(F!=INVALID_HANDLE_VALUE && TempCurrSize){
		RBlockWrite(F,TempBuffer,TempCurrSize);
	}
	if(F!=INVALID_HANDLE_VALUE){
		RClose(F);
	}
	delete[]TempBuffer;	
	F=INVALID_HANDLE_VALUE;
	Pos=0;
	TempBuffer=NULL;
	TempCurrSize=0;
}
FsType FILE_WriteBinStream::Skip(FsType size){
	assert(1);
	return 0;
}
*/
