#define _SIFT_API_EXPORT
#include "sift_wrapper.h"

Sift* Sift::_sift = NULL;


Sift::Sift()
	: _hsiftgpu(NULL)
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

	char * argv[] = {"-fo", "-1",  "-v", "1"};
	int argc = sizeof(argv)/sizeof(char*);
	_sift_gpu->ParseParam(argc, argv);

	if (_sift_gpu->CreateContextGL() != SiftGPU::SIFTGPU_FULL_SUPPORTED)
	{
		delete _sift_gpu;
		_sift_gpu = NULL;
		delete _matcher;
		_matcher = NULL;
		FREE_MYLIB(_hsiftgpu);
		throw;
	}

	_matcher->VerifyContextGL();
}

Sift::~Sift()
{
	if (_sift != NULL)
	{
		delete _sift_gpu;
		_sift_gpu = NULL;
		delete _matcher;
		_matcher = NULL;
		FREE_MYLIB(_hsiftgpu);
	}
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

void Sift::Release()
{
	if (_sift != NULL)
	{
		delete _sift;
		_sift = NULL;
	}
}

void Sift::GetKeyPoints(const char* image_path, std::vector<SiftGPU::SiftKeypoint>& key, std::vector<float>& descriptor)
{
	if (_sift_gpu->RunSIFT(image_path))
	{
		int num = _sift_gpu->GetFeatureNum();
		key.resize(num);
		descriptor.resize(128*num);

		_sift_gpu->GetFeatureVector(&key[0], &descriptor[0]);
	}
}

