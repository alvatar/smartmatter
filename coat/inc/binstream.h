#include <cstddef>
#include <string>


typedef unsigned int HANDLE;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef char* LPCSTR;
typedef void* LPVOID;


#define INVALID_HANDLE_VALUE -1



typedef HANDLE ResFile;
//Opening the resource file
ResFile RReset(LPCSTR lpFileName);
//Rewriting file
ResFile RRewrite(LPCSTR lpFileName);
ResFile RAppend(LPCSTR lpFileName);
//Getting size of the resource file
DWORD RFileSize(HANDLE hFile);
// Setting file position 
DWORD RSeek(ResFile hFile,int pos);
DWORD RSeekFromCurrent(ResFile hFile,int nBytes);
//Reading the file
DWORD RBlockRead(ResFile hFile,LPVOID lpBuffer,DWORD BytesToRead);
//Writing the file
DWORD RBlockWrite(ResFile hFile,LPVOID lpBuffer,DWORD BytesToWrite);
//Returns last error
DWORD IOresult(void);
//Close the file
void RClose(ResFile hFile);

typedef DWORD FsType;


//! Binary stream
//!
class BinStream
{
    public:
        // virtual void  Write(void* data,FsType size)=0;
        // virtual FsType Read(void* data,FsType size)=0;
        // virtual void  WriteToFile(const char* Name)=0;
        virtual void  ReadFromFile(const char* Name)=0;
        // virtual FsType Size()=0;
        // virtual void  SetReadPos(FsType pos)=0;
        // virtual void  SetWritePos(FsType pos)=0;
        // virtual FsType GetReadPos()=0;
        // virtual FsType GetWritePos()=0;
        // virtual BYTE* GetData()=0;
        // virtual FsType Skip(FsType size)=0;
        // virtual void  Clear()=0;
        void  WriteBYTE(BYTE c);
        void  WriteWORD(WORD w);
        void  WriteDWORD(DWORD D);
        BYTE  ReadBYTE();  
        WORD  ReadWORD();
        DWORD ReadDWORD();
};


class FILE_ReadBinStream : public BinStream
{
    ResFile F;
    FsType Pos;
    FsType fSize;

    BYTE* TempBuffer;
    FsType TempSize;
    FsType TempPos;
    FsType TempCurrSize;

    public:
    FILE_ReadBinStream(const char* Name=NULL);
    ~FILE_ReadBinStream();	
    // bool Valid();
    // virtual void  Write(void* data,FsType size);
    // virtual FsType Read(void* data,FsType size);
    // virtual FsType Skip(FsType size);
    // virtual void  WriteToFile(const char* Name);
    virtual void  ReadFromFile(const char* Name);
    // virtual FsType Size();
    // virtual void  SetReadPos(FsType pos);
    // virtual void  SetWritePos(FsType pos);
    // virtual FsType GetReadPos();
    // virtual FsType GetWritePos();
    // virtual BYTE* GetData();
    // virtual void  Clear();

    static DWORD make_magic(const std::string magic);
};


/*
class FileWriteBinStream : public BinStream
{
    ResFile F;	
    FsType Pos;

    public:
    FileWriteBinStream(const char* Name=NULL);
    ~FileWriteBinStream();	
    bool Valid();
    virtual void  Write(void* data,FsType size);
    virtual FsType Read(void* data,FsType size);
    virtual void  WriteToFile(const char* Name);
    virtual void  ReadFromFile(const char* Name);
    virtual FsType Size();
    virtual void  SetReadPos(FsType pos);
    virtual void  SetWritePos(FsType pos);
    virtual FsType GetReadPos();
    virtual FsType GetWritePos();
    virtual BYTE* GetData();
    virtual void  Clear();
    virtual FsType Skip(FsType size);
};

class FILE_WriteBinStream:public BinStream{
	ResFile F;    
	FsType Pos;
	BYTE* TempBuffer;
	FsType TempSize;
	FsType TempCurrSize;
	FsType TempPos;
public:
	void MoveStructure(FILE_WriteBinStream* Dest);
	FILE_WriteBinStream(const char* Name);
	~FILE_WriteBinStream();	
	bool Valid();
	virtual void  Write(void* data,FsType size);
	virtual FsType Read(void* data,FsType size);
	virtual void  WriteToFile(const char* Name);
	virtual void  ReadFromFile(const char* Name);
	virtual FsType Size();
	virtual void  SetReadPos(FsType pos);
	virtual void  SetWritePos(FsType pos);
	virtual FsType GetReadPos();
	virtual FsType GetWritePos();
	virtual BYTE* GetData();
	virtual void  Clear();
	virtual FsType Skip(FsType size);
};
class BinStreamCounter:public BinStream{
public:
	BinStreamCounter(){
		sz=0;
	}
	int sz;
	virtual void  Write(void* data,FsType size){sz+=size;}
	virtual FsType Read(void* data,FsType size){return size;};
	virtual void  WriteToFile(const char* Name){};
	virtual void  ReadFromFile(const char* Name){};
	virtual FsType Size(){return sz;};
	virtual void  SetReadPos(FsType pos){};
	virtual void  SetWritePos(FsType pos){};
	virtual FsType GetReadPos(){return 0;};
	virtual FsType GetWritePos(){return sz;};
	virtual BYTE* GetData(){return NULL;};
	virtual void  Clear(){sz=0;};	
	virtual FsType Skip(FsType size){return 0;};
};
class MemoryBinStream:public BinStream{
	BYTE* Data;
	FsType _Size;
	FsType _MaxSize;
	FsType _WritePos;
	FsType _ReadPos;
public:
	MemoryBinStream();
	~MemoryBinStream();
			void  CheckSize(FsType size);
	virtual void  Write(void* data,FsType size);
	virtual FsType Read(void* data,FsType size);
	virtual void  WriteToFile(const char* Name);
	virtual void  ReadFromFile(const char* Name);
	virtual FsType Size();
	virtual void  SetReadPos(FsType pos);
	virtual void  SetWritePos(FsType pos);
	virtual FsType GetReadPos();
	virtual FsType GetWritePos();
	virtual BYTE* GetData();
	virtual void  Clear();
	virtual FsType Skip(FsType size){_ReadPos+=size;return size;};
};
*/
