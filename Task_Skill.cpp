//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Skill.h"
#include  "Task_Player.h"
#include  "Task_Sword.h"
#include  "Task_Axe.h"
#include  "Task_Shot00.h"

namespace  Skill
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/debug.png");
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
		this->render2D_Priority[1] = 1.0f;
	
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
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(0, 0, 32, 32);
		ML::Box2D  src(0, 0, 32, 32);
		this->res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	void Object::Skill(BChara* from_)
	{
		switch (this->ss)
		{
		case SelectedSkill::JumpUp:
			//�W�����v�񐔑����i�ő�3��j
			//from_->jumpMax++;
			break;
		case SelectedSkill::DashUp:
			//�_�b�V���񐔑���
			//from_->dashMax++;
			break;
		case SelectedSkill::HpUp:
			//�̗͑���
			//from_->hp++;
			break;
		case SelectedSkill::AtkUp:
			//�U���͑���
			//from_->atk++;
			break;
		case SelectedSkill::Special1:
			break;
		}
	}
	//-------------------------------------------------------------------
	void Object::SkillImage()
	{

	}
	//-------------------------------------------------------------------
	//����̓��ꋭ��
	void Object::WeaponSpecial1(Weapon weapon_)
	{
		auto   player = ge->GetTasks<Player::Object>(Player::defGroupName, Player::defName);
		auto   axe = ge->GetTask<Axe::Object>(Axe::defGroupName, Axe::defName);
		switch (this->weapon)
		{
		case Weapon::Sword:
			//���̓��ꋭ��1

			break;
		case Weapon::Axe:
			//���̓��ꋭ��1
			for (int i = 1; i <= 3; ++i)
			{
				axe->moveVec = ML::Vec2(-7, -3 * i);
				//axe->pos = player->pos + ML::Vec2(-30, 0);
				/*auto axe = Axe::Object::Create(true);
				axe->Level(this);
				axe->moveVec = ML::Vec2(-7, -3 * i);
				axe->pos = this->pos + ML::Vec2(-30, 0);*/
			}
			break;
		case Weapon::Gun:
			//�e�̓��ꋭ��1
			for (int i = 1; i <= 3; ++i)
			{
				/*auto gun = Shot00::Object::Create(true);
				gun->Level(this);
				gun->moveVec = ML::Vec2(-8, 0);
				gun->pos = this->pos + ML::Vec2(-20 * i, 0);*/
			}
			break;
		}
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