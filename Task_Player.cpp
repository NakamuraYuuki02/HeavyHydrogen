//-------------------------------------------------------------------
//�v���C��
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
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Fumiko.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->img.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		this->render2D_Priority[1] = 0.5f;
		this->hitBase = ML::Box2D(-8, -20, 16, 40);
		this->angle_LR = Angle_LR::Right;
		this->controller = ge->in1;

		this->motion = Motion::Stand;		//�L�����������
		this->maxSpeed = 3.0f;		//�ő�ړ����x�i���j
		this->addSpeed = 1.0f;		//���s�����x�i�n�ʂ̉e���ł�����x�ł��������
		this->decSpeed = 0.5f;		//�ڒn��Ԃ̎��̑��x�����ʁi���C
		this->dashSpeed = 6.0f;		//�_�b�V�����̉�����
		this->maxFallSpeed = 10.0f;	//�ő嗎�����x
		this->jumpPow = -8.0f;		//�W�����v�́i�����j�c5�}�X���炢
		this->gravity = ML::Gravity(32) * 5; //�d�͉����x�����ԑ��x�ɂ����Z��

		/*this->ss.push_back(SelectedSkill::JumpUp);
		this->ss.push_back(SelectedSkill::DashUp);*/
		Skill();

		this->hp = ge->hp;
		this->hpMax = ge->hpMax;
		this->atk = ge->atk;
		this->jumpCnt = ge->jumpCnt;			//�W�����v��
		this->jumpMax = ge->jumpMax;			//�W�����v�����
		this->dashCnt = ge->dashCnt;			//�_�b�V����
		this->dashMax = ge->dashMax;			//�_�b�V�������
		this->attackCnt = 0;					//�U����
		this->attackMax = 0;					//�U�������
		this->WeaponLevel = 1;					//���탌�x��
		this->CreateNum = 1;
		
		this->weapon = ge->sw;
		//this->weapon = Weapon::Gun;
		
		//���^�X�N�̐���
		
		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->KillAll_G("Main");
		ge->KillAll_G("Field");
		ge->KillAll_G("Player");
		ge->KillAll_G("Enemy");
		ge->KillAll_G("Goal");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		this->moveCnt++;
		this->animCnt++;
		
		if (this->unHitTime > 0) { this->unHitTime--; }
		//�v�l�E�󋵔��f
		this->Think();
		//�����[�V�����ɑΉ���������
		this->Move();
		//�߂荞�܂Ȃ��ړ�
		ML::Vec2  est = this->moveVec;
		this->CheckMove(est);

		////�����蔻��
		//{
		//	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
		//	auto targets = ge->GetTasks<BChara>("�A�C�e��");
		//	for (auto it = targets->begin(); it != targets->end(); ++it)
		//	{
		//		//����ɐڐG�̗L�����m�F������
		//		if ((*it)->CheckHit(me))
		//		{
		//			//����Ƀ_���[�W�̏������s�킹��
		//			BChara::AttackInfo at = { 0,0,0 };
		//			(*it)->Received(this, at);
		//			break;
		//		}
		//	}
		//}

		//�����蔻��
		//{
		//	ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
		//	auto targets = ge->GetTasks<BChara>("�A�C�e��");
		//	for (auto it = targets->begin(); it != targets->end(); ++it)
		//	{
		//		//����ɐڐG�̗L�����m�F������
		//		if ((*it)->CheckHit(me))
		//		{
		//			//����Ƀ_���[�W�̏������s�킹��
		//			BChara::AttackInfo at = { 1,0,0 };
		//			(*it)->Received(this, at);
		//			break;
		//		}
		//	}
		//}

		//�J�����̈ʒu���Ē���
		{
			//�v���C������ʂ̉����ɒu�����i����͉�ʒ����j
			int  px = ge->camera2D.w / 2;
			int  py = ge->camera2D.h / 2;
			//�v���C������ʒ����ɒu�������̃J�����̍�����W�����߂�
			int  cpx = int(this->pos.x) - px;
			int  cpy = int(this->pos.y) - py;
			//�J�����̍��W���X�V
			ge->camera2D.x = cpx;
			ge->camera2D.y = cpy;
			if (auto   map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName)) {
				map->AdjustCameraPos();
			}
		}

		//auto  inp = this->controller->GetState(); //�f�o�b�O�p
		//if (inp.R1.down)
		//{
		//	auto Skill = Skill::Object::Create(true);
		//	Skill->actionNo = 0;
		//	Skill->Skill(this);
		//}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (this->unHitTime > 0)
		{
			if ((this->unHitTime / 4) % 2 == 0)
			{
				return; //8�t���[����4�t���[���摜��\�����Ȃ�
			}
		}
		BChara::DrawInfo  di = this->Anim();
		di.draw.Offset(this->pos);
		//�X�N���[���Ή�
		di.draw.Offset(-ge->camera2D.x, -ge->camera2D.y);

		this->res->img->Draw(di.draw, di.src);
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		auto  inp = this->controller->GetState();
		BChara::Motion  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��

		//�v�l�i���́j��󋵂ɉ����ă��[�V������ύX���鎖��ړI�Ƃ��Ă���B
		//���[�V�����̕ύX�ȊO�̏����͍s��Ȃ�
		switch (nm) {
		case  Motion::Stand:	//�����Ă���
			if (inp.SE.on) { nm = Motion::Walk; }
			if (inp.L3.on) { nm = Motion::Walk; }
			if (inp.S1.down) { nm = Motion::TakeOff; }
			//if (inp.B3.down) { nm = Motion::Attack2; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }//���� ��Q�@����
			break;
		case  Motion::Walk:		//�����Ă���
			if (inp.SE.off && inp.L3.off) { nm = Motion::Stand; }
			if (inp.S1.down) { nm = Motion::TakeOff; }
			//if (inp.B3.down) { nm = Motion::Attack2; }
			if (inp.B4.down) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Jump:		//�㏸��
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
		case  Motion::Fall:		//������
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			if (inp.S1.down && this->jumpMax >= 2) { nm = Motion::Jump2; }
			break;
		case  Motion::Fall2:		//������
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			if (inp.S1.down && this->jumpMax >= 3) { nm = Motion::Jump3; }
			break;
		case  Motion::Fall3:		//������
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			if (inp.B4.down && this->attackCnt <= this->attackMax) { nm = Motion::Attack; }
			if (inp.S9.down && this->dashCnt < this->dashMax || inp.S0.down && this->dashCnt < this->dashMax) { nm = Motion::Dash; }
			break;
			//�󒆂ŏo����U���͈��
		case  Motion::Attack:	//�U����
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
		//case  Motion::Attack2:	//�U����
		//	if (this->moveCnt == 8) { nm = Motion::Stand; }
		//	break;
		case  Motion::TakeOff:	//��ї���
			if (this->moveCnt >= 3) { nm = Motion::Jump; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Landing:	//���n
			if (this->moveCnt >= 3) { nm = Motion::Stand; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case Motion::Bound:     //�_���[�W���󂯂Đ������ł���
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
		//���[�V�����X�V
		this->UpdateMotion(nm);
	}
	//-----------------------------------------------------------------------------
	//���[�V�����ɑΉ���������
	//(���[�V�����͕ύX���Ȃ��j
	void  Object::Move()
	{
		auto  inp = this->controller->GetState();

		//�d�͉���
		switch (this->motion) {
		default:
			if (motion == Motion::Attack || motion == Motion::DashCt)
			{
				this->moveVec.y = min(this->moveVec.y - this->gravity, this->maxFallSpeed);
			}
			//�㏸���������͑����ɒn�ʂ�����
			if (this->moveVec.y < 0 ||
				this->CheckFoot() == false) {
				//this->moveVec.y = 1.0f;//������
				this->moveVec.y = min(this->moveVec.y + this->gravity, this->maxFallSpeed);
			}
			//�n�ʂɐڐG���Ă���
			else {
				this->moveVec.y = 0.0f;
			}
			break;
			//�d�͉����𖳌�������K�v�����郂�[�V�����͉���case�������i���ݑΏۖ����j
		case Motion::Dash:
		case Motion::Unnon:	break;
		}

		//�ړ����x����
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
			//�ړ����x�����𖳌�������K�v�����郂�[�V�����͉���case������
		case Motion::Bound:
		case Motion::Unnon:	break;
		}
		//-----------------------------------------------------------------
		//���[�V�������ɌŗL�̏���
		switch (this->motion) {
		case  Motion::Stand:	//�����Ă���
			break;
		case  Motion::Walk:		//�����Ă���
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
		case  Motion::Fall:		//������
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
		case  Motion::Fall2:		//������
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
		case  Motion::Fall3:		//������
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
		case  Motion::Jump:		//�㏸��
			this->jumpCnt = 1;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow; //�����ݒ�
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
		case  Motion::Jump2:		//�㏸��
			this->jumpCnt = 2;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow * 0.9f; //�����ݒ�
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
		case  Motion::Jump3:		//�㏸��
			this->jumpCnt = 3;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow * 0.9f; //�����ݒ�
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
		case Motion::Attack:	//�U����
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
		//case  Motion::Attack2:	//�U����
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
	//�A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		ML::Color  defColor(1, 1, 1, 1);
		BChara::DrawInfo imageTable[] = {
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(0, 32, 24, 32), defColor },	//��~						 0
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 4, 32, 24, 32), defColor },	//���s1					 1
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 3, 31, 24, 32), defColor },	//���s�Q					 2
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 4, 32, 24, 32), defColor },	//���s�R					 3
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 5, 32, 24, 32), defColor },	//���s�S					 4
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 16, 32 * 3, 24, 32), defColor },	//�W�����v			 5
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 10, 32, 24, 32), defColor },	//���� ��ї����O		 6
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 11, 32, 24, 32), defColor },	//���n					 7
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 6, 32, 24, 32), defColor },   //�_���[�W				 8
			//{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 9, 32, 24, 32), defColor },  //�U��					 
			{ ML::Box2D(-14, -20, 28, 40), ML::Box2D(24 * 8, 32 * 4, 24, 32), defColor }, //�_�b�V��				 9
			//draw							src
			//{ ML::Box2D(-8, -20, 16, 40), ML::Box2D(0, 0, 32, 80), defColor },	//��~
			//{ ML::Box2D(-2, -20, 16, 40), ML::Box2D(32, 0, 32, 80), defColor },	//���s�P
			//{ ML::Box2D(-10, -20, 24, 40), ML::Box2D(64, 0, 48, 80), defColor },	//���s�Q dash
			//{ ML::Box2D(-10, -20, 24, 40), ML::Box2D(112, 0, 48, 80), defColor },	//���s�R
			//{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(48, 80, 48, 80), defColor },	//�W�����v
			//{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(96, 80, 48, 80), defColor },	//����
			//{ ML::Box2D(-12, -12, 24, 32), ML::Box2D(0, 80, 48, 64), defColor },	//��ї����O
			//{ ML::Box2D(-12, -12, 24, 32), ML::Box2D(144, 80, 48, 64), defColor },	//���n
			//{ ML::Box2D(-12, -12, 24, 40), ML::Box2D(176, 0, 48, 80), defColor },   //�_���[�W
			//{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(190, 80, 48, 80), defColor },  //�U��
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	�W�����v------------------------------------------------------------------------
		case  Motion::Jump:		rtv = imageTable[5];	break;
			//	�W�����v2------------------------------------------------------------------------
		case  Motion::Jump2:		rtv = imageTable[5];	break;
			//	�W�����v3------------------------------------------------------------------------
		case  Motion::Jump3:		rtv = imageTable[5];	break;
			//	��~----------------------------------------------------------------------------
		case  Motion::Stand:	rtv = imageTable[0];	break;
			//	���s----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 8;
			work %= 4;
			rtv = imageTable[work + 1];
			break;
			//	����----------------------------------------------------------------------------
		case  Motion::Fall:		rtv = imageTable[6];	break;
			//	����2----------------------------------------------------------------------------
		case  Motion::Fall2:		rtv = imageTable[6];	break;
			//	����3----------------------------------------------------------------------------
		case  Motion::Fall3:		rtv = imageTable[6];	break;
			//��ї����O-----------------------------------------------------------------------
		case  Motion::TakeOff:  rtv = imageTable[6];    break;
			//  ���n----------------------------------------------------------------------------
		case  Motion::Landing:  rtv = imageTable[7];    break;
			//  �_���[�W------------------------------------------------------------------------
		case  Motion::Bound:    rtv = imageTable[8];    break;
			//�@�U��----------------------------------------------------------------------------
		case  Motion::Attack:   rtv = imageTable[2]; break;
			//�@�_�b�V��------------------------------------------------------------------------
		case  Motion::Dash:      rtv = imageTable[9]; break;
			//  �_�b�V���N�[��------------------------------------------------------------------
		case  Motion::DashCt:    rtv = imageTable[9]; break;
			//  �U��2--------------------------------------------------------------------------
		//case  Motion::Attack2:  rtv = imageTable[9]; break;
		}
		//	�����ɉ����ĉ摜�����E���]����
		if (Angle_LR::Left == this->angle_LR) {
			rtv.draw.x = -rtv.draw.x;
			rtv.draw.w = -rtv.draw.w;
		}
		return rtv;
	}
	//-----------------------------------------------------------------------------
	//�ڐG���̉��������i�K���󂯐g�̏����Ƃ��Ď�������j
	void Object::Received(BChara* from_, AttackInfo at_)
	{
		if (motion == Motion::DashCt)
		{
			return;
		}
		if (this->unHitTime > 0)
		{
			return; //���G���Ԓ��̓_���[�W���󂯂Ȃ�
		}
		this->unHitTime = 90;
		this->hp -= at_.power; //������
		ge->hp = this->hp;
		if (this->hp <= 0)
		{
			this->Kill();
		}
		//������΂����
		if (this->pos.x > from_->pos.x)
		{
			this->moveVec = ML::Vec2(+4, -9);
		}
		else
		{
			this->moveVec = ML::Vec2(-4, -9);
		}
		this->UpdateMotion(Motion::Bound);
		//from_�͍U�����Ă�������A�J�E���^�[�Ȃǂŋt�Ƀ_���[�W��^�������Ƃ��Ɏg��
	}
	//-------------------------------------------------------------------
	//�X�L��
	void Object::Skill()
	{
 		for (int i = 0; i < ge->ss.size(); ++i)
		{
			switch (ge->ss[i]) //���ge�����Ł[
			{
			case MyPG::MyGameEngine::SelectedSkill::JumpUp:
				//�W�����v�񐔑����i�ő�3��j
				++ge->jumpMax;
				break;
			case MyPG::MyGameEngine::SelectedSkill::DashUp:
				//�_�b�V���񐔑���
				ge->dashMax++;
				break;
			case MyPG::MyGameEngine::SelectedSkill::HpUp:
				//�̗͑���
				ge->hp++;
				break;
			case MyPG::MyGameEngine::SelectedSkill::AtkUp:
				//�U���͑���
				ge->atk++;
				break;
			case MyPG::MyGameEngine::SelectedSkill::Special1:
				//����̓��ꋭ��1
				//this->WeaponSpecial1(weapon);
				this->CreateNum = 3;
				break;
				//case SelectedSkill::Special2:
				//	//����̓��ꋭ��2
				//	break;
			}
		}
	}
	//-------------------------------------------------------------------
	void Object::SkillImage()
	{

	}
	//-------------------------------------------------------------------
	//����̓��ꋭ��
	//void Object::WeaponSpecial1(Weapon weapon_)
	//{
	//	//this->CreateNum = 3;
	//	switch (this->weapon)
	//	{
	//	case Weapon::Sword:
	//	{
	//		auto sword = ge->GetTask<Sword::Object>(Map2D::defGroupName, Map2D::defName);
	//		//���̓��ꋭ��1
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
	//		//���̓��ꋭ��1
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
	//		//�e�̓��ꋭ��1
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
	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
				//�i���\�b�h�����ςȂ̂͋��o�[�W�����̃R�s�[�ɂ��o�O��������邽��
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
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
	//���\�[�X�N���X�̐���
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