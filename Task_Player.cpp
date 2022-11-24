//-------------------------------------------------------------------
//�v���C��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Map2D.h"


namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/chara02.png");
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
		this->dashSpeed = 5.0f;
		this->maxFallSpeed = 10.0f;	//�ő嗎�����x
		this->jumpPow = -8.0f;		//�W�����v�́i�����j
		this->gravity = ML::Gravity(32) * 5; //�d�͉����x�����ԑ��x�ɂ����Z��
		this->hp = 10;
		this->jumpCnt = 0;

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
		{
			ML::Box2D me = this->hitBase.OffsetCopy(this->pos);
			auto targets = ge->GetTasks<BChara>("Enemy");
			for (auto it = targets->begin(); it != targets->end(); ++it)
			{
				//����ɐڐG�̗L�����m�F������
				if ((*it)->CheckHit(me))
				{
					//����Ƀ_���[�W�̏������s�킹��
					BChara::AttackInfo at = { 1,0,0 };
					(*it)->Received(this, at);
					break;
				}
			}
		}

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
			if (inp.LStick.BL.on) { nm = Motion::Walk; }
			if (inp.LStick.BR.on) { nm = Motion::Walk; }
			if (inp.S1.down) { nm = Motion::TakeOff; }
			//if (inp.B3.down) { nm = Motion::Attack2; }
			if (inp.B4.down) { nm = Motion::Attack; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }//���� ��Q�@����
			break;
		case  Motion::Walk:		//�����Ă���
			if (inp.LStick.BL.off && inp.LStick.BR.off) { nm = Motion::Stand; }
			if (inp.S1.down) { nm = Motion::TakeOff; }
			//if (inp.B3.down) { nm = Motion::Attack2; }
			if (inp.B4.down) { nm = Motion::Attack; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			if (this->CheckFoot() == false) { nm = Motion::Fall; }
			break;
		case  Motion::Jump:		//�㏸��
			if (this->moveVec.y >= 0) { nm = Motion::Fall; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			break;
		case Motion::Jump2:
			if (this->moveVec.y >= 0) { nm = Motion::Fall2; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			break;
		case  Motion::Fall:		//������
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			if (inp.S1.down) { nm = Motion::Jump2; }
			break;
		case  Motion::Fall2:		//������
			if (this->CheckFoot() == true) { nm = Motion::Landing; }
			//if (inp.SE.down) { nm = Motion::Dash; }
			break;
		case  Motion::Attack:	//�U����
			if (this->moveCnt == 8) { nm = Motion::Stand; }
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
		case  Motion::Fall:		//������
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
		case  Motion::Fall2:		//������
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
		case  Motion::Jump:		//�㏸��
			this->jumpCnt++;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow; //�����ݒ�
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
		case  Motion::Jump2:		//�㏸��
			this->jumpCnt++;
			if (this->moveCnt == 0)
			{
				this->moveVec.y = this->jumpPow; //�����ݒ�
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
		case  Motion::Attack:	//�U����
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
			//draw							src
			{ ML::Box2D(-8, -20, 16, 40), ML::Box2D(0, 0, 32, 80), defColor },	//��~
			{ ML::Box2D(-2, -20, 16, 40), ML::Box2D(32, 0, 32, 80), defColor },	//���s�P
			{ ML::Box2D(-10, -20, 24, 40), ML::Box2D(64, 0, 48, 80), defColor },	//���s�Q dash
			{ ML::Box2D(-10, -20, 24, 40), ML::Box2D(112, 0, 48, 80), defColor },	//���s�R
			{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(48, 80, 48, 80), defColor },	//�W�����v
			{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(96, 80, 48, 80), defColor },	//����
			{ ML::Box2D(-12, -12, 24, 32), ML::Box2D(0, 80, 48, 64), defColor },	//��ї����O
			{ ML::Box2D(-12, -12, 24, 32), ML::Box2D(144, 80, 48, 64), defColor },	//���n
			{ ML::Box2D(-12, -12, 24, 40), ML::Box2D(176, 0, 48, 80), defColor },   //�_���[�W
			{ ML::Box2D(-12, -20, 24, 40), ML::Box2D(190, 80, 48, 80), defColor },  //�U��
		};
		BChara::DrawInfo  rtv;
		int  work;
		switch (this->motion) {
		default:		rtv = imageTable[0];	break;
			//	�W�����v------------------------------------------------------------------------
		case  Motion::Jump:		rtv = imageTable[4];	break;
			//	�W�����v2------------------------------------------------------------------------
		case  Motion::Jump2:		rtv = imageTable[4];	break;
			//	��~----------------------------------------------------------------------------
		case  Motion::Stand:	rtv = imageTable[0];	break;
			//	���s----------------------------------------------------------------------------
		case  Motion::Walk:
			work = this->animCnt / 8;
			work %= 3;
			rtv = imageTable[work + 1];
			break;
			//	����----------------------------------------------------------------------------
		case  Motion::Fall:		rtv = imageTable[5];	break;
			//	����2----------------------------------------------------------------------------
		case  Motion::Fall2:		rtv = imageTable[5];	break;
			//��ї����O-----------------------------------------------------------------------
		case  Motion::TakeOff:  rtv = imageTable[6];    break;
			//  ���n----------------------------------------------------------------------------
		case  Motion::Landing:  rtv = imageTable[7];    break;
			//  �_���[�W------------------------------------------------------------------------
		case  Motion::Bound:    rtv = imageTable[8];    break;
			//�@�U��----------------------------------------------------------------------------
		case  Motion::Attack:   rtv = imageTable[9]; break;
			//�@�_�b�V��------------------------------------------------------------------------
		case  Motion::Dash:      rtv = imageTable[2]; break;
			//  �_�b�V���N�[��------------------------------------------------------------------
		case  Motion::DashCt:    rtv = imageTable[2]; break;
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
		if (this->unHitTime > 0)
		{
			return; //���G���Ԓ��̓_���[�W���󂯂Ȃ�
		}
		this->unHitTime = 90;
		this->hp -= at_.power; //������
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