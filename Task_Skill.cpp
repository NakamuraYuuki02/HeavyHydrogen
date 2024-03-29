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
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/debug.png");
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
		this->img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//uϊ»v^XNΆ¬ΙPρΎ―s€
	bool  Object::Initialize()
	{
		//X[p[NXϊ»
		__super::Initialize(defGroupName, defName, true);
		//\[XNXΆ¬or\[X€L
		this->res = Resource::Create();

		//f[^ϊ»
		this->render2D_Priority[1] = 1.0f;
	
		//^XNΜΆ¬

		return  true;
	}
	//-------------------------------------------------------------------
	//uIΉv^XNΑΕΙPρΎ―s€
	bool  Object::Finalize()
	{
		//f[^^XNπϊ


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ψ«p¬^XNΜΆ¬
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[Ιs€
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//uQc`ζvPt[Ιs€
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
			//WvρΑiΕε3ρj
			//from_->jumpMax++;
			break;
		case SelectedSkill::DashUp:
			//_bVρΑ
			//from_->dashMax++;
			break;
		case SelectedSkill::HpUp:
			//ΜΝΑ
			//from_->hp++;
			break;
		case SelectedSkill::AtkUp:
			//UΝΑ
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
	//νΜΑκ­»
	void Object::WeaponSpecial1(Weapon weapon_)
	{
		auto   player = ge->GetTasks<Player::Object>(Player::defGroupName, Player::defName);
		auto   axe = ge->GetTask<Axe::Object>(Axe::defGroupName, Axe::defName);
		switch (this->weapon)
		{
		case Weapon::Sword:
			//ΜΑκ­»1

			break;
		case Weapon::Axe:
			//ΜΑκ­»1
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
			//eΜΑκ­»1
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

	//
	//ΘΊΝξ{IΙΟXsvΘ\bh
	//
	//-------------------------------------------------------------------
	//^XNΆ¬ϋ
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWΙo^
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYΙΈs΅½ηKill
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
	//\[XNXΜΆ¬
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