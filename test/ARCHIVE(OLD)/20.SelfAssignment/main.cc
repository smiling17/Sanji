#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>

class Paint {
	public:
		Paint(int painter ) : painter_(painter) {}
		Paint(Paint& ) = default;
		Paint& operator=(Paint& p) { fprintf(stdout, "operator call\n");
			if (&p == this) {
				fprintf(stdout, "same instance\n");
				return *this;
			}
			p.myPainter();
//			*this = p; /* 자체 무한 루프.. */
//			this = &p;
			painter_ = p.getPainter();
			fprintf(stdout, "operator call end\n");
			return *this;
		}
		~Paint() {}
		int getPainter() { return painter_; }
		void draw() { fprintf(stdout, "그림그리기\n"); }
		void myPainter() { fprintf(stdout, "my painter : %d\n", painter_); }
	private:
		int painter_;
};

int main() {
	int ret = -1;
	Paint paint1(10);
	paint1.myPainter();
	
	fprintf(stdout, "===== TEST 1 =====\n");
	Paint paint2(20);
	paint2.myPainter();
	fprintf(stdout, "===== TEST 2 =====\n");
	paint2 = paint1;
	paint2.myPainter();
	fprintf(stdout, "===== TEST 3 =====\n");
	paint2 = paint2;
	paint2.myPainter();

	ret = 0;
	return ret;
}
