//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Axe.h"
#include  "Task_Map2D.h"


namespace  Axe
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
		this->render2D_Priority[1] = 0.4f;
		this->pos.x = 0;
		this->pos.y = 0;
		this->hitBase = ML::Box2D(-8, -8, 16, 16);
		this->moveVec = ML::Vec2(0, 0);
		this->moveCnt = 0;
		this->gravity = ML::Gravity(32) * 5;
		this->hp = 5;
		this->atk = 5;

		this->WeaponLevel = 99;

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
		//���E�̎��Ԃ��}���������
		if (this->moveCnt >= 30) {
			//���Ő\��
			this->Kill();
			return;
		}
		//�ړ�
		this->pos += this->moveVec;
		//�d��
		this->moveVec.y += this->gravity;

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
					BChara::AttackInfo at = { this->hp,0,0 };
					(*it)->Received(this, at);
					this->Kill();
					break;
				}
			}
		}

		//�ړ���ŏ�Q���ɐڐG���������
		//�}�b�v�����݂��邩���ׂĂ���A�N�Z�X
		if (auto   map = ge->GetTask<Map2D::Object>(Map2D::defGroupName, Map2D::defName)) {
			ML::Box2D  hit = this->hitBase.OffsetCopy(this->pos);
			if (true == map->CheckHit(hit)) {
				//���Ő\��
				this->Kill();

				////�Ƃ肠�������͂΂�T����
				//for (int c = 0; c < 4; ++c) {
				//	auto  eff = Effect00::Object::Create(true);
				//	eff->pos = this->pos;
				//}
				//return;
			}
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(-8, -8, 16, 16);
		draw.Offset(this->pos);
		ML::Box2D  src(0, 0, 32, 32);

		//�X�N���[���Ή�
		draw.Offset(-ge->camera2D.x, -ge->camera2D.y);
		this->res->img->Draw(draw, src);
	}
	//-------------------------------------------------------------------
	//�����Level
	void Object::Level(BChara* from_)
	{
		
		switch (WeaponLevel)
		{
		case 0:
		case 1:
			this->hitBase = ML::Box2D(-24, -24, 48, 48);
			break;
		case 2:
			break;
		case 3:
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