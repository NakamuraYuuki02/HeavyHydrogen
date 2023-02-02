//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Sword.h"
#include  "Task_Map2D.h"
#include  "Task_Player.h"

namespace  Sword
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Sword.png");
		this->img2 = DG::Image::Create("./data/image/Sword2.png");
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
		this->render2D_Priority[1] = 0.4f;
		this->pos.x = 0;
		this->pos.y = 0;
		this->angle = 0;
		this->angle_LR = Angle_LR::Right;
		this->atk = 3;
		
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
		this->moveCnt++;
		//限界の時間を迎えたら消滅
		if (this->moveCnt >= 15) {
			//消滅申請
			this->Kill();
			return;
		}

		//回転
		if (this->angle_LR == Angle_LR::Right)
		{
			this->angle += ML::ToRadian(15);
		}
		else
		{
			this->angle -= ML::ToRadian(15);
		}
		
		//移動
		this->pos += this->moveVec;

		//当たり判定
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto targets = ge->GetTasks<BChara>("Enemy");
			for (auto it = targets->begin(); it != targets->end(); ++it)
			{
				//相手に接触の有無を確認させる
				if ((*it)->CheckHit(me))
				{
					//相手にダメージの処理を行わせる
					BChara::AttackInfo at = { this->atk,0,0 };
					(*it)->Received(this, at);
					this->Kill();
					break;
				}
			}
		}

		//移動先で障害物に接触したら消滅
		//マップが存在するか調べてからアクセス
		if (auto   map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName)) {
			ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
			if (true == map->CheckHit(hit)) {
				//消滅申請
				this->Kill();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(-12, -12, 24, 24);
		draw.Offset(this->pos);
		ML::Box2D  src(0, 0, 16, 16);
		//ML::Box2D src(0, 0, 32, 32);

		//スクロール対応
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		if (this->angle_LR == Angle_LR::Right)
		{
			this->res->img->Rotation(this->angle, ML::Vec2(24, 24));
			this->res->img->Draw(draw, src);
		}
		if(this->angle_LR==Angle_LR::Left)
		{
			this->res->img2->Rotation(this->angle, ML::Vec2(0, 24));
			this->res->img2->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	
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