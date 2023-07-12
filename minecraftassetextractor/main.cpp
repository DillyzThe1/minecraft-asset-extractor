#include <iostream>
#include "tinyfiledialogs.h"
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "zip_file.hpp"

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
	std::cout << "Minecraft Asset Extractor (By DillyzThe1)\n\n";
	char folderPath[250];
	sprintf(folderPath, "C:\\Users\\%s\\AppData\\Roaming\\.minecraft\\assets\\indexes\\indexes", std::getenv("USERNAME"));
	std::cout << folderPath << std::endl;
	std::string file_path = tinyfd_openFileDialog("Locate Index File", folderPath, 1, fileFilter, "Minecraft Index JSON", 0);
	if (file_path.empty()) {
		std::cout << "No file given!\n";
		return 0;
	}
	std::cout << file_path << std::endl;

	std::string actualFileName;
	actualFileName = file_path.substr(file_path.find_last_of("\\") + 1, file_path.find_last_of(".") - file_path.find_last_of("\\") - 1);
	std::cout << "we've been given a file called " << actualFileName << std::endl << std::endl;
	std::string minecraftAssetsFolder;
	minecraftAssetsFolder = file_path.substr(0, file_path.rfind("assets"));
	minecraftAssetsFolder = minecraftAssetsFolder + std::string("assets\\");
	std::cout << "we've been given the folder " << minecraftAssetsFolder << std::endl << std::endl;

	// THE ACTUAL EXTRACTION
	std::ifstream indexFile_stream(file_path);
	json indexFile = json::parse(indexFile_stream);
	indexFile_stream.close();
	int objectCount = indexFile["objects"].size();
	std::cout << objectCount << " objects are pending for extraction." << std::endl << std::endl;
	miniz_cpp::zip_file outputZip;
	std::string notetxt("These are the Minecraft assets indexed by " + actualFileName + ", ripped by MinecraftAssetExtractor.\nHave fun!\n\n       - DillyzThe1\n\nFailed Files:");

	int totalSize = 0;
	for (auto curObj : indexFile["objects"].items()) {
		std::cout << "Object Name: " << curObj.key().substr(curObj.key().find_last_of("/") + 1, curObj.key().length() - curObj.key().find_last_of("/") - 1).c_str() << std::endl;
		std::string objHash = curObj.value()["hash"];
		std::cout << "Object Hash: " << objHash.c_str() << std::endl;
		std::string hashHeader = objHash.substr(0, 2);
		std::cout << "Object Hash Header: " << hashHeader.c_str() << std::endl;
		int objSize = curObj.value()["size"];
		std::cout << "Object Size: " << objSize << std::endl;
		std::string assetPath_in = minecraftAssetsFolder + std::string("objects\\") + hashHeader + std::string("\\") + objHash;
		try {
			if (curObj.key().find("music") == -1) {
				std::ifstream assetIn_stream(assetPath_in, std::ios::binary);
				if (assetIn_stream.fail() || !assetIn_stream.is_open()) {
					std::cout << "Cannot open " << assetPath_in << "!" << std::endl;
					continue;
				}
				std::stringstream ss;
				ss << assetIn_stream.rdbuf();
				outputZip.writestr(curObj.key(), ss.str());
				totalSize += objSize;
				assetIn_stream.close();
			}
			else {
				std::cout << "Unsupported File." << std::endl;
				notetxt += "\n- " + curObj.key() + " (Music is unsupported)";
			}
		}
		catch (std::exception e) {
			notetxt += "\n- " + curObj.key() + " (" + e.what() + ")";
		}
		std::cout << "Total Archive Size: " << totalSize << std::endl << std::endl;
	}
	outputZip.writestr("note.txt", notetxt);
	// SAVE ALL THE ASSETS
	sprintf(folderPath, "C:\\Users\\%s\\Downloads\\mcassets_%s.zip", std::getenv("USERNAME"), actualFileName.c_str());
	char* save_path = tinyfd_saveFileDialog("Minecraft Assets ZIP", folderPath, 1, fileFilter2, "A ZIP containing all of your minecraft assets.");
	if (!save_path) {
		std::cout << "No file saved!\n";
		return 0;
	}
	outputZip.save(save_path);
	std::cout << "saved to " << save_path << std::endl << std::endl;
	return 0;
}