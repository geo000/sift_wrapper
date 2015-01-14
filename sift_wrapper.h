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
#include <vector>

#define FREE_MYLIB FreeLibrary
#define GET_MYPROC GetProcAddress

#include "SiftGPU.h"

typedef SiftGPU* (*pCreateNewSiftGPU)(int);
typedef SiftMatchGPU* (*pCreateNewSiftMatchGPU)(int);

struct SamePoint
{
	float lx;
	float ly;
	float rx;
	float ry;

	SamePoint(){lx = 0; ly = 0; rx = 0; ry = 0;}
	SamePoint(float lx_, float ly_, float rx_, float ry_)
	{
		lx = lx_;
		ly = ly_;
		rx = rx_;
		ry = ry_;
	}
};

class _SIFT_API_ Sift
{
public:

	static Sift* GetInstance();
	static void Release();
	void GetKeyPoints(const char* image_path, std::vector<SiftGPU::SiftKeypoint>& key, std::vector<float>& descriptor);
	void SaveKeyPoints(const char* image_path, const char* keypoint_path);
	void LoadKeyPoints(const char* keypoint_path, std::vector<SiftGPU::SiftKeypoint>& key, std::vector<float>& descriptor);
	int GetMachedPoints(std::vector<SiftGPU::SiftKeypoint>& key1, const float* descriptor1, std::vector<SiftGPU::SiftKeypoint>& key2, const float* descriptor2, std::vector<SamePoint>& result);

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

