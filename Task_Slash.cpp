//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Sword.h"
#include  "Task_Slash.h"
#include  "Task_Map2D.h"
#include  "Task_Player.h"

namespace  Slash
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/slashEffect.png");
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
		this->hitBase = ML::Box2D(-16, -16, 32, 32);
		this->moveVec = ML::Vec2(0, 0);
		this->moveCnt = 0;
		this->angle_LR = Angle_LR::Right;
		int anim[3] = {5,4,3};
		for (int i = 0; i < 3; ++i)
		{
			int x = anim[i];
			this->slashImage[i] = ML::Box2D(x * 128, 64*2, 128, 156);
		}
		this->animCnt = 0;
		//this->hp = 5;
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
		//if (auto   map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName)) {
		//	ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
		//	if (true == map->CheckHit(hit)) {
		//		//消滅申請
		//		//this->Kill();

		//		////とりあえず星はばら撒くよ
		//		//for (int c = 0; c < 4; ++c) {
		//		//	auto  eff = Effect00::Object::Create(true);
		//		//	eff->pos = this->pos;
		//		//}
		//		//return;
		//	}
		//}

		this->animCnt++;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(-24, -24, 48, 48);
		if (this->angle_LR == Angle_LR::Left)
		{
			draw.x = -draw.x; 
			draw.w = -draw.w;
		}
		draw.Offset(this->pos);
		ML::Box2D  src = slashImage[int(this->animCnt / 6) % 3 ];

		//スクロール対応
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->img->Draw(draw, src);
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