#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "path_vec.h"
#include "path_constant.h"

namespace path_tracing 
{
	struct Hitpoint {//�Փ˓_
		double distance;//ray�̎n�_��������_�܂ł̋���
		Vec normal;//�����_�̖@��
		Vec position;//�����_�̍��W(���[���h���W)
		Hitpoint() : distance(kINF), normal(), position() {}
	};
	struct Intersection 
	{
		Hitpoint hitpoint;
		int object_id;//���̂�id
		Intersection() : object_id(-1) {}
	};
};

#endif
