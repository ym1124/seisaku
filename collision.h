#pragma once
#include "collision.h"
#include "skin_object.h"
#include "static_object.h"

class collision
{
private:
	static std::vector<std::shared_ptr<skin_object>> terrains;
	static std::vector<std::shared_ptr<static_object>> terrains2;
	static std::vector<static_object*> collision::terrains3;
public:
	enum PickType {
		SKIN,//fbx
	    STATIC_SHARE,//shared_ptr/obj
		STATIC_POINTER//ptr/obj
	};
	// �n�`�̓o�^�Ɖ���
	static void setTerrain(std::shared_ptr<skin_object>& obj);
	static void setTerrain(std::shared_ptr<static_object>& obj);//shared
	static void setTerrain(static_object* obj);
	static void unSetTerrain(/*std::shared_ptr<StaticObject>& obj*/);
	// �o�^����Ă���n�`�ɂ������Ẵ��C�s�b�N
	//�߂�l��materialIndex�̂܂�
	static int RayPick(const DirectX::XMFLOAT3& startPosition,const DirectX::XMFLOAT3& endPosition,DirectX::XMFLOAT3* outPosition,DirectX::XMFLOAT3* outNormal,float* outLength,int type);
	static void autoRayPick(std::shared_ptr<static_object> obj, float ray_range);
	static void autoRayPick(static_object* obj, float ray_range, collision::PickType type = STATIC_SHARE);
	static void autoRayPick(std::shared_ptr<skin_object> obj, float ray_range);
	static void terrainsFaceUpdate(collision::PickType type = STATIC_SHARE);
	// �o�^����Ă���n�`�ɑ΂��Ă̔��ˊ֐�
	static int Reflect(
		const DirectX::XMFLOAT3& startPosition,
		const DirectX::XMFLOAT3& endPosition,
		float rate,//���ˑ��x������
		DirectX::XMFLOAT3* outPosition,//���ˌ�̈ʒu
		DirectX::XMFLOAT3* outReflect,//���ˌ�̑��x�x�N�g��
		int mesh_type
		); 
};

