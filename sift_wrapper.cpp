#define _SIFT_API_EXPORT
#include "sift_wrapper.h"

Sift* Sift::_sift = NULL;


Sift::Sift():_hsiftgpu(NULL)
	, _pCreateNewSiftGPU(NULL)
	, _pCreateNewSiftMatchGPU(NULL)
	, _sift_gpu(NULL)
	, _matcher(NULL)
{
	_hsiftgpu = LoadLibrary("siftgpu64.dll");
	if (_hsiftgpu == NULL)
	{
		throw;
	}

	_pCreateNewSiftGPU = (SiftGPU* (*) (int)) GET_MYPROC(_hsiftgpu, "CreateNewSiftGPU");
	_pCreateNewSiftMatchGPU = (SiftMatchGPU* (*)(int)) GET_MYPROC(_hsiftgpu, "CreateNewSiftMatchGPU");
	_sift_gpu = _pCreateNewSiftGPU(1);
	_matcher = _pCreateNewSiftMatchGPU(4096);
}

Sift* Sift::GetInstance()
{
	if (_sift == NULL)
	{
		try
		{
			_sift = new Sift();
		}
		catch (...)
		{
			return NULL;
		}
	}
	return _sift;
}
