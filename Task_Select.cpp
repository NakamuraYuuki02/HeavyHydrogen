//-------------------------------------------------------------------
//セレクトシーン
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Select.h"
#include  "Task_Game.h"

#include  "randomLib.h"

namespace  Select
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->selectUI = DG::Image::Create("./data/image/debug.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->selectUI.reset();
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
		this->moveVec = ML::Vec2(75, 0);												//移動力
		int x = ge->screen2DWidth / 2;
		int y = ge->screen2DHeight / 2;
		this->posMin = ML::Vec2(x - moveVec.x * 2, y);									//画面中央から x - moveVec.x * 2
		this->pos = this->posMin;														//初期値はMin
		this->posMax = this->posMin + moveVec * 4;								        //Minから x + moveVec * 4
		this->drawBase = ML::Box2D(-25, -25, 50, 50);
		

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

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
		
		if (inp.ST.down)
		{
			//自身に消滅要請
			this->Kill();
		}
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
			this->res->selectUI->Draw(draw, src);
		}
		switch (this->state)
		{
		case SelectionState::Before:
			//選択前
			break;
		case SelectionState::Weapon:
			//武器選択
			break;
		case SelectionState::Skill:
			//スキル選択
			SelectedSkill();
			break;
		case SelectionState::Stage:
			//ステージ選択
			SelectedStage();
			break;
		case SelectionState::After:
			//選択後
			break;
		}
	}
	//-------------------------------------------------------------------
	//セレクトメソッド 各セレクトメソッドを呼ぶ
	void  Object::Select()
	{
		//初回以降
		if (ge->stageNum > 0)
		{
			//  Before		//選択前
			//  Weapon		//武器選択
			// 	Skill		//スキル選択
			//	Stage		//ステージ選択
			//	After		//選択後
			switch (this->state)
			{
			case SelectionState::Before:
				//選択前
				break;
			case SelectionState::Weapon:
				//武器選択
				//初回以降処理なし
				break;
			case SelectionState::Skill:
				//スキル選択
				SelectedSkill();
				break;
			case SelectionState::Stage:
				//ステージ選択
				SelectedStage();
				break;
			case SelectionState::After:
				//選択後
				break;
			}
		}
		//初回
		else
		{
			switch (this->state)
			{
			case SelectionState::Before:
				//選択前
				break;
			case SelectionState::Weapon:
				//武器選択
				SelectedWeapon();
				break;
			case SelectionState::Skill:
				//スキル選択
				SelectedSkill();
				break;
			case SelectionState::Stage:
				//ステージ選択
				SelectedStage();
				break;
			case SelectionState::After:
				//選択後
				break;
			}
		}
	}
	//-------------------------------------------------------------------
	//武器選択メソッド
	void Object::SelectWeapon()
	{
		auto inp = ge->in1->GetState();

	}
	//-------------------------------------------------------------------
	//スキル選択メソッド
	void Object::SelectSkill()
	{
		auto inp = ge->in1->GetState();
		this->pos = this->posMin;
		this->selectedNumber = 0;
		
		//A左移動
		if (inp.SE.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			++this->selectedNumber;
		}
		//D右移動
		if (inp.L3.down && this->pos.x < this->posMax.x)
		{
			this->pos += moveVec;
			--this->selectedNumber;
		}

		//スペース決定
		if (inp.S1.down)
		{
			//snをssへ変換
			switch (this->selectedNumber)
			{
			case 0:
				this->s = SelectedSkill::JumpUp;
				break;
			case 1:
				this->s = SelectedSkill::DashUp;
				break;
			case 2:
				this->s = SelectedSkill::HpUp;
				break;
			case 3:
				this->s = SelectedSkill::AtkUp;
				break;
			case 4:
				//sp1 or sp2
				/*if ()
				{

				}*/
				break;
			}

		}
	}
	//-------------------------------------------------------------------
	//ステージ選択メソッド
	void Object::SelectStage()
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