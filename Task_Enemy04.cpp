//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Enemy04.h"
#include "Task_Player.h"
#include "Task_EnemyShot01.h"

namespace  Enemy04
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Chara00.png");
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
		this->render2D_Priority[1] = 0.6f;
		this->hitBase = ML::Box2D(-15, -15, 30, 30);
		this->angle_LR = Angle_LR::Left;
		this->motion = Motion::Stand;
		this->maxSpeed = 1.5f;
		this->addSpeed = 0.7f;
		this->decSpeed = 0.5f;
		this->maxFallSpeed = 10.0f;
		this->jumpPow = -6.0f;
		this->gravity = ML::Gravity(32) * 5;
		this->hp = 20;
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


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
		this->attackCnt++;
		this->Move();
		//�v�l�E�󋵔��f
		this->Think();
		ML::Vec2 est = this->moveVec;
		this->CheckMove(est);
		if (attackCnt % 10 == 0) {
			//�e�𐶐�
			auto shot = EnemyShot01::Object::Create(true);
			shot->pos = this->pos;
			if (this->angle_LR == Angle_LR::Right)
			{
				shot->moveVec = ML::Vec2(10, 0);
			}
			else
			{
				shot->moveVec = ML::Vec2(-10, 0);
			}
		}
		if (attackCnt % 10 == 0) {
			//�e�𐶐�
			auto shot = EnemyShot01::Object::Create(true);
			shot->pos = this->pos;
			if (this->angle_LR == Angle_LR::Right)
			{
				shot->moveVec = ML::Vec2(10, 10);
			}
			else
			{
				shot->moveVec = ML::Vec2(-10, 10);
			}
		}
		if (attackCnt % 10 == 0) {
			//�e�𐶐�
			auto shot = EnemyShot01::Object::Create(true);
			shot->pos = this->pos;
			if (this->angle_LR == Angle_LR::Right)
			{
				shot->moveVec = ML::Vec2(10, -10);
			}
			else
			{
				shot->moveVec = ML::Vec2(-10, -10);
			}
		}

		//�����蔻��
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto targets = ge->GetTasks<BChara>("Player");
			for (auto it = targets->begin(); it != targets->end(); ++it) {
				//����ɐڐG�̗L�����m�F������
				if ((*it)->CheckHit(me)) {
					//����Ƀ_���[�W�̏������s�킹��
					BChara::AttackInfo  at = { 4,0,0 };
					(*it)->Received(this, at);
					break;
				}
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D draw(this->hitBase);
		draw.Offset(this->pos);
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		ML::Box2D src(24, 7, 32, 23);
		this->res->img->Draw(draw, src);
	}
	//-----------------------------------------------------------------------------
	//�v�l���󋵔��f�@���[�V��������
	void  Object::Think()
	{
		BChara::Motion  nm = this->motion;	//�Ƃ肠�������̏�Ԃ��w��

		//�v�l�i���́j��󋵂ɉ����ă��[�V������ύX���鎖��ړI�Ƃ��Ă���B
		//���[�V�����̕ύX�ȊO�̏����͍s��Ȃ�
		switch (nm) {
		case  Motion::Stand:	//�����Ă���
			nm = Motion::Walk;
			break;
		case  Motion::Walk:		//�����Ă���
			if (this->CheckFront_LR() == true) { nm = Motion::Turn; }
			break;
		case  Motion::Jump:		//�㏸��
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			break;
		case  Motion::Fall:		//������
			if (this->CheckFoot() == true) { nm = Motion::Stand; }
			break;
		case  Motion::Attack:	//�U����
			break;
			if (this->moveCnt == 8) { nm = Motion::Stand; }
			break;
		case  Motion::TakeOff:	//��ї���
			break;
		case  Motion::Landing:	//���n

			break;
		case Motion::Bound:     //�_���[�W���󂯂Đ������ł���
			if (this->moveCnt >= 12 && this->CheckFoot() == true)
			{
				nm = Motion::Stand;
			}
			break;
		case Motion::Turn:
			if (this->moveCnt >= 5) { nm = Motion::Stand; }
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
		//�d�͉���
		switch (this->motion) {
		default:
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
		case Motion::Unnon:	break;
		case Motion::Walk: break;
		case Motion::Fall: break;
		case Motion::Stand: break;
		case Motion::Turn: break;
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
			if (this->angle_LR == Angle_LR::Left) {
				this->moveVec.x =
					max(-this->maxSpeed, this->moveVec.x - this->addSpeed);
			}
			if (this->angle_LR == Angle_LR::Right) {
				this->moveVec.x =
					min(+this->maxSpeed, this->moveVec.x + this->addSpeed);
			}
			break;
		case  Motion::Fall:		//������
			break;
		case  Motion::Jump:		//�㏸��
			break;
		case  Motion::Attack:	//�U����
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
	//�A�j���[�V��������
	BChara::DrawInfo  Object::Anim()
	{
		BChara::DrawInfo  rtv;
		//	�����ɉ����ĉ摜�����E���]����
		if (Angle_LR::Left == this->angle_LR) {
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
		//������΂����
		if (this->pos.x > from_->pos.x)
		{
			this->moveVec = ML::Vec2(+3, -3);
		}
		else
		{
			this->moveVec = ML::Vec2(-3, -3);
		}
		this->UpdateMotion(Motion::Bound);
	}
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