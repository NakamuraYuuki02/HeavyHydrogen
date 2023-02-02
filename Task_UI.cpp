//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_UI.h"

namespace  UI
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->UIhp = DG::Image::Create("./data/image/heart2.png");
		this->UInum = DG::Image::Create("./data/image/UItext.png");
		this->gun = DG::Image::Create("./data/image/handgun2.png");
		this->sword = DG::Image::Create("./data/image/Sword.png");
		this->axe = DG::Image::Create("./data/image/Axe.png");
		this->tutorial = DG::Image::Create("./data/image/tutorial2.png");
		this->command = DG::Image::Create("./data/image/command2.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->UIhp.reset();
		this->UInum.reset();
		this->gun.reset();
		this->sword.reset();
		this->axe.reset();
		this->tutorial.reset();
		this->command.reset();
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
		this->weapon = ge->sw;
		
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
		ML::Box2D cDraw(0, 235, 207, 32);
		ML::Box2D cSrc(0, 0, 207, 32);
		this->res->command->Draw(cDraw, cSrc);

		auto  inp = ge->in1->GetState();
		//�`���[�g���A��
		if (true == inp.B1.on)//Z�L�[��������\��
		{
			ML::Box2D tDraw(ge->screen2DWidth / 2 - 100, ge->screen2DHeight / 2 - 100, 320, 192);
			ML::Box2D tSrc(0, 0, 320, 192);
			this->res->tutorial->Draw(tDraw, tSrc);
		}

		//hp��(�����l3)�n�[�g��\��
		for (int i = 0; i < ge->hp; ++i)
		{
			ML::Box2D hpDraw(i * 22, 0, 20, 20);
			ML::Box2D hpSrc(0, 0, 12, 12);
			this->res->UIhp->Draw(hpDraw, hpSrc);
		}

		//����A�C�R��
		switch (this->weapon)
		{
		case MyPG::MyGameEngine::SelectedWeapon::Sword:
		    {
			  ML::Box2D SwordDraw(20, 30, 28, 28);
			  ML::Box2D SwordSrc(0, 0, 16, 16);
			  this->res->sword->Rotation(0, ML::Vec2(0, 0));
			  this->res->sword->Draw(SwordDraw, SwordSrc);
			  break;
		    }
		case MyPG::MyGameEngine::SelectedWeapon::Axe:
		    { 
			  ML::Box2D AxeDraw(20, 30, 26, 26);
			  ML::Box2D AxeSrc(0, 0, 14, 14);
			  this->res->axe->Rotation(0, ML::Vec2(0, 0));
			  this->res->axe->Draw(AxeDraw, AxeSrc);
			  break;
			}
		case MyPG::MyGameEngine::SelectedWeapon::Gun:
		    { 
			  ML::Box2D GunDraw(20, 30, 45, 30);
			  ML::Box2D GunSrc(0, 0, 51, 30);
			  this->res->gun->Rotation(0, ML::Vec2(0, 0));
			  this->res->gun->Draw(GunDraw, GunSrc);
			  break;
			}
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