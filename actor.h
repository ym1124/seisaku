#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
using namespace DirectX;

#include "substance.h"
#include "constant_buffer.h"
#include "shader_ex.h"
#include "render_state.h"

#include "vertex.h"

enum class COMMANDS { IDLE, RUN, ATTACK, TURN_LEFT, TURN_RIGHT, JUMP, DEATH };

class avatar
{
public:
	std::unique_ptr<descartes::substance<vf::skinned_mesh>> model;
	struct shader_constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	};
	std::unique_ptr<descartes::constant_buffer<shader_constants>> constant_buffer;

	XMFLOAT4 position = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 direction = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
	XMFLOAT4 velocity = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	float max_speed = 5;
	float max_turning = 180;

	enum STATES { STATE_IDLE, STATE_AIM };
	STATES state = STATE_IDLE;
	float vertical_factor = 0; /*-1 < vertical_factor < +1*/
	float horizontal_factor = 0; /*-1 < horizontal_factor < +1*/
								 //private:

	enum ANIMATION_CLIP {
		CLIP_TPOSE, CLIP_IDLE, CLIP_FIRE, CLIP_WALK, CLIP_EMP,
		CLIP_AIM_CC,
		CLIP_AIM_D,
		CLIP_AIM_U,
		CLIP_AIM_L,
		CLIP_AIM_LD,
		CLIP_AIM_LU,
		CLIP_AIM_R,
		CLIP_AIM_RD,
		CLIP_AIM_RU,
	};
	ANIMATION_CLIP animation_clip = CLIP_TPOSE;
	float animation_tick = 0.0f;
	std::int32_t animation_frame = 0;
	void animate(float elapsed_time);

public:
	avatar(ID3D11Device* device, const char* filename, const std::vector<std::string>& animation_filenames, int animation_sampling_rate = 0);
	//avatar() = default;
	virtual ~avatar() = default;

	void update(std::deque<COMMANDS>& command_queue, float speed_scale, float turning_scale, float elapsed_time);
	void render(ID3D11DeviceContext* immediate_context);
	//void render(ID3D11DeviceContext* immediate_context, float vertical_factor /*-1 < horizontal_factor < +1*/, float horizontal_factor /*-1 < horizontal_factor < +1*/);
	void shadowcast(ID3D11DeviceContext* immediate_context);

private:
	void render(ID3D11DeviceContext* immediate_context, const descartes::substance<vf::skinned_mesh>::mesh& mesh);


private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11ClassLinkage> class_linkage;
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ClassInstance>> class_instances;
	std::unique_ptr<descartes::rasterizer_state> rasterizer_states[2];

private:
	XMFLOAT4X4 world_transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	bool has_command(std::deque<COMMANDS>& command_queue, COMMANDS key)
	{
		return std::find(command_queue.begin(), command_queue.end(), key) != command_queue.end();
	}

	STATES last_state = STATE_IDLE;



};
