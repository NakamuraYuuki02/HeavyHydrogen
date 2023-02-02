//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_UI.h"

namespace  UI
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->UIhp = DG::Image::Create("./data/image/heart2.png");
		this->UInum = DG::Image::Create("./data/image/UItext.png");
		this->gun = DG::Image::Create("./data/image/handgun2.png");
		this->sword = DG::Image::Create("./data/image/Sword.png");
		this->axe = DG::Image::Create("./data/image/Axe.png");
		this->tutorial = DG::Image::Create("./data/image/tutorial2.png");
		this->command = DG::Image::Create("./data/image/command2.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->UIhp.reset();
		this->UInum.reset();
		this->gun.reset();
		this->sword.reset();
		this->axe.reset();
		this->tutorial.reset();
		this->command.reset();
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
		this->weapon = ge->sw;
		
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
		ML::Box2D cDraw(0, 235, 207, 32);
		ML::Box2D cSrc(0, 0, 207, 32);
		this->res->command->Draw(cDraw, cSrc);

		auto  inp = ge->in1->GetState();
		//チュートリアル
		if (true == inp.B1.on)//Zキー押したら表示
		{
			ML::Box2D tDraw(ge->screen2DWidth / 2 - 100, ge->screen2DHeight / 2 - 100, 320, 192);
			ML::Box2D tSrc(0, 0, 320, 192);
			this->res->tutorial->Draw(tDraw, tSrc);
		}

		//hp分(初期値3)ハートを表示
		for (int i = 0; i < ge->hp; ++i)
		{
			ML::Box2D hpDraw(i * 22, 0, 20, 20);
			ML::Box2D hpSrc(0, 0, 12, 12);
			this->res->UIhp->Draw(hpDraw, hpSrc);
		}

		//武器アイコン
		switch (this->weapon)
		{
		case MyPG::MyGameEngine::SelectedWeapon::Sword:
		    {
			  ML::Box2D SwordDraw(20, 30, 28, 28);
			  ML::Box2D SwordSrc(0, 0, 16, 16);
			  this->res->sword->Rotation(0, ML::Vec2(0, 0));
			  this->res->sword->Draw(SwordDraw, SwordSrc);
			  break;
		    }
		case MyPG::MyGameEngine::SelectedWeapon::Axe:
		    { 
			  ML::Box2D AxeDraw(20, 30, 26, 26);
			  ML::Box2D AxeSrc(0, 0, 14, 14);
			  this->res->axe->Rotation(0, ML::Vec2(0, 0));
			  this->res->axe->Draw(AxeDraw, AxeSrc);
			  break;
			}
		case MyPG::MyGameEngine::SelectedWeapon::Gun:
		    { 
			  ML::Box2D GunDraw(20, 30, 45, 30);
			  ML::Box2D GunSrc(0, 0, 51, 30);
			  this->res->gun->Rotation(0, ML::Vec2(0, 0));
			  this->res->gun->Draw(GunDraw, GunSrc);
			  break;
			}
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