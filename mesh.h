#pragma once

#include <directxmath.h>

#include "skin_mesh_ex.h"
#include "static_mesh_ex.h"

#include "geometric_primitive.h"
//#include "..\..\Game\Header\Light.h"

#include "camera.h"

#include "model.h"
#include "model_renderer.h"

#include "shader.h"

class my_mesh
{
private:
	bool				bLoad;//読み込み状態か//解放したらfalse
	bool				isAnim;//アニメーションがあるか//なければfalse

	int					anim_index;//アニメーションに振り分けられた番号

	struct bounding_box
	{
		DirectX::XMFLOAT3 min = { +FLT_MAX, +FLT_MAX, +FLT_MAX };
		DirectX::XMFLOAT3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	};
	bounding_box boundingBox;

public:
	std::unique_ptr<model>	 _model;
	//std::unique_ptr<ModelRenderer>			model_renderer;

	DirectX::XMFLOAT3	pos;				//	「位置」
	DirectX::XMFLOAT3	angle;				//	「(回転)角度」
	DirectX::XMFLOAT3	scale;				//	「大きさ」

	DirectX::XMFLOAT4	color;


	DirectX::XMFLOAT4	orientation;		//	「姿勢」

	DirectX::XMFLOAT3	right;				//	「右方向」
	DirectX::XMFLOAT3	up;					//	「頭方向」
	DirectX::XMFLOAT3	forward;			//	「前方向」

	bool isCullNone = false;
	float	anim_timer;

	my_mesh() :bLoad(), isAnim(), anim_index(),pos(), angle(), scale(), color(), orientation(), right(), up(), forward(), anim_timer() {}

	void	initialize();
	bool	load(const char* fbx_filename);
	//int	AnimationInit(bool loop = false);

	int getMaterialNum() { return _model->getMaterialNum(); }

	void setMaterialTex(const int index, const wchar_t* filename);
	void setAllMaterialTex(const wchar_t* filename);

	//void	SetPrimitive(GeometricPrimitive* primitive);
	//void	SetMesh(MyMesh& org);
	void	release();
	void	render(shader_ex *shader, main_camera* m_camera, DirectX::XMFLOAT4& light_dir, const bool& clock_wise = false, float tex_timer = 0.0f, float anim_timer = 0.0f);
	void	render(shader *shader, main_camera* m_camera, DirectX::XMFLOAT4& light_dir, const bool& clock_wise = false, float tex_timer = 0.0f, float anim_timer = 0.0f);
	void	bRender(shader_ex *shader, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& camera_pos, const bool& clock_wise = false, float tex_timer = 0.0f, float anim_timer = 0.0f);
	//int	AddAnimation(const char* fbx_filename);

	//void PlayAnimation(int animation_index, bool loop = false);

	DirectX::XMFLOAT4X4	getWorldMatrix();

	DirectX::XMFLOAT3& getBonePos(const char* bone_name);
	DirectX::XMFLOAT3& getBoneDirection(const char* bone_name);

	DirectX::XMFLOAT3& getBoneDirectionX(const char* bone_name);
	DirectX::XMFLOAT3& getBoneDirectionY(const char* bone_name);
	DirectX::XMFLOAT3& getBoneDirectionZ(const char* bone_name);

	//bool GetBlendMode() { return model->GetBlendMode(); }

	const DirectX::XMFLOAT3& get_Max()const
	{
		return boundingBox.max;
	}
	const DirectX::XMFLOAT3& get_Min()const
	{
		return boundingBox.min;
	}

	void focusMove(camera *cam);

};


class geometric_mesh
{
private:
	bool				bLoad;

public:
	geometric_primitive* obj;

	DirectX::XMFLOAT3	pos;				//	「位置」
	DirectX::XMFLOAT3	angle;				//	「(回転)角度」
	DirectX::XMFLOAT3	scale;				//	「大きさ」

	DirectX::XMFLOAT4	color;

	geometric_mesh() :bLoad(), obj(), pos(), angle(), scale(), color() {}
	~geometric_mesh() { release(); }

	void	initialize();
	//bool	Load(const char* fbx_filename);
	void	setPrimitive(geometric_primitive* primitive);
	void	setMesh(geometric_mesh& org);
	void	release();
	void	render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& collision_color = { 0.0f,1.0f,0.0f,1.0f }, const float& _scale = 1.0f, float timer = 0.0f);

	DirectX::XMFLOAT4X4	getWorldMatrix();
};


class my_static_mesh
{
private:
	bool		bLoad;

public:
	s_mesh* obj;

	DirectX::XMFLOAT3	pos;				//	「位置」
	DirectX::XMFLOAT3	angle;				//	「(回転)角度」
	DirectX::XMFLOAT3	scale;				//	「大きさ」

	DirectX::XMFLOAT4	orientation;		//	「姿勢」

	DirectX::XMFLOAT3	right;				//	「右方向」
	DirectX::XMFLOAT3	up;					//	「頭方向」
	DirectX::XMFLOAT3	forward;			//	「前方向」

	DirectX::XMFLOAT4	color;

	my_static_mesh() :bLoad(), obj(), pos(), angle(), scale(), color(), orientation(), right(), up(), forward() { }

	void	initialize();
	bool	load(const wchar_t* obj_filename);
	//void	SetPrimitive(GeometricPrimitive* primitive);
	void	release();
	void	render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir, const DirectX::XMFLOAT4& camera_pos, const float timer = 0.0f);

	void setOrientation(const DirectX::XMFLOAT3& direction);

	DirectX::XMFLOAT4X4	getWorldMatrix();
};

class bill_board
{
private:
	bool				bLoad;

public:
	geometric_board* obj;

	DirectX::XMFLOAT3	pos;				//	「位置」
	DirectX::XMFLOAT3	angle;				//	「(回転)角度」
	DirectX::XMFLOAT3	scale;				//	「大きさ」

	DirectX::XMFLOAT4	color;

	bill_board() :bLoad(), obj(), pos(), angle(), scale(), color() {}

	void	initialize(const wchar_t* tex_filename);
	void	release();
	void	render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT4& light_dir);

	~bill_board() { release(); }

	DirectX::XMFLOAT4X4	getWorldMatrix();
};