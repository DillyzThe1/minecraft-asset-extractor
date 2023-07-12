#include <iostream>
#include "tinyfiledialogs.h"

char const* fileFilter[1] = { "*.json" };
int main() {
	std::cout << " m,m .m  mien craft f it\n";
	char folderPath[250];
	sprintf(folderPath, "C:\\Users\\%s\\AppData\\Roaming\\.minecraft\\assets\\indexes\\indexes", std::getenv("USERNAME"));
	std::cout << folderPath << std::endl;
	char* file_path = tinyfd_openFileDialog("Locate Index File", folderPath, 1, fileFilter, "Minecraft Index JSON", 0);
	if (!file_path) {
		std::cout << "No file given!\n";
		return 0;
	}
	std::cout << file_path << std::endl;
	return 0;
}