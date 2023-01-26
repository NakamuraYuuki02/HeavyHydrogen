//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include  "Task_Select.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Title.bmp");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->img.reset( );
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
		auto inp = ge->in1->GetState();

		this->logoPosY += 9;
		if (this->logoPosY >= 0) {
			this->logoPosY = 0;
		}

		if (this->logoPosY == 0) {
			if (inp.ST.down || inp.S1.down)
			{
				//タイトルの次のセレクトシーンを指定。
				ge->ns = MyPG::MyGameEngine::NextScene::Select;
				//自身に消滅要請
				this->Kill();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(0, 0, 480, 270);
		ML::Box2D  src(0, 0, 240, 135);

		draw.Offset(0, this->logoPosY);
		this->res->img->Draw(draw, src);
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