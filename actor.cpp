#include <random>

#include "actor.h"
#include "material.h"

#pragma warning(disable:6011)

avatar::avatar(ID3D11Device* device, const char* filename, const std::vector<std::string>& animation_filenames, int animation_sampling_rate)
{
	HRESULT hr = S_OK;

	state = STATE_IDLE;
	position = XMFLOAT4(0.0f, 0.0f, 12.0f, 1.0f);
	XMStoreFloat4x4(&world_transform, XMMatrixRotationY(180 * 0.01745f) * XMMatrixTranslation(position.x, position.y, position.z));

	float angle = 180;
	direction.x = sinf(angle * 0.01745f);
	direction.y = 0;
	direction.z = cosf(angle * 0.01745f);

	velocity = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	animation_clip = CLIP_IDLE; //CLIP_IDLE CLIP_AIM

	model = std::make_unique<descartes::substance<vf::skinned_mesh>>(filename, false/*triangulate*/, animation_filenames, animation_sampling_rate);
	model->create_render_objects(device, true);

	// create materials for PBR
	char media_directory[256];
	strcpy_s(media_directory, filename);
	PathFindFileNameA(media_directory)[0] = '\0';
	create_materials(media_directory, model->materials);

	// customize for IBIS
	descartes::material* material;
	material = model->find_material("ibis_hair");
	if (material)
	{
		material->two_sided = true;
		//for (descartes::material::property& property : material->properties)
		//{
		//	property.generate_mips = true;
		//	property.levels = 0u;
		//}
	}
	//material = model->find_material("ibis_face");
	//if (material)
	//{
	//	for (descartes::material::property& property : material->properties)
	//	{
	//		property.generate_mips = true;
	//		property.levels = 0u;
	//	}
	//}
	model->create_shader_resource_views(device, media_directory);

	descartes::create_vs_from_cso(device, "avatar_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.GetAddressOf(), vf::skinned_mesh::input_element_desc, _countof(vf::skinned_mesh::input_element_desc), true);
#if 1
	descartes::create_ps_from_cso(device, "avatar_ps.cso", pixel_shader.ReleaseAndGetAddressOf(), class_linkage.GetAddressOf(), 0);
#else
	descartes::create_ps_from_cso(device, "avatar_ps.cso", pixel_shader.ReleaseAndGetAddressOf(), class_linkage.GetAddressOf(), reflector.GetAddressOf(), true);
	size_t number_of_interface_slots = reflector->GetNumInterfaceSlots();
	ID3D11ShaderReflectionVariable* shader_reflection_variable = reflector->GetVariableByName("material_assembler");
	size_t interface_slot = shader_reflection_variable->GetInterfaceSlot(0);
#endif

	Microsoft::WRL::ComPtr<ID3D11ClassInstance> default_material_assembler;
	hr = class_linkage->CreateClassInstance("default_material_assembler", 0, 0, 0, 0, default_material_assembler.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	class_instances.insert(std::make_pair("default_material_assembler", default_material_assembler));

	std::vector<std::string> custom_materials
	{
		"ibis_body",
		"ibis_item",
		"ibis_eye_out",
		"ibis_eye_in",
		"ibis_face",
		"ibis_hair",
		"ibis_ui",
	};
	for (std::string customized_material_name : custom_materials)
	{
		const descartes::material* material = model->find_material(customized_material_name.c_str());
		if (material)
		{
			Microsoft::WRL::ComPtr<ID3D11ClassInstance> class_instance;
			hr = class_linkage->CreateClassInstance(customized_material_name.c_str(), 0, 0, 0, 0, class_instance.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			class_instances.insert(std::make_pair(customized_material_name.c_str(), class_instance));
		}
	}

	constant_buffer = std::make_unique<descartes::constant_buffer<shader_constants>>(device);

	rasterizer_states[0] = std::make_unique<descartes::rasterizer_state>(device, D3D11_FILL_SOLID, D3D11_CULL_BACK);
	rasterizer_states[1] = std::make_unique<descartes::rasterizer_state>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE);
}

void avatar::animate(float elapsed_time)
{
	if (!model || model->animation_takes.size() < 1)
	{
		return;
	}

	static ANIMATION_CLIP last_animation_clip;
	last_animation_clip = animation_clip;

	switch (state)
	{
	case STATE_IDLE:
		break;
	case STATE_AIM:
		break;
	}

	if (animation_clip != last_animation_clip)
	{
		animation_frame = 0;
		animation_tick = 0;
	}

	//animation_clip = CLIP_IDLE; //CLIP_IDLE CLIP_AIM
	//animation_frame = static_cast<size_t>(animation_tick / model->substance->animation_takes.at(animation_clip).sampling_time);
	animation_frame = static_cast<std::int32_t>(animation_tick * model->animation_takes.at(animation_clip).sampling_rate);
	//animation_frame = 0;
	//animation_tick = 0;
	std::int32_t last_of_frame = model->animation_takes.at(animation_clip).number_of_keyframes - 1;
	if (animation_frame > last_of_frame) //end of frames
	{
		animation_frame = 0;
		animation_tick = 0;
		switch (animation_clip)
		{
		case CLIP_IDLE:
			animation_clip = CLIP_IDLE;
			state = STATE_IDLE;
			break;
		case CLIP_FIRE:
			animation_clip = CLIP_FIRE;
			state = STATE_AIM;
			break;
		case CLIP_EMP:
			animation_clip = CLIP_EMP;
			state = STATE_IDLE;
			break;
		}
	}
	animation_tick += elapsed_time * 1.0f;
}

void avatar::update(std::deque<COMMANDS>& command_queue, float speed_scale, float turning_scale, float elapsed_time)
{
	XMMATRIX S, R, T;

	XMVECTOR X, Y, Z;
	Y = XMVectorSet(0, 1, 0, 0);
	Z = XMVector3Normalize(XMLoadFloat4(&direction));
	X = XMVector3Cross(Y, Z);
	Y = XMVector3Normalize(XMVector3Cross(Z, X));

	float speed = 0;
	float turning = 0;
	for (COMMANDS command : command_queue)
	{
		switch (command)
		{
		case COMMANDS::IDLE:
			break;
		case COMMANDS::RUN:
			speed = max_speed * speed_scale;
			break;
		case COMMANDS::ATTACK:
			break;
		case COMMANDS::JUMP:
			break;
		case COMMANDS::TURN_LEFT:
		case COMMANDS::TURN_RIGHT:
			turning = max_turning * turning_scale;
			break;
		case COMMANDS::DEATH:
			break;
		}
	}

	R = XMMatrixRotationAxis(Y, turning * 0.01745f * elapsed_time);
	Z = XMVector4Transform(Z, R);
	XMStoreFloat4(&direction, Z);

	velocity.x = speed * direction.x;
	velocity.z = speed * direction.z;
	//velocity.y -= 9.8f * elapsed_time;
	velocity.y -= 0 * elapsed_time;

	position.x += velocity.x * elapsed_time;
	position.y += velocity.y * elapsed_time;
	position.z += velocity.z * elapsed_time;

	R = XMMatrixIdentity();
	R.r[0] = X;
	R.r[1] = Y;
	R.r[2] = Z;
	S = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	T = XMMatrixTranslation(position.x, position.y, position.z);
	XMStoreFloat4x4(&world_transform, S * R * T);

	animate(elapsed_time);

	last_state = state;
}
//void avatar::render(ID3D11DeviceContext* immediate_context)
//{
//	std::string current_class_instance;
//	std::string previous_class_instance;
//
//	rasterizer_states[0]->activate(immediate_context);
//	immediate_context->VSSetShader(vertex_shader.Get(), 0, 0);
//	immediate_context->IASetInputLayout(input_layout.Get());
//
//	size_t number_of_animations = model->animation_takes.size();
//	for (descartes::substance<vf::skinned_mesh>::mesh& mesh : model->meshes)
//	{
//		UINT stride = sizeof(vf::skinned_mesh);
//		UINT offset = 0;
//		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
//		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		if (number_of_animations > 0)
//		{
//			//const descartes::animation *animation = mesh.find_animation(substance->animation_takes.at(animation_clip).name.c_str());
//			const descartes::animation& animation = model->animations.at(animation_clip);
//			const descartes::skeletal& skeletal_transform = animation.skeletal_animation.at(animation_frame);
//
//			size_t number_of_bones = mesh.bind_bones.size();
//			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'number_of_bones' exceeds MAX_BONES.");
//			size_t index_of_bone = 0;
//			for (const descartes::bind_bone& bind_bone : mesh.bind_bones)
//			{
//				const descartes::bone* bone = skeletal_transform.find(bind_bone.name.c_str());
//				_ASSERT_EXPR(bone, L"Not found the bone");
//
//				XMStoreFloat4x4(&constant_buffer->data.bone_transforms[index_of_bone++],
//					XMMatrixTranspose(
//						XMLoadFloat4x4(&bind_bone.offset_matrix)
//						* XMLoadFloat4x4(&bone->transform)
//					)
//				);
//			}
//			XMStoreFloat4x4
//			(
//				&constant_buffer->data.world,
//				XMMatrixTranspose
//				(
//					XMLoadFloat4x4(&model->system_unit_transform) *
//					XMLoadFloat4x4(&model->axis_system_transform) *
//					XMLoadFloat4x4(&world_transform)
//				)
//			);
//		}
//		else
//		{
//			XMStoreFloat4x4
//			(
//				&constant_buffer->data.world,
//				XMMatrixTranspose
//				(
//					//XMLoadFloat4x4(&mesh.global_transform) *
//					XMLoadFloat4x4(&model->system_unit_transform) *
//					XMLoadFloat4x4(&model->axis_system_transform) *
//					XMLoadFloat4x4(&world_transform)
//				)
//			);
//			for (size_t index_of_bone = 0; index_of_bone < MAX_BONES; ++index_of_bone)
//			{
//				constant_buffer->data.bone_transforms[index_of_bone] = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
//			}
//		}
//		constant_buffer->activate(immediate_context, 0, true, false);
//		std::function<void(const descartes::subset&)> drawcall = [&](const descartes::subset& subset)
//		{
//			const descartes::material* material = model->find_material(subset.material_name.c_str());
//			if (material->two_sided)
//			{
//				rasterizer_states[1]->activate(immediate_context);
//			}
//			for (UINT index_of_property = 0; index_of_property < 3; ++index_of_property)
//			{
//				const descartes::texture* texture = model->find_texture(material->properties.at(index_of_property).texture_keyname.c_str());
//				immediate_context->PSSetShaderResources(index_of_property, 1, texture->shader_resource_view.GetAddressOf());
//			}
//			immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
//			if (material->two_sided)
//			{
//				rasterizer_states[1]->deactivate(immediate_context);
//			}
//		};
//		for (descartes::subset& subset : mesh.subsets)
//		{
//			if (subset.invisible)
//			{
//				continue;
//			}
//#if 0
//			//immediate_context->PSSetShader(pixel_shader.Get(), class_instances.at("default_material_assembler").GetAddressOf(), 1);
//			immediate_context->PSSetShader(pixel_shader.Get(), 0, 0);
//#else
//			std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ClassInstance>>::const_iterator iterator = class_instances.find(subset.material_name);
//
//			if (iterator == class_instances.end())
//			{
//				current_class_instance = "default_material_assembler";
//			}
//			else
//			{
//				current_class_instance = iterator->first;
//			}
//			if (previous_class_instance != current_class_instance)
//			{
//				previous_class_instance = current_class_instance;
//				immediate_context->PSSetShader(pixel_shader.Get(), class_instances.at(current_class_instance).GetAddressOf(), 1);
//			}
//#endif
//			drawcall(subset);
//			immediate_context->Flush();
//		}
//		constant_buffer->deactivate(immediate_context);
//	}
//	rasterizer_states[0]->deactivate(immediate_context);
//}
void avatar::render(ID3D11DeviceContext* immediate_context)
{
	rasterizer_states[0]->activate(immediate_context);
	immediate_context->VSSetShader(vertex_shader.Get(), 0, 0);
	immediate_context->IASetInputLayout(input_layout.Get());
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const descartes::animation& animation = model->animations.at(animation_clip);
	const descartes::skeletal& skeletal_transform = animation.skeletal_animation.at(animation_frame);

	for (const descartes::substance<vf::skinned_mesh>::mesh& mesh : model->meshes)
	{

		if (state == STATE_AIM)
		{
			enum { CENTRE, DOWN, UP, LEFT, DOWN_LEFT, UP_LEFT, RIGHT, DOWN_RIGHT, UP_RIGHT };
			const descartes::animation* animations[]
			{
				&model->animations.at(CLIP_AIM_CC),
				&model->animations.at(CLIP_AIM_D),
				&model->animations.at(CLIP_AIM_U),
				&model->animations.at(CLIP_AIM_L),
				&model->animations.at(CLIP_AIM_LD),
				&model->animations.at(CLIP_AIM_LU),
				&model->animations.at(CLIP_AIM_R),
				&model->animations.at(CLIP_AIM_RD),
				&model->animations.at(CLIP_AIM_RU),
			};

			size_t number_of_bones = mesh.bind_bones.size();
			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");

			const std::vector<descartes::bone>* skeletals[2] = {};
			std::vector<descartes::bone>* blended_skeletal;

			std::vector<descartes::bone> blended_skeletals[3];
			blended_skeletals[0].resize(number_of_bones);
			blended_skeletals[1].resize(number_of_bones);
			blended_skeletals[2].resize(number_of_bones);

			size_t aims[2];

			aims[0] = CENTRE;
			aims[1] = vertical_factor > 0 ? UP : DOWN;
			vertical_factor = fabsf(vertical_factor);
			skeletals[0] = &animations[aims[0]]->skeletal_animation.at(0).bones;
			skeletals[1] = &animations[aims[1]]->skeletal_animation.at(0).bones;
			blended_skeletal = &blended_skeletals[0];
			blend_skeletals(&model->scene_graph, XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), skeletals, vertical_factor, blended_skeletal);

			aims[0] = horizontal_factor > 0 ? RIGHT : LEFT;
			aims[1] = aims[1] == UP ? (aims[0] == RIGHT ? UP_RIGHT : UP_LEFT) : (aims[0] == RIGHT ? DOWN_RIGHT : DOWN_LEFT);
			horizontal_factor = fabsf(horizontal_factor);
			skeletals[0] = &animations[aims[0]]->skeletal_animation.at(0).bones;
			skeletals[1] = &animations[aims[1]]->skeletal_animation.at(0).bones;
			blended_skeletal = &blended_skeletals[1];
			blend_skeletals(&model->scene_graph, XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), skeletals, vertical_factor, blended_skeletal);

			skeletals[0] = &blended_skeletals[0];
			skeletals[1] = &blended_skeletals[1];
			blended_skeletal = &blended_skeletals[2];
			blend_skeletals(&model->scene_graph, XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), skeletals, horizontal_factor, blended_skeletal);
			for (size_t index_of_bone = 0; index_of_bone < number_of_bones; ++index_of_bone)
			{
				_ASSERT_EXPR(mesh.bind_bones.at(index_of_bone).name == blended_skeletal->at(index_of_bone).name, L"The names of the two bones are not the same");
				XMStoreFloat4x4
					(
						&constant_buffer->data.bone_transforms[index_of_bone],
						XMMatrixTranspose
						(
							XMLoadFloat4x4(&mesh.bind_bones.at(index_of_bone).offset_matrix) *
							XMLoadFloat4x4(&blended_skeletal->at(index_of_bone).transform)
							)
						);
			}
		}
		else
		{
			size_t number_of_animations = model->animation_takes.size();
			assert(number_of_animations > 0);

			size_t number_of_bones = mesh.bind_bones.size();
			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'number_of_bones' exceeds MAX_BONES.");
			size_t index_of_bone = 0;
			for (const descartes::bind_bone& bind_bone : mesh.bind_bones)
			{
				const descartes::bone* bone = skeletal_transform.find(bind_bone.name.c_str());
				_ASSERT_EXPR(bone, L"Not found the bone");

				XMStoreFloat4x4
					(
						&constant_buffer->data.bone_transforms[index_of_bone++],
						XMMatrixTranspose
						(
							XMLoadFloat4x4(&bind_bone.offset_matrix) *
							XMLoadFloat4x4(&bone->transform)
							)
						);
			}
			XMStoreFloat4x4
				(
					&constant_buffer->data.world,
					XMMatrixTranspose
					(
						XMLoadFloat4x4(&model->system_unit_transform) *
						XMLoadFloat4x4(&model->axis_system_transform) *
						XMLoadFloat4x4(&world_transform)
						)
					);
		}

		constant_buffer->activate(immediate_context, 0, true, false);
		render(immediate_context, mesh);
		constant_buffer->deactivate(immediate_context);

	}
	rasterizer_states[0]->deactivate(immediate_context);
}
void avatar::render(ID3D11DeviceContext* immediate_context, const descartes::substance<vf::skinned_mesh>::mesh& mesh)
{
	UINT stride = sizeof(vf::skinned_mesh);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	std::function<void(const descartes::subset&)> drawcall = [&](const descartes::subset& subset)
	{
		const descartes::material* material = model->find_material(subset.material_name.c_str());
		if (material->two_sided)
		{
			rasterizer_states[1]->activate(immediate_context);
		}
		for (UINT index_of_property = 0; index_of_property < 3; ++index_of_property)
		{
			const descartes::texture* texture = model->find_texture(material->properties.at(index_of_property).texture_keyname.c_str());
			immediate_context->PSSetShaderResources(index_of_property, 1, texture->shader_resource_view.GetAddressOf());
		}
		immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		if (material->two_sided)
		{
			rasterizer_states[1]->deactivate(immediate_context);
		}
	};

	std::string current_class_instance;
	std::string previous_class_instance;
	for (const descartes::subset& subset : mesh.subsets)
	{
		if (subset.invisible)
		{
			continue;
		}
#if 0
		//immediate_context->PSSetShader(pixel_shader.Get(), class_instances.at("default_material_assembler").GetAddressOf(), 1);
		immediate_context->PSSetShader(pixel_shader.Get(), 0, 0);
#else
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ClassInstance>>::const_iterator iterator = class_instances.find(subset.material_name);

		if (iterator == class_instances.end())
		{
			current_class_instance = "default_material_assembler";
		}
		else
		{
			current_class_instance = iterator->first;
		}
		if (previous_class_instance != current_class_instance)
		{
			previous_class_instance = current_class_instance;
			immediate_context->PSSetShader(pixel_shader.Get(), class_instances.at(current_class_instance).GetAddressOf(), 1);
		}
#endif
		drawcall(subset);
		immediate_context->Flush();
	}
}
//void avatar::render(ID3D11DeviceContext* immediate_context, float vertical_factor /*-1 < vertical_factor < +1*/, float horizontal_factor /*-1 < horizontal_factor < +1*/)
//{
//	std::string current_class_instance;
//	std::string previous_class_instance;
//
//	rasterizer_states[0]->activate(immediate_context);
//	immediate_context->VSSetShader(vertex_shader.Get(), 0, 0);
//	immediate_context->IASetInputLayout(input_layout.Get());
//
//	size_t number_of_animations = model->animation_takes.size();
//	_ASSERT_EXPR(number_of_animations > 0, "Animation data is required for this process.");
//
//	for (descartes::substance<vf::skinned_mesh>::mesh& mesh : model->meshes)
//	{
//		UINT stride = sizeof(vf::skinned_mesh);
//		UINT offset = 0;
//		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
//		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		enum { CENTRE, DOWN, UP, LEFT, DOWN_LEFT, UP_LEFT, RIGHT, DOWN_RIGHT, UP_RIGHT };
//		const descartes::animation* animations[]
//		{
//			&model->animations.at(CLIP_AIM_CC),
//			&model->animations.at(CLIP_AIM_D),
//			&model->animations.at(CLIP_AIM_U),
//			&model->animations.at(CLIP_AIM_L),
//			&model->animations.at(CLIP_AIM_LD),
//			&model->animations.at(CLIP_AIM_LU),
//			&model->animations.at(CLIP_AIM_R),
//			&model->animations.at(CLIP_AIM_RD),
//			&model->animations.at(CLIP_AIM_RU),
//		};
//
//		size_t number_of_bones = mesh.bind_bones.size();
//		_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
//
//		const std::vector<descartes::bone>* skeletals[2] = {};
//		std::vector<descartes::bone>* blended_skeletal;
//
//		std::vector<descartes::bone> blended_skeletals[3];
//		blended_skeletals[0].resize(number_of_bones);
//		blended_skeletals[1].resize(number_of_bones);
//		blended_skeletals[2].resize(number_of_bones);
//
//		size_t aims[2];
//
//		aims[0] = CENTRE;
//		aims[1] = vertical_factor > 0 ? UP : DOWN;
//		vertical_factor = fabsf(vertical_factor);
//		skeletals[0] = &animations[aims[0]]->skeletal_animation.at(0).bones;
//		skeletals[1] = &animations[aims[1]]->skeletal_animation.at(0).bones;
//		blended_skeletal = &blended_skeletals[0];
//		blend_skeletals(&model->scene_graph, XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), skeletals, vertical_factor, blended_skeletal);
//
//		aims[0] = horizontal_factor > 0 ? RIGHT : LEFT;
//		aims[1] = aims[1] == UP ? (aims[0] == RIGHT ? UP_RIGHT : UP_LEFT) : (aims[0] == RIGHT ? DOWN_RIGHT : DOWN_LEFT);
//		horizontal_factor = fabsf(horizontal_factor);
//		skeletals[0] = &animations[aims[0]]->skeletal_animation.at(0).bones;
//		skeletals[1] = &animations[aims[1]]->skeletal_animation.at(0).bones;
//		blended_skeletal = &blended_skeletals[1];
//		blend_skeletals(&model->scene_graph, XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), skeletals, vertical_factor, blended_skeletal);
//
//		skeletals[0] = &blended_skeletals[0];
//		skeletals[1] = &blended_skeletals[1];
//		blended_skeletal = &blended_skeletals[2];
//		blend_skeletals(&model->scene_graph, XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1), skeletals, horizontal_factor, blended_skeletal);
//
//#if 0
//		size_t index_of_bone = 0;
//		for (const descartes::bind_bone& bind_bone : mesh.bind_bones)
//		{
//			const descartes::bone* bone = nullptr;
//			for (const descartes::bone& iterator : *blended_skeletal)
//			{
//				if (bind_bone.name == iterator.name)
//				{
//					bone = &iterator;
//					break;
//				}
//			}
//			_ASSERT_EXPR(bone, L"Not found the bone");
//			XMStoreFloat4x4(&constant_buffer->data.bone_transforms[index_of_bone++],
//				XMMatrixTranspose(
//					XMLoadFloat4x4(&bind_bone.offset_matrix)
//					* XMLoadFloat4x4(&bone->transform)
//				)
//			);
//		}
//#else
//		for (size_t index_of_bone = 0; index_of_bone < number_of_bones; ++index_of_bone)
//		{
//			_ASSERT_EXPR(mesh.bind_bones.at(index_of_bone).name == blended_skeletal->at(index_of_bone).name, L"The names of the two bones are not the same");
//			XMStoreFloat4x4
//			(
//				&constant_buffer->data.bone_transforms[index_of_bone],
//				XMMatrixTranspose
//				(
//					XMLoadFloat4x4(&mesh.bind_bones.at(index_of_bone).offset_matrix) *
//					XMLoadFloat4x4(&blended_skeletal->at(index_of_bone).transform)
//				)
//			);
//		}
//#endif
//
//		XMStoreFloat4x4
//		(
//			&constant_buffer->data.world,
//			XMMatrixTranspose
//			(
//				XMLoadFloat4x4(&model->system_unit_transform) *
//				XMLoadFloat4x4(&model->axis_system_transform) *
//				XMLoadFloat4x4(&world_transform)
//			)
//		);
//
//		constant_buffer->activate(immediate_context, 0, true, false);
//		std::function<void(const descartes::subset&)> drawcall = [&](const descartes::subset& subset)
//		{
//			const descartes::material* material = model->find_material(subset.material_name.c_str());
//			if (material->two_sided)
//			{
//				rasterizer_states[1]->activate(immediate_context);
//			}
//			for (UINT index_of_property = 0; index_of_property < 3; ++index_of_property)
//			{
//				const descartes::texture* texture = model->find_texture(material->properties.at(index_of_property).texture_keyname.c_str());
//				immediate_context->PSSetShaderResources(index_of_property, 1, texture->shader_resource_view.GetAddressOf());
//			}
//			immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
//			if (material->two_sided)
//			{
//				rasterizer_states[1]->deactivate(immediate_context);
//			}
//		};
//		for (descartes::subset& subset : mesh.subsets)
//		{
//			if (subset.invisible)
//			{
//				continue;
//			}
//#if 0
//			//immediate_context->PSSetShader(pixel_shader.Get(), class_instances.at("default_material_assembler").GetAddressOf(), 1);
//			immediate_context->PSSetShader(pixel_shader.Get(), 0, 0);
//#else
//			std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ClassInstance>>::const_iterator iterator = class_instances.find(subset.material_name);
//
//			if (iterator == class_instances.end())
//			{
//				current_class_instance = "default_material_assembler";
//			}
//			else
//			{
//				current_class_instance = iterator->first;
//			}
//			if (previous_class_instance != current_class_instance)
//			{
//				previous_class_instance = current_class_instance;
//				immediate_context->PSSetShader(pixel_shader.Get(), class_instances.at(current_class_instance).GetAddressOf(), 1);
//			}
//#endif
//			drawcall(subset);
//			immediate_context->Flush();
//		}
//		constant_buffer->deactivate(immediate_context);
//	}
//	rasterizer_states[0]->deactivate(immediate_context);
//}
void avatar::shadowcast(ID3D11DeviceContext* immediate_context)
{
	immediate_context->VSSetShader(vertex_shader.Get(), 0, 0);
	immediate_context->IASetInputLayout(input_layout.Get());

	size_t number_of_animations = model->animation_takes.size();
	for (descartes::substance<vf::skinned_mesh>::mesh& mesh : model->meshes)
	{
		UINT stride = sizeof(vf::skinned_mesh);
		UINT offset = 0;
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (number_of_animations > 0)
		{
			//const descartes::animation *animation = mesh.find_animation(substance->animation_takes.at(animation_clip).name.c_str());
			const descartes::animation& animation = model->animations.at(animation_clip);
			const descartes::skeletal& skeletal_transform = animation.skeletal_animation.at(animation_frame);

			size_t number_of_bones = mesh.bind_bones.size();
			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'number_of_bones' exceeds MAX_BONES.");
			size_t index_of_bone = 0;
			for (const descartes::bind_bone& bind_bone : mesh.bind_bones)
			{
				const descartes::bone* bone = skeletal_transform.find(bind_bone.name.c_str());
				_ASSERT_EXPR(bone, L"Not found the bone");

				XMStoreFloat4x4(&constant_buffer->data.bone_transforms[index_of_bone++],
					XMMatrixTranspose(
						XMLoadFloat4x4(&bind_bone.offset_matrix)
						* XMLoadFloat4x4(&bone->transform)
						)
					);
			}
			XMStoreFloat4x4
				(
					&constant_buffer->data.world,
					XMMatrixTranspose
					(
						XMLoadFloat4x4(&model->system_unit_transform) *
						XMLoadFloat4x4(&model->axis_system_transform) *
						XMLoadFloat4x4(&world_transform)
						)
					);
		}
		else
		{
			XMStoreFloat4x4
				(
					&constant_buffer->data.world,
					XMMatrixTranspose
					(
						//XMLoadFloat4x4(&mesh.global_transform) *
						XMLoadFloat4x4(&model->system_unit_transform) *
						XMLoadFloat4x4(&model->axis_system_transform) *
						XMLoadFloat4x4(&world_transform)
						)
					);
			for (size_t index_of_bone = 0; index_of_bone < MAX_BONES; ++index_of_bone)
			{
				constant_buffer->data.bone_transforms[index_of_bone] = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
			}
		}
		constant_buffer->activate(immediate_context, 0, true, false);

		for (descartes::subset& subset : mesh.subsets)
		{
			if (subset.invisible)
			{
				continue;
			}
			immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
		constant_buffer->deactivate(immediate_context);
	}
}