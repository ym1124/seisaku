#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "path_vec.h"

namespace path_tracing 
{
	typedef Vec Color;//Vec�^��Color�^�Ƃ��Č^��`

	enum ReflectionType {
		REFLECTION_TYPE_DIFFUSE,	// ���S�g�U�ʁB������Lambertian�ʁB
		REFLECTION_TYPE_SPECULAR,	// ���z�I�ȋ��ʁB
		REFLECTION_TYPE_REFRACTION,	// ���z�I�ȃK���X�I�����B
	};
	const double kIor = 1.5; // �K���X�ʂɑ΂�����ܗ�(Index of refraction)
};

#endif
