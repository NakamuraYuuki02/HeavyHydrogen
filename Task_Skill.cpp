//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Skill.h"
#include  "Task_Player.h"
#include  "Task_Sword.h"
#include  "Task_Axe.h"
#include  "Task_Shot00.h"

namespace  Skill
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/debug.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset();
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
		this->render2D_Priority[1] = 1.0f;
	
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
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(0, 0, 32, 32);
		ML::Box2D  src(0, 0, 32, 32);
		this->res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	void Object::Skill(BChara* from_)
	{
		switch (this->ss)
		{
		case SelectedSkill::JumpUp:
			//ジャンプ回数増加（最大3回）
			//from_->jumpMax++;
			break;
		case SelectedSkill::DashUp:
			//ダッシュ回数増加
			//from_->dashMax++;
			break;
		case SelectedSkill::HpUp:
			//体力増加
			//from_->hp++;
			break;
		case SelectedSkill::AtkUp:
			//攻撃力増加
			//from_->atk++;
			break;
		case SelectedSkill::Special1:
			break;
		}
	}
	//-------------------------------------------------------------------
	void Object::SkillImage()
	{

	}
	//-------------------------------------------------------------------
	//武器の特殊強化
	void Object::WeaponSpecial1(Weapon weapon_)
	{
		auto   player = ge->GetTasks<Player::Object>(Player::defGroupName, Player::defName);
		auto   axe = ge->GetTask<Axe::Object>(Axe::defGroupName, Axe::defName);
		switch (this->weapon)
		{
		case Weapon::Sword:
			//剣の特殊強化1

			break;
		case Weapon::Axe:
			//斧の特殊強化1
			for (int i = 1; i <= 3; ++i)
			{
				axe->moveVec = ML::Vec2(-7, -3 * i);
				//axe->pos = player->pos + ML::Vec2(-30, 0);
				/*auto axe = Axe::Object::Create(true);
				axe->Level(this);
				axe->moveVec = ML::Vec2(-7, -3 * i);
				axe->pos = this->pos + ML::Vec2(-30, 0);*/
			}
			break;
		case Weapon::Gun:
			//銃の特殊強化1
			for (int i = 1; i <= 3; ++i)
			{
				/*auto gun = Shot00::Object::Create(true);
				gun->Level(this);
				gun->moveVec = ML::Vec2(-8, 0);
				gun->pos = this->pos + ML::Vec2(-20 * i, 0);*/
			}
			break;
		}
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