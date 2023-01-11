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
		this->UIhp = DG::Image::Create("./data/image/HPheart.png");
		this->UInum = DG::Image::Create("./data/image/UItext.png");
		this->gun = DG::Image::Create("./data/image/Gun.png");
		this->sword = DG::Image::Create("./data/image/Gun.png");
		this->axe = DG::Image::Create("./data/image/Gun.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->UIhp.reset();
		this->UInum.reset();
		this->gun.reset();
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
		//hp��(�����l3)�n�[�g��\��
		for (int i = 0; i < ge->hp; ++i)
		{
			ML::Box2D hpDraw(i * 22, 0, 22, 22);
			ML::Box2D hpSrc(0, 0, 465, 396);
			this->res->UIhp->Draw(hpDraw, hpSrc);
		}

		//�}�b�v���\��
		char mapNum[10];
		sprintf(mapNum, "%02d", ge->stageNum);
		for (int i = 0; i < 2; ++i) {
			int num = mapNum[i] - '0';//'0'== 48
			ML::Box2D numDraw(0, 25, 32, 32);
			ML::Box2D numSrc(num * 30, 0, 30, 52);
			numDraw.Offset(i * 22, 0);//�����Ԋu
			this->res->UInum->Draw(numDraw, numSrc);
		}

		//����A�C�R��
		switch (this->ui)
		{
		case UI::Sword:
		    {
			  ML::Box2D SwordDraw(50, 30, 45, 30);
			  ML::Box2D SwordSrc(0, 0, 15, 10);
			  this->res->sword->Draw(SwordDraw, SwordSrc);
			  break;
		    }
		case UI::Axe:
		    { 
			  ML::Box2D AxeDraw(50, 30, 45, 30);
			  ML::Box2D AxeSrc(0, 0, 15, 10);
			  this->res->axe->Draw(AxeDraw, AxeSrc);
			  break;
			}
		case UI::Gun:
		    { 
			  ML::Box2D GunDraw(50, 30, 45, 30);
			  ML::Box2D GunSrc(0, 0, 15, 10);
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