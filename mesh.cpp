#include "mesh.h"
#include "framework.h"
#include "input_data.h"


#include "geometric_primitive.h"
#include "static_mesh_ex.h"

/*******************************************************************************
メッシュ初期化
*******************************************************************************/
void	my_mesh::initialize()
{
	//	情報初期値
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

	//	モデル読み込み
	_model = nullptr;
	//model_renderer = nullptr;

	anim_index = -1;
}

/*******************************************************************************
メッシュモデル読み込み
引数
fbx_filename:メッシュファイル名(FBX形式)
*******************************************************************************/
bool my_mesh::load(const char* fbx_filename)
{
	//	モデル読み込み
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
メッシュ解放
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
メッシュ描画
引数
view:ビュー変換行列
projection:投影変換行列
light_dir:平行光ベクトル
*******************************************************************************/
void my_mesh::render(shader_ex *shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir, const bool& clock_wise, float tex_timer, float anim_timer)
{
	//モデルが無ければ描画処理を一切行わない
	if (!_model)		return;

	//	ワールド変換行列取得
	//DirectX::XMFLOAT4X4 W = GetWorldMatrix();

	//	Matrix -> Float4x4 変換
	DirectX::XMFLOAT4X4 view_projection;
	DirectX::XMFLOAT4X4 world = getWorldMatrix();
	/*DirectX::XMStoreFloat4x4(&view_projection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));*/

	DirectX::XMStoreFloat4x4(&view_projection, DirectX::XMLoadFloat4x4(&m_camera->getView()) * DirectX::XMLoadFloat4x4(&m_camera->getProjection()));
	//DirectX::XMStoreFloat4x4(&world, W);

	//	描画
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
	//	モデルが無ければ描画処理を一切行わない
	if (!_model)		return;

	DirectX::XMFLOAT4X4	view2 = view;
	DirectX::XMMATRIX	inv_view2;

	//	共通の行列算出(改変view逆行列、ViewProjection行列)
	//DirectX::XMStoreFloat4x4(&view2, view);						//	一旦4x4に変換
	view2._41 = 0.0f; view2._42 = 0.0f; 						//	viewの位置情報だけを削除
	view2._43 = 0.0f; view2._44 = 1.0f;							//	
	inv_view2 = DirectX::XMLoadFloat4x4(&view2);				//	Matrixへ再変換
	inv_view2 = DirectX::XMMatrixInverse(nullptr, inv_view2);	//	view行列の逆行列

	DirectX::XMMATRIX	VP;
	VP = DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection);									//	投影行列合成

																												//	ワールド変換行列取得
	DirectX::XMFLOAT4X4 world = getWorldMatrix();

	//	Matrix -> Float4x4 変換
	DirectX::XMFLOAT4X4 view_projection;

	DirectX::XMMATRIX wvp = inv_view2 * VP;

	DirectX::XMStoreFloat4x4(&view_projection, wvp);

	//	描画
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
	//	モデルが無ければ描画処理を一切行わない
	if (!_model)		return;

	//	ワールド変換行列取得
	//DirectX::XMFLOAT4X4 W = GetWorldMatrix();

	//	Matrix -> Float4x4 変換
	DirectX::XMFLOAT4X4 view_projection;
	DirectX::XMFLOAT4X4 world = getWorldMatrix();
	DirectX::XMStoreFloat4x4(&view_projection, DirectX::XMLoadFloat4x4(&m_camera->getView()) * DirectX::XMLoadFloat4x4(&m_camera->getProjection()));
	//DirectX::XMStoreFloat4x4(&world, W);

	//	描画
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
メッシュ情報からワールド変換行列取得
*******************************************************************************/
DirectX::XMFLOAT4X4	my_mesh::getWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, T;

	//	初期化
	world = DirectX::XMMatrixIdentity();

	//	拡大・縮小
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	回転
	DirectX::XMMATRIX Rx, Ry, Rz;
	Rx = DirectX::XMMatrixRotationX(angle.x);				//	X軸を基準とした回転行列
	Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y軸を基準とした回転行列
	Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z軸を基準とした回転行列
	R = Rz * Ry * Rx;
	//R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orientation));

	//	平行移動
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	ワールド変換行列
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
	//forwardを返す
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

	//rightを返す
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
	//upを返す
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
	//forwardを返す
	return Vec3Normalize(forward);
}

void my_mesh::focusMove(camera *cam)
{
	//x,z平面において計算する
	//前後
	DirectX::XMFLOAT2 cam_to_obj = DirectX::XMFLOAT2(pos.x - cam->getEye().x, pos.z - cam->getEye().z);
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat2(&cam_to_obj);
	vec = DirectX::XMVector2Normalize(vec);
	DirectX::XMStoreFloat2(&cam_to_obj, vec);
	//左右
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
	//	情報初期値
	pos = DirectX::XMFLOAT3(0, 0, 0);
	angle = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);


	color = DirectX::XMFLOAT4(1, 1, 1, 1);

	//	モデル読み込み
	obj = nullptr;
}

void geometric_mesh::setPrimitive(geometric_primitive* primitive)
{
	obj = primitive;
	bLoad = true;
}

/*******************************************************************************
メッシュモデル設定(使い回し型)
引数
primitive:メッシュモデル
*******************************************************************************/
void geometric_mesh::setMesh(geometric_mesh& org)
{
	*this = org;
	bLoad = false;
}

/*******************************************************************************
メッシュ解放
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
メッシュ描画
引数
view:ビュー変換行列
projection:投影変換行列
light_dir:平行光ベクトル
*******************************************************************************/
void geometric_mesh::render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& collision_color, const float& _scale, float timer)
{
	//	モデルが無ければ描画処理を一切行わない
	if (!obj)		return;

	scale = Vec3Multiply({ 1.0f,1.0f,1.0f }, _scale);

	//	Matrix -> Float4x4 変換
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMFLOAT4X4 world = getWorldMatrix();
	DirectX::XMStoreFloat4x4(&world_view_projection, DirectX::XMLoadFloat4x4(&world) * DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));

	color = collision_color;

	//	描画
	obj->render(FRAMEWORK.getImmediateContext(), world_view_projection, world, light_dir, color, false);
}


/*******************************************************************************
メッシュ情報からワールド変換行列取得
*******************************************************************************/
DirectX::XMFLOAT4X4 geometric_mesh::getWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	初期化
	world = DirectX::XMMatrixIdentity();

	//	拡大・縮小
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	回転
	Rx = DirectX::XMMatrixRotationX(angle.x);				//	X軸を基準とした回転行列
	Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y軸を基準とした回転行列
	Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z軸を基準とした回転行列
	R = Rz * Ry * Rx;

	//	平行移動
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	ワールド変換行列
	world = S * R * T;


	DirectX::XMFLOAT4X4 _world;

	DirectX::XMStoreFloat4x4(&_world, world);

	return	_world;
}

/*******************************************************************************
メッシュ初期化
*******************************************************************************/
void my_static_mesh::initialize()
{
	//	情報初期値
	pos = DirectX::XMFLOAT3(0, 0, 0);
	angle = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);

	up = { 0.0f,1.0f,0.0f };
	right = { 1.0f,0.0f,0.0f };
	forward = { 0.0f,0.0f,1.0f };

	orientation = { 0.0f,0.0f,0.0f,1.0f };

	color = DirectX::XMFLOAT4(1, 1, 1, 1);

	//	モデル読み込み
	obj = nullptr;
}

/*******************************************************************************
メッシュモデル読み込み
引数
fbx_filename:メッシュファイル名(FBX形式)
*******************************************************************************/
bool my_static_mesh::load(const wchar_t* obj_filename)
{
	//	モデル読み込み
	obj = new s_mesh(FRAMEWORK.getDevice(), obj_filename);
	bLoad = true;

	return	true;
}

/*******************************************************************************
メッシュ解放
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
メッシュ描画
引数
view:ビュー変換行列
projection:投影変換行列
light_dir:平行光ベクトル
*******************************************************************************/
void my_static_mesh::render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& camera_pos, const float timer)
{
	//	モデルが無ければ描画処理を一切行わない
	if (!obj)		return;

	//	ワールド変換行列取得
	DirectX::XMFLOAT4X4 world = getWorldMatrix();

	//	Matrix -> Float4x4 変換
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMStoreFloat4x4(&world_view_projection, DirectX::XMLoadFloat4x4(&world) * DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));


	//	描画
	obj->render(FRAMEWORK.getImmediateContext(), world_view_projection, world, light_dir, color, camera_pos, timer, false);
}

void my_static_mesh::setOrientation(const DirectX::XMFLOAT3& direction)
{
	DirectX::XMFLOAT3 front = Vec3Normalize(direction);

	orientation = { 0.0f,front.x,front.y,front.z };

	//DirectX::XMVECTOR orient = DirectX::XMLoadFloat4(&orientation);

	////②宇宙船の現在姿勢（orientation）における前方（forward）をホーム方向とし、
	////方向（d）への回転軸（axis）と回転角（angle）を求める
	//DirectX::XMFLOAT3 axis;	//回転軸
	//float dangle;			//回転角

	//DirectX::XMFLOAT3 dir = Vec3Normalize(direction);
	//DirectX::XMFLOAT3 _forward = Vec3Normalize(forward);

	//axis = Vec3Cross(_forward, dir);

	//dangle = Vec3Dot(_forward, dir);
	//dangle = acosf(dangle);

	////回転角（angle）が微小な場合は、回転を行わない
	//if (fabs(dangle) > 1e-8f)
	//{
	//	//③回転軸（axis）と回転角（axis）から回転クオータニオン（q）を求める
	//	DirectX::XMVECTOR quaternion;

	//	axis = Vec3Normalize(axis);
	//	DirectX::XMVECTOR _axis = DirectX::XMLoadFloat3(&axis);

	//	quaternion = DirectX::XMQuaternionRotationAxis(_axis, dangle);

	//	//④宇宙船の現在姿勢（orientation）に回転クオータニオン（q）を合成する
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
メッシュ情報からワールド変換行列取得
*******************************************************************************/
DirectX::XMFLOAT4X4 my_static_mesh::getWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, T;

	//	初期化
	world = DirectX::XMMatrixIdentity();

	//	拡大・縮小
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	回転
	//Rx = DirectX::XMMatrixRotationX(orientation.x);				//	X軸を基準とした回転行列
	//Ry = DirectX::XMMatrixRotationY(orientation.y);				//	Y軸を基準とした回転行列
	//Rz = DirectX::XMMatrixRotationZ(orientation.z);				//	Z軸を基準とした回転行列
	//R = Rz * Ry * Rx;
	R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orientation));

	//	平行移動
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	ワールド変換行列
	world = S * R * T;


	DirectX::XMFLOAT4X4 _world;

	DirectX::XMStoreFloat4x4(&_world, world);

	return	_world;
}

void bill_board::initialize(const wchar_t* tex_filename)
{
	//	情報初期値
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
	//	モデルが無ければ描画処理を一切行わない
	if (!obj)		return;

	DirectX::XMFLOAT4X4	view2 = view;
	DirectX::XMMATRIX	inv_view2;

	//	共通の行列算出(改変view逆行列、ViewProjection行列)
	//DirectX::XMStoreFloat4x4(&view2, view);						//	一旦4x4に変換
	view2._41 = 0.0f; view2._42 = 0.0f; 						//	viewの位置情報だけを削除
	view2._43 = 0.0f; view2._44 = 1.0f;							//	
	inv_view2 = DirectX::XMLoadFloat4x4(&view2);				//	Matrixへ再変換
	inv_view2 = DirectX::XMMatrixInverse(nullptr, inv_view2);	//	view行列の逆行列

	DirectX::XMMATRIX	VP;
	VP = DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection);									//	投影行列合成

																												//	ワールド変換行列取得
	DirectX::XMFLOAT4X4 world = getWorldMatrix();

	//	Matrix -> Float4x4 変換
	DirectX::XMFLOAT4X4 world_view_projection;

	DirectX::XMMATRIX wvp = inv_view2 * DirectX::XMLoadFloat4x4(&world) * VP;

	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	//blender::Set(blender::ADD);

	//	描画
	obj->render(FRAMEWORK.getImmediateContext(), world_view_projection, world, light_dir, color);

	//blender::Set(blender::ALPHA);
}

DirectX::XMFLOAT4X4 bill_board::getWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	初期化
	world = DirectX::XMMatrixIdentity();

	//	拡大・縮小
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	回転
	Rx = DirectX::XMMatrixRotationX(angle.x);				//	X軸を基準とした回転行列
	Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y軸を基準とした回転行列
	Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z軸を基準とした回転行列
	R = Rz * Ry * Rx;

	//	平行移動
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	ワールド変換行列
	world = S * R * T;

	DirectX::XMFLOAT4X4 _world;

	DirectX::XMStoreFloat4x4(&_world, world);

	return	_world;
}
