//-------------------------------------------------------------------
//セレクトシーン
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Select.h"
#include  "Task_Title.h"
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
		this->selectedNumber = 0;
		this->moveVec = ML::Vec2(75, 0);												//移動力
		int x = ge->screen2DWidth / 2;
		int y = ge->screen2DHeight / 2;
		this->posMin = ML::Vec2(x - moveVec.x * 2, y);									//画面中央から x - moveVec.x * 2
		this->pos = this->posMin;														//初期値はMin
		this->posMax = this->posMin + moveVec * 4;										//Minから x + moveVec * 4
		this->drawBase = ML::Box2D(-25, -25, 50, 50);
		this->state = SelectionState::Before;
		for (int i = 0; i < 3; ++i)
		{
			//クリアしたステージの次ステージを三つ取得
			this->stage[i] = ge->eapsedStage + i + 1;
		}
		
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("Select");

		if (!ge->QuitFlag() && this->nextTaskCreate)
		{
			//★引き継ぎタスクの生成
			/*switch (ge->ns)
			{
			case MyPG::MyGameEngine::NextScene::Title:
				auto title = Title::Object::Create(true);
				break;
			case MyPG::MyGameEngine::NextScene::Game:
				auto game = Game::Object::Create(true);
				break;
			}*/
			if (ge->ns == MyPG::MyGameEngine::NextScene::Game)
			{
				auto next = Game::Object::Create(true);
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
		auto inp = ge->in1->GetState();
		
		Select();

		if (inp.ST.down)
		{
			//タイトルにもどる。
			ge->ns = MyPG::MyGameEngine::NextScene::Title;
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
			break;
		case SelectionState::Stage:
			//ステージ選択
			break;
		case SelectionState::After:
			//選択後
			break;
		}
	}
	//-------------------------------------------------------------------
	//State更新メソッド
	bool Object::UpDateSelectionState(SelectionState state_)
	{
		//更新されていない場合は処理をしない
		if (state_ == this->state)
		{
			return false;
		}
		else
		{
			//ステートを更新
			this->state = state_;
			//選択値をリセット
			this->selectedNumber = 0;
			//UI表示位置をずらすため各値を設定
			switch (this->state)
			{
			case SelectionState::Before:
				//選択前
				break;
			case SelectionState::Weapon:
				//武器選択
				this->posMin = ML::Vec2(ge->screen2DWidth / 2 - moveVec.x * 1, ge->screen2DHeight / 2);
				this->pos = this->posMin;
				this->posMax = this->posMin + moveVec * 2;
				break;
			case SelectionState::Skill:
				//スキル選択
				this->posMin = ML::Vec2(ge->screen2DWidth / 2 - moveVec.x * 2, ge->screen2DHeight / 2);
				this->pos = this->posMin;
				this->posMax = this->posMin + moveVec * 4;
				this->s = MyPG::MyGameEngine::SelectedSkill::Non;
				//GetRandom(0, 1);
				break;
			case SelectionState::Stage:
				//ステージ選択
				//初回以降
				if (ge->stageNum>0)
				{
					this->posMin = ML::Vec2(ge->screen2DWidth / 2 - moveVec.x * 1, ge->screen2DHeight / 2);
					this->pos = this->posMin;
					this->posMax = this->posMin + moveVec * 2;
				}
				//初回
				else
				{
					this->posMin = ML::Vec2(ge->screen2DWidth / 2, ge->screen2DHeight / 2);
					this->pos = this->posMin;
					this->posMax = this->posMin;
				}
				break;
			case SelectionState::After:
				//選択後
				break;
			}
		}
	}
	//-------------------------------------------------------------------
	//セレクトメソッド 各セレクトメソッドを呼ぶ
	void  Object::Select()
	{
		Object::SelectionState sstate = this->state;
		//初回以降
		if (ge->stageNum > 0)
		{
			//  Before		//選択前
			//  Weapon		//武器選択
			// 	Skill		//スキル選択
			//	Stage		//ステージ選択
			//	After		//選択後
			switch (sstate)
			{
			case SelectionState::Before:
				//選択前
				sstate = SelectionState::Weapon;
				break;
			case SelectionState::Weapon:
				//武器選択
				//初回以降処理なし
				sstate = SelectionState::Skill;
				break;
			case SelectionState::Skill:
				//スキル選択
				if (SelectSkill())
				{
					sstate = SelectionState::Stage;
				}
				break;
			case SelectionState::Stage:
				//ステージ選択
				if (SelectStage())
				{
					sstate = SelectionState::After;
				}
				break;
			case SelectionState::After:
				//選択後
				ge->ns = MyPG::MyGameEngine::NextScene::Game;
				//自身に消滅要請
				this->Kill();
				break;
			}
		}
		//初回
		else
		{
			switch (sstate)
			{
			case SelectionState::Before:
				//選択前
				sstate = SelectionState::Weapon;
				break;
			case SelectionState::Weapon:
				//武器選択
				if (SelectWeapon())
				{
					sstate = SelectionState::Skill;
				}
				break;
			case SelectionState::Skill:
				//スキル選択
				if (SelectSkill())
				{
					sstate = SelectionState::Stage;
				}
				break;
			case SelectionState::Stage:
				//ステージ選択
				if (SelectStageFirst())
				{
					sstate = SelectionState::After;
				}
				break;
			case SelectionState::After:
				//選択後
				ge->ns = MyPG::MyGameEngine::NextScene::Game;
				//自身に消滅要請
				this->Kill();
				break;
			}
		}
		UpDateSelectionState(sstate);
	}
	//-------------------------------------------------------------------
	//武器選択メソッド
	bool Object::SelectWeapon()
	{
		auto inp = ge->in1->GetState();
		bool r = false;
		
		//A左移動
		if (inp.SE.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//D右移動
		if (inp.L3.down && this->pos.x < this->posMax.x)
		{
			this->pos += moveVec;
			++this->selectedNumber;
		}
		//スペース決定
		if (inp.S1.down)
		{
			//武器を設定
			switch (this->selectedNumber)
			{
			case 0:
				ge->sw = MyPG::MyGameEngine::SelectedWeapon::Sword;
				break;
			case 1:
				ge->sw = MyPG::MyGameEngine::SelectedWeapon::Axe;
				break;
			case 2:
				ge->sw = MyPG::MyGameEngine::SelectedWeapon::Gun;
				break;
			}
			r = true;
		}
		return r;
	}
	//-------------------------------------------------------------------
	//スキル選択メソッド
	bool Object::SelectSkill()
	{
		auto inp = ge->in1->GetState();
		bool r = false;

		//A左移動
		if (inp.SE.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//D右移動
		if (inp.L3.down && this->pos.x < this->posMax.x)
		{
			this->pos += moveVec;
			++this->selectedNumber;
		}
		//スペース決定
		if (inp.S1.down)
		{
			//snをssへ変換
			switch (this->selectedNumber)
			{
			case 0:
				this->s = MyPG::MyGameEngine::SelectedSkill::JumpUp;
				break;
			case 1:
				this->s = MyPG::MyGameEngine::SelectedSkill::DashUp;
				break;
			case 2:
				this->s = MyPG::MyGameEngine::SelectedSkill::HpUp;
				break;
			case 3:
				this->s = MyPG::MyGameEngine::SelectedSkill::AtkUp;
				break;
			case 4:
				//sp1 or sp2
				/*if ()
				{
				}*/
				break;
			}
			ge->ss[0] = this->s;
			r = true;
		}
		return r;
	}
	//-------------------------------------------------------------------
	//ステージ選択メソッド
	bool Object::SelectStage()
	{
		auto inp = ge->in1->GetState();
		bool r = false;

		//A左移動
		if (inp.SE.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//D右移動
		if (inp.L3.down && this->pos.x < this->posMax.x)
		{
			this->pos += moveVec;
			++this->selectedNumber;
		}
		//スペース決定
		if (inp.S1.down)
		{
			ge->selectedStage = this->stage[this->selectedNumber]; 
			r = true;
		}
		return r;
	}
	//-------------------------------------------------------------------
	//初回ステージ選択メソッド
	bool Object::SelectStageFirst()
	{
		auto inp = ge->in1->GetState();
		bool r = false;

		//スペース決定
		if (inp.S1.down)
		{
			ge->selectedStage = 0;
			r = true;
		}
		return r;
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