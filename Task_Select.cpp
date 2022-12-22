//-------------------------------------------------------------------
//�Z���N�g�V�[��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Select.h"
#include  "Task_Skill.h"
#include  "Task_Game.h"

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
		this->nos = 2;					//2��I���\
		this->selectedNumber = 0;
		this->moveVec = ML::Vec2(75, 0);												//�ړ���
		int x = ge->screen2DWidth / 2;
		int y = ge->screen2DHeight / 2;
		this->posMin = ML::Vec2(x - moveVec.x * 2, y);									//��ʒ������� x - moveVec.x * 2
		this->pos = this->posMin;														//�����l��Min
		this->posMax = this->posMin + moveVec * 4;								        //Min���� x + moveVec * 4
		this->drawBase = ML::Box2D(-25, -25, 50, 50);
		

		//���^�X�N�̐���
		auto skill = Skill::Object::Create(true);

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->KillAll_G("Skill");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
			auto next = Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		if (inp.L1.down) {
			//���g�ɏ��ŗv��
			this->Kill();
		}

		//A���ړ�
		if (inp.SE.down)
		{
			if (this->pos.x > this->posMin.x)//140/135>140/135
			{
				this->pos -= moveVec;
			}
		}
		//D�E�ړ�
		if (inp.L3.down)
		{
			if (this->pos.x < this->posMax.x)
			{
 				this->pos += moveVec;
			}
		}
		//�X�y�[�X����
		if (inp.S1.down)
		{

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
		switch (this->ss)
		{
		case SelectionState::Before:
			//�I��O
			break;
		case SelectionState::Weapon:
			//����I��
			break;
		case SelectionState::Skill:
			//�X�L���I��
			SelectedSkill();
			break;
		case SelectionState::Stage:
			//�X�e�[�W�I��
			SelectedStage();
			break;
		case SelectionState::After:
			//�I����
			break;
		}
	}
	//-------------------------------------------------------------------
	//�Z���N�g���\�b�h �e�Z���N�g���\�b�h���Ă�
	void  Object::Select()
	{
		//����ȍ~
		if (stageNum > 0)
		{
			//  Before		//�I��O
			//  Weapon		//����I��
			// 	Skill		//�X�L���I��
			//	Stage		//�X�e�[�W�I��
			//	After		//�I����
			switch (this->ss)
			{
			case SelectionState::Before:
				//�I��O
				break;
			case SelectionState::Weapon:
				//����I��
				//����ȍ~�����Ȃ�
				break;
			case SelectionState::Skill:
				//�X�L���I��
				SelectedSkill();
				break;
			case SelectionState::Stage:
				//�X�e�[�W�I��
				SelectedStage();
				break;
			case SelectionState::After:
				//�I����
				break;
			}
		}
		//����
		else
		{
			switch (this->ss)
			{
			case SelectionState::Before:
				//�I��O
				break;
			case SelectionState::Weapon:
				//����I��
				SelectedWeapon();
				break;
			case SelectionState::Skill:
				//�X�L���I��
				SelectedSkill();
				break;
			case SelectionState::Stage:
				//�X�e�[�W�I��
				SelectedStage();
				break;
			case SelectionState::After:
				//�I����
				break;
			}
		}
	}
	//-------------------------------------------------------------------
	//����I�����\�b�h
	void Object::SelectedWeapon()
	{
		auto inp = ge->in1->GetState();

	}
	//-------------------------------------------------------------------
	//����I�����\�b�h
	void Object::SelectedSkill()
	{
		auto inp = ge->in1->GetState();
	}
	//-------------------------------------------------------------------
	//����I�����\�b�h
	void Object::SelectedStage()
	{
		auto inp = ge->in1->GetState();
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