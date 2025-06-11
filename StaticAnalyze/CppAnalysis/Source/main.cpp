#include "hardtree.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main(){
	std::string User;
	while(std::cin >> User){

		if(User == "GetLog"){
			hardTree<char> Words("Words/");
			auto Size = Words.size();
			auto Leafs = Words.null_count();
			auto BranFa = (Size - 1) / static_cast<float>(Size - Leafs);
			std::cout << "Size: " << Size << '\n';
			std::cout << "Leafs: " << Leafs << '\n';
			std::cout << "Branchinig Factor: " << BranFa << '\n';
		}else if(User == "Write"){
			hardTree<char> Words("Words/");
			std::fstream File("../words_alpha.txt");
			std::string Input;
	
	
			while(std::getline(File, Input)){
				Input.pop_back();
				for(size_t i = 0 ; i < Input.size() ; i++){
					if(!Words.is_in_child(Input[i])){
						Words.mkdir(Input[i]);
					}
					Words.cd(Input[i]);
				}
				Words.mknull('!');
				Words.root();
				std::cout << Input << '\n';
			}
		}else if(User == "Check"){
			hardTree<char> Words("Words/");
			std::string Input;
			while(std::cin >> Input){
				if(Input ==  "ls"){
					auto List = Words.ls();
					for(auto each : List){
						std::cout << '[' << each << ']';
					}
					std::cout << '\n';
				}else if(Input == "cd"){
					char Data;
					std::cin >> Data;
					if(Data == '.'){
						Words.cd_up();
					}else{
						Words.cd(Data);
					}	
				}else if(Input == "Exit"){
					break;
				}
			}
		}else if(User == "CalB"){
			hardTree<char> Words("Words/");
			auto List = Words.ls();
			for(auto Way : List){
				Words.cd(Way);
				Words.cd_up();
			}
		}else if(User == "Exit"){
			break;
		}
			std::cout << "End of Circle\n";
	}
	std::cout << "End of programming\n";
	return 0;
}
