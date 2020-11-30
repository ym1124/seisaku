#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "path_vec.h"
#include "path_constant.h"

namespace path_tracing 
{
	struct Hitpoint {//衝突点
		double distance;//rayの始点から交差点までの距離
		Vec normal;//交差点の法線
		Vec position;//交差点の座標(ワールド座標)
		Hitpoint() : distance(kINF), normal(), position() {}
	};
	struct Intersection 
	{
		Hitpoint hitpoint;
		int object_id;//物体のid
		Intersection() : object_id(-1) {}
	};
};

#endif
