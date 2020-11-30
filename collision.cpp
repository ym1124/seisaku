//#include "collision.h"

std::vector<std::shared_ptr<skin_object>> collision::terrains;
std::vector<std::shared_ptr<static_object>> collision::terrains2;
std::vector<static_object*> collision::terrains3;

//std::shared_ptr<skin_object> collision::terrain;
//std::shared_ptr<static_object> collision::terrain2;

void collision::setTerrain(std::shared_ptr<skin_object>& obj)
{
	//terrain.reset(obj.get());���L����������ĕʂ̃|�C���^������̂ŁA�ŏ���null����������ł��Ȃ�
	//terrain = obj;
	std::shared_ptr<skin_object> temp = obj;
	terrains.push_back(temp);
}

void collision::setTerrain(std::shared_ptr<static_object>& obj)//unique�ɂ�����
{
	////terrain.reset(obj.get());���L����������ĕʂ̃|�C���^������̂ŁA�ŏ���null����������ł��Ȃ�
	//terrain2 = obj;
	std::shared_ptr<static_object> temp = obj;
	terrains2.push_back(temp);
}

void collision::setTerrain(static_object* obj)
{
	////terrain.reset(obj.get());���L����������ĕʂ̃|�C���^������̂ŁA�ŏ���null����������ł��Ȃ�
	//terrain2 = obj;
	static_object* temp = obj;
	terrains3.push_back(temp);
}

void collision::unSetTerrain(/*std::shared_ptr<StaticObject>& obj*/)
{
	//terrain.reset();
	for (int i = 0; i < terrains2.size(); i++)
	{
		terrains2[i].reset();
	}
}

int collision::RayPick(const DirectX::XMFLOAT3 & startPosition, const DirectX::XMFLOAT3 & endPosition, DirectX::XMFLOAT3 *outPosition, DirectX::XMFLOAT3 *outNormal, float* outLength, int type)
{
	switch (type)
	{
	case PickType::SKIN:
		for (int i = 0; i < terrains.size(); i++)
		{
			terrains[i].get()->getMesh()->RayPick(startPosition, endPosition, outPosition, outNormal, outLength);
		}
		return true;
		break;
	case PickType::STATIC_SHARE:
		//return terrain2.get()->getMesh()->RayPick(startPosition, endPosition, outPosition, outNormal, outLength);
		for (int i = 0; i < terrains2.size(); i++)
		{
			terrains2[i].get()->getMesh()->RayPick(startPosition, endPosition, outPosition, outNormal, outLength);
		}
		return true;
		break;
	case PickType::STATIC_POINTER:
		for (int i = 0; i < terrains3.size(); i++)
		{
			terrains3[i]->getMesh()->RayPick(startPosition, endPosition, outPosition, outNormal, outLength);
		}
		return true;
		break;
	}
}

void collision::autoRayPick(std::shared_ptr<static_object> obj, float ray_range)
{
	DirectX::XMFLOAT3 rayStart(obj->getPosition().x, obj->getPosition().y + ray_range, obj->getPosition().z);
	DirectX::XMFLOAT3 rayEnd(obj->getPosition().x, obj->getPosition().y - ray_range, obj->getPosition().z);
	DirectX::XMFLOAT3 outPosition;//���W�󂯎��p�ϐ�
	DirectX::XMFLOAT3 outNormal;//�@���󂯎��p�ϐ�
	float dist;
	collision::RayPick(rayStart, rayEnd, &outPosition, &outNormal, &dist, collision::PickType::STATIC_SHARE);
	obj->setPosition(outPosition);
	//fbx->update();
	obj->setPosture(outNormal);
}

void collision::autoRayPick(static_object* obj, float ray_range , collision::PickType type)
{
	DirectX::XMFLOAT3 rayStart(obj->getPosition().x, obj->getPosition().y + ray_range, obj->getPosition().z);
	DirectX::XMFLOAT3 rayEnd(obj->getPosition().x, obj->getPosition().y - ray_range, obj->getPosition().z);
	DirectX::XMFLOAT3 outPosition;//���W�󂯎��p�ϐ�
	DirectX::XMFLOAT3 outNormal;//�@���󂯎��p�ϐ�
	float dist;
	collision::RayPick(rayStart, rayEnd, &outPosition, &outNormal, &dist, type);
	obj->setPosition(outPosition);
	//fbx->update();
	obj->setPosture(outNormal);
}

void collision::autoRayPick(std::shared_ptr<skin_object> obj, float ray_range)
{
	DirectX::XMFLOAT3 rayStart(obj->getPosition().x, obj->getPosition().y + ray_range, obj->getPosition().z);
	DirectX::XMFLOAT3 rayEnd(obj->getPosition().x, obj->getPosition().y - ray_range, obj->getPosition().z);
	DirectX::XMFLOAT3 outPosition;//���W�󂯎��p�ϐ�
	DirectX::XMFLOAT3 outNormal;//�@���󂯎��p�ϐ�
	float dist;
	collision::RayPick(rayStart, rayEnd, &outPosition, &outNormal, &dist, collision::PickType::SKIN);
	obj->setPosition(outPosition);
	//fbx->update();
	obj->setPosture(outNormal);
}

//�I�u�W�F�N�g���ړ�������RayPick�p��face���ړ����Ȃ��Ƃ����Ȃ�
void collision::terrainsFaceUpdate(collision::PickType type)
{
	//std::vector<DirectX::XMMATRIX> tm;
	//std::vector<DirectX::XMMATRIX> rm;

	switch (type)
	{
	case collision::PickType::STATIC_SHARE:
		//�����鏇�ԑ厖�I
		for (int i = 0; i < terrains2.size(); i++)
		{
			//��]�s��쐬
			DirectX::XMMATRIX r;
			float x, y, z;
			x = terrains2[i]->getAngle().x*(DirectX::XM_PI / 180.0f) - terrains2[i]->getOldAngle().x*(DirectX::XM_PI / 180.0f);
			y = terrains2[i]->getAngle().y*(DirectX::XM_PI / 180.0f) - terrains2[i]->getOldAngle().y*(DirectX::XM_PI / 180.0f);
			z = terrains2[i]->getAngle().z*(DirectX::XM_PI / 180.0f) - terrains2[i]->getOldAngle().z*(DirectX::XM_PI / 180.0f);
			r = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
			//�ݒu���Ȃ�������̍��W�ɕϊ�
			for (auto& it : terrains2[i]->getMesh()->faces)
			{
				for (int i = 0; i < 3; i++)
				{
					//��]������
					//VECTOR�ɕϊ�
					DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(it.position[i].x, it.position[i].y, it.position[i].z, 1.0f));
					//��]�s��ƍ���
					pos = DirectX::XMVector4Transform(pos, r);
					//���W��face�ɕԂ�
					DirectX::XMStoreFloat3(&it.position[i], pos);
				}
			}
			terrains2[i]->setOldAngle(terrains2[i]->getAngle());
			//rm.push_back(r);
		}

		for (int i = 0; i < terrains2.size(); i++)
		{
			//�g��k���s��쐬
			DirectX::XMMATRIX s;
			float x, y, z;
			x = terrains2[i]->getScale().x / terrains2[i]->getOldScale().x;
			y = terrains2[i]->getScale().y / terrains2[i]->getOldScale().y;
			z = terrains2[i]->getScale().z / terrains2[i]->getOldScale().z;
			s = DirectX::XMMatrixScaling(x, y, z);
			//�ݒu���Ȃ�������̍��W�ɕϊ�
			for (auto& it : terrains2[i]->getMesh()->faces)
			{
				for (int i = 0; i < 3; i++)
				{
					//�g��k��������
					//VECTOR�ɕϊ�
					DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(it.position[i].x, it.position[i].y, it.position[i].z, 1.0f));
					//�g��k���s��ƍ���
					pos = DirectX::XMVector4Transform(pos, s);
					//���W��face�ɕԂ�
					DirectX::XMStoreFloat3(&it.position[i], pos);
				}
			}
			terrains2[i]->setOldScale(terrains2[i]->getScale());
			//tm.push_back(t);
		}

		for (int i = 0; i < terrains2.size(); i++)
		{
			//���s�ړ��s��쐬
			DirectX::XMMATRIX t;
			float x, y, z;
			x = terrains2[i]->getPosition().x - terrains2[i]->getOldPosition().x;
			y = terrains2[i]->getPosition().y - terrains2[i]->getOldPosition().y;
			z = terrains2[i]->getPosition().z - terrains2[i]->getOldPosition().z;
			t = DirectX::XMMatrixTranslation(x, y, z);
			//�ݒu���Ȃ�������̍��W�ɕϊ�
			for (auto& it : terrains2[i]->getMesh()->faces)
			{
				for (int i = 0; i < 3; i++)
				{
					//�ړ�������
					//VECTOR�ɕϊ�
					DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(it.position[i].x, it.position[i].y, it.position[i].z, 1.0f));
					//�ړ��s��ƍ���
					pos = DirectX::XMVector4Transform(pos, t);
					//���W��face�ɕԂ�
					DirectX::XMStoreFloat3(&it.position[i], pos);
				}
			}
			terrains2[i]->setOldPosition(terrains2[i]->getPosition());
			//tm.push_back(t);
		}
		break;
	case collision::PickType::STATIC_POINTER:
		//�����鏇�ԑ厖�I
		for (int i = 0; i < terrains3.size(); i++)
		{
			//��]�s��쐬
			DirectX::XMMATRIX r;
			float x, y, z;
			x = terrains3[i]->getAngle().x*(DirectX::XM_PI / 180.0f) - terrains3[i]->getOldAngle().x*(DirectX::XM_PI / 180.0f);
			y = terrains3[i]->getAngle().y*(DirectX::XM_PI / 180.0f) - terrains3[i]->getOldAngle().y*(DirectX::XM_PI / 180.0f);
			z = terrains3[i]->getAngle().z*(DirectX::XM_PI / 180.0f) - terrains3[i]->getOldAngle().z*(DirectX::XM_PI / 180.0f);
			r = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
			//�ݒu���Ȃ�������̍��W�ɕϊ�
			for (auto& it : terrains3[i]->getMesh()->faces)
			{
				for (int i = 0; i < 3; i++)
				{
					//��]������
					//VECTOR�ɕϊ�
					DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(it.position[i].x, it.position[i].y, it.position[i].z, 1.0f));
					//��]�s��ƍ���
					pos = DirectX::XMVector4Transform(pos, r);
					//���W��face�ɕԂ�
					DirectX::XMStoreFloat3(&it.position[i], pos);
				}
			}
			terrains3[i]->setOldAngle(terrains3[i]->getAngle());
			//rm.push_back(r);
		}

		for (int i = 0; i < terrains3.size(); i++)
		{
			//�g��k���s��쐬
			DirectX::XMMATRIX s;
			float x, y, z;
			x = terrains3[i]->getScale().x / terrains3[i]->getOldScale().x;
			y = terrains3[i]->getScale().y / terrains3[i]->getOldScale().y;
			z = terrains3[i]->getScale().z / terrains3[i]->getOldScale().z;
			s = DirectX::XMMatrixScaling(x, y, z);
			//�ݒu���Ȃ�������̍��W�ɕϊ�
			for (auto& it : terrains3[i]->getMesh()->faces)
			{
				for (int i = 0; i < 3; i++)
				{
					//�g��k��������
					//VECTOR�ɕϊ�
					DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(it.position[i].x, it.position[i].y, it.position[i].z, 1.0f));
					//�g��k���s��ƍ���
					pos = DirectX::XMVector4Transform(pos, s);
					//���W��face�ɕԂ�
					DirectX::XMStoreFloat3(&it.position[i], pos);
				}
			}
			terrains3[i]->setOldScale(terrains3[i]->getScale());
			//tm.push_back(t);
		}

		for (int i = 0; i < terrains3.size(); i++)
		{
			//���s�ړ��s��쐬
			DirectX::XMMATRIX t;
			float x, y, z;
			x = terrains3[i]->getPosition().x - terrains3[i]->getOldPosition().x;
			y = terrains3[i]->getPosition().y - terrains3[i]->getOldPosition().y;
			z = terrains3[i]->getPosition().z - terrains3[i]->getOldPosition().z;
			t = DirectX::XMMatrixTranslation(x, y, z);
			//�ݒu���Ȃ�������̍��W�ɕϊ�
			for (auto& it : terrains3[i]->getMesh()->faces)
			{
				for (int i = 0; i < 3; i++)
				{
					//�ړ�������
					//VECTOR�ɕϊ�
					DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(it.position[i].x, it.position[i].y, it.position[i].z, 1.0f));
					//�ړ��s��ƍ���
					pos = DirectX::XMVector4Transform(pos, t);
					//���W��face�ɕԂ�
					DirectX::XMStoreFloat3(&it.position[i], pos);
				}
			}
			terrains3[i]->setOldPosition(terrains3[i]->getPosition());
			//tm.push_back(t);
		}
		break;
	}
	



	//for (int j = 0; j < terrains2.size(); j++)
	//{
	//	DirectX::XMMATRIX srt;
	//	//srt = rm[i] * tm[i];//�����鏇�ԑ厖�I
	//	//rm.at(j)*tm.at(j);
	//	//�ݒu���Ȃ�������̍��W�ɕϊ�
	//	for (auto& it : terrains2[j]->getMesh()->faces)
	//	{
	//		for (int i = 0; i < 3; i++)
	//		{
	//			//VECTOR�ɕϊ�
	//			DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(it.position[i].x, it.position[i].y, it.position[i].z, 1.0f));
	//			//�s��ƍ���
	//			pos = DirectX::XMVector4Transform(pos, rm[j]);
	//			pos = DirectX::XMVector4Transform(pos, tm[j]);
	//			//���W��face�ɕԂ�
	//			DirectX::XMStoreFloat3(&it.position[i], pos);
	//		}
	//	}
	//}
}

//int collision::Reflect(const DirectX::XMFLOAT3 & startPosition, const DirectX::XMFLOAT3 & endPosition, float rate, DirectX::XMFLOAT3 * outPosition, DirectX::XMFLOAT3 * outReflect,int mesh_type)
//{
//	DirectX::XMFLOAT3 hitPosition, hitNormal;//���������ʂ̍��W�A�@��
//	float outLength;
//	int ret = RayPick(startPosition, endPosition, &hitPosition, &hitNormal, &outLength,mesh_type);
//	if (ret < 0)
//	{
//		return ret;
//	}
//	// �ǂ̖@��
//	DirectX::XMVECTOR wall_n = DirectX::XMLoadFloat3(&hitNormal);//hitNormal�͐��K���ς�
//
//																 // ���˃x�N�g����@���Ɏˉe//���˃x�N�g�������߂邽�߂̓��˃x�N�g���͕ǂɖ��܂�
//	DirectX::XMVECTOR incident;//���˃x�N�g��
//	incident = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&endPosition), DirectX::XMLoadFloat3(&startPosition));
//	//�ˉe�x�N�g���v�Z
//	DirectX::XMVECTOR length = DirectX::XMVector3Dot(incident, DirectX::XMVectorNegate(wall_n));//�ǖ@���̋t�x�N�g���ɓ��˃x�N�g�����ˉe
//	float len;//����
//	DirectX::XMStoreFloat(&len, length);//�ϊ�
//										//DirectX::XMFLOAT3 anti_n;
//										//DirectX::XMStoreFloat3(&anti_n,DirectX::XMVectorNegate(wall_n));//�ǂ̋t�@���x�N�g����ϊ�
//	DirectX::XMFLOAT3 wn;
//	DirectX::XMStoreFloat3(&wn, wall_n);
//	wn = DirectX::XMFLOAT3(wn.x*len, wn.y*len, wn.z*len);//�ˉe�x�N�g���{����
//	DirectX::XMFLOAT3 wn2;
//	wn2 = DirectX::XMFLOAT3(wn.x * 2, wn.y * 2, wn.z * 2);
//
//	DirectX::XMFLOAT3 incident_f;
//	DirectX::XMStoreFloat3(&incident_f, incident);
//	DirectX::XMFLOAT3 reflect = DirectX::XMFLOAT3(wn2.x + incident_f.x, wn2.y + incident_f.y, wn2.z + incident_f.z);
//
//	*outPosition = DirectX::XMFLOAT3(hitPosition.x + reflect.x, hitPosition.y + reflect.y, hitPosition.z + reflect.z);
//	*outReflect = reflect;
//	return ret;
//}