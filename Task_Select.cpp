//-------------------------------------------------------------------
//ZNgV[
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Select.h"
#include  "Task_Skill.h"
#include  "Task_Game.h"

namespace  Select
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		this->UI = DG::Image::Create("./data/image/debug.png");
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
		this->UI.reset();
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
		this->nos = 2;					//2ρIπΒ\
		this->selectedNumber = 0;
		//ζΚ
		this->center = ML::Vec2(ge->screen2DWidth / 2, ge->screen2DHeight / 2);
		this->pos = this->center;
		this->posMax = this->center - ML::Vec2(100, 0);
		this->drawBase = ML::Box2D(-25, -25, 50, 50);
		this->moveVec = ML::Vec2(50, 0);

		//^XNΜΆ¬
		auto skill = Skill::Object::Create(true);

		return  true;
	}
	//-------------------------------------------------------------------
	//uIΉv^XNΑΕΙPρΎ―s€
	bool  Object::Finalize()
	{
		//f[^^XNπϊ
		ge->KillAll_G("Skill");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ψ«p¬^XNΜΆ¬
			auto next = Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[Ιs€
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		if (inp.L1.down) {
			//©gΙΑΕvΏ
			this->Kill();
		}

		//Aθ
		if (inp.SE.down)
		{
			this->pos -= moveVec;
		}
		//Dθ
		if (inp.L3.down)
		{
			this->pos += moveVec;
		}
		//Xy[Xθ
		if (inp.S1.down)
		{

		}
		//XLIπ

		//}bvIπ

	}
	//-------------------------------------------------------------------
	//uQc`ζvPt[Ιs€
	void  Object::Render2D_AF()
	{
		//UIΌ
		{
			ML::Box2D draw = drawBase;
			ML::Box2D src(0, 0, 32, 32);
			draw.Offset(this->pos);
			this->res->UI->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//ZNg\bh eZNg\bhπΔΤ
	void  Object::Select()
	{
		//ρΘ~
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
		//ρ
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
	//νIπ\bh
	void Object::SelectedWeapon()
	{
		auto inp = ge->in1->GetState();

	}
	//-------------------------------------------------------------------
	//νIπ\bh
	void Object::SelectedSkill()
	{
		auto inp = ge->in1->GetState();
	}
	//-------------------------------------------------------------------
	//νIπ\bh
	void Object::SelectedStage()
	{
		auto inp = ge->in1->GetState();
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