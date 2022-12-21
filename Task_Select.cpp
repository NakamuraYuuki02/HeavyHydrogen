//-------------------------------------------------------------------
//セレクトシーン
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Select.h"
#include  "Task_Skill.h"
#include  "Task_Game.h"

namespace  Select
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->UI = DG::Image::Create("./data/image/debug.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->UI.reset();
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
		this->nos = 2;					//2回選択可能
		this->selectedNumber = 0;
		//画面中央
		this->center = ML::Vec2(ge->screen2DWidth / 2, ge->screen2DHeight / 2);
		this->pos = this->center;
		this->posMax = this->center - ML::Vec2(100, 0);
		this->drawBase = ML::Box2D(-25, -25, 50, 50);
		this->moveVec = ML::Vec2(50, 0);

		//★タスクの生成
		auto skill = Skill::Object::Create(true);

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("Skill");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto next = Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		if (inp.L1.down) {
			//自身に消滅要請
			this->Kill();
		}

		//A決定
		if (inp.SE.down)
		{
			this->pos -= moveVec;
		}
		//D決定
		if (inp.L3.down)
		{
			this->pos += moveVec;
		}
		//スペース決定
		if (inp.S1.down)
		{

		}
		//スキル選択処理

		//マップ選択処理

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//UI仮
		{
			ML::Box2D draw = drawBase;
			ML::Box2D src(0, 0, 32, 32);
			draw.Offset(this->pos);
			this->res->UI->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//セレクトメソッド 各セレクトメソッドを呼ぶ
	void  Object::Select()
	{
		//初回以降
		if (stageNum > 0)
		{
			switch (this->ss)
			{
			case SelectionState::Skill:
				SelectedSkill();
				break;
			case SelectionState::Stage:
				SelectedStage();
				break;
			}
		}
		//初回
		else
		{
			switch (this->ss)
			{
			case SelectionState::Weapon:
				SelectedWeapon();
				break;
			case SelectionState::Skill:
				SelectedSkill();
				break;
			case SelectionState::Stage:
				SelectedStage();
				break;
			}
		}
	}
	//-------------------------------------------------------------------
	//武器選択メソッド
	void Object::SelectedWeapon()
	{
		auto inp = ge->in1->GetState();

	}
	//-------------------------------------------------------------------
	//武器選択メソッド
	void Object::SelectedSkill()
	{
		auto inp = ge->in1->GetState();
	}
	//-------------------------------------------------------------------
	//武器選択メソッド
	void Object::SelectedStage()
	{
		auto inp = ge->in1->GetState();
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