#include "mesh.h"
#include "framework.h"
#include "input_data.h"


#include "geometric_primitive.h"
#include "static_mesh_ex.h"

/*******************************************************************************
���b�V��������
*******************************************************************************/
void	my_mesh::initialize()
{
	//	��񏉊��l
	pos = DirectX::XMFLOAT3(0, 0, 0);
	angle = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);

	up = { 0.0f,1.0f,0.0f };
	right = { 1.0f,0.0f,0.0f };
	forward = { 0.0f,0.0f,1.0f };

	orientation = { 0.0f,0.0f,0.0f,1.0f };

	color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	anim_timer = 0.0f;
	isAnim = false;

	//	���f���ǂݍ���
	_model = nullptr;
	//model_renderer = nullptr;

	anim_index = -1;
}

/*******************************************************************************
���b�V�����f���ǂݍ���
����
fbx_filename:���b�V���t�@�C����(FBX�`��)
*******************************************************************************/
bool my_mesh::load(const char* fbx_filename)
{
	//	���f���ǂݍ���
	std::unique_ptr<ModelData> model_data = std::make_unique<ModelData>();
	BinaryInput(fbx_filename, *model_data);

	for (auto& meshes : model_data->meshes)
	{
		for (auto& v : meshes.vertices)
		{
			boundingBox.max.x = std::fmax(v.position.x, boundingBox.max.x);
			boundingBox.max.y = std::fmax(v.position.y, boundingBox.max.y);
			boundingBox.max.z = std::fmax(v.position.z, boundingBox.max.z);
			boundingBox.min.x = std::fmin(v.position.x, boundingBox.min.x);
			boundingBox.min.y = std::fmin(v.position.y, boundingBox.min.y);
			boundingBox.min.z = std::fmin(v.position.z, boundingBox.min.z);
		}
	}

	std::shared_ptr<model_resource> _model_resource = std::make_shared<model_resource>(FRAMEWORK.getDevice(), std::move(model_data));
	_model = std::make_unique<model>(_model_resource);

	bLoad = true;
	return	true;
}

//int my_mesh::AnimationInit(bool loop)
//{
//	model->PlayAnimation(0, loop);
//	isAnim = true;
//
//	anim_index = 0;
//
//	return anim_index;
//}

void my_mesh::setMaterialTex(const int index, const wchar_t* filename)
{
	_model->setMaterialTex(FRAMEWORK.getDevice(), index, filename);
}

void my_mesh::setAllMaterialTex(const wchar_t* filename)
{
	for (int i = -1; i < _model->getMaterialNum(); i++) {
		_model->setMaterialTex(FRAMEWORK.getDevice(), i, filename);
	}
}

/*******************************************************************************
���b�V�����
*******************************************************************************/
void	my_mesh::release()
{
	if (_model && bLoad)
	{
		_model.release();
	}
	_model = nullptr;
	//model_renderer = nullptr;
	bLoad = false;
}

/*******************************************************************************
���b�V���`��
����
view:�r���[�ϊ��s��
projection:���e�ϊ��s��
light_dir:���s���x�N�g��
*******************************************************************************/
void my_mesh::render(shader_ex *shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir, const bool& clock_wise, float tex_timer, float anim_timer)
{
	//���f����������Ε`�揈������؍s��Ȃ�
	if (!_model)		return;

	//	���[���h�ϊ��s��擾
	//DirectX::XMFLOAT4X4 W = GetWorldMatrix();

	//	Matrix -> Float4x4 �ϊ�
	DirectX::XMFLOAT4X4 view_projection;
	DirectX::XMFLOAT4X4 world = getWorldMatrix();
	/*DirectX::XMStoreFloat4x4(&view_projection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));*/

	DirectX::XMStoreFloat4x4(&view_projection, DirectX::XMLoadFloat4x4(&m_camera->getView()) * DirectX::XMLoadFloat4x4(&m_camera->getProjection()));
	//DirectX::XMStoreFloat4x4(&world, W);

	//	�`��
	//if (isAnim)
	//	_model->updateAnimation(anim_timer);

	_model->calculateLocalTransform();
	_model->calculateWorldTransform(world);

	static std::unique_ptr< model_renderer > mdl_renderer = std::make_unique<model_renderer>(FRAMEWORK.getDevice());

	mdl_renderer->cullNone = isCullNone;

	mdl_renderer->begin(FRAMEWORK.getImmediateContext(), shader, world, view_projection, light_dir, DirectX::XMFLOAT4(m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z,1.0f), clock_wise);
	mdl_renderer->draw(FRAMEWORK.getImmediateContext(), shader,*_model, color, tex_timer);
	mdl_renderer->end(FRAMEWORK.getImmediateContext());

	shader->inactivate();
}

void my_mesh::bRender(shader_ex *shader, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& camera_pos, const bool& clock_wise, float tex_timer, float anim_timer)
{
	//	���f����������Ε`�揈������؍s��Ȃ�
	if (!_model)		return;

	DirectX::XMFLOAT4X4	view2 = view;
	DirectX::XMMATRIX	inv_view2;

	//	���ʂ̍s��Z�o(����view�t�s��AViewProjection�s��)
	//DirectX::XMStoreFloat4x4(&view2, view);						//	��U4x4�ɕϊ�
	view2._41 = 0.0f; view2._42 = 0.0f; 						//	view�̈ʒu��񂾂����폜
	view2._43 = 0.0f; view2._44 = 1.0f;							//	
	inv_view2 = DirectX::XMLoadFloat4x4(&view2);				//	Matrix�֍ĕϊ�
	inv_view2 = DirectX::XMMatrixInverse(nullptr, inv_view2);	//	view�s��̋t�s��

	DirectX::XMMATRIX	VP;
	VP = DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection);									//	���e�s�񍇐�

																												//	���[���h�ϊ��s��擾
	DirectX::XMFLOAT4X4 world = getWorldMatrix();

	//	Matrix -> Float4x4 �ϊ�
	DirectX::XMFLOAT4X4 view_projection;

	DirectX::XMMATRIX wvp = inv_view2 * VP;

	DirectX::XMStoreFloat4x4(&view_projection, wvp);

	//	�`��
	//if (isAnim)
	//	model->UpdateAnimation(anim_timer);

	_model->calculateLocalTransform();
	_model->calculateWorldTransform(world);

	static std::unique_ptr< model_renderer > mdl_renderer = std::make_unique<model_renderer>(FRAMEWORK.getDevice());


	mdl_renderer->begin(FRAMEWORK.getImmediateContext(), shader, world, view_projection, light_dir, camera_pos, clock_wise);
	mdl_renderer->draw(FRAMEWORK.getImmediateContext(), shader,*_model, color, tex_timer);
	mdl_renderer->end(FRAMEWORK.getImmediateContext());
}

void my_mesh::render(shader *shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir,  const bool& clock_wise, float tex_timer, float anim_timer)
{
	//	���f����������Ε`�揈������؍s��Ȃ�
	if (!_model)		return;

	//	���[���h�ϊ��s��擾
	//DirectX::XMFLOAT4X4 W = GetWorldMatrix();

	//	Matrix -> Float4x4 �ϊ�
	DirectX::XMFLOAT4X4 view_projection;
	DirectX::XMFLOAT4X4 world = getWorldMatrix();
	DirectX::XMStoreFloat4x4(&view_projection, DirectX::XMLoadFloat4x4(&m_camera->getView()) * DirectX::XMLoadFloat4x4(&m_camera->getProjection()));
	//DirectX::XMStoreFloat4x4(&world, W);

	//	�`��
	//if (isAnim)
	//	model->UpdateAnimation(anim_timer);

	_model->calculateLocalTransform();
	_model->calculateWorldTransform(world);

	static std::unique_ptr< model_renderer > mdl_renderer = std::make_unique<model_renderer>(FRAMEWORK.getDevice());

	mdl_renderer->cullNone = isCullNone;

	mdl_renderer->begin(FRAMEWORK.getImmediateContext(), shader, world, view_projection, light_dir, DirectX::XMFLOAT4(m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z, 1.0f), clock_wise);
	mdl_renderer->draw(FRAMEWORK.getImmediateContext(), *_model, color, tex_timer);
	mdl_renderer->end(FRAMEWORK.getImmediateContext());

	//shader->Inactivate();
}

//int my_mesh::AddAnimation(const char* fbx_filename)
//{
//	std::unique_ptr<ModelData> anim_data = std::make_unique<ModelData>();
//
//	BinaryInput(fbx_filename, *anim_data);
//
//	model->GetModelResource()->GetAnimations().push_back(anim_data->animations.at(0));
//
//	anim_index++;
//	return anim_index;
//}

//void my_mesh::PlayAnimation(int animation_index, bool loop)
//{
//	model->PlayAnimation(animation_index, loop);
//}

/*******************************************************************************
���b�V����񂩂烏�[���h�ϊ��s��擾
*******************************************************************************/
DirectX::XMFLOAT4X4	my_mesh::getWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, T;

	//	������
	world = DirectX::XMMatrixIdentity();

	//	�g��E�k��
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	��]
	DirectX::XMMATRIX Rx, Ry, Rz;
	Rx = DirectX::XMMatrixRotationX(angle.x);				//	X������Ƃ�����]�s��
	Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y������Ƃ�����]�s��
	Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z������Ƃ�����]�s��
	R = Rz * Ry * Rx;
	//R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orientation));

	//	���s�ړ�
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	���[���h�ϊ��s��
	world = S * R * T;

	DirectX::XMFLOAT4X4 world_matrix = {};

	DirectX::XMStoreFloat4x4(&world_matrix, world);
	return	world_matrix;
}

DirectX::XMFLOAT3& my_mesh::getBonePos(const char* bone_name)
{
	DirectX::XMMATRIX _m;
	DirectX::XMMATRIX local_bone_m = DirectX::XMLoadFloat4x4(&_model->getBoneLocalMatrix(bone_name));
	//DirectX::XMMATRIX world_m = DirectX::XMLoadFloat4x4(&GetWorldMatrix());

	_m = local_bone_m;

	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, _m);

	return DirectX::XMFLOAT3(m._41, m._42, m._43);
}

DirectX::XMFLOAT3& my_mesh::getBoneDirection(const char* bone_name)
{
	//forward��Ԃ�
	return getBoneDirectionZ(bone_name);
}

DirectX::XMFLOAT3& my_mesh::getBoneDirectionX(const char* bone_name)
{
	DirectX::XMMATRIX _m;
	DirectX::XMMATRIX local_bone_m = DirectX::XMLoadFloat4x4(&_model->getBoneLocalMatrix(bone_name));
	//DirectX::XMMATRIX world_m = DirectX::XMLoadFloat4x4(&GetWorldMatrix());

	_m = local_bone_m;

	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, _m);

	DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(m._11, m._12, m._13);

	//right��Ԃ�
	return Vec3Normalize(right);
}

DirectX::XMFLOAT3& my_mesh::getBoneDirectionY(const char* bone_name)
{
	DirectX::XMMATRIX _m;
	DirectX::XMMATRIX local_bone_m = DirectX::XMLoadFloat4x4(&_model->getBoneLocalMatrix(bone_name));
	//DirectX::XMMATRIX world_m = DirectX::XMLoadFloat4x4(&GetWorldMatrix());

	_m = local_bone_m;

	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, _m);

	DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(m._21, m._22, m._23);
	//up��Ԃ�
	return Vec3Normalize(up);
}

DirectX::XMFLOAT3& my_mesh::getBoneDirectionZ(const char* bone_name)
{
	DirectX::XMMATRIX _m;
	DirectX::XMMATRIX local_bone_m = DirectX::XMLoadFloat4x4(&_model->getBoneLocalMatrix(bone_name));
	//DirectX::XMMATRIX world_m = DirectX::XMLoadFloat4x4(&GetWorldMatrix());

	_m = local_bone_m;

	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, _m);

	DirectX::XMFLOAT3 forward = DirectX::XMFLOAT3(m._31, m._32, m._33);
	//forward��Ԃ�
	return Vec3Normalize(forward);
}

void my_mesh::focusMove(camera *cam)
{
	//x,z���ʂɂ����Čv�Z����
	//�O��
	DirectX::XMFLOAT2 cam_to_obj = DirectX::XMFLOAT2(pos.x - cam->getEye().x, pos.z - cam->getEye().z);
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat2(&cam_to_obj);
	vec = DirectX::XMVector2Normalize(vec);
	DirectX::XMStoreFloat2(&cam_to_obj, vec);
	//���E
	DirectX::XMFLOAT3 cam_to_obj3 = DirectX::XMFLOAT3(pos.x - cam->getEye().x, pos.y - cam->getEye().y, pos.z - cam->getEye().z);
	DirectX::XMVECTOR vec3 = DirectX::XMLoadFloat3(&cam_to_obj3);
	vec3 = DirectX::XMVector3Normalize(vec3);
	DirectX::XMVECTOR vec_up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0));
	vec_up = DirectX::XMVector3Normalize(vec_up);
	DirectX::XMVECTOR vec_cross = DirectX::XMVector3Cross(vec_up, vec3);
	DirectX::XMStoreFloat3(&cam_to_obj3, vec_cross);

	if (GetAsyncKeyState('W') < 0)
		pos = DirectX::XMFLOAT3(pos.x + cam_to_obj.x / 5.0f, pos.y, pos.z + cam_to_obj.y / 5.0f);
	if (GetAsyncKeyState('S') < 0)
		pos = DirectX::XMFLOAT3(pos.x - cam_to_obj.x / 5.0f, pos.y, pos.z - cam_to_obj.y / 5.0f);
	if (GetAsyncKeyState('A') < 0)
		pos = DirectX::XMFLOAT3(pos.x - cam_to_obj3.x / 5.0f, pos.y, pos.z - cam_to_obj3.z / 5.0f);
	if (GetAsyncKeyState('D') < 0)
		pos = DirectX::XMFLOAT3(pos.x + cam_to_obj3.x / 5.0f, pos.y, pos.z + cam_to_obj3.z / 5.0f);
	if (GetAsyncKeyState('Q') < 0)
		pos = DirectX::XMFLOAT3(pos.x, pos.y + 0.2f, pos.z);
	if (GetAsyncKeyState('E') < 0)
		pos = DirectX::XMFLOAT3(pos.x, pos.y - 0.2f, pos.z);

	//DirectX::XMFLOAT3 dimensions(1, 1, 1);

	//static DirectX::XMFLOAT3 angles(0, 0, 0);
	if (GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState('M') < 0)
	{
		if (GetAsyncKeyState(VK_UP) < 0)
			angle.x += DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_DOWN) < 0)
			angle.x -= DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_LEFT) < 0)
			angle.y += DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_RIGHT) < 0)
			angle.y -= DirectX::XM_PI / 180.0f * 10;
		//if (GetAsyncKeyState('UP') & 1)
		angle.z = 0;
	}
}








/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void geometric_mesh::initialize()
{
	//	��񏉊��l
	pos = DirectX::XMFLOAT3(0, 0, 0);
	angle = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);


	color = DirectX::XMFLOAT4(1, 1, 1, 1);

	//	���f���ǂݍ���
	obj = nullptr;
}

void geometric_mesh::setPrimitive(geometric_primitive* primitive)
{
	obj = primitive;
	bLoad = true;
}

/*******************************************************************************
���b�V�����f���ݒ�(�g���񂵌^)
����
primitive:���b�V�����f��
*******************************************************************************/
void geometric_mesh::setMesh(geometric_mesh& org)
{
	*this = org;
	bLoad = false;
}

/*******************************************************************************
���b�V�����
*******************************************************************************/
void geometric_mesh::release()
{
	if (obj && bLoad)
	{
		delete	obj;
	}
	obj = nullptr;
	bLoad = false;
}

/*******************************************************************************
���b�V���`��
����
view:�r���[�ϊ��s��
projection:���e�ϊ��s��
light_dir:���s���x�N�g��
*******************************************************************************/
void geometric_mesh::render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& collision_color, const float& _scale, float timer)
{
	//	���f����������Ε`�揈������؍s��Ȃ�
	if (!obj)		return;

	scale = Vec3Multiply({ 1.0f,1.0f,1.0f }, _scale);

	//	Matrix -> Float4x4 �ϊ�
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMFLOAT4X4 world = getWorldMatrix();
	DirectX::XMStoreFloat4x4(&world_view_projection, DirectX::XMLoadFloat4x4(&world) * DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));

	color = collision_color;

	//	�`��
	obj->render(FRAMEWORK.getImmediateContext(), world_view_projection, world, light_dir, color, false);
}


/*******************************************************************************
���b�V����񂩂烏�[���h�ϊ��s��擾
*******************************************************************************/
DirectX::XMFLOAT4X4 geometric_mesh::getWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	������
	world = DirectX::XMMatrixIdentity();

	//	�g��E�k��
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	��]
	Rx = DirectX::XMMatrixRotationX(angle.x);				//	X������Ƃ�����]�s��
	Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y������Ƃ�����]�s��
	Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z������Ƃ�����]�s��
	R = Rz * Ry * Rx;

	//	���s�ړ�
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	���[���h�ϊ��s��
	world = S * R * T;


	DirectX::XMFLOAT4X4 _world;

	DirectX::XMStoreFloat4x4(&_world, world);

	return	_world;
}

/*******************************************************************************
���b�V��������
*******************************************************************************/
void my_static_mesh::initialize()
{
	//	��񏉊��l
	pos = DirectX::XMFLOAT3(0, 0, 0);
	angle = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);

	up = { 0.0f,1.0f,0.0f };
	right = { 1.0f,0.0f,0.0f };
	forward = { 0.0f,0.0f,1.0f };

	orientation = { 0.0f,0.0f,0.0f,1.0f };

	color = DirectX::XMFLOAT4(1, 1, 1, 1);

	//	���f���ǂݍ���
	obj = nullptr;
}

/*******************************************************************************
���b�V�����f���ǂݍ���
����
fbx_filename:���b�V���t�@�C����(FBX�`��)
*******************************************************************************/
bool my_static_mesh::load(const wchar_t* obj_filename)
{
	//	���f���ǂݍ���
	obj = new s_mesh(FRAMEWORK.getDevice(), obj_filename);
	bLoad = true;

	return	true;
}

/*******************************************************************************
���b�V�����
*******************************************************************************/
void my_static_mesh::release()
{
	if (obj && bLoad)
	{
		delete	obj;
	}
	obj = nullptr;
	bLoad = false;
}

/*******************************************************************************
���b�V���`��
����
view:�r���[�ϊ��s��
projection:���e�ϊ��s��
light_dir:���s���x�N�g��
*******************************************************************************/
void my_static_mesh::render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& camera_pos, const float timer)
{
	//	���f����������Ε`�揈������؍s��Ȃ�
	if (!obj)		return;

	//	���[���h�ϊ��s��擾
	DirectX::XMFLOAT4X4 world = getWorldMatrix();

	//	Matrix -> Float4x4 �ϊ�
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMStoreFloat4x4(&world_view_projection, DirectX::XMLoadFloat4x4(&world) * DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));


	//	�`��
	obj->render(FRAMEWORK.getImmediateContext(), world_view_projection, world, light_dir, color, camera_pos, timer, false);
}

void my_static_mesh::setOrientation(const DirectX::XMFLOAT3& direction)
{
	DirectX::XMFLOAT3 front = Vec3Normalize(direction);

	orientation = { 0.0f,front.x,front.y,front.z };

	//DirectX::XMVECTOR orient = DirectX::XMLoadFloat4(&orientation);

	////�A�F���D�̌��ݎp���iorientation�j�ɂ�����O���iforward�j���z�[�������Ƃ��A
	////�����id�j�ւ̉�]���iaxis�j�Ɖ�]�p�iangle�j�����߂�
	//DirectX::XMFLOAT3 axis;	//��]��
	//float dangle;			//��]�p

	//DirectX::XMFLOAT3 dir = Vec3Normalize(direction);
	//DirectX::XMFLOAT3 _forward = Vec3Normalize(forward);

	//axis = Vec3Cross(_forward, dir);

	//dangle = Vec3Dot(_forward, dir);
	//dangle = acosf(dangle);

	////��]�p�iangle�j�������ȏꍇ�́A��]���s��Ȃ�
	//if (fabs(dangle) > 1e-8f)
	//{
	//	//�B��]���iaxis�j�Ɖ�]�p�iaxis�j�����]�N�I�[�^�j�I���iq�j�����߂�
	//	DirectX::XMVECTOR quaternion;

	//	axis = Vec3Normalize(axis);
	//	DirectX::XMVECTOR _axis = DirectX::XMLoadFloat3(&axis);

	//	quaternion = DirectX::XMQuaternionRotationAxis(_axis, dangle);

	//	//�C�F���D�̌��ݎp���iorientation�j�ɉ�]�N�I�[�^�j�I���iq�j����������
	//	orient = DirectX::XMQuaternionMultiply(orient, quaternion);

	//	//if (fabs(dangle) >= DirectX::XMConvertToRadians(90.0f)) {
	//	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(orient);
	//	DirectX::XMFLOAT4X4 m;
	//	DirectX::XMStoreFloat4x4(&m, mat);

	//	m._11 = -m._11;
	//	m._12 = -m._12;
	//	m._13 = -m._13;

	//	m._21 = -m._21;
	//	m._22 = -m._22;
	//	m._23 = -m._23;

	//	orient = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&m));
	////}

	//DirectX::XMStoreFloat4(&orientation, orient);
	//}
}


/*******************************************************************************
���b�V����񂩂烏�[���h�ϊ��s��擾
*******************************************************************************/
DirectX::XMFLOAT4X4 my_static_mesh::getWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, T;

	//	������
	world = DirectX::XMMatrixIdentity();

	//	�g��E�k��
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	��]
	//Rx = DirectX::XMMatrixRotationX(orientation.x);				//	X������Ƃ�����]�s��
	//Ry = DirectX::XMMatrixRotationY(orientation.y);				//	Y������Ƃ�����]�s��
	//Rz = DirectX::XMMatrixRotationZ(orientation.z);				//	Z������Ƃ�����]�s��
	//R = Rz * Ry * Rx;
	R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orientation));

	//	���s�ړ�
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	���[���h�ϊ��s��
	world = S * R * T;


	DirectX::XMFLOAT4X4 _world;

	DirectX::XMStoreFloat4x4(&_world, world);

	return	_world;
}

void bill_board::initialize(const wchar_t* tex_filename)
{
	//	��񏉊��l
	pos = DirectX::XMFLOAT3(0, 0, 0);
	angle = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);

	color = DirectX::XMFLOAT4(1, 1, 1, 1);

	if (!obj) obj = new geometric_board(FRAMEWORK.getDevice(), tex_filename);
	bLoad = true;
}

void bill_board::release()
{
	if (obj && bLoad)
	{
		delete	obj;
	}
	obj = nullptr;
	bLoad = false;
}

void bill_board::render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir)
{
	//	���f����������Ε`�揈������؍s��Ȃ�
	if (!obj)		return;

	DirectX::XMFLOAT4X4	view2 = view;
	DirectX::XMMATRIX	inv_view2;

	//	���ʂ̍s��Z�o(����view�t�s��AViewProjection�s��)
	//DirectX::XMStoreFloat4x4(&view2, view);						//	��U4x4�ɕϊ�
	view2._41 = 0.0f; view2._42 = 0.0f; 						//	view�̈ʒu��񂾂����폜
	view2._43 = 0.0f; view2._44 = 1.0f;							//	
	inv_view2 = DirectX::XMLoadFloat4x4(&view2);				//	Matrix�֍ĕϊ�
	inv_view2 = DirectX::XMMatrixInverse(nullptr, inv_view2);	//	view�s��̋t�s��

	DirectX::XMMATRIX	VP;
	VP = DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection);									//	���e�s�񍇐�

																												//	���[���h�ϊ��s��擾
	DirectX::XMFLOAT4X4 world = getWorldMatrix();

	//	Matrix -> Float4x4 �ϊ�
	DirectX::XMFLOAT4X4 world_view_projection;

	DirectX::XMMATRIX wvp = inv_view2 * DirectX::XMLoadFloat4x4(&world) * VP;

	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	//blender::Set(blender::ADD);

	//	�`��
	obj->render(FRAMEWORK.getImmediateContext(), world_view_projection, world, light_dir, color);

	//blender::Set(blender::ALPHA);
}

DirectX::XMFLOAT4X4 bill_board::getWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	������
	world = DirectX::XMMatrixIdentity();

	//	�g��E�k��
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	��]
	Rx = DirectX::XMMatrixRotationX(angle.x);				//	X������Ƃ�����]�s��
	Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y������Ƃ�����]�s��
	Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z������Ƃ�����]�s��
	R = Rz * Ry * Rx;

	//	���s�ړ�
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	���[���h�ϊ��s��
	world = S * R * T;

	DirectX::XMFLOAT4X4 _world;

	DirectX::XMStoreFloat4x4(&_world, world);

	return	_world;
}
