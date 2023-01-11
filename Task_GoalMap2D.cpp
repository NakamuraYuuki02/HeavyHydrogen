//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_GoalMap2D.h"
#include  "Task_Goal.h"
#include  "Task_Goal5.h"
#include  "Task_Goal6.h"
#include  "Task_Goal7.h"
#include  "Task_Goal8.h"
#include  "Task_Goal9.h"

namespace  GoalMap2D
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
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
		this->render2D_Priority[1] = 0.9f;
		//�}�b�v�̃[���N���A
		for (int y = 0; y < 100; ++y) {
			for (int x = 0; x < 100; ++x) {
				this->arr[y][x] = 0;
			}
		}
		this->sizeX = 0;
		this->sizeY = 0;
		this->hitBase = ML::Box2D(block, block, block, block);

		//�}�b�v�`�b�v���̏�����
		for (int c = 0; c < 100; ++c) {
			int  x = (c % 10);
			int  y = (c / 10);
			this->chip[c] = ML::Box2D(x * 32, y * 32, 32, 32);
		}

		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		this->img.reset();

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
	
	}
	//-------------------------------------------------------------------
	//�}�b�v�ǂݍ���
	bool  Object::Load(const  string&  fpath_)
	{
		//�t�@�C�����J���i�ǂݍ��݁j
		ifstream   fin(fpath_);
		if (!fin) { return  false; }//�ǂݍ��ݎ��s

		//�}�b�v�z��T�C�Y�̓ǂݍ���
		fin >> this->sizeX >> this->sizeY;
		this->hitBase = ML::Box2D(0, 0, this->sizeX * block, this->sizeY * block);

		//�}�b�v�z��f�[�^�̓ǂݍ���
		for (int y = 0; y < this->sizeY; ++y) {
			for (int x = 0; x < this->sizeX; ++x) {
				fin >> this->arr[y][x];
			}
		}
		fin.close();

		return true;
	}
	//-------------------------------------------------------------------
	void Object::SetGoal()
	{
		for (int y = 0; y < this->sizeY; ++y) {
			for (int x = 0; x < this->sizeX; ++x) {
				// �`�b�v�̔ԍ��ɂ���ĕς���
				switch (this->arr[y][x])
				{
				   case 0:
				   {
					  auto g = Goal::Object::Create(true);
					  g->pos.x = x * 16;
					  g->pos.y = y * 16;
					  break;
				   }
				   case 5:
				   {
					   auto g5 = Goal5::Object::Create(true);
					   g5->pos.x = x * 16;
					   g5->pos.y = y * 16;
					   break;
				   }
				   case 6:
				   {
					   auto g6 = Goal6::Object::Create(true);
					   g6->pos.x = x * 16;
					   g6->pos.y = y * 16;
					   break;
				   }
				   case 7:
				   {
					   auto g7 = Goal7::Object::Create(true);
					   g7->pos.x = x * 16;
					   g7->pos.y = y * 16;
					   break;
				   }
				   case 8:
				   {
					   auto g8 = Goal8::Object::Create(true);
					   g8->pos.x = x * 16;
					   g8->pos.y = y * 16;
					   break;
				   }
				   case 9:
				   {
					   auto g9 = Goal9::Object::Create(true);
					   g9->pos.x = x * 16;
					   g9->pos.y = y * 16;
					   break;
				   }
				}
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