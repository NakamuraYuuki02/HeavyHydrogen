//-------------------------------------------------------------------
//エンディング
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->endBack = DG::Image::Create("./data/image/endBack.png");
		this->pressQ = DG::Image::Create("./data/image/pressQ.png");
		this->clear = DG::Image::Create("./data/image/clear.png");
		this->number = DG::Image::Create("./data/image/UItext.png");
		this->rankA = DG::Image::Create("./data/image/rankA.png");
		this->rankB = DG::Image::Create("./data/image/rankB.png");
		this->rankC = DG::Image::Create("./data/image/rankC.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->endBack.reset();
		this->pressQ.reset();
		this->clear.reset();
		this->number.reset();
		this->rankA.reset();
		this->rankB.reset();
		this->rankC.reset();
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
		this->time = 0;
		this->timeCnt = 0;

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
			auto  nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		this->timeCnt++;
		if (this->timeCnt == 45)
		{
			this->timeCnt = 0;
			this->time++;
		}

		if (inp.ST.down || inp.S1.down)
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
		//エンディング背景
		ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
		ML::Box2D src(50, 50, 800, 400);
		this->res->endBack->Draw(draw, src);

		//pressQkey
		ML::Box2D Qdraw(150, 220, 192, 32);
		ML::Box2D Qsrc(0, 0, 192, 32);
		if (this->time % 2 == 0)
		{
			this->res->pressQ->Draw(Qdraw, Qsrc);
		}
		
		//クリアしたステージ数
		ML::Box2D Cdraw(100, 10, 150, 32);
		ML::Box2D Csrc(0, 0, 225, 32);
		this->res->clear->Draw(Cdraw, Csrc);
		
		//クリアしたステージ数のnumber
		char mapNum[10];
		sprintf(mapNum, "%02d", ge->elapsedNum - 1);
		for (int i = 0; i < 2; ++i) 
		{
			int num = mapNum[i] - '0';//'0'== 48
			ML::Box2D numDraw(250, 50, 50, 50);
			ML::Box2D numSrc(num * 30, 0, 30, 52);
			numDraw.Offset(i * 35, 0);//文字間隔
			this->res->number->Draw(numDraw, numSrc);
		}

		//ステージクリア数によって言葉を変える
		if (ge->elapsedNum - 1 <= 2)
		{
			ML::Box2D cAdraw(155, 150, 208, 64);
			ML::Box2D cAsrc(0, 0, 104, 32);
			this->res->rankC->Draw(cAdraw, cAsrc);
		}
		else if (ge->elapsedNum - 1 <= 4)
		{
			ML::Box2D cBdraw(150, 150, 208, 64);
			ML::Box2D cBsrc(0, 0, 104, 32);
			this->res->rankB->Draw(cBdraw, cBsrc);
		}
		else if (ge->elapsedNum - 1 == 5)
		{
			ML::Box2D cCdraw(150, 150, 208, 64);
			ML::Box2D cCsrc(0, 0, 104, 32);
			this->res->rankA->Draw(cCdraw, cCsrc);
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