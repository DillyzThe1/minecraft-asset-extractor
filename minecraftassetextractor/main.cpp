#include <iostream>
#include "tinyfiledialogs.h"
#include <fstream>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;

char const* fileFilter[1] = { "*.json" };
char const* fileFilter2[1] = { "*.zip" };

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

int main() {
	// GET INDEX FILE
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

	std::string actualFileName;
	std::string fpcopy = file_path;
	actualFileName = fpcopy.substr(fpcopy.find_last_of("\\") + 1, fpcopy.find_last_of(".") - fpcopy.find_last_of("\\") - 1);
	std::cout << "we've been given a file called " << actualFileName << std::endl;
	std::cout << std::endl;

	std::string minecraftAssetsFolder;
	const char* assetsname = "assets";
	minecraftAssetsFolder = fpcopy.substr(0, fpcopy.rfind(assetsname));
	minecraftAssetsFolder = minecraftAssetsFolder + std::string("assets\\");
	std::cout << "we've been given the folder " << minecraftAssetsFolder << std::endl;
	std::cout << std::endl;

	// THE ACTUAL EXTRACTION

	std::filesystem::path parentTempDir = std::filesystem::temp_directory_path();
	char tempDir_cstr[250];
	sprintf(tempDir_cstr, "%sMCAssetExtractor\\", parentTempDir.string().c_str());
	std::filesystem::path tempDir = tempDir_cstr;
	std::filesystem::path tempAssetsDir = tempDir.string() + std::string("assets\\");

	std::cout << parentTempDir << std::endl;
	std::cout << tempDir_cstr << std::endl;
	std::cout << tempDir.string().c_str() << std::endl;
	std::cout << std::endl;

	std::filesystem::create_directory(tempDir);
	std::filesystem::create_directory(tempAssetsDir);

	std::cout << std::endl;

	// json reading
	std::ifstream indexFile_stream(file_path);
	json indexFile = json::parse(indexFile_stream);
	indexFile_stream.close();

	int objectCount = indexFile["objects"].size();
	std::cout << objectCount << " objects are to be extracted." << std::endl << std::endl;

	int counter = -1;
	for (auto curObj : indexFile["objects"].items()) {
		counter++;

		if (counter >= 2)
			continue;
		//json mainObject = indexFile["objects"].type_name();
		//std::string objectHash = indexFile["objects"][i]["hash"];
		//std::string objectSize = indexFile["objects"][i]["size"];

		//std::cout << objectHash << " - " << objectSize << std::endl;'
		
		std::string thepathstr = tempAssetsDir.string() + curObj.key().substr(0, curObj.key().find_last_of("/") + 1);
		replace(thepathstr, "/", "\\");
		std::filesystem::path pathWeHave = thepathstr;
		std::cout << pathWeHave.string().c_str() << std::endl;
		std::filesystem::create_directories(pathWeHave);

		std::string objName = curObj.key().substr(curObj.key().find_last_of("/") + 1, curObj.key().length() - curObj.key().find_last_of("/") - 1);
		std::cout << "Object Name: " << objName.c_str() << std::endl;

		std::string objHash = curObj.value()["hash"];
		std::cout << "Object Hash: " << objHash.c_str() << std::endl;

		std::string hashHeader = objHash.substr(0, 2);
		std::cout << "Object Hash Header: " << hashHeader.c_str() << std::endl;

		int objSize = curObj.value()["size"];
		std::cout << "Object Size: " << objSize << std::endl;

		std::string assetPath_in = minecraftAssetsFolder + std::string("objects\\") + hashHeader + std::string("\\") + objHash;
		std::string assetPath_out = tempAssetsDir.string() + curObj.key();
		replace(assetPath_out, "/", "\\");


		std::ifstream assetIn_stream(assetPath_in, std::ios::binary);
		if (assetIn_stream.fail() || !assetIn_stream.is_open()) {
			std::cout << "Cannot open " << assetPath_in << "!" << std::endl;
			continue;
		}

		std::ofstream assetOut_stream(assetPath_out, std::ios::binary);
		if (assetOut_stream.fail() || !assetOut_stream.is_open()) {
			std::cout << "Cannot open " << assetPath_out << "!" << std::endl;
			continue;
		}

		assetOut_stream << assetIn_stream.rdbuf();
		assetOut_stream.close();
		assetIn_stream.close();
	
		std::cout /* << curObj.key()*/ << std::endl;
	}

	// SAVE ALL THE ASSETS

	sprintf(folderPath, "C:\\Users\\%s\\Downloads\\mcassets_%s.zip", std::getenv("USERNAME"), actualFileName.c_str());
	char* save_path = tinyfd_saveFileDialog("Minecraft Assets ZIP", folderPath, 1, fileFilter2, "A ZIP containing all of your minecraft assets.");
	if (!save_path) {
		std::cout << "No file saved!\n";
		//std::filesystem::remove_all(tempDir);
		return 0;
	}
	std::cout << save_path << std::endl;
	std::cout << std::endl;

	//std::filesystem::remove_all(tempDir);
	std::cout << "Removed temp folder." << std::endl;
	std::cout << std::endl;
	return 0;
}