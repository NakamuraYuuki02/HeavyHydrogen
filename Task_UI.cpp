//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_UI.h"

namespace  UI
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		this->UIhp = DG::Image::Create("./data/image/heart2.png");
		this->UInum = DG::Image::Create("./data/image/UItext.png");
		this->gun = DG::Image::Create("./data/image/Gun.png");
		this->sword = DG::Image::Create("./data/image/Sword.png");
		this->axe = DG::Image::Create("./data/image/Axe.png");
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
		this->UIhp.reset();
		this->UInum.reset();
		this->gun.reset();
		this->sword.reset();
		this->axe.reset();
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
		this->weapon = ge->sw;
		
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
		//hpͺ(ϊl3)n[gπ\¦
		for (int i = 0; i < ge->hp; ++i)
		{
			ML::Box2D hpDraw(i * 22, 0, 20, 20);
			ML::Box2D hpSrc(0, 0, 12, 12);
			this->res->UIhp->Draw(hpDraw, hpSrc);
		}

		//νACR
		switch (this->weapon)
		{
		case MyPG::MyGameEngine::SelectedWeapon::Sword:
		    {
			  ML::Box2D SwordDraw(20, 30, 28, 28);
			  ML::Box2D SwordSrc(0, 0, 16, 16);
			  this->res->sword->Draw(SwordDraw, SwordSrc);
			  break;
		    }
		case MyPG::MyGameEngine::SelectedWeapon::Axe:
		    { 
			  ML::Box2D AxeDraw(20, 30, 26, 26);
			  ML::Box2D AxeSrc(0, 0, 14, 14);
			  this->res->axe->Draw(AxeDraw, AxeSrc);
			  break;
			}
		case MyPG::MyGameEngine::SelectedWeapon::Gun:
		    { 
			  ML::Box2D GunDraw(20, 30, 45, 30);
			  ML::Box2D GunSrc(0, 0, 15, 10);
			  this->res->gun->Draw(GunDraw, GunSrc);
			  break;
			}
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