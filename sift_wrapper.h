#ifndef _SIFT_WRAPPER_H
#define _SIFT_WRAPPER_H

#ifdef _SIFT_API_EXPORT
#define _SIFT_API_ __declspec(dllexport)
#else
#define _SIFT_API_ __declspec(dllimport)
#endif

#include <afx.h>
#include <afxwin.h>
#include <WinDef.h>

#define FREE_MYLIB FreeLibrary
#define GET_MYPROC GetProcAddress

#include "SiftGPU.h"

typedef SiftGPU* (*pCreateNewSiftGPU)(int);
typedef SiftMatchGPU* (*pCreateNewSiftMatchGPU)(int);

class _SIFT_API_ Sift
{
public:
	static Sift* GetInstance();
	static void Release();
protected:
private:
	Sift();
	~Sift();
	static Sift* _sift;
	HINSTANCE _hsiftgpu;
	pCreateNewSiftGPU _pCreateNewSiftGPU;
	pCreateNewSiftMatchGPU _pCreateNewSiftMatchGPU;
	SiftGPU* _sift_gpu;
	SiftMatchGPU* _matcher;
};



#endif

