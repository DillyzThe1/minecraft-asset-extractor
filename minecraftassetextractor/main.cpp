#include <iostream>
#include "tinyfiledialogs.h"
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "zip_file.hpp"

//#define MUSIC_EXPORTS true

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
	#if MUSIC_EXPORTS
	std::cout << "HEADS UP!!!! MUSIC EXPORTS ARE TURNED ON AND THEY CRASH!!!!\n";
	#endif
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
	std::ifstream indexFile_stream(file_path);
	json indexFile = json::parse(indexFile_stream);
	indexFile_stream.close();

	int objectCount = indexFile["objects"].size();
	std::cout << objectCount << " objects are to be extracted." << std::endl << std::endl;

	miniz_cpp::zip_file outputZip;
	#if MUSIC_EXPORTS
	miniz_cpp::zip_file outputZip_music;
	#endif
	std::string notetxt("These are the Minecraft assets indexed by " + actualFileName + ", ripped by MinecraftAssetExtractor.\nHave fun!\n\n       - DillyzThe1\n\nFailed Files:");

	int counter = -1;
	int totalSize = 0;
	for (auto curObj : indexFile["objects"].items()) {
		counter++;

		std::string objName = curObj.key().substr(curObj.key().find_last_of("/") + 1, curObj.key().length() - curObj.key().find_last_of("/") - 1);
		std::cout << "Object Name: " << objName.c_str() << std::endl;

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
				#if MUSIC_EXPORTS
				std::ifstream assetIn_stream(assetPath_in, std::ios::binary);
				if (assetIn_stream.fail() || !assetIn_stream.is_open()) {
					std::cout << "Cannot open " << assetPath_in << "!" << std::endl;
					continue;
				}

				std::stringstream ss;
				ss << assetIn_stream.rdbuf();
				outputZip_music.writestr(curObj.key(), ss.str());
				totalSize += objSize;

				assetIn_stream.close();
				#else
				std::cout << "Unsupported File." << std::endl;
				notetxt += "\n- " + curObj.key() + " (Music is unsupported)";
				#endif
			}
		}
		catch (std::exception e) {
			notetxt += "\n- " + curObj.key() + " (" + e.what() + ")";
		}
		

		std::cout << "Total Archive Size: " << totalSize << std::endl;
	
		std::cout /* << curObj.key()*/ << std::endl;
	}

	outputZip.writestr("note.txt", notetxt);
	#if MUSIC_EXPORTS
	outputZip_music.writestr("note.txt", notetxt);
	#endif

	// SAVE ALL THE ASSETS

	sprintf(folderPath, "C:\\Users\\%s\\Downloads\\mcassets_%s.zip", std::getenv("USERNAME"), actualFileName.c_str());
	char* save_path = tinyfd_saveFileDialog("Minecraft Assets ZIP", folderPath, 1, fileFilter2, "A ZIP containing all of your minecraft assets.");
	if (!save_path) {
		std::cout << "No file saved!\n";
		return 0;
	}
	outputZip.save(save_path);

	#if MUSIC_EXPORTS
	sprintf(folderPath, "C:\\Users\\%s\\Downloads\\mcassets_MUSIC_%s.zip", std::getenv("USERNAME"), actualFileName.c_str());
	char* save_path2 = tinyfd_saveFileDialog("Minecraft Music ZIP", folderPath, 1, fileFilter2, "A ZIP containing all of your minecraft music.");
	if (!save_path2) {
		std::cout << "No file saved!\n";
		return 0;
	}
	outputZip_music.save(save_path2);
	#endif

	std::cout << "saved to " << save_path << std::endl;
	std::cout << std::endl;
	return 0;
}