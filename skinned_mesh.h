#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>

#include "substance.h"
#include "constant_buffer.h"

#include "vertex.h"

class skinned_mesh : public descartes::substance<vf::skinned_mesh>
{
public:
	skinned_mesh(ID3D11Device* device, const char* filename, int animation_sampling_rate = 0);
	skinned_mesh(ID3D11Device* device, const char* filename, const std::vector<std::string>& animation_filenames, int animation_sampling_rate = 0);

	virtual ~skinned_mesh() = default;
	skinned_mesh(skinned_mesh&) = delete;
	skinned_mesh& operator =(skinned_mesh&) = delete;

	struct shader_constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	};
	std::unique_ptr<descartes::constant_buffer<shader_constants>> constant_buffer;
	void render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, size_t animation_clip, size_t animation_frame);
};
