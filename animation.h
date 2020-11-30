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

	//再生するアニメーションを設定
	bool set(const char* animationName);

	bool isPlay() const { return m_current_animation >= 0; }
	//void Play(int animation_index, bool loop = false);
	//更新
	bool update(model* model, bool loop = false);

	//アニメーションの追加
	bool add(model* model, const char* fbxFileName, const char* animationName);
	bool add(const char* animationName);

	//ボーンブレンドしてる最中かどうか
	bool getBlendMode() { return m_blend_mode; }

	//アニメーションをとめる
	void stop() { m_pause = true; }

	//アニメーションの再生
	void play() { m_pause = false; }

	//アニメーションの再生速度を倍率によって設定する(初期値1.0f/60.0f * 1.0f)
	void playSpeedMagnification(const float magnification) { m_playSpeed = (1.0f / 60.0f) * magnification; }

	//現在のアニメーションかどうか
	bool isCurrentAnimation(const char* animationName);

	//現在のアニメーションの取得
	std::string getCurrentAnimation();

private:
	int		m_animID = -1;					//アニメーション番号
	int		m_current_animation = -1;		//現在のアニメーション
	int		m_prev_current_animation = -1;	//1フレーム前のアニメーション

	int		m_blend_seconds = 0;

	float	m_current_seconds = 0.0f;
	bool	m_loop_animation = false;		//アニメーションをループさせるかどうか
	bool	m_end_animation = false;		//アニメーションが終わったかどうか

	bool	m_blend_mode = false;	//ボーンブレンドしてる最中ならtrue

	bool	m_pause = false;		//一時停止させるかどうか　trueなら一時停止する

	float	m_playSpeed = 1.0f / 60.0f;		//アニメーションの速さ

											//std::less<>をつけると文字列リテラルをfindやlower_bound,upper_boundなどのアルゴリズムに渡したときの
											//コピーを無くすことができる

	std::map<std::string, int, std::less<>> m_index; //アニメーションの名前と番号の関連付け
};