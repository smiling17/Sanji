#include <stdio.h>

class First {
	public:
		virtual void Shout (){ fprintf(stdout, "내가 첫번째따! \n"); }
	private:
		int num;
};

class Second : public First
{
	public:
		virtual void Shout () override { fprintf(stdout, "내가 두번째다! \n"); }
};

class Final : public Second
{
	public:
//		virtual void Shout () override { fprintf(stdout, "내가 세번째다! \n"); }
		void Shout2 () { fprintf(stdout, "세번째 나만의 함수! \n"); }
};

int main() {
	int ret = -1;

	First* pFirst = new First();
	pFirst->Shout();

	Second* pSec = new Second();
	pSec->Shout();

	Final* pfinal = new Final(); 
	pfinal->Shout();

	ret = 0;
	return ret;
}
