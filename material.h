#pragma once

#include <string>
#include <unordered_map>
#include <directxmath.h>
using namespace DirectX;

#include "substance.h"

struct rules_of_nomenclature
{
	std::string name;
	std::string prifix;
	std::vector<std::string> suffixes;
	std::string extension;

	XMFLOAT4 colour = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	bool force_srgb = false;
	bool generate_mips = false;
	std::uint32_t levels = 1u; // Number of mip map levels
	bool alpha_channel_to_opaque_flag = true; // This parameter is used on load.
};
void create_materials(const char* media_directory, std::unordered_map<std::string, descartes::material>& materials);

