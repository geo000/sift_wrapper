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
}

bool Sift::Init()
{
	_hsiftgpu = LoadLibrary("SiftGPU.dll");
	if (_hsiftgpu == NULL)
	{
		return false;
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
		_hsiftgpu = NULL;
		return false;
	}

	_matcher->VerifyContextGL();
	return true;
}

Sift::~Sift()
{
	if (_sift_gpu != NULL)
	{
		delete _sift_gpu;
		_sift_gpu = NULL;
	}
	if (_matcher != NULL)
	{
		delete _matcher;
		_matcher = NULL;
	}
	if (_hsiftgpu != NULL)
	{
		FREE_MYLIB(_hsiftgpu);
		_hsiftgpu = NULL;
	}
}

Sift* Sift::GetInstance()
{
	if (_sift == NULL)
	{
		_sift = new Sift();
		if (!_sift->Init())
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

void Sift::GetKeyPoints(const char* image_path,
	std::vector<SiftGPU::SiftKeypoint>& key,
	std::vector<float>& descriptor)
{
	if (_sift_gpu->RunSIFT(image_path))
	{
		int num = _sift_gpu->GetFeatureNum();
		key.resize(num);
		descriptor.resize(128*num);

		_sift_gpu->GetFeatureVector(&key[0], &descriptor[0]);
	}
}

int Sift::GetMatchedPoints(std::vector<SiftGPU::SiftKeypoint>& key1,
	const float* descriptor1, std::vector<SiftGPU::SiftKeypoint>& key2,
	const float* descriptor2, std::vector<SamePoint>& result)
{
	_matcher->SetDescriptors(0, key1.size(), descriptor1);
	_matcher->SetDescriptors(1, key2.size(), descriptor2);

	int (*match_buf)[2] = new int [key1.size()][2];

	int num_match = _matcher->GetSiftMatch(key1.size(), match_buf);

	result.resize(num_match);

	for (int i = 0; i < num_match; ++i)
	{
		result[i] = SamePoint(key1[match_buf[i][0]].x, key1[match_buf[i][0]].y,
			key1[match_buf[i][1]].x, key1[match_buf[i][1]].y);
	}

	delete []match_buf;

	return num_match;
}

