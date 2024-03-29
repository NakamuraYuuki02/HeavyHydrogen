//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include  "Task_Select.h"
#include  "Task_GameBG.h"
#include  "Task_Goal.h"
#include  "Task_Map2D.h"
#include  "Task_Player.h"
#include  "Task_Title.h"
#include  "Task_Ending.h"
#include  "Task_Enemy01.h"
#include  "Task_Enemy02.h"
#include  "Task_Enemy03.h"
#include  "Task_Enemy04.h"
#include  "Task_Enemy05.h"
#include  "Task_GoalMap2D.h"
#include  "Task_CharaMap2D.h"
#include  "Task_EnemyMap2D.h"
#include  "Task_UI.h"

namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		ge->camera2D = ML::Box2D(0, 0, 480, 270);

		//★タスクの生成

		//マップ(ステージ)の生成
		auto  map = Map2D::Object::Create(true);
		string mapName, mapPath;
		mapName = to_string(ge->selectedStage) + "MapStage.txt";
		mapPath = "./data/Map/" + mapName;
		map->Load(mapPath);

		//マップ(ゴール)の生成
		auto  g = GoalMap2D::Object::Create(true);
		string goalName, goalPath;
		goalName = to_string(ge->selectedStage) + "MapGoal.txt";
		goalPath = "./data/Map/" + goalName;
		g->Load(goalPath);
		g->SetGoal();

		//プレイヤの生成
		auto  p = CharaMap2D::Object::Create(true);
		string playerName, playerPath;
		playerName = to_string(ge->selectedStage) + "MapChara.txt";
		playerPath = "./data/Map/" + playerName;
		p->Load(playerPath);
		p->SetChara();

		//敵の生成
		auto  e = EnemyMap2D::Object::Create(true);
		string enemyName, enemyPath;
		enemyName = to_string(ge->selectedStage) + "MapChara.txt";
		enemyPath = "./data/Map/" + enemyName;
		e->Load(enemyPath);
		e->SetEnemy();

		//UIの生成
		auto ui = UI::Object::Create(true);

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("Main");
		ge->KillAll_G("Field");
		ge->KillAll_G("GM");
		ge->KillAll_G("Player");
		ge->KillAll_G("Enemy");
		ge->KillAll_G("Goal");
		ge->KillAll_G("Goal5");
		ge->KillAll_G("Goal6");
		ge->KillAll_G("Goal7");
		ge->KillAll_G("Goal8");
		ge->KillAll_G("Goal9");
		ge->KillAll_G("Shot(Player)");
		ge->KillAll_G("Shot(Enemy)");
		ge->KillAll_G("Sword");
		ge->KillAll_G("Axe");
		ge->KillAll_G("UI");
		ge->KillAll_G("CM");
		ge->KillAll_G("EM");

		if (!ge->QuitFlag() && this->nextTaskCreate)
		{
			//★引き継ぎタスクの生成
			if (ge->ns == MyPG::MyGameEngine::NextScene::Select)
			{
				auto next = Select::Object::Create(true);
			}
			else if(ge->ns == MyPG::MyGameEngine::NextScene::Endnig)
			{
				auto next = Ending::Object::Create(true);
			}
			else
			{
				auto next = Title::Object::Create(true);
			}
		}
			return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState( );
		if (inp.ST.down)
		{
			ge->ns = MyPG::MyGameEngine::NextScene::Title;
			//自身に消滅要請
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}