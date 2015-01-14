#ifndef _SIFT_WRAPPER_H
#define _SIFT_WRAPPER_H

#ifdef _SIFT_API_EXPORT
#define _SIFT_API_ __declspec(dllexport)
#else
#define _SIFT_API_ __declspec(dllimport)
#endif

#define NULL 0

#include "SiftGPU.h"

class _SIFT_API_ Sift
{
public:
	static Sift* GetInstance();
protected:
private:
	Sift();
	~Sift();
	static Sift* _sift;
};



#endif

