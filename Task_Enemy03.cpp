//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Enemy03.h"
#include "Task_Player.h"
#include "Task_EnemyShot01.h"

namespace  Enemy03
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/skull02_blue01_spriteSheet.png");
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
		this->render2D_Priority[1] = 0.6f;
		this->hitBase = ML::Box2D(-15, -15, 30, 30);
		this->angle_LR = Angle_LR::Left;
		this->motion = Motion::Stand;
		this->maxSpeed = 1.3f;
		this->addSpeed = 0.7f;
		this->decSpeed = 0.5f;
		this->maxFallSpeed = 10.0f;
		this->jumpPow = -6.0f;
		this->gravity = ML::Gravity(32) * 5;
		this->hp = 5;
		this->atk = 1;
		this->attackCnt = 0;
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
		this->animCnt++;
		this->attackCnt++;
		this->Move();
		//思考・状況判断
		this->Think();

		ML::Vec2 est = this->moveVec;
		this->CheckMove(est);
		//当たり判定
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto targets = ge->GetTasks<BChara>("Player");
			for (auto it = targets->begin(); it != targets->end(); ++it) {
				//相手に接触の有無を確認させる
				if ((*it)->CheckHit(me)) {
					//相手にダメージの処理を行わせる
					BChara::AttackInfo  at = { atk,0,0 };
					(*it)->Received(this, at);
					break;
				}
			}
		}
		
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//スクロール対応
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);
	}
	//-----------------------------------------------------------------------------
	//思考＆状況判断　モーション決定
	void  Object::Think()
	{
		BChara::Motion  nm = this->motion;	//とりあえず今の状態を指定

		//思考（入力）や状況に応じてモーションを変更する事を目的としている。
		//モーションの変更以外の処理は行わない
		switch (nm) {
		case  Motion::Stand:	//立っている
			nm = Motion::Walk;
			//if (this->CheckFoot() == false) { nm = Motion::Fall; }//足元 障害　無し
			break;
		case  Motion::Walk:		//歩いている
			if (this->CheckFront_LR() == true) { nm = Motion::Turn; }
			if (this->moveCnt >= 180) { nm = Motion::Turn; }
			//if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Jump:		//上昇中
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			break;
		case  Motion::Fall:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Stand; }
			break;
		case  Motion::Attack:	//攻撃中
			break;
			if (this->moveCnt == 8) { nm = Motion::Stand; }
			break;
		case  Motion::TakeOff:	//飛び立ち
			break;
		case  Motion::Landing:	//着地
			//if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case Motion::Bound:     //ダメージを受けて吹き飛んでいる
			if (this->moveCnt >= 12 /*&& this->CheckFoot() == true*/)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::Turn:
			if (this->moveCnt >= 5) { nm = Motion::Stand; }
			break;
		}
		//モーション更新
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//モーションに対応した処理
	//(モーションは変更しない）
	void  Object::Move()
	{
		//重力加速
		switch (this->motion) {
		default:
			//上昇中もしくは足元に地面が無い
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
				//this->moveVec.y = 1.0f;//仮処理
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			//地面に接触している
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//重力加速を無効化する必要があるモーションは下にcaseを書く（現在対象無し）
		case Motion::Unnon:	break;
		case Motion::Stand: break;
		case Motion::Walk: break;
		case Motion::Fall: break;
		case Motion::Turn: break;
		case Motion::Bound: break;
		}

		//移動速度減衰
		switch (this->motion) {
		default:
			if (this->moveVec.x < 0)
			{
				this->moveVec.x = min(this->moveVec.x + this->decSpeed, 0);
			}
			else
			{
				this->moveVec.x = max(this->moveVec.x - this->decSpeed, 0);
			}
			break;
			//移動速度減衰を無効化する必要があるモーションは下にcaseを書く
		case Motion::Bound:
		case Motion::Unnon:	break;
		}
		//-----------------------------------------------------------------
		//モーション毎に固有の処理
		switch (this->motion) {
		case  Motion::Stand:	//立っている
			break;
		case  Motion::Walk:		//歩いている
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x =
					max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (this->angle_LR == Angle_LR::Right) {
				this->moveVec.x =
					min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case  Motion::Fall:		//落下中
			break;
		case  Motion::Jump:		//上昇中
			break;
		case  Motion::Attack:	//攻撃中
			break;
		case  Motion::Turn:
			if (this->moveCnt == 3) {
				if (this->angle_LR == Angle_LR::Left) {
					this->angle_LR = Angle_LR::Right;
				}
				else { this->angle_LR = Angle_LR::Left; }
			}
			break;
		}
	}
	//-----------------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			{ ML::Box2D(-15, -15, 30, 30), ML::Box2D(0, 0, 31, 31), defColor },			//	立ち			0
			{ ML::Box2D(-15, -15, 30, 30), ML::Box2D(192, 0, 31, 31), defColor },		//	歩行１		1
			{ ML::Box2D(-15, -15, 30, 30), ML::Box2D(224, 0, 31, 31), defColor },		//	歩行２		2
			{ ML::Box2D(-15, -15, 30, 30), ML::Box2D(256, 0, 31, 31), defColor },		//	歩行３		3
			{ ML::Box2D(-15, -15, 30, 30), ML::Box2D(288, 0, 31, 31), defColor },		//	歩行４		4
			{ ML::Box2D(-15, -15, 30, 30), ML::Box2D(513, 0, 31, 31), defColor },		//	ダメージ		5

		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	ジャンプ------------------------------------------------------------------------
		case  Motion::Jump:		rtv = imageTable[0];	break;
			//	ジャンプ2------------------------------------------------------------------------
		case  Motion::Jump2:		rtv = imageTable[0];	break;
			//	ジャンプ3------------------------------------------------------------------------
		case  Motion::Jump3:		rtv = imageTable[0];	break;
			//	停止----------------------------------------------------------------------------
		case  Motion::Stand:	rtv = imageTable[0];	break;
			//	歩行----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 8;
			work %= 4;
			rtv = imageTable[work + 1];
			break;
			//	落下----------------------------------------------------------------------------
		case  Motion::Fall:		rtv = imageTable[0];	break;
			//	落下2----------------------------------------------------------------------------
		case  Motion::Fall2:		rtv = imageTable[0];	break;
			//	落下3----------------------------------------------------------------------------
		case  Motion::Fall3:		rtv = imageTable[0];	break;
			//飛び立つ直前-----------------------------------------------------------------------
		case  Motion::TakeOff:  rtv = imageTable[0];    break;
			//  着地----------------------------------------------------------------------------
		case  Motion::Landing:  rtv = imageTable[0];    break;
			//  ダメージ------------------------------------------------------------------------
		case  Motion::Bound:    rtv = imageTable[5];    break;
			//　攻撃----------------------------------------------------------------------------
		case  Motion::Attack:   rtv = imageTable[0]; break;
			//　ダッシュ------------------------------------------------------------------------
		case  Motion::Dash:      rtv = imageTable[0]; break;
			//  ダッシュクール------------------------------------------------------------------
		case  Motion::DashCt:    rtv = imageTable[0]; break;
			//  攻撃2--------------------------------------------------------------------------
		//case  Motion::Attack2:  rtv = imageTable[9]; break;
		}
		//	向きに応じて画像を左右反転する
		if (Angle_LR::Right == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
	}
	//-------------------------------------------------------------------
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		this->hp -= at_.power;
		if (this->hp <= 0) {
			this->Kill();
		}
		//吹き飛ばされる
		if (this->pos.x > from_->pos.x)
		{
			this->moveVec = ML::Vec2(+5, 0);
		}
		else
		{
			this->moveVec = ML::Vec2(-5, 0);
		}
		this->UpdateMotion(Motion::Bound);
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