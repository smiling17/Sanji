#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

class WininetHandle {
	public:
		WininetHandle(int index) : index_(index) {}
	private:
		int index_;
};

class HandleTable {
	public:
		virtual void createHandle(int index) = 0;
		void setUsed(bool used) { used_ = used; }
	private:
		std::vector<WininetHandle*> handle_table_;
		bool used_;
};

//std::vector<WininetHandle*> HandleTable::handle_table_ = std::vector<WininetHandle*>();

class WininetHandleTable : public HandleTable {
	public:
		void createHandle(int index) override {
			WininetHandle *handle = new WininetHandle(index);
			setUsed(false);
//			used_ = true;
//			handle_table_.push_back(handle);
		}
};

int main() {
	int ret = -1;

	ret = 0;
	return ret;
}

/**
 *
 * 결론
 * 1) static 맴버로는 virtual를 쓸 수가 없다.
 * 2) protected attribute 는 public 상속을 해도 내부에서 사용할 수가 없음
 * 2-1) protected 상속을 해야 Derived 객체 내에서 사용할 수 있다.
 */
