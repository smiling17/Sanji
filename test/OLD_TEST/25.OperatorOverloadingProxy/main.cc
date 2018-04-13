#include <stdio.h>

#include <string>

class Image {
	public:
		Image(std::string name) : name_(name) {}
		void getImage() { fprintf(stdout, "Image name : %s\n", name_.c_str()); }
		void setImage(const char* name) { name_ = name; }
	private:
		std::string name_;
};

class ImagePtr {
	public:
		ImagePtr(const char* imageFile) : file_(imageFile) {}
		~ImagePtr() {}
		Image* operator->() {
			return LoadImage();
		}
		Image& operator*() {
			return *LoadImage();
		}
	private:
		Image* LoadImage() {
			if (image_ == 0) {
				image_ = new Image(file_);
			}
			return image_;
		}
		Image* image_;
		std::string file_;
};

int main() {
	ImagePtr* ptr = new ImagePtr("sangjun");
	(*ptr)->getImage();
	return 0;
}
