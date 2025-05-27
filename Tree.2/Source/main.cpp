#include "hardtree.h"
#include <iostream>

int main(){
	hardTree<char> Test("Test/");

	std::string Input = "";
	while(Input != "exit"){
		std::cin >> Input;
		if(Input == "mkdir"){
			char Dir;
			std::cin >> Dir;
			Test.mkdir(Dir);
		}else if(Input == "cd"){
			char Dir;
			std::cin >> Dir;
			if(Dir == '.'){
				Test.cd_up();
			}else{
				Test.cd(Dir);
			}
		}else if(Input == "ls"){
			auto List = Test.ls();
			for(const auto& Output : List){
				std::cout << Output << ' ';
			}
			std::cout << '\n';
		}
	}


	return 0;
}
