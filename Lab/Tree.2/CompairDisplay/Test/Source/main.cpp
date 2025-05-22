#include "hardmap.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

std::string get_dir_name();

int main(){
	hardMap<int, char> Test(get_dir_name() + "/");
	int Input;
	while(std::cin >> Input){
		std::cout << "-----------------------------\n";
		Test.insert({Input, 'i'});
		for(auto Temp : Test){
			std::cout << Temp.first << ' ';
		}
		std::cout << '\n';
	}
	return 0;
}

std::string get_dir_name(){
	std::string DirName;
	std::ifstream ConfigFile("Configs/config.ini");
	if(!ConfigFile){
		throw std::runtime_error("Config File not found");
	}
	std::string Line;
	while(getline(ConfigFile, Line)){
		if(Line.substr(0, 7) == "BaseDir"){
			Line.erase(0, 8);
			return Line;
		}
	}
	throw std::runtime_error("BaseDir hasn't defined");
}
