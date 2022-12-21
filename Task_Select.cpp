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
		this->UI = DG::Image::Create("./data/image/debug.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->UI.reset();
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
		//��ʒ���
		this->center = ML::Vec2(ge->screen2DWidth / 2, ge->screen2DHeight / 2);
		this->pos = this->center;
		this->posMax = this->center - ML::Vec2(100, 0);
		this->drawBase = ML::Box2D(-25, -25, 50, 50);
		this->moveVec = ML::Vec2(50, 0);

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

		//A����
		if (inp.SE.down)
		{
			this->pos -= moveVec;
		}
		//D����
		if (inp.L3.down)
		{
			this->pos += moveVec;
		}
		//�X�y�[�X����
		if (inp.S1.down)
		{

		}
		//�X�L���I������

		//�}�b�v�I������

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
			this->res->UI->Draw(draw, src);
		}
	}
	//-------------------------------------------------------------------
	//�Z���N�g���\�b�h �e�Z���N�g���\�b�h���Ă�
	void  Object::Select()
	{
		//����ȍ~
		if (stageNum > 0)
		{
			switch (this->ss)
			{
			case SelectionState::Skill:
				SelectedSkill();
				break;
			case SelectionState::Stage:
				SelectedStage();
				break;
			}
		}
		//����
		else
		{
			switch (this->ss)
			{
			case SelectionState::Weapon:
				SelectedWeapon();
				break;
			case SelectionState::Skill:
				SelectedSkill();
				break;
			case SelectionState::Stage:
				SelectedStage();
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