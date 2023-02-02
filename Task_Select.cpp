//-------------------------------------------------------------------
//�Z���N�g�V�[��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Select.h"
#include  "Task_Title.h"
#include  "Task_Game.h"

#include  "sound.h"

#include  "randomLib.h"

namespace  Select
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->selectBack = DG::Image::Create("./data/image/SelectSceneBack.png");
		this->selectUI = DG::Image::Create("./data/image/debug.png");
		this->stageNumberUI = DG::Image::Create("./data/image/StageNumberUI.png");
		this->stageNumber = DG::Image::Create("./data/image/UItext.png");
		this->sword = DG::Image::Create("./data/image/Sword.png");
		this->axe = DG::Image::Create("./data/image/Axe.png");
		this->gun = DG::Image::Create("./data/image/Gun.png");
		this->jump = DG::Image::Create("./data/image/jump.png");
		this->dash = DG::Image::Create("./data/image/Dash.png");
		this->heart = DG::Image::Create("./data/image/heart2.png");
		this->powerUP = DG::Image::Create("./data/image/PowerUP.png");
		this->swordSP = DG::Image::Create("./data/image/SwordSP.png");
		this->axeSP = DG::Image::Create("./data/image/AxeSP.png");
		this->gunSP = DG::Image::Create("./data/image/GunSP.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->selectBack.reset();
		this->selectUI.reset();
		this->stageNumberUI.reset();
		this->stageNumber.reset();
		this->sword.reset();
		this->axe.reset();
		this->gun.reset();
		this->jump.reset();
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
		this->selectedNumber = 0;
		this->moveVec = ML::Vec2(75, 0);												//�ړ���
		int x = ge->screen2DWidth / 2;
		int y = ge->screen2DHeight / 2;
		this->posMin = ML::Vec2(x - moveVec.x * 2, y);									//��ʒ������� x - moveVec.x * 2
		this->pos = this->posMin;														//�����l��Min
		this->posMax = this->posMin + moveVec * 4;										//Min���� x + moveVec * 4
		this->drawBase = ML::Box2D(-25, -25, 50, 50);
		this->state = SelectionState::Before;
		for (int i = 0; i < 3; ++i)
		{
			//�N���A�����X�e�[�W�̎��X�e�[�W���O�擾
			this->stage[i] = ge->elapsedStage + i + 1;
		}
		this->stopCnt = 0;
		
		bgm::LoadFile("BGM1", "./data/Sound/BGM/waterloobridge.mp3");
		bgm::Play("BGM1");

		se::LoadFile("SE1", "./data/Sound/SE/isi.wav");
		se::LoadFile("SE2", "./data/Sound/SE/1.wav");
		se::LoadFile("SE3", "./data/Sound/SE/2.wav");
		//se::LoadFile("SE4", "./data/Sound/SE/3.wav");
		se::LoadFile("SE5", "./data/Sound/SE/4.wav");

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->KillAll_G("Select");

		if (!ge->QuitFlag() && this->nextTaskCreate)
		{
			//�������p���^�X�N�̐���
			/*switch (ge->ns)
			{
			case MyPG::MyGameEngine::NextScene::Title:
				auto title = Title::Object::Create(true);
				break;
			case MyPG::MyGameEngine::NextScene::Game:
				auto game = Game::Object::Create(true);
				break;
			}*/
			if (ge->ns == MyPG::MyGameEngine::NextScene::Game)
			{
				auto next = Game::Object::Create(true);
			}
			else
			{
				auto next = Title::Object::Create(true);
			}
		}

		bgm::Stop("BGM1");

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		
		Select();
		PlaySE();

		if (inp.ST.down)
		{
			//�^�C�g���ɂ��ǂ�B
			ge->ns = MyPG::MyGameEngine::NextScene::Title;
			//���g�ɏ��ŗv��
			this->Kill();
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�w�i
		{
			ML::Box2D draw(0, 0, ge->screen2DWidth, ge->screen2DHeight);
			ML::Box2D src(50,50,800,400); 
			this->res->selectBack->Draw(draw, src);
		}
		//�A�C�R��
		{
			ML::Box2D draw = this->drawBase;
			ML::Box2D src[5];
			for (int i = 0; i < 5; ++i)
			{
				src[i] = ML::Box2D(-1, -1, -1, -1);
			}
			switch (this->state)
			{
			case SelectionState::Before:
				//�I��O
				break;
			case SelectionState::Weapon:
				//����I��
				src[0] = ML::Box2D(0, 0, 16, 16);
				src[1] = ML::Box2D(0, 0, 14, 14);
				src[2] = ML::Box2D(0, 0, 15, 10);
				draw.Offset(this->posMin);
				this->res->sword->Draw(draw, src[0]);
				draw.Offset(this->moveVec);
				this->res->axe->Draw(draw, src[1]);
				draw.Offset(this->moveVec);
				this->res->gun->Draw(draw, src[2]);
				break;
			case SelectionState::Skill:
				//�X�L���I��
				src[0] = ML::Box2D(0, 0, 31, 31);
				src[1] = ML::Box2D(0, 0, 12, 12);
				draw.Offset(this->posMin);
				this->res->jump->Draw(draw, src[0]);
				draw.Offset(this->moveVec);
				this->res->dash->Draw(draw, src[0]);
				draw.Offset(this->moveVec);
				this->res->heart->Draw(draw, src[1]);
				draw.Offset(this->moveVec);
				this->res->powerUP->Draw(draw, src[0]);
				draw.Offset(this->moveVec);
				switch (ge->sw)
				{
				case MyPG::MyGameEngine::SelectedWeapon::Sword:
					this->res->swordSP->Draw(draw, src[0]);
					break;
				case MyPG::MyGameEngine::SelectedWeapon::Axe:
					this->res->axeSP->Draw(draw, src[0]);
					break;
				case MyPG::MyGameEngine::SelectedWeapon::Gun:
					this->res->gunSP->Draw(draw, src[0]);
					break;
				}
				break;
			case SelectionState::Stage:
				//�X�e�[�W�I��
				break;
			case SelectionState::After:
				//�I����
				break;
			}
		}
		//UI��
		{
			ML::Box2D draw = this->drawBase;
			ML::Box2D src(0, 0, 32, 32);
			draw.Offset(this->pos);
			this->res->selectUI->Draw(draw, src);
		}
		//�u�X�e�[�W���v�\��
		ML::Box2D stageNumDraw(320, 240, 120, 20);
		ML::Box2D stageNumSrc(0, 0, 120, 20);
		this->res->stageNumberUI->Draw(stageNumDraw, stageNumSrc);

		//�}�b�v���\��
		char mapNum[10];
		sprintf(mapNum, "%02d", ge->elapsedNum);
		for (int i = 0; i < 2; ++i) {
			int num = mapNum[i] - '0';//'0'== 48
			ML::Box2D numDraw(420, 230, 18, 32);
			ML::Box2D numSrc(num * 30, 0, 30, 52);
			numDraw.Offset(i * 18, 0);//�����Ԋu
			this->res->stageNumber->Draw(numDraw, numSrc);
		}
	}
	//-------------------------------------------------------------------
	//State�X�V���\�b�h
	bool Object::UpDateSelectionState(SelectionState state_)
	{
		//�X�V����Ă��Ȃ��ꍇ�͏��������Ȃ�
		if (state_ == this->state)
		{
			return false;
		}
		else
		{
			//�X�e�[�g���X�V
			this->state = state_;
			//�I��l�����Z�b�g
			this->selectedNumber = 0;
			//UI�\���ʒu�����炷���ߊe�l��ݒ�
			switch (this->state)
			{
			case SelectionState::Before:
				//�I��O
				break;
			case SelectionState::Weapon:
				//����I��
				this->posMin = ML::Vec2(ge->screen2DWidth / 2 - moveVec.x * 1, ge->screen2DHeight / 2);
				this->pos = this->posMin;
				this->posMax = this->posMin + moveVec * 2;
				break;
			case SelectionState::Skill:
				//�X�L���I��
				this->posMin = ML::Vec2(ge->screen2DWidth / 2 - moveVec.x * 2, ge->screen2DHeight / 2);
				this->pos = this->posMin;
				this->posMax = this->posMin + moveVec * 4;
				this->s = MyPG::MyGameEngine::SelectedSkill::Non;
				//GetRandom(0, 1);
				break;
			case SelectionState::Stage:
				//�X�e�[�W�I��
				this->posMin = ML::Vec2(ge->screen2DWidth / 2 - moveVec.x * 1, ge->screen2DHeight / 2);
				this->pos = this->posMin;
				this->posMax = this->posMin + moveVec * 2;
				break;
			case SelectionState::After:
				//�I����
				break;
			}
		}
	}
	//-------------------------------------------------------------------
	//�Z���N�g���\�b�h �e�Z���N�g���\�b�h���Ă�
	void  Object::Select()
	{
		Object::SelectionState sstate = this->state;
		//����ȍ~
		if (ge->elapsedNum > 0)
		{
			//  Before		//�I��O
			//  Weapon		//����I��
			// 	Skill		//�X�L���I��
			//	Stage		//�X�e�[�W�I��
			//	After		//�I����
			switch (sstate)
			{
			case SelectionState::Before:
				//�I��O
				sstate = SelectionState::Weapon;
				break;
			case SelectionState::Weapon:
				//����I��
				//����ȍ~�����Ȃ�
				sstate = SelectionState::Skill;
				break;
			case SelectionState::Skill:
				//�X�L���I��
				if (SelectSkill())
				{
					sstate = SelectionState::Stage;
				}
				break;
			case SelectionState::Stage:
				//�X�e�[�W�I��
				if (SelectStage())
				{
					sstate = SelectionState::After;
				}
				break;
			case SelectionState::After:
				//�I����
				ge->ns = MyPG::MyGameEngine::NextScene::Game;
				//���g�ɏ��ŗv��
				this->Kill();
				break;
			}
		}
		//����
		else
		{
			switch (sstate)
			{
			case SelectionState::Before:
				//�I��O
				sstate = SelectionState::Weapon;
				break;
			case SelectionState::Weapon:
				//����I��
				if (SelectWeapon())
				{
					sstate = SelectionState::Skill;
				}
				break;
			case SelectionState::Skill:
				//�X�L���I��
				sstate = SelectionState::Stage;
				break;
			case SelectionState::Stage:
				//�X�e�[�W�I��
				sstate = SelectionState::After;
				break;
			case SelectionState::After:
				//�I����
				ge->ns = MyPG::MyGameEngine::NextScene::Game;
				//���g�ɏ��ŗv��
				this->Kill();
				break;
			}
		}
		UpDateSelectionState(sstate);
	}
	//-------------------------------------------------------------------
	//����I�����\�b�h
	bool Object::SelectWeapon()
	{
		auto inp = ge->in1->GetState();
		bool r = false;
		
		//���ړ�
		if (inp.LStick.BL.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//�E�ړ�
		if (inp.LStick.BR.down && this->pos.x < this->posMax.x)
		{
			this->pos += moveVec;
			++this->selectedNumber;
		}
		//�X�y�[�X����
		if (inp.S1.down)
		{
			//�����ݒ�
			switch (this->selectedNumber)
			{
			case 0:
				ge->sw = MyPG::MyGameEngine::SelectedWeapon::Sword;
				break;
			case 1:
				ge->sw = MyPG::MyGameEngine::SelectedWeapon::Axe;
				break;
			case 2:
				ge->sw = MyPG::MyGameEngine::SelectedWeapon::Gun;
				break;
			}
			r = true;
		}
		return r;
	}
	//-------------------------------------------------------------------
	//�X�L���I�����\�b�h
	bool Object::SelectSkill()
	{
		auto inp = ge->in1->GetState();
		bool r = false;

		//���ړ�
		if (inp.LStick.BL.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//�E�ړ�
		if (inp.LStick.BR.down && this->pos.x < this->posMax.x)
		{
			this->pos += moveVec;
			++this->selectedNumber;
		}
		//�X�y�[�X����
		if (inp.S1.down)
		{
			//sn��ss�֕ϊ�
			switch (this->selectedNumber)
			{
			case 0:
				this->s = MyPG::MyGameEngine::SelectedSkill::JumpUp;
				break;
			case 1:
				this->s = MyPG::MyGameEngine::SelectedSkill::DashUp;
				break;
			case 2:
				this->s = MyPG::MyGameEngine::SelectedSkill::HpUp;
				break;
			case 3:
				this->s = MyPG::MyGameEngine::SelectedSkill::AtkUp;
				break;
			case 4:
				//sp1 or sp2
				/*if ()
				{
				}*/
				break;
			}
			ge->ss[0] = this->s;
			r = true;
		}
		return r;
	}
	//-------------------------------------------------------------------
	//�X�e�[�W�I�����\�b�h
	bool Object::SelectStage()
	{
		auto inp = ge->in1->GetState();
		bool r = false;

		//���ړ�
		if (inp.LStick.BL.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//�E�ړ�
		if (inp.LStick.BR.down && this->pos.x < this->posMax.x)
		{
			this->pos += moveVec;
			++this->selectedNumber;
		}
		//�X�y�[�X����
		if (inp.S1.down)
		{
			ge->selectedStage = this->stage[this->selectedNumber];
			ge->elapsedStage = ge->selectedStage;
			r = true;
		}
		return r;
	}
	//-------------------------------------------------------------------
	//SE�Đ����\�b�h
	void Object::PlaySE()
	{
		auto inp = ge->in1->GetState();
		string se1 = "SE2";
		string se2 = "SE5";

		if (inp.LStick.BL.down || inp.LStick.BR.down)
		{
			se::Play(se2);
			this->stopCnt = 10;
		}
		if (inp.S1.down)
		{
			se::Play(se1);
			this->stopCnt = 10;
		}
		if (this->stopCnt < 0)
		{
			se::AllStop();
		}
		--this->stopCnt;
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