#pragma once
#pragma once

#include <map>
#include <string>

class model;

class animation
{
public:
	animation(const char* animationName);
	~animation() {}

	//�Đ�����A�j���[�V������ݒ�
	bool set(const char* animationName);

	bool isPlay() const { return m_current_animation >= 0; }
	//void Play(int animation_index, bool loop = false);
	//�X�V
	bool update(model* model, bool loop = false);

	//�A�j���[�V�����̒ǉ�
	bool add(model* model, const char* fbxFileName, const char* animationName);
	bool add(const char* animationName);

	//�{�[���u�����h���Ă�Œ����ǂ���
	bool getBlendMode() { return m_blend_mode; }

	//�A�j���[�V�������Ƃ߂�
	void stop() { m_pause = true; }

	//�A�j���[�V�����̍Đ�
	void play() { m_pause = false; }

	//�A�j���[�V�����̍Đ����x��{���ɂ���Đݒ肷��(�����l1.0f/60.0f * 1.0f)
	void playSpeedMagnification(const float magnification) { m_playSpeed = (1.0f / 60.0f) * magnification; }

	//���݂̃A�j���[�V�������ǂ���
	bool isCurrentAnimation(const char* animationName);

	//���݂̃A�j���[�V�����̎擾
	std::string getCurrentAnimation();

private:
	int		m_animID = -1;					//�A�j���[�V�����ԍ�
	int		m_current_animation = -1;		//���݂̃A�j���[�V����
	int		m_prev_current_animation = -1;	//1�t���[���O�̃A�j���[�V����

	int		m_blend_seconds = 0;

	float	m_current_seconds = 0.0f;
	bool	m_loop_animation = false;		//�A�j���[�V���������[�v�����邩�ǂ���
	bool	m_end_animation = false;		//�A�j���[�V�������I��������ǂ���

	bool	m_blend_mode = false;	//�{�[���u�����h���Ă�Œ��Ȃ�true

	bool	m_pause = false;		//�ꎞ��~�����邩�ǂ����@true�Ȃ�ꎞ��~����

	float	m_playSpeed = 1.0f / 60.0f;		//�A�j���[�V�����̑���

											//std::less<>������ƕ����񃊃e������find��lower_bound,upper_bound�Ȃǂ̃A���S���Y���ɓn�����Ƃ���
											//�R�s�[�𖳂������Ƃ��ł���

	std::map<std::string, int, std::less<>> m_index; //�A�j���[�V�����̖��O�Ɣԍ��̊֘A�t��
};