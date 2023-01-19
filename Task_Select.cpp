//-------------------------------------------------------------------
//�Z���N�g�V�[��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Select.h"
#include  "Task_Title.h"
#include  "Task_Game.h"

#include  "randomLib.h"

namespace  Select
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->selectUI = DG::Image::Create("./data/image/debug.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->selectUI.reset();
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
			this->stage[i] = ge->eapsedStage + i + 1;
		}
		
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
		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		
		Select();

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
		//UI��
		{
			ML::Box2D draw = drawBase;
			ML::Box2D src(0, 0, 32, 32);
			draw.Offset(this->pos);
			this->res->selectUI->Draw(draw, src);
		}
		switch (this->state)
		{
		case SelectionState::Before:
			//�I��O
			break;
		case SelectionState::Weapon:
			//����I��
			break;
		case SelectionState::Skill:
			//�X�L���I��
			break;
		case SelectionState::Stage:
			//�X�e�[�W�I��
			break;
		case SelectionState::After:
			//�I����
			break;
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
				//����ȍ~
				if (ge->stageNum>0)
				{
					this->posMin = ML::Vec2(ge->screen2DWidth / 2 - moveVec.x * 1, ge->screen2DHeight / 2);
					this->pos = this->posMin;
					this->posMax = this->posMin + moveVec * 2;
				}
				//����
				else
				{
					this->posMin = ML::Vec2(ge->screen2DWidth / 2, ge->screen2DHeight / 2);
					this->pos = this->posMin;
					this->posMax = this->posMin;
				}
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
		if (ge->stageNum > 0)
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
				if (SelectSkill())
				{
					sstate = SelectionState::Stage;
				}
				break;
			case SelectionState::Stage:
				//�X�e�[�W�I��
				if (SelectStageFirst())
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
		UpDateSelectionState(sstate);
	}
	//-------------------------------------------------------------------
	//����I�����\�b�h
	bool Object::SelectWeapon()
	{
		auto inp = ge->in1->GetState();
		bool r = false;
		
		//A���ړ�
		if (inp.SE.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//D�E�ړ�
		if (inp.L3.down && this->pos.x < this->posMax.x)
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

		//A���ړ�
		if (inp.SE.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//D�E�ړ�
		if (inp.L3.down && this->pos.x < this->posMax.x)
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

		//A���ړ�
		if (inp.SE.down && this->pos.x > this->posMin.x)
		{
			//140/135>140/135
			this->pos -= moveVec;
			--this->selectedNumber;
		}
		//D�E�ړ�
		if (inp.L3.down && this->pos.x < this->posMax.x)
		{
			this->pos += moveVec;
			++this->selectedNumber;
		}
		//�X�y�[�X����
		if (inp.S1.down)
		{
			ge->selectedStage = this->stage[this->selectedNumber]; 
			r = true;
		}
		return r;
	}
	//-------------------------------------------------------------------
	//����X�e�[�W�I�����\�b�h
	bool Object::SelectStageFirst()
	{
		auto inp = ge->in1->GetState();
		bool r = false;

		//�X�y�[�X����
		if (inp.S1.down)
		{
			ge->selectedStage = 0;
			r = true;
		}
		return r;
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