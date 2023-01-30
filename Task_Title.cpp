//-------------------------------------------------------------------
//�^�C�g�����
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include  "Task_Select.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->img = DG::Image::Create("./data/image/Title.bmp");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->img.reset( );
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
		this->logoPosY = -270;
		DataInitialize();
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->KillAll_G("Title");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//if(ge->ns == MyPG::MyGameEngine::NextScene::Select)
			//�������p���^�X�N�̐���
			auto  next = Select::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		this->logoPosY += 9;
		if (this->logoPosY >= 0) {
			this->logoPosY = 0;
		}

		if (this->logoPosY == 0) {
			if (inp.ST.down || inp.S1.down)
			{
				//�^�C�g���̎��̃Z���N�g�V�[�����w��B
				ge->ns = MyPG::MyGameEngine::NextScene::Select;
				//���g�ɏ��ŗv��
				this->Kill();
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(0, 0, 480, 270);
		ML::Box2D  src(0, 0, 240, 135);

		draw.Offset(0, this->logoPosY);
		this->res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//�f�[�^���������\�b�h �I�����ꂽ���e�Ȃǂ��^�C�g���Ŗ��񏉊���
	void Object::DataInitialize()
	{
		//���� ����Ɉ�I��
		ge->sw = MyPG::MyGameEngine::SelectedWeapon::Non;
		//�X�L�� �����I��
		for (int i = 0; i < ge->ssn; ++i)
		{
			ge->ss[i] = MyPG::MyGameEngine::SelectedSkill::Non;
		}
		//�X�e�[�W �����I��
		ge->selectedStage = 0;

		//�Q�[���N���A�ɕK�v�ȃX�e�[�W��
		ge->clearStageNum = 6;
		//�X�e�[�W�ʉߐ��A�Q�[���i�s�x
		ge->elapsedNum = 0;
		//�o�߂����X�e�[�W ��O�̃X�e�[�W�̔ԍ�
		ge->elapsedStage = 0;

		//�v���C���[�X�e�[�^�X
		ge->hp = 3;				//�̗�	
		ge->hpMax = 10;			//�ő�̗�
		ge->atk = 5;				//�U����
		ge->jumpCnt = 0;			//�W�����v��
		ge->jumpMax = 1;			//�W�����v�����
		ge->dashCnt = 0;			//�_�b�V����
		ge->dashMax = 0;			//�_�b�V�������
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