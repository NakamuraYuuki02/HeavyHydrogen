//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EnemyMap2D.h"
#include  "Task_Enemy01.h"
#include  "Task_Enemy02.h"
#include  "Task_Enemy03.h"
#include  "Task_Enemy04.h"
#include  "Task_Enemy05.h"

namespace  EnemyMap2D
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		this->render2D_Priority[1] = 0.9f;
		//マップのゼロクリア
		for (int y = 0; y < 100; ++y) {
			for (int x = 0; x < 100; ++x) {
				this->arr[y][x] = 0;
			}
		}
		this->sizeX = 0;
		this->sizeY = 0;
		this->hitBase = ML::Box2D(block, block, block, block);

		//マップチップ情報の初期化
		for (int c = 0; c < 100; ++c) {
			int  x = (c % 10);
			int  y = (c / 10);
			this->chip[c] = ML::Box2D(x * 32, y * 32, 32, 32);
		}

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		this->img.reset();

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
	
	}
	//-------------------------------------------------------------------
	//マップ読み込み
	bool  Object::Load(const  string&  fpath_)
	{
		//ファイルを開く（読み込み）
		ifstream   fin(fpath_);
		if (!fin) { return  false; }//読み込み失敗

		//マップ配列サイズの読み込み
		fin >> this->sizeX >> this->sizeY;
		this->hitBase = ML::Box2D(0, 0, this->sizeX * block, this->sizeY * block);

		//マップ配列データの読み込み
		for (int y = 0; y < this->sizeY; ++y) {
			for (int x = 0; x < this->sizeX; ++x) {
				fin >> this->arr[y][x];
			}
		}
		fin.close();

		return true;
	}
	//-------------------------------------------------------------------
	void Object::SetEnemy()
	{
		for (int y = 0; y < this->sizeY; ++y) {
			for (int x = 0; x < this->sizeX; ++x) {
				// チップの番号によって変える
				switch (this->arr[y][x])
				{
				   case 0:
				   {
					  auto g = Enemy04::Object::Create(true);
					  g->pos.x = x * 16;
					  g->pos.y = y * 16;
					  break;
				   }
				   case 1:
				   {
					  auto g = Enemy03::Object::Create(true);
					  g->pos.x = x * 16;
					  g->pos.y = y * 16;
					  break;
				   }
				   case 2:
				   {
					  auto g = Enemy02::Object::Create(true);
					  g->pos.x = x * 16;
					  g->pos.y = y * 16;
					  break;
				   }
				   case 4:
				   { 
					  auto g = Enemy01::Object::Create(true);
					  g->pos.x = x * 16;
					  g->pos.y = y * 16;
					  break;
				   }
				   case 5:
				   {
					   auto g = Enemy05::Object::Create(true);
					   g->pos.x = x * 16;
					   g->pos.y = y * 16;
					   break;
				   }
				}
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
				//（メソッド名が変なのは旧バージョンのコピーによるバグを回避するため
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