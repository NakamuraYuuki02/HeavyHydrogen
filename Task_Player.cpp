//-------------------------------------------------------------------
//プレイヤ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map2D.h"
#include  "Task_Shot00.h"
#include  "Task_Sword.h"
#include  "Task_Axe.h"
#include  "Task_Gun.h"
#include  "Task_Slash.h"

#include  "Task_Skill.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Fumiko.png");
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
		this->dashSpeed = 6.0f;		//ダッシュ時の加速量
		this->maxFallSpeed = 10.0f;	//最大落下速度
		this->jumpPow = -8.0f;		//ジャンプ力（初速）縦5マスくらい
		this->gravity = ML::Gravity(32) * 5; //重力加速度＆時間速度による加算量

		/*this->ss.push_back(SelectedSkill::JumpUp);
		this->ss.push_back(SelectedSkill::DashUp);*/
		Skill();

		this->hp = ge->hp;
		this->hpMax = ge->hpMax;
		this->atk = ge->atk;
		this->jumpCnt = ge->jumpCnt;			//ジャンプ回数
		this->jumpMax = ge->jumpMax;			//ジャンプ上限回数
		this->dashCnt = ge->dashCnt;			//ダッシュ回数
		this->dashMax = ge->dashMax;			//ダッシュ上限回数
		this->attackCnt = 0;					//攻撃回数
		this->attackMax = 0;					//攻撃上限回数
		this->WeaponLevel = 1;					//武器レベル
		this->CreateNum = 1;
		
		this->weapon = ge->sw;
		//this->weapon = Weapon::Gun;
		
		//★タスクの生成
		
		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("Main");
		ge->KillAll_G("Field");
		ge->KillAll_G("Player");
		ge->KillAll_G("Enemy");
		ge->KillAll_G("Goal");

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
		//{
		//	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
		//	auto targets = ge->GetTasks<BChara>("アイテム");
		//	for (auto it = targets->begin(); it != targets->end(); ++it)
		//	{
		//		//相手に接触の有無を確認させる
		//		if ((*it)->CheckHit(me))
		//		{
		//			//相手にダメージの処理を行わせる
		//			BChara::AttackInfo at = { 1,0,0 };
		//			(*it)->Received(this, at);
		//			break;
		//		}
		//	}
		//}

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

		//auto  inp = this->controller->GetState(); //デバッグ用
		//if (inp.R1.down)
		//{
		//	auto Skill = Skill::Object::Create(true);
		//	Skill->actionNo = 0;
		//	Skill->Skill(this);
		//}
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
			if (inp.SE.on) { nm = Motion::Walk; }
			if (inp.L3.on) { nm = Motion::Walk; }
			if (inp.S1.down) { nm = Motion::TakeOff; }
			//if (inp.B3.down) { nm = Motion::Attack2; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }//足元 障害　無し
			break;
		case  Motion::Walk:		//歩いている
			if (inp.SE.off && inp.L3.off) { nm = Motion::Stand; }
			if (inp.S1.down) { nm = Motion::TakeOff; }
			//if (inp.B3.down) { nm = Motion::Attack2; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Jump:		//上昇中
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S1.down && this->jumpMax >= 2) { nm = Motion::Jump2; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			break;
		case Motion::Jump2:
			if (this->moveVec.y >= 0) { nm = Motion::Fall2; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S1.down && this->jumpMax >= 3) { nm = Motion::Jump3; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			break;
		case Motion::Jump3:
			if (this->moveVec.y >= 0) { nm = Motion::Fall3; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			break;
		case  Motion::Fall:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			if (inp.S1.down && this->jumpMax >= 2) { nm = Motion::Jump2; }
			break;
		case  Motion::Fall2:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			if (inp.S1.down && this->jumpMax >= 3) { nm = Motion::Jump3; }
			break;
		case  Motion::Fall3:		//落下中
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			break;
			//空中で出来る攻撃は一回
		case  Motion::Attack:	//攻撃中
			if (this->WeaponLevel < 2)
			{
				if (this->moveCnt == 45 && this->CheckFoot() == true) { nm = Motion::Stand; }
				if (this->moveCnt == 45 && this->CheckFoot() == false) { nm = Motion::Fall3; }
			}
			else
			{
				if (this->moveCnt == 15 && this->CheckFoot() == true) { nm = Motion::Stand; }
				if (this->moveCnt == 15 && this->CheckFoot() == false) { nm = Motion::Fall3; }
			}
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
		case Motion::Dash:
				nm = Motion::DashCt;
			break;
		case Motion::DashCt:
			if (this->moveCnt == 15)
			{
				if (this->CheckFoot() == true) { nm = Motion::Stand; }
				if (this->moveVec.y >= 0 || jumpCnt == 1) { nm = Motion::Fall; }
				if (this->moveVec.y >= 0 || jumpCnt == 2) { nm = Motion::Fall2; }
				if (this->moveVec.y >= 0 || jumpCnt == 3) { nm = Motion::Fall3; }
			}
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
		auto  inp = this->controller->GetState();

		//重力加速
		switch (this->motion) {
		default:
			if (motion == Motion::Attack || motion == Motion::DashCt)
			{
				this->moveVec.y = min(this->moveVec.y - this->gravity, this->maxFallSpeed);
			}
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
		case Motion::Dash:
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
			if (inp.SE.on)
			{
				this->angle_LR = Angle_LR::Left;
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.L3.on)
			{
				this->angle_LR = Angle_LR::Right;
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Fall:		//落下中
			if (inp.SE.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.L3.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			if (this->CheckFoot() == true)
			{
				this->jumpCnt = 0;
			}
			break;
		case  Motion::Fall2:		//落下中
			if (inp.SE.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.L3.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			if (this->CheckFoot() == true)
			{
				this->jumpCnt = 0;
			}
			break;
		case  Motion::Fall3:		//落下中
			if (inp.SE.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.L3.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			if (this->CheckFoot() == true)
			{
				this->jumpCnt = 0;
			}
			break;
		case  Motion::Jump:		//上昇中
			this->jumpCnt = 1;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow; //初速設定
			}
			if (this->CheckHead() == true)
			{
				this->moveVec.y = 0;
			}
			if (inp.SE.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.L3.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Jump2:		//上昇中
			this->jumpCnt = 2;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow * 0.9f; //初速設定
			}
			if (this->CheckHead() == true)
			{
				this->moveVec.y = 0;
			}
			if (inp.SE.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.L3.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case  Motion::Jump3:		//上昇中
			this->jumpCnt = 3;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow * 0.9f; //初速設定
			}
			if (this->CheckHead() == true)
			{
				this->moveVec.y = 0;
			}
			if (inp.SE.on)
			{
				this->moveVec.x = -this->maxSpeed;
			}
			if (inp.L3.on)
			{
				this->moveVec.x = this->maxSpeed;
			}
			break;
		case Motion::Dash:
			this->dashCnt++;
			if (this->angle_LR == Angle_LR::Right)
			{
				this->moveVec.x = this->maxSpeed + dashSpeed;
			}
			if (this->angle_LR == Angle_LR::Left)
			{
				this->moveVec.x = -this->maxSpeed - dashSpeed;
			}
			this->moveVec.y = 0;
			break;
		case Motion::DashCt:
			break;
		case Motion::Attack:	//攻撃中
			if (this->moveCnt == 6)
			{
				switch (this->weapon)
				{
				case MyPG::MyGameEngine::SelectedWeapon::Sword:
				{
					if (this->angle_LR == Angle_LR::Right)
					{
						auto sword = Sword::Object::Create(true);
						sword->pos = this->pos + ML::Vec2(15, 0);
						for (int i = 0; i < CreateNum; ++i)
						{
							auto slash = Slash::Object::Create(true);

							//pw
							/*auto axe2 = Axe::Object::Create(true);
							axe2->moveVec = ML::Vec2(7, -8);
							auto axe3 = Axe::Object::Create(true);
							axe3->moveVec = ML::Vec2(7, -4);

							axe2->pos = this->pos + ML::Vec2(30, 0);

							axe3->pos = this->pos + ML::Vec2(30, 0);*/
							//pw

							slash->pos = this->pos + ML::Vec2(30 + (15 * i), 0);
						}
					}
					else
					{
						auto sword = Sword::Object::Create(true);
						sword->pos = this->pos + ML::Vec2(-15, 0);
						for (int i = 0; i < CreateNum; ++i)
						{
							auto slash = Slash::Object::Create(true);

							//pw
							/*auto axe2 = Axe::Object::Create(true);
							axe2->moveVec = ML::Vec2(7, -8);
							auto axe3 = Axe::Object::Create(true);
							axe3->moveVec = ML::Vec2(7, -4);

							axe2->pos = this->pos + ML::Vec2(30, 0);

							axe3->pos = this->pos + ML::Vec2(30, 0);*/
							//pw

							slash->pos = this->pos + ML::Vec2(-30 + (-15 * i), 0);
						}
					}
				}
				break;
				case MyPG::MyGameEngine::SelectedWeapon::Axe:
				{
					if (this->angle_LR == Angle_LR::Right)
					{
						for (int i = 0; i < CreateNum; ++i)
						{
							auto axe = Axe::Object::Create(true);
							axe->moveVec = ML::Vec2(7, -4 + (-4 * i));

							//pw
							/*auto axe2 = Axe::Object::Create(true);
							axe2->moveVec = ML::Vec2(7, -8);
							auto axe3 = Axe::Object::Create(true);
							axe3->moveVec = ML::Vec2(7, -4);

							axe2->pos = this->pos + ML::Vec2(30, 0);

							axe3->pos = this->pos + ML::Vec2(30, 0);*/
							//pw

							axe->pos = this->pos + ML::Vec2(30, 0);
						}
					}
					else
					{
						for (int i = 0; i < CreateNum; ++i)
						{
							auto axe = Axe::Object::Create(true);
							axe->moveVec = ML::Vec2(-7, -4 + (-4 * i));

							//pw
							/*auto axe2 = Axe::Object::Create(true);
							axe2->moveVec = ML::Vec2(7, -8);
							auto axe3 = Axe::Object::Create(true);
							axe3->moveVec = ML::Vec2(7, -4);

							axe2->pos = this->pos + ML::Vec2(30, 0);

							axe3->pos = this->pos + ML::Vec2(30, 0);*/
							//pw

							axe->pos = this->pos + ML::Vec2(-30, 0);
						}
					}
				}
				break;
				case MyPG::MyGameEngine::SelectedWeapon::Gun:
				{
					if (this->angle_LR == Angle_LR::Right)
					{
						auto gun = Gun::Object::Create(true);
						gun->pos = this->pos + ML::Vec2(15, 0);
						for (int i = 0; i < CreateNum; ++i)
						{
							auto shot = Shot00::Object::Create(true);
							shot->moveVec = ML::Vec2(7, 0);

							//pw
							/*auto gun2 = Shot00::Object::Create(true);
							gun2->moveVec = ML::Vec2(7, 0);
							auto gun3 = Shot00::Object::Create(true);
							gun3->moveVec = ML::Vec2(7, 0);

							gun2->pos = this->pos + ML::Vec2(30, 0);

							gun3->pos = this->pos + ML::Vec2(30, 0);*/
							//pw

							shot->pos = this->pos + ML::Vec2(30 + (15 * i), 0);
						}
					}
					else
					{
						auto gun = Gun::Object::Create(true);
						gun->pos = this->pos + ML::Vec2(-15, 0);
						for (int i = 0; i < CreateNum; ++i)
						{
							auto gun = Shot00::Object::Create(true);
							gun->moveVec = ML::Vec2(-7, 0);

							//pw
							/*auto gun2 = Shot00::Object::Create(true);
							gun2->moveVec = ML::Vec2(7, 0);
							auto gun3 = Shot00::Object::Create(true);
							gun3->moveVec = ML::Vec2(7, 0);

							gun2->pos = this->pos + ML::Vec2(30, 0);

							gun3->pos = this->pos + ML::Vec2(30, 0);*/
							//pw

							gun->pos = this->pos + ML::Vec2(-30 + (-15 * i), 0);
						}
					}
				}
				break;
				}
				if (this->CheckFoot() == false)
				{
					this->moveVec.y = 0;
					this->attackCnt++;
				}
			}
			break;
		case Motion::Landing:
			this->dashCnt = 0;
			this->attackCnt = 0;
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
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(0, 32, 24, 32), defColor },	//停止						 0
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 4, 32, 24, 32), defColor },	//歩行1					 1
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 3, 31, 24, 32), defColor },	//歩行２					 2
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 4, 32, 24, 32), defColor },	//歩行３					 3
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 5, 32, 24, 32), defColor },	//歩行４					 4
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 16, 32 * 3, 24, 32), defColor },	//ジャンプ			 5
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 10, 32, 24, 32), defColor },	//落下 飛び立つ直前		 6
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 11, 32, 24, 32), defColor },	//着地					 7
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 6, 32, 24, 32), defColor },   //ダメージ				 8
			//{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 9, 32, 24, 32), defColor },  //攻撃					 
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 8, 32 * 4, 24, 32), defColor }, //ダッシュ				 9
			//draw							src
			//{ ML::Box2D(-8, -20, 16, 40), ML::Box2D(0, 0, 32, 80), defColor },	//停止
			//{ ML::Box2D(-2, -20, 16, 40), ML::Box2D(32, 0, 32, 80), defColor },	//歩行１
			//{ ML::Box2D(-10, -20, 24, 40), ML::Box2D(64, 0, 48, 80), defColor },	//歩行２ dash
			//{ ML::Box2D(-10, -20, 24, 40), ML::Box2D(112, 0, 48, 80), defColor },	//歩行３
			//{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(48, 80, 48, 80), defColor },	//ジャンプ
			//{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(96, 80, 48, 80), defColor },	//落下
			//{ ML::Box2D(-12, -12, 24, 32), ML::Box2D(0, 80, 48, 64), defColor },	//飛び立つ直前
			//{ ML::Box2D(-12, -12, 24, 32), ML::Box2D(144, 80, 48, 64), defColor },	//着地
			//{ ML::Box2D(-12, -12, 24, 40), ML::Box2D(176, 0, 48, 80), defColor },   //ダメージ
			//{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(190, 80, 48, 80), defColor },  //攻撃
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	ジャンプ------------------------------------------------------------------------
		case  Motion::Jump:		rtv = imageTable[5];	break;
			//	ジャンプ2------------------------------------------------------------------------
		case  Motion::Jump2:		rtv = imageTable[5];	break;
			//	ジャンプ3------------------------------------------------------------------------
		case  Motion::Jump3:		rtv = imageTable[5];	break;
			//	停止----------------------------------------------------------------------------
		case  Motion::Stand:	rtv = imageTable[0];	break;
			//	歩行----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 8;
			work %= 4;
			rtv = imageTable[work + 1];
			break;
			//	落下----------------------------------------------------------------------------
		case  Motion::Fall:		rtv = imageTable[6];	break;
			//	落下2----------------------------------------------------------------------------
		case  Motion::Fall2:		rtv = imageTable[6];	break;
			//	落下3----------------------------------------------------------------------------
		case  Motion::Fall3:		rtv = imageTable[6];	break;
			//飛び立つ直前-----------------------------------------------------------------------
		case  Motion::TakeOff:  rtv = imageTable[6];    break;
			//  着地----------------------------------------------------------------------------
		case  Motion::Landing:  rtv = imageTable[7];    break;
			//  ダメージ------------------------------------------------------------------------
		case  Motion::Bound:    rtv = imageTable[8];    break;
			//　攻撃----------------------------------------------------------------------------
		case  Motion::Attack:   rtv = imageTable[2]; break;
			//　ダッシュ------------------------------------------------------------------------
		case  Motion::Dash:      rtv = imageTable[9]; break;
			//  ダッシュクール------------------------------------------------------------------
		case  Motion::DashCt:    rtv = imageTable[9]; break;
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
		if (motion == Motion::DashCt)
		{
			return;
		}
		if (this->unHitTime > 0)
		{
			return; //無敵時間中はダメージを受けない
		}
		this->unHitTime = 90;
		this->hp -= at_.power; //仮処理
		ge->hp = this->hp;
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
	//-------------------------------------------------------------------
	//スキル
	void Object::Skill()
	{
 		for (int i = 0; i < ge->ss.size(); ++i)
		{
			switch (ge->ss[i]) //後でgeかわるでー
			{
			case MyPG::MyGameEngine::SelectedSkill::JumpUp:
				//ジャンプ回数増加（最大3回）
				++ge->jumpMax;
				break;
			case MyPG::MyGameEngine::SelectedSkill::DashUp:
				//ダッシュ回数増加
				ge->dashMax++;
				break;
			case MyPG::MyGameEngine::SelectedSkill::HpUp:
				//体力増加
				ge->hp++;
				break;
			case MyPG::MyGameEngine::SelectedSkill::AtkUp:
				//攻撃力増加
				ge->atk++;
				break;
			case MyPG::MyGameEngine::SelectedSkill::Special1:
				//武器の特殊強化1
				//this->WeaponSpecial1(weapon);
				this->CreateNum = 3;
				break;
				//case SelectedSkill::Special2:
				//	//武器の特殊強化2
				//	break;
			}
		}
	}
	//-------------------------------------------------------------------
	void Object::SkillImage()
	{

	}
	//-------------------------------------------------------------------
	//武器の特殊強化
	//void Object::WeaponSpecial1(Weapon weapon_)
	//{
	//	//this->CreateNum = 3;
	//	switch (this->weapon)
	//	{
	//	case Weapon::Sword:
	//	{
	//		auto sword = ge->GetTask<Sword::Object>(Map2D::defGroupName, Map2D::defName);
	//		//剣の特殊強化1
	//		if (this->angle_LR == Angle_LR::Right)
	//		{
	//			for (int i = 0; i < 1; ++i)
	//			{
	//				
	//				sword->moveVec = ML::Vec2(-8 * i, 0);
	//				sword->pos = this->pos + ML::Vec2(30, -5);
	//			}
	//		}
	//		else
	//		{
	//			for (int i = 0; i < 1; ++i)
	//			{
	//				
	//				sword->moveVec = ML::Vec2(-8 * i, 0);
	//				sword->pos = this->pos + ML::Vec2(-30, -5);
	//			}
	//		}
	//	}	
	//	break;
	//	case Weapon::Axe:
	//	{
	//		auto axe = ge->GetTask<Axe::Object>(Map2D::defGroupName, Map2D::defName);
	//		//斧の特殊強化1
	//		if (this->angle_LR == Angle_LR::Right)
	//		{
	//			for (int i = 1; i <= 3; ++i)
	//			{
	//				axe->moveVec = ML::Vec2(7, -3 * i);
	//				axe->pos = this->pos + ML::Vec2(30, 0);
	//			}
	//		}
	//		else
	//		{
	//			for (int i = 1; i <= 3; ++i)
	//			{
	//				
	//				axe->moveVec = ML::Vec2(-7, -3 * i);
	//				axe->pos = this->pos + ML::Vec2(-30, 0);
	//			}
	//		}
	//	}	
	//	break;
	//	case Weapon::Gun:
	//	{
	//		auto gun = ge->GetTask<Shot00::Object>(Map2D::defGroupName, Map2D::defName);
	//		//銃の特殊強化1
	//		if (this->angle_LR == Angle_LR::Right)
	//		{
	//			for (int i = 1; i <= 3; ++i)
	//			{
	//				
	//				gun->moveVec = ML::Vec2(8, 0);
	//				gun->pos = this->pos + ML::Vec2(20 * i, 0);
	//			}
	//		}
	//		else
	//		{
	//			for (int i = 1; i <= 3; ++i)
	//			{
	//				
	//				gun->moveVec = ML::Vec2(-8, 0);
	//				gun->pos = this->pos + ML::Vec2(-20 * i, 0);
	//			}
	//		}
	//	}
	//	break;
	//	}
	//}
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