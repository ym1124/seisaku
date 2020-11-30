#pragma once

class nuaturalNumber
{
public:
	nuaturalNumber() :m_num(0) {}
	~nuaturalNumber() {}

	//初期値の設定
	void initNum(const int num) { m_num = num; }

	void addNum() { m_num += 1; }
	void addNum(const int num) { m_num += num; }

	void subNum() { if (!isZero())m_num -= 1; }
	void subNum(const int num)
	{
		if (!isZero()) m_num -= num;
		if (m_num < 0)m_num = 0;
	}

	//数が0かどうか確かめる
	bool isZero() { return (m_num == 0); }	//数が0ならtrue

public://getter,setter
	   //数の取得
	const int getNum() { return m_num; }

private:
	int m_num;
};

//数管理クラス
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