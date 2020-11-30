#ifndef _PPM_H_
#define _PPM_H_

#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>

#include "path_material.h"

namespace path_tracing 
{
	inline double clamp(double x) {//0~1�ɃN�����v
		if (x < 0.0)
			return 0.0;
		if (x > 1.0)
			return 1.0;
		return x;
	}

	inline int to_int(double x) {//2.2�̓K���}�␳�l//0.5�������ƂŎl�̌ܓ������ۂɌJ��グ��int�^�ɃL���X�g���Ă�
		return int(pow(clamp(x), 1 / 2.2) * 255 + 0.5);
	}

	//ppm�`���Ƃ��ĕۑ�
	void save_ppm_file(const std::string &filename, const Color *image, const int width, const int height) {
		FILE *f;
		fopen_s(&f, filename.c_str(), "wb");
		fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
		for (int i = 0; i < width * height; i++)
			fprintf(f, "%d %d %d ", to_int(image[i].x), to_int(image[i].y), to_int(image[i].z));
		fclose(f);
	}
};

#endif
