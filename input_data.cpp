#include "input_data.h"
#include <ostream>

bool BinaryInput(const char * fbx_filename, ModelData& data)
{
	std::ifstream ifs;
	ifs.open((std::string(fbx_filename)).c_str(), std::ios::binary);

	//ì«Ç›çûÇ›é∏îs
	if (!ifs) {
		return false;
	}

	{
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(data);
	}
	ifs.close();

	for (auto &m : data.meshes)
	{
		for (auto &n : m.tinvers)
		{
			XMFLOAT4X4 inverse;

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					inverse.m[i][j] = n.m[i][j];
				}
			}

			m.inverse_transforms.push_back(inverse);
		}
	}

	return true;
}