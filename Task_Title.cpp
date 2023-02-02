//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include  "Task_Select.h"
#include "Task_Map2D.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{

		this->img = DG::Image::Create("./data/image/title.png");
		this->titleImage = DG::Image::Create("./data/image/ff8bbf23761c1c62.png");
		this->start = DG::Image::Create("./data/image/space.png");
		this->player = DG::Image::Create("./data/image/Fumiko.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->titleImage.reset();
		this->start.reset();
		this->player.reset();
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

		this->cnt = 0;
		this->start = true;
		int d[2] = { 1, 3 };
		int anim[4] = { 4, 3, 4, 5 };
		for (int i = 0; i < 2; ++i) {
			int y = d[i];
			for (int j = 0; j < 4; ++j) {
				int x = anim[j];
				playerImage[i][j] = ML::Box2D(24 * x, 32 * y, 24, 32);
			}
		}
		this->pos.x = 50;
		this->pos.y = 220;
		this->angle = 0;

		this->logoPosY = -270;
		DataInitialize();

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("Title");
		ge->KillAll_G("Field");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//if(ge->ns == MyPG::MyGameEngine::NextScene::Select)
			//★引き継ぎタスクの生成
			auto  next = Select::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		if (this->cnt < 30) {
			this->start = true;
		}
		else if (this->cnt < 60) {
			this->start = false;
		}
		else {
			this->cnt = 0;
		}
		this->cnt++;
		this->animCnt++;

		if (this->angle == 0) {
			this->pos.x++;
		}
		else
		{
			this->pos.x--;
		}

		if (pos.x == 400) {
			this->angle = 1;
		}
		else if (pos.x == 50) {
			this->angle = 0;
		}

		auto inp = ge->in1->GetState();

		if (inp.S1.down)
		{
			//タイトルの次のセレクトシーンを指定。
			ge->ns = MyPG::MyGameEngine::NextScene::Select;
			//自身に消滅要請
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(0, 0, 480, 270);
		ML::Box2D  src(0, 0, 846, 852);

		this->res->img->Draw(draw, src);

		ML::Box2D draw2(ge->screen2DWidth / 2 - 100, ge->screen2DHeight / 4, 200, 40);
		ML::Box2D src2(0, 0, 207, 32);

		this->res->titleImage->Draw(draw2, src2);

		if (this->start) {
			ML::Box2D draw3(ge->screen2DWidth / 2 - 75, ge->screen2DHeight / 2 + 50, 150, 20);
			ML::Box2D src3(0, 0, 241, 32);

			this->res->start->Draw(draw3, src3);
		}

		ML::Box2D draw4(0, 0, 24, 32);
		draw4.Offset(this->pos);
		ML::Box2D src4 = playerImage[this->angle][(this->animCnt / 10) % 4];

		this->res->player->Draw(draw4, src4);
	}
	//-------------------------------------------------------------------
	//データ初期化メソッド 選択された内容などをタイトルで毎回初期化
	void Object::DataInitialize()
	{
		//武器 初回に一つ選択
		ge->sw = MyPG::MyGameEngine::SelectedWeapon::Non;
		//スキル 毎回二つ選択
		for (int i = 0; i < ge->ssn; ++i)
		{
			ge->ss[i] = MyPG::MyGameEngine::SelectedSkill::Non;
		}
		//ステージ 毎回一つ選択
		ge->selectedStage = 0;

		//ゲームクリアに必要なステージ数
		ge->clearStageNum = 6;
		//ステージ通過数、ゲーム進行度
		ge->elapsedNum = 0;
		//経過したステージ 一つ前のステージの番号
		ge->elapsedStage = 0;

		//プレイヤーステータス
		ge->hp = 3;				//体力	
		ge->hpMax = 10;			//最大体力
		ge->atk = 5;				//攻撃力
		ge->jumpCnt = 0;			//ジャンプ回数
		ge->jumpMax = 1;			//ジャンプ上限回数
		ge->dashCnt = 0;			//ダッシュ回数
		ge->dashMax = 0;			//ダッシュ上限回数
	}

	//ML::Box2D src5(24 * 4, 32, 24, 32);	//歩行1				
	//ML::Box2D src6(24 * 3, 31, 24, 32);	//歩行２				
	//ML::Box2D src7(24 * 4, 32, 24, 32);	//歩行３				
	//ML::Box2D src8(24 * 5, 32, 24, 32);	//歩行４			
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