#include "../sift_wrapper.h"
#pragma comment(lib, "../../Release/sift_wrapper.lib")

int main(int argc, char* argv[])
{
	Sift* sift = Sift::GetInstance();
	std::vector<SiftGPU::SiftKeypoint> keys1;
	std::vector<float> descriptor1;
	sift->GetKeyPoints("C:\\data\\A1.tif", keys1, descriptor1);
	
	std::vector<SiftGPU::SiftKeypoint> keys2;
	std::vector<float> descriptor2;
	sift->GetKeyPoints("C:\\data\\A2.tif", keys2, descriptor2);

	std::vector<SamePoint> result;
	sift->GetMatchedPoints(keys1, &descriptor1[0], keys2, &descriptor2[0], result);
	Sift::Release();

	return 0;
}