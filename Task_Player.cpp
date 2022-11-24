//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map2D.h"


namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/chara02.png");
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
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-8, -20, 16, 40);
		this->angle_LR = Angle_LR::Right;
		this->controller = ge->in1;

		this->motion = Motion::Stand;		//キャラ初期状態
		this->maxSpeed = 3.0f;		//最大移動速度（横）
		this->addSpeed = 1.0f;		//歩行加速度（地面の影響である程度打ち消される
		this->decSpeed = 0.5f;		//接地状態の時の速度減衰量（摩擦
		this->dashSpeed = 5.0f;
		this->maxFallSpeed = 10.0f;	//最大落下速度
		this->jumpPow = -8.0f;		//ジャンプ力（初速）
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量
		this->hp = 10;
		this->jumpCnt = 0;

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
		
		if (this->unHitTime > 0) { this->unHitTime--; }
		//思考・状況判断
		this->Think();
		//現モーションに対応した制御
		this->Move();
		//めり込まない移動
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		////当たり判定
		//{
		//	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
		//	auto targets = ge->GetTasks<BChara>("アイテム");
		//	for (auto it = targets->begin(); it != targets->end(); ++it)
		//	{
		//		//相手に接触の有無を確認させる
		//		if ((*it)->CheckHit(me))
		//		{
		//			//相手にダメージの処理を行わせる
		//			BChara::AttackInfo at = { 0,0,0 };
		//			(*it)->Received(this, at);
		//			break;
		//		}
		//	}
		//}

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
					BChara::AttackInfo at = { 1,0,0 };
					(*it)->Received(this, at);
					break;
				}
			}
		}

		//カメラの位置を再調整
		{
			//プレイヤを画面の何処に置くか（今回は画面中央）
			int  px = ge->camera2D.w / 2;
			int  py = ge->camera2D.h / 2;
			//プレイヤを画面中央に置いた時のカメラの左上座標を求める
			int  cpx = int(this->pos.x) - px;
			int  cpy = int(this->pos.y) - py;
			//カメラの座標を更新
			ge->camera2D.x = cpx;
			ge->camera2D.y = cpy;
			if (auto   map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName)) {
				map->AdjustCameraPos();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->unHitTime > 0)
		{
			if ((this->unHitTime / 4) % 2 == 0)
			{
				return; //8フレーム中4フレーム画像を表示しない
			}
		}
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
		auto  inp = this->controller->GetState();
		BChara::Motion  nm = this->motion;	//とりあえず今の状態を指定

		//思考（入力）や状況に応じてモーションを変更する事を目的としている。
		//モーションの変更以外の処理は行わない
		switch (nm) {
		case  Motion::Stand:	//立っている
			if (inp.LStick.BL.on) { nm = Motion::Walk; }
			if (inp.LStick.BR.on) { nm = Motion::Walk; }
			if (inp.S1.down) { nm = Motion::TakeOff; }
			//if (inp.B3.down) { nm = Motion::Attack2; }
			if (inp.B4.down) { nm = Motion::Attack; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }//足元 障害　無し
			break;
		case  Motion::Walk:		//歩いている
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Stand; }
			if (inp.S1.down) { nm = Motion::TakeOff; }
			//if (inp.B3.down) { nm = Motion::Attack2; }
			if (inp.B4.down) { nm = Motion::Attack; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Jump:		//上昇中
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			break;
		case Motion::Jump2:
			if (this->moveVec.y >= 0) { nm = Motion::Fall2; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			break;
		case  Motion::Fall:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			if (inp.S1.down) { nm = Motion::Jump2; }
			break;
		case  Motion::Fall2:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			break;
		case  Motion::Attack:	//攻撃中
			if (this->moveCnt == 8) { nm = Motion::Stand; }
			break;
		//case  Motion::Attack2:	//攻撃中
		//	if (this->moveCnt == 8) { nm = Motion::Stand; }
		//	break;
		case  Motion::TakeOff:	//飛び立ち
			if (this->moveCnt >= 3) { nm = Motion::Jump; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Landing:	//着地
			if (this->moveCnt >= 3) { nm = Motion::Stand; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case Motion::Bound:     //ダメージを受けて吹き飛んでいる
			if (this->moveCnt >= 12 && this->CheckFoot() == true)
			{
				nm = Motion::Stand;
			}
			break;
		/*case Motion::Dash:
			nm = Motion::DashCt;
			break;
		case Motion::DashCt:
			if (this->moveCnt == 15)
			{
				if (this->CheckFoot() == true) { nm = Motion::Stand; }
				if (this->moveVec.y >= 0 || jumpCnt == 1) { nm = Motion::Fall; }
				if (this->moveVec.y >= 0 || jumpCnt == 2) { nm = Motion::Fall2; }
			}
			break;*/
		}
		//モーション更新
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//モーションに対応した処理
	//(モーションは変更しない）
	void  Object::Move()
	{
		auto  inp = this->controller->GetState();
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
			if (inp.LStick.BL.on)
			{
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Fall:		//落下中
			if (inp.LStick.BL.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			if (this->CheckFoot() == true)
			{
				this->jumpCnt = 0;
			}
			break;
		case  Motion::Fall2:		//落下中
			if (inp.LStick.BL.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			if (this->CheckFoot() == true)
			{
				this->jumpCnt = 0;
			}
			break;
		case  Motion::Jump:		//上昇中
			this->jumpCnt++;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow; //初速設定
			}
			if (this->CheckHead() == true)
			{
				this->moveVec.y = 0;
			}
			if (inp.LStick.BL.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Jump2:		//上昇中
			this->jumpCnt++;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow; //初速設定
			}
			if (this->CheckHead() == true)
			{
				this->moveVec.y = 0;
			}
			if (inp.LStick.BL.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.LStick.BR.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			break;
			break;
		case Motion::Dash:
			if (this->angle_LR == Angle_LR::Right)
			{
				this->moveVec.x = this->maxSpeed + dashSpeed;
				
			}
			if (this->angle_LR == Angle_LR::Left)
			{
				this->moveVec.x = -this->maxSpeed - dashSpeed;
			}
			break;
		case  Motion::Attack:	//攻撃中
			if (this->moveCnt == 4)
			{
				if (this->angle_LR == Angle_LR::Right)
				{
					/*auto shot = Shot00::Object::Create(true);
					shot->moveVec = ML::Vec2(8, 0);
					shot->pos = this->pos + ML::Vec2(30, 0);*/
				}
				else
				{
					/*auto shot = Shot00::Object::Create(true);
					shot->moveVec = ML::Vec2(-8, 0);
					shot->pos = this->pos + ML::Vec2(-30, 0);*/
				}
			}
			break;
		//case  Motion::Attack2:	//攻撃中
		//	if (this->moveCnt == 4)
		//	{
		//		if (this->angle_LR == Angle_LR::Right)
		//		{
		//			auto shot01 = Shot01::Object::Create(true);
		//			shot01->moveVec = ML::Vec2(4, -8);
		//			shot01->pos = this->pos + ML::Vec2(30, 0);
		//		}
		//		else
		//		{
		//			auto shot01 = Shot01::Object::Create(true);
		//			shot01->moveVec = ML::Vec2(-4, -8);
		//			shot01->pos = this->pos + ML::Vec2(-30, 0);
		//		}
		//	}
			//break;
		}
	}
	//-----------------------------------------------------------------------------
	//アニメーション制御
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			//draw							src
			{ ML::Box2D(-8, -20, 16, 40), ML::Box2D(0, 0, 32, 80), defColor },	//停止
			{ ML::Box2D(-2, -20, 16, 40), ML::Box2D(32, 0, 32, 80), defColor },	//歩行１
			{ ML::Box2D(-10, -20, 24, 40), ML::Box2D(64, 0, 48, 80), defColor },	//歩行２ dash
			{ ML::Box2D(-10, -20, 24, 40), ML::Box2D(112, 0, 48, 80), defColor },	//歩行３
			{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(48, 80, 48, 80), defColor },	//ジャンプ
			{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(96, 80, 48, 80), defColor },	//落下
			{ ML::Box2D(-12, -12, 24, 32), ML::Box2D(0, 80, 48, 64), defColor },	//飛び立つ直前
			{ ML::Box2D(-12, -12, 24, 32), ML::Box2D(144, 80, 48, 64), defColor },	//着地
			{ ML::Box2D(-12, -12, 24, 40), ML::Box2D(176, 0, 48, 80), defColor },   //ダメージ
			{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(190, 80, 48, 80), defColor },  //攻撃
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	ジャンプ------------------------------------------------------------------------
		case  Motion::Jump:		rtv = imageTable[4];	break;
			//	ジャンプ2------------------------------------------------------------------------
		case  Motion::Jump2:		rtv = imageTable[4];	break;
			//	停止----------------------------------------------------------------------------
		case  Motion::Stand:	rtv = imageTable[0];	break;
			//	歩行----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 1];
			break;
			//	落下----------------------------------------------------------------------------
		case  Motion::Fall:		rtv = imageTable[5];	break;
			//	落下2----------------------------------------------------------------------------
		case  Motion::Fall2:		rtv = imageTable[5];	break;
			//飛び立つ直前-----------------------------------------------------------------------
		case  Motion::TakeOff:  rtv = imageTable[6];    break;
			//  着地----------------------------------------------------------------------------
		case  Motion::Landing:  rtv = imageTable[7];    break;
			//  ダメージ------------------------------------------------------------------------
		case  Motion::Bound:    rtv = imageTable[8];    break;
			//　攻撃----------------------------------------------------------------------------
		case  Motion::Attack:   rtv = imageTable[9]; break;
			//　ダッシュ------------------------------------------------------------------------
		case  Motion::Dash:      rtv = imageTable[2]; break;
			//  ダッシュクール------------------------------------------------------------------
		case  Motion::DashCt:    rtv = imageTable[2]; break;
			//  攻撃2--------------------------------------------------------------------------
		//case  Motion::Attack2:  rtv = imageTable[9]; break;
		}
		//	向きに応じて画像を左右反転する
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
	}
	//-----------------------------------------------------------------------------
	//接触時の応答処理（必ず受け身の処理として実装する）
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		if (this->unHitTime > 0)
		{
			return; //無敵時間中はダメージを受けない
		}
		this->unHitTime = 90;
		this->hp -= at_.power; //仮処理
		if (this->hp <= 0)
		{
			this->Kill();
		}
		//吹き飛ばされる
		if (this->pos.x > from_->pos.x)
		{
			this->moveVec = ML::Vec2(+4, -9);
		}
		else
		{
			this->moveVec = ML::Vec2(-4, -9);
		}
		this->UpdateMotion(Motion::Bound);
		//from_は攻撃してきた相手、カウンターなどで逆にダメージを与えたいときに使う
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