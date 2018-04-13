#include <stdio.h>

#include <string>
#include <vector>

class Parent {
	public:
		Parent() {}
		virtual ~Parent() {}
};

class Child 
	: public Parent 
{
	public:
		static std::vector<Child*> GetChildList() { return child_list_; }
		static void InsertChild(int age) {
			Child *child = new Child(age);
			child_list_.push_back(child);
			fprintf(stdout, "child 개수 : %d\n", child_list_.size());
		}
		~Child() {}
	private:
		int age_;
		Child(int age) : age_(age) {}
		static std::vector<Child*> child_list_;
};

std::vector<Child*> Child::child_list_ = std::vector<Child*>();

int main() {
	Child::InsertChild(10);
	return 0;
}


