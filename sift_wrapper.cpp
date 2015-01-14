#define _SIFT_API_EXPORT
#include "sift_wrapper.h"

Sift* Sift::_sift = NULL;


Sift::Sift()
{

}

Sift* Sift::GetInstance()
{
	if (_sift == NULL)
	{
		_sift = new Sift();
	}
	return _sift;
}

