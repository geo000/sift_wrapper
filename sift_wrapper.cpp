#define _SIFT_API_EXPORT
#include "sift_wrapper.h"
#include "GL/GL.h"

Sift* Sift::_sift = NULL;


Sift::Sift()
	: _hsiftgpu(NULL)
	, _pCreateNewSiftGPU(NULL)
	, _pCreateNewSiftMatchGPU(NULL)
	, _sift_gpu(NULL)
	, _matcher(NULL)
	, _pImage(NULL)
{
	CoInitialize(NULL);
}

bool Sift::Init()
{
	_hsiftgpu = LoadLibrary("SiftGPU.dll");
	if (_hsiftgpu == NULL)
	{
		return false;
	}

	HRESULT hres = CoCreateInstance(CLSID_ImageDriverX, NULL, CLSCTX_ALL, IID_IImageX, (void**)_pImage);
	if (FAILED(hres))
	{
		FREE_MYLIB(_hsiftgpu);
		_hsiftgpu = NULL;
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
	if (_pImage != NULL)
	{
		_pImage->Release();
		_pImage = NULL;
	}
	CoUninitialize();
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
		_pImage->Open(_bstr_t(image_path), modeRead);
		int rows = 0;
		_pImage->GetRows(&rows);
		_pImage->Close();
		for (int i = 0; i < num; ++i)
		{
			key[i].y = rows-key[i].y;
		}
	}
	else
	{
		_pImage->Open(_bstr_t(image_path), modeRead);

		int cols = 0, rows = 0;
		int bandnum = 0;
		int bpb = 0;

		_pImage->GetCols(&cols);
		_pImage->GetRows(&rows);
		_pImage->GetBandNum(&bandnum);
		_pImage->GetBPB(&bpb);

		if (bpb == 2)
		{
			_pImage->Close();
			return;
		}
		else if (bpb == 1 && (bandnum == 1 || bandnum == 3))
		{
			std::vector<SiftGPU::SiftKeypoint> key_;
			std::vector<float> descriptor_;
			const int block_size = 512;
			unsigned char* pbuf = new unsigned char[bandnum*cols*block_size];
			memset(pbuf, 0, bandnum*cols*block_size);

			for (int i = 0; i < rows;)
			{
				if (i+block_size < rows)
				{
					_pImage->ReadImg(0, i, cols, i+block_size, pbuf, cols, block_size, bandnum, 0, 0, cols, block_size, -1, 0);
					if (bandnum == 1)
					{
						_sift_gpu->RunSIFT(cols, block_size, pbuf, GL_LUMINANCE, GL_UNSIGNED_BYTE);
					}
					else
					{
						_sift_gpu->RunSIFT(cols, block_size, pbuf, GL_RGB, GL_UNSIGNED_BYTE);
					}
					int num = _sift_gpu->GetFeatureNum();
					key_.resize(num);
					descriptor_.resize(128*num);
					_sift_gpu->GetFeatureVector(&key_[0], &descriptor_[0]);

					for (int n = 0; n < num; ++n)
					{
						key_[i].y = i+key_[i].y;
						key.push_back(key_[i]);
						for (int m = 0; m < 128; ++m)
						{
							descriptor.push_back(descriptor_[m]);
						}
					}

					key_.swap(std::vector<SiftGPU::SiftKeypoint> ());
					descriptor_.swap(std::vector<float> ());

					i += block_size;
				}
				else
				{
					delete []pbuf;
					pbuf = new unsigned char[bandnum*cols*(rows-i)];
					memset(pbuf, 0, bandnum*cols*(rows-i));
					_pImage->ReadImg(0, i, cols, rows, pbuf, cols, rows-i, bandnum, 0, 0, cols, rows-i, -1, 0);
					if (bandnum == 1)
					{
						_sift_gpu->RunSIFT(cols, rows-i, pbuf, GL_LUMINANCE, GL_UNSIGNED_BYTE);
					}
					else
					{
						_sift_gpu->RunSIFT(cols, rows-i, pbuf, GL_RGB, GL_UNSIGNED_BYTE);
					}

					int num = _sift_gpu->GetFeatureNum();
					key_.resize(num);
					descriptor_.resize(128*num);
					_sift_gpu->GetFeatureVector(&key_[0], &descriptor_[0]);

					for (int n = 0; n < num; ++n)
					{
						key_[i].y = i+key_[i].y;
						key.push_back(key_[i]);
						for (int m = 0; m < 128; ++m)
						{
							descriptor.push_back(descriptor_[m]);
						}
					}

					key_.swap(std::vector<SiftGPU::SiftKeypoint> ());
					descriptor_.swap(std::vector<float> ());

					i = rows;
				}
			}

			delete []pbuf;
			pbuf = NULL;
		}

		_pImage->Close();
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

