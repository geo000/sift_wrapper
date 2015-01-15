#include "../sift_wrapper.h"
#pragma comment(lib, "../../Release/sift_wrapper.lib")

int main(int argc, char* argv[])
{
	Sift* sift = Sift::GetInstance();
	std::vector<SiftGPU::SiftKeypoint> keys1;
	std::vector<float> descriptor1;
	sift->GetKeyPoints("C:\\data\\110270110034.tif", keys1, descriptor1);
	
	std::vector<SiftGPU::SiftKeypoint> keys2;
	std::vector<float> descriptor2;
	sift->GetKeyPoints("C:\\data\\110270111031.tif", keys2, descriptor2);

	std::vector<SamePoint> result;
	printf("Matched %d points.\n", sift->GetMatchedPoints(keys1, &descriptor1[0], keys2, &descriptor2[0], result));
	keys1.swap(std::vector<SiftGPU::SiftKeypoint> ());
	keys2.swap(std::vector<SiftGPU::SiftKeypoint> ());
	descriptor1.swap(std::vector<float> ());
	descriptor2.swap(std::vector<float> ());
	Sift::Release();

	return 0;
}