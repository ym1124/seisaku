#pragma once

class nuaturalNumber
{
public:
	nuaturalNumber() :m_num(0) {}
	~nuaturalNumber() {}

	//�����l�̐ݒ�
	void initNum(const int num) { m_num = num; }

	void addNum() { m_num += 1; }
	void addNum(const int num) { m_num += num; }

	void subNum() { if (!isZero())m_num -= 1; }
	void subNum(const int num)
	{
		if (!isZero()) m_num -= num;
		if (m_num < 0)m_num = 0;
	}

	//����0���ǂ����m���߂�
	bool isZero() { return (m_num == 0); }	//����0�Ȃ�true

public://getter,setter
	   //���̎擾
	const int getNum() { return m_num; }

private:
	int m_num;
};

//���Ǘ��N���X
class numManager
{
public:
	nuaturalNumber karage;
	nuaturalNumber plBullet;

	static numManager* getInstance()
	{
		static numManager instance;
		return &instance;
	}

private:
	numManager() {}
	~numManager() {}
};

#define pNumManager numManager::getInstance()