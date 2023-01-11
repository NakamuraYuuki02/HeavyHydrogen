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
		this->UIhp = DG::Image::Create("./data/image/HPheart.png");
		this->UInum = DG::Image::Create("./data/image/UItext.png");
		this->gun = DG::Image::Create("./data/image/Gun.png");
		this->sword = DG::Image::Create("./data/image/Gun.png");
		this->axe = DG::Image::Create("./data/image/Gun.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->UIhp.reset();
		this->UInum.reset();
		this->gun.reset();
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
		//hp分(初期値3)ハートを表示
		for (int i = 0; i < ge->hp; ++i)
		{
			ML::Box2D hpDraw(i * 22, 0, 22, 22);
			ML::Box2D hpSrc(0, 0, 465, 396);
			this->res->UIhp->Draw(hpDraw, hpSrc);
		}

		//マップ数表示
		char mapNum[10];
		sprintf(mapNum, "%02d", ge->stageNum);
		for (int i = 0; i < 2; ++i) {
			int num = mapNum[i] - '0';//'0'== 48
			ML::Box2D numDraw(0, 25, 32, 32);
			ML::Box2D numSrc(num * 30, 0, 30, 52);
			numDraw.Offset(i * 22, 0);//文字間隔
			this->res->UInum->Draw(numDraw, numSrc);
		}

		//武器アイコン
		switch (this->ui)
		{
		case UI::Sword:
		    {
			  ML::Box2D SwordDraw(50, 30, 45, 30);
			  ML::Box2D SwordSrc(0, 0, 15, 10);
			  this->res->sword->Draw(SwordDraw, SwordSrc);
			  break;
		    }
		case UI::Axe:
		    { 
			  ML::Box2D AxeDraw(50, 30, 45, 30);
			  ML::Box2D AxeSrc(0, 0, 15, 10);
			  this->res->axe->Draw(AxeDraw, AxeSrc);
			  break;
			}
		case UI::Gun:
		    { 
			  ML::Box2D GunDraw(50, 30, 45, 30);
			  ML::Box2D GunSrc(0, 0, 15, 10);
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