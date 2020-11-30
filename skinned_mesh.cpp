#include <Shlwapi.h>

#include "utilities.h"
#include "shader_ex.h"
#include "texture_ex.h"

#include "skinned_mesh.h"

using namespace DirectX;

skinned_mesh::skinned_mesh(ID3D11Device* device, const char* filename, int animation_sampling_rate) :
	descartes::substance<vf::skinned_mesh>(filename, false/*no triangulate*/, animation_sampling_rate)
{
	create_render_objects(device, true);

	char media_directory[256];
	strcpy_s(media_directory, filename);
	PathFindFileNameA(media_directory)[0] = '\0';
	create_shader_resource_views(device, media_directory);

	constant_buffer = std::make_unique<descartes::constant_buffer<shader_constants>>(device);
}
skinned_mesh::skinned_mesh(ID3D11Device* device, const char* filename, const std::vector<std::string>& animation_filenames, int animation_sampling_rate) :
	descartes::substance<vf::skinned_mesh>(filename, false/*no triangulate*/, animation_filenames, animation_sampling_rate)

{
	create_render_objects(device, true);

	char media_directory[256];
	strcpy_s(media_directory, filename);
	PathFindFileNameA(media_directory)[0] = '\0';
	create_shader_resource_views(device, media_directory);

	constant_buffer = std::make_unique<descartes::constant_buffer<shader_constants>>(device);
}
void skinned_mesh::render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, size_t animation_clip, size_t animation_frame)
{
	size_t number_of_animations = animation_takes.size();
	for (descartes::substance<vf::skinned_mesh>::mesh& mesh : meshes)
	{
		UINT stride = sizeof(vf::skinned_mesh);
		UINT offset = 0;

		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (number_of_animations > 0)
		{
			//const descartes::animation *animation = mesh.find_animation(substance->animation_takes.at(animation_clip).name.c_str());
			const descartes::animation& animation = animations.at(animation_clip);
			const descartes::skeletal& skeletal_transform = animation.skeletal_animation.at(animation_frame);

			size_t number_of_bones = mesh.bind_bones.size();
			_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'number_of_bones' exceeds MAX_BONES.");
			size_t cb_slot = 0;
			for (const descartes::bind_bone & bind_bone : mesh.bind_bones)
			{
				const descartes::bone* bone = skeletal_transform.find(bind_bone.name.c_str());
				_ASSERT_EXPR(bone, L"Not found the bone");

				XMStoreFloat4x4(&constant_buffer->data.bone_transforms[cb_slot++],
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
						XMLoadFloat4x4(&system_unit_transform) *
						XMLoadFloat4x4(&axis_system_transform) *
						XMLoadFloat4x4(&world)
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
						XMLoadFloat4x4(&mesh.global_transform) *
						XMLoadFloat4x4(&system_unit_transform) *
						XMLoadFloat4x4(&axis_system_transform) *
						XMLoadFloat4x4(&world)
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
			const descartes::material* material = find_material(subset.material_name.c_str());
			for (UINT index_of_property = 0; index_of_property < 3; ++index_of_property)
			{
				const descartes::texture* texture = find_texture(material->properties.at(index_of_property).texture_keyname.c_str());
				if (texture)
				{
					immediate_context->PSSetShaderResources(index_of_property, 1, texture->shader_resource_view.GetAddressOf());
				}
			}
			immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
		constant_buffer->deactivate(immediate_context);
	}
}
