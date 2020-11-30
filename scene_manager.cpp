
//#include "scenes.h"
//#include "multi_thread.h"
#include "framework.h"
#include "scenes.h"
#include "scene_manager.h"


//**********************************************
//
//		シーンの管理
//
//**********************************************

//マルチスレッド版
//void scene_manager::Update(float elapsedTime)
//{
//	currentScene->Update(elapsedTime);
//}
//
//void scene_manager::Render(float elapsedTime)
//{
//	currentScene->Render(elapsedTime);
//
//}
//
//void scene_manager::ChangeScene(scene_load* scene)
//{
//	//ManagerのcurrentにLoadのシーンを入れる(LoadSceneのUpdateとRenderを行うため)
//	//currentScene.release();
//	currentScene.reset(scene);
//	
//	//currentScene = scene;
//
//	if (!scene->nextScene->initialized)//次のシーンが初期化されていなければ
//		scene->Initialize();//ここで別スレッドを立てる
//}

//通常版
void scene_manager::Update(float elapsedTime)
{
	currentScene->Update(elapsedTime);
}

void scene_manager::Render(float elapsedTime)
{
	currentScene->Render(elapsedTime);
}

void scene_manager::ChangeScene(int scene_num)
{
	//ManagerのcurrentにLoadのシーンを入れる(LoadSceneのUpdateとRenderを行うため)
	//currentScene.release();

	//ptr引数から変更2020/7/15

	if (currentScene)
	{
		delete currentScene;
		currentScene = nullptr;
	}

	switch (scene_num)
	{
	case 1:
		currentScene = new scene71;
		break;
	case 2:
		currentScene = new scene62;
		break;
	case 3:
		currentScene = new scene45;
		break;
	case 4:
		currentScene = new scene72;
		break;
	case 5:
		currentScene = new scene66;
		break;
	case 6:
		currentScene = new scene36;
		break;
	case 7:
		currentScene = new scene16;
		break;
	case 8:
		currentScene = new scene61;
		break;
	case 9:
		currentScene = new scene60;
		break;
	case 11:
		currentScene = new scene59;
		break;


		//case SELECT_SCENE::FIREWORK:
		//	currentScene = new fire_work;
		//	break;
		//case SELECT_SCENE::TEST:
		//	currentScene = new test;
		//	break;
			//case SELECT_SCENE::TITLE:
			//	currentScene = new scene_title;
			//	break;
			//case SELECT_SCENE::MAIN:
			//	currentScene = new scene_main;
			//	break;
			//case SELECT_SCENE::S1:
			//	currentScene = new scene1;
			//	break;
			//case SELECT_SCENE::S2:
			//	currentScene = new scene2;
			//	break;
				//case SELECT_SCENE::S5:
				//	currentScene = new scene5;
				//	break;
				//case SELECT_SCENE::S7:
				//	currentScene = new scene7;
				//	break;
				//case SELECT_SCENE::S8:
				//	currentScene = new scene8;
				//	break;
				//case SELECT_SCENE::S10:
				//	currentScene = new scene10;
				//	break;
				//case SELECT_SCENE::S12:
				//	currentScene = new scene12;
				//	break;
				//case SELECT_SCENE::S13:
				//	currentScene = new scene13;
				//	break;
				//case SELECT_SCENE::S14:
				//	currentScene = new scene14;
				//	break;
	//case SELECT_SCENE::S15:
	//	currentScene = new scene15;
	//	break;
	//case SELECT_SCENE::S16:
	//	currentScene = new scene16;
	//	break;
	//case SELECT_SCENE::S17:
	//	currentScene = new scene17;
	//	break;
		//case SELECT_SCENE::S21:
		//	currentScene = new scene21;
		//	break;
		//case SELECT_SCENE::S22:
		//	currentScene = new scene22;
		//	break;
		//case SELECT_SCENE::S24:
		//	currentScene = new scene24;
		//	break;
		//case SELECT_SCENE::S25:
		//	currentScene = new scene25;
		//	break;
		//case SELECT_SCENE::S26:
		//	currentScene = new scene26;
		//	break;
		//case SELECT_SCENE::S27:
		//	currentScene = new scene27;
		//	break;
		//case SELECT_SCENE::S28:
		//	currentScene = new scene28;
		//	break;
/*		case SELECT_SCENE::S29:
			currentScene = new scene29;
			break;*/
			/*case SELECT_SCENE::S30:
				currentScene = new scene30;
				break;*/
				//case SELECT_SCENE::S31:
				//	currentScene = new scene31;
				//	break;
				//	case SELECT_SCENE::S32:
				//		currentScene = new scene32;
				//		break;
						//case SELECT_SCENE::S33:
						//	currentScene = new scene33;
						//	break;
						//case SELECT_SCENE::S34:
						//	currentScene = new scene34;
						//	break;
				//case SELECT_SCENE::S35:
				//	currentScene = new scene35;
				//	break;
			/*	case SELECT_SCENE::S36:
					currentScene = new scene36;
					break;*/
					//case SELECT_SCENE::S37:
					//	currentScene = new scene37;
					//	break;
					//case SELECT_SCENE::S38:
					//	currentScene = new scene38;
					//	break;
					//case SELECT_SCENE::S39:
					//	currentScene = new scene39;
					//	break;
						//case SELECT_SCENE::S40:
						//	currentScene = new scene40;
						//	break;
					//case SELECT_SCENE::S41:
					//	currentScene = new scene41;
					//	break;
						//case SELECT_SCENE::S42:
						//	currentScene = new scene42;
						//	break;
						//case SELECT_SCENE::S43:
						//	currentScene = new scene43;
						//	break;
						/*case SELECT_SCENE::S44:
							currentScene = new scene44;
							break;*/
	//case SELECT_SCENE::S45:
	//	currentScene = new scene45;
	//	break;
		/*case SELECT_SCENE::S46:
			currentScene = new scene46;
			break;*/
			//case SELECT_SCENE::S47:
			//	currentScene = new scene47;
			//	break;
			//case SELECT_SCENE::S48:
			//	currentScene = new scene48;
			//	break;
			//case SELECT_SCENE::S49:
			//	currentScene = new scene49;
			//	break;
				//case SELECT_SCENE::S50:
				//	currentScene = new scene50;
				//	break;
		/*		case SELECT_SCENE::S51:
					currentScene = new scene51;
					break;*/
					//case SELECT_SCENE::S52:
					//	currentScene = new scene52;
					//	break;
					//case SELECT_SCENE::S53:
					//	currentScene = new scene53;
					//	break;
														/*case SELECT_SCENE::S54:
															currentScene = new scene54;
															break;
														case SELECT_SCENE::S55:
															currentScene = new scene55;
															break;*/
															//case SELECT_SCENE::S56:
															//	currentScene = new scene56;
															//	break; 
															//case SELECT_SCENE::S57:
															//	currentScene = new scene57;
															//	break;
															//case SELECT_SCENE::S58:
															//	currentScene = new scene58;
															//	break;
	//														case SELECT_SCENE::S59:
	//															currentScene = new scene59;
	//															break;
	//case SELECT_SCENE::S60:
	//	currentScene = new scene60;
	//	break;
	//case SELECT_SCENE::S61:
	//	currentScene = new scene61;
	//	break;
	//case SELECT_SCENE::S62:
	//	currentScene = new scene62;
	//	break;
		//case SELECT_SCENE::S63:
		//	currentScene = new scene63;
		//	break;
	//case SELECT_SCENE::S64:
	//	currentScene = new scene64;
	//	break;
	//case SELECT_SCENE::S65:
	//	currentScene = new scene65;
	//	break;
	//case SELECT_SCENE::S66:
	//	currentScene = new scene66;
	//	break;
	//case SELECT_SCENE::S67:
	//	currentScene = new scene67;
	//	break;
	//case SELECT_SCENE::S68:
	//	currentScene = new scene68;
	//	break;
	//case SELECT_SCENE::S69:
	//	currentScene = new scene69;
	//	break;
	//case SELECT_SCENE::S70:
	//	currentScene = new scene70;
	//	break;
	//case SELECT_SCENE::S71:
	//	currentScene = new scene71;
	//	break;
	//case SELECT_SCENE::S72:
	//	currentScene = new scene72;
	//	break;
	case SELECT_SCENE::S73:
		currentScene = new scene73;
		break;
	case SELECT_SCENE::S74:
		currentScene = new scene74;
		break;
	case SELECT_SCENE::S75:
		currentScene = new scene75;
		break;
	}
	currentScene->Initialize();
}

void scene_manager::SceneJump()
{
#if USE_IMGUI
	ImGui::Begin("Scene Select");

	if (ImGui::TreeNode("scene status"))
	{
		const char *str[] = { "tessellation", "path tracing", "ssao", "ssr","volume light","pbr","gpu particle","dof","sea raymarching"};
		bool flg = ImGui::ListBox("demo", &status->demo_num, str, IM_ARRAYSIZE(str));
		//基本のライト
		if (flg)
		{
			Light::ResetLight();
			ChangeScene(status->demo_num + 1);
		}
		ImGui::TreePop();
	}
	ImGui::End();
#endif

#if !USE_IMGUI
	if (GetAsyncKeyState(VK_F1) & 1)
	{
		Light::ResetLight();
		ChangeScene(1);
	}
	if (GetAsyncKeyState(VK_F2) & 1)
	{
		Light::ResetLight();
		ChangeScene(2);
	}
	if (GetAsyncKeyState(VK_F3) & 1)
	{
		Light::ResetLight();
		ChangeScene(3);
	}
	if (GetAsyncKeyState(VK_F4) & 1)
	{
		Light::ResetLight();
		ChangeScene(4);
	}
	if (GetAsyncKeyState(VK_F5) & 1)
	{
		Light::ResetLight();
		ChangeScene(5);
	}
	if (GetAsyncKeyState(VK_F6) & 1)
	{
		Light::ResetLight();
		ChangeScene(6);
	}
	if (GetAsyncKeyState(VK_F7) & 1)
	{
		Light::ResetLight();
		ChangeScene(7);
	}
	if (GetAsyncKeyState(VK_F8) & 1)
	{
		Light::ResetLight();
		ChangeScene(8);
	}
	if (GetAsyncKeyState(VK_F9) & 1)
	{
		Light::ResetLight();
		ChangeScene(9);
	}
	if (GetAsyncKeyState(VK_F11) & 1)
	{
		Light::ResetLight();
		ChangeScene(11);
	}
#endif
}