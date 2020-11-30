#pragma once

#include <DirectXMath.h>

class physics
{
private:
	const float gravity = 9.80665f;//unit::m/s^2
	 //enable flg(false����Influence�֐��̒��g�����s���Ȃ�)
	bool gravity_enable;
	bool friction_enable;

protected:
	float weight;//���� unit::kg
	float max_static_friction;//�ő�Î~���C�� unit::N
	float dynamic_friction;//�����C�� unit::N
	float coefficient_of_restitution;//�����W��
	DirectX::XMFLOAT3 position;//world_position
	DirectX::XMFLOAT3 velocity;//���x unit::m/s
	DirectX::XMFLOAT3 accelerator;//�����x unit::m/s^2

private://PhysicalUpdate�Ŏ��s���Ă���̂ŌĂ΂Ȃ��đ��v
	inline void influenceOfGravity(const float _delta_time)
	{
		//�t���O��false���͏d�͂̉e�����󂯂Ȃ�
		if (gravity_enable == false)
			return;

		//�d�͂ɂ��^���Ȃ̂Ŏ��ʂɂ͉e�����󂯂Ȃ�
		velocity.y -= gravity * _delta_time;
	}
	inline void positionUpdate(const float _delta_time)
	{
		position.x += velocity.x * _delta_time;
		position.y += velocity.y * _delta_time;
		position.z += velocity.z * _delta_time;
	}

protected:

public:
	inline void physicalUpdate(const float _delta_time)
	{
		influenceOfGravity(_delta_time);
		positionUpdate(_delta_time);
	}

	//���̕��̂Ɛڂ��Ă���Ƃ��Ɏg�p����(�ڂ��Ă��镨�̂̍ő�Î~���C�͂Ɠ����C�͂����������Ă���)
	inline void influenceOfFriction(const float _delta_time, const float _max_static_friction, const float _dynamic_friction)
	{
		//�t���O��false���͖��C�͂̉e�����󂯂Ȃ�
		if (friction_enable == false)
			return;

		//���������ɑ��x�����邩�̔���(���C�Ȃ̂Ő��������݂̂ł�����͂�)
		float velocity_h = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

		//���x������Ƃ�(�ړ����Ă���̂œ����C�͂���p����)
		if (velocity_h > .0f)
		{
			//���������̗�(�����x�Ǝ��ʂ������邱�Ƃŗ͂ɕϊ��\�ɂ���)
			float power = sqrtf(accelerator.x * accelerator.x + accelerator.z * accelerator.z) * weight;

			//�ڂ��Ă��镨��(�n�ʂȂ�)�̓����C�͂��͂��������Ȃ�(�����A��������)�ꍇ
			if (power != _dynamic_friction)
			{
				//������(�����x)�̎Z�o
				float length = ((power - _dynamic_friction) / power) / weight;

				//���x�ɉ��Z����
				velocity.x += (accelerator.x * _delta_time) * length;
				velocity.z += (accelerator.z * _delta_time) * length;
			}

			//�������ꍇ(�����^��)�͉��������������Ȃ����߉����L�q���Ȃ�
		}

		//���x���Ȃ��Ƃ�(�ړ����Ă��Ȃ��̂ŐÎ~���C�͂���p����)
		else if (velocity_h == .0f)
		{
			//���������̉����x�Ɏ��ʂ������邱�Ƃɂ���ė�(N)�ɕϊ�����
			float power = sqrtf(accelerator.x * accelerator.x + accelerator.z * accelerator.z) * weight;

			//�ڂ��Ă��镨��(�n�ʂȂ�)�̐Î~���C�͂��͂��傫���ꍇ
			if (power > _max_static_friction)
			{
				//�x�N�g���̒���(����)���Z�o
				float length = ((power - _max_static_friction) / power) / weight;

				//���x�ɉ��Z����
				velocity.x += (accelerator.x * length) * _delta_time;
				velocity.z += (accelerator.z * length) * _delta_time;
			}

			//�������ꍇ�Ɠ������ꍇ�͉����N���Ȃ��̂ŋL�q���Ȃ�
		}

		//���x�̃x�N�g���̒������}�C�i�X�͂��������̂ŃG���[����(debug���݂̂ł����Ǝv�����̂�assert���g�p)
		else
		{
			assert(0 && "the value of this variable is an improbable value");
		}
	}

	//���̕��̂ɏՓ˂����ۂɎg�p����(position�̏C�����֐����ŏ����ς�)
	//���̂������܂��͐����ȏꍇ�ɂ̂ݎg�p(�Փ˂������̂̔����͂ƏՓˈʒu�A�Փ˕�����n��)
	inline void verticalRebound(const float _coefficient_of_restitution, const DirectX::XMFLOAT3 _pos, const bool _horizontal_direction)
	{
		//�����W����0<=e<=1�Ȃ̂Ŕ͈̓`�F�b�N
		assert((.0f <= _coefficient_of_restitution && _coefficient_of_restitution <= 1.0f) && "the value of this variable is an improbable value");

		//�Ƃ肠����position�̏C��
		position = _pos;

		//���������̒��˕Ԃ�
		if (_horizontal_direction)
		{
			//�Փ˂����Ώە��̔�����(�Ԃ������ۂ̔����W��)�𑬓x�ɂ�����
			velocity.x *= _coefficient_of_restitution;
			velocity.z *= _coefficient_of_restitution;

			//���˕Ԃ����̂Ŏ��̃t���[������̉����x�͋t�����ɂȂ�
			accelerator.x *= -(_coefficient_of_restitution);
			accelerator.z *= -(_coefficient_of_restitution);
		}

		//���������̒��˕Ԃ�
		else
		{
			//�Փ˂����Ώە��̔�����(�Ԃ������ۂ̔����W��)�𑬓x�ɂ�����
			velocity.y *= _coefficient_of_restitution;
			//���˕Ԃ����̂Ŏ��̃t���[������̉����x�͋t�����ɂȂ�
			accelerator.y *= -(_coefficient_of_restitution);
		}
	}
	//���������ȊO�ł̏Փˎ��Ɏg�p(�Փ˂������̂̔����͂ƏՓˈʒu(world���W)�A�Փ˂����ʂ̌����Ă������(�@��)��n��)
	//�߂荞��ł���Ƃ���z�肵�đg��ł�
	inline void nonVerticalRebound(const float _coefficient_of_restitution, const DirectX::XMFLOAT3 _pos, const DirectX::XMFLOAT3 _vector)
	{
		//�����W����0<=e<=1�Ȃ̂Ŕ͈̓`�F�b�N
		assert((.0f <= _coefficient_of_restitution && _coefficient_of_restitution <= 1.0f) && "the value of this variable is an improbable value");

		//�i�s�����ƏՓ˖ʂ̌����x�N�g���œ������Ă邩����(���όv�Z)
		//TODO �ϐ����ς������I
		DirectX::XMVECTOR norm_vec = DirectX::XMLoadFloat3(&_vector);
		norm_vec = DirectX::XMVector3Normalize(norm_vec);
		DirectX::XMVECTOR norm_velo = XMLoadFloat3(&velocity);
		norm_velo = DirectX::XMVector3Normalize(norm_velo);
		DirectX::XMVECTOR result = DirectX::XMVector3Dot(norm_vec, norm_velo);
		float judge;
		DirectX::XMStoreFloat(&judge, result);

		//�\�ʂ���΂߂ɓ������Ă��鎞
		if (0 < judge && judge < 1.0f)
		{
			//�Փˉӏ��ƌ��݂̃|�W�V�����Ńx�N�g��������(�����K���ɂȂ��Ă�)
			DirectX::XMFLOAT3 vec_b = { _pos.x - position.x,_pos.y - position.y,_pos.z - position.z };

			//�Փ˖ʂ̌���(�@��)�Ɏˉe����(�Ȃ�ƂȂ�������XMVECTOR�g�킸����Ă�)
			//�Փ˖ʂ̌���(�@��)�𐳋K������
			float length_vec = sqrtf(_vector.x * _vector.x + _vector.y * _vector.y + _vector.z * _vector.z);
			DirectX::XMFLOAT3 n_vec = { _vector.x / length_vec,_vector.y / length_vec,_vector.z / length_vec };

			//�x�N�g��2����όv�Z����
			float dot = vec_b.x * _vector.x + vec_b.y * _vector.y + vec_b.z * _vector.z;
			float value = dot / length_vec;
			DirectX::XMFLOAT3 projection_vec = { n_vec.x * value,n_vec.y * value,n_vec.z * value };

			//���݂̃|�W�V�����Ɏˉe�x�N�g����2�{�����Z���ړ������Ĉʒu�̏C��������
			position.x += projection_vec.x * 2;
			position.y += projection_vec.y * 2;
			position.z += projection_vec.z * 2;

			//�ړ���̃|�W�V�����ƏՓˉӏ��Ńx�N�g��������(�P�ʃx�N�g���ɂ���)
			DirectX::XMFLOAT3 reflection_vec = { position.x - _pos.x,position.y - _pos.y,position.z - _pos.z };
			float length_reflection = sqrtf(reflection_vec.x * reflection_vec.x + reflection_vec.y * reflection_vec.y + reflection_vec.z * reflection_vec.z);
			reflection_vec.x /= length_reflection;
			reflection_vec.y /= length_reflection;
			reflection_vec.z /= length_reflection;

			//�v�Z�ɂ���ċ��܂����x�N�g�������X�̑��x�x�N�g���̑傫���ɂ���
			float length_velo = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
			velocity.x = reflection_vec.x * length_velo;
			velocity.y = reflection_vec.y * length_velo;
			velocity.z = reflection_vec.z * length_velo;

			//�����W���𑬓x�ɂ�����
			velocity.x *= -_coefficient_of_restitution;
			velocity.y *= -_coefficient_of_restitution;
			velocity.z *= -_coefficient_of_restitution;
		}

		//�\�ʂ��琂���ɓ������Ă���ꍇ
		else if (judge == 1.0f)
		{
			//�����W���𑬓x�ɂ�����
			velocity.x *= -_coefficient_of_restitution;
			velocity.y *= -_coefficient_of_restitution;
			velocity.z *= -_coefficient_of_restitution;
		}

		//���ʂ��瓖����̂͂��������̂ŃG���[����(debug���݂̂ł����Ɣ��f��������assert�֐����g�p)
		else
			assert(0 && "the value of this variable is an improbable value");
	}

	//reset
	inline void resetAllVariable()
	{
		gravity_enable = true;
		friction_enable = true;

		weight = .0f;
		max_static_friction = .0f;
		dynamic_friction = .0f;
		coefficient_of_restitution = .0f;
		position = { .0f,.0f,.0f };
		velocity = { .0f,.0f,.0f };
		accelerator = { .0f,.0f,.0f };
	}
	inline void resetAccelerator() { accelerator = { .0f,.0f,.0f }; }
	inline void resetVelocity() { velocity = { .0f,.0f,.0f }; }

	//getter
	inline float getCoefficientOfRestitution() { return coefficient_of_restitution; }
	inline float getMaxStaticFriction() { return max_static_friction; }
	inline float getDynamicFriction() { return dynamic_friction; }
	//setter

};
