#include "hardmap.h"
#include <iostream>

int main(){
	hardMap<int, char> Map("Map/");

	char Input = ' ';
	while(Input != 'f'){
		std::cin >> Input;
		switch(Input){
			case 'i':{
				int Key;
				char Val;	
				std::cin >> Key >> Val;
				Map.insert({Key, Val});
			break;}case 'd':{
				for(auto It : Map){
					std::cout << It.first << " " << It.second << '|';
				}
				std::cout << '\n';
			break;}case 's':{
				int Key;
				std::cin >> Key;
				auto It = Map.find(Key);
				std::cout << (*It).second << '\n';
			break;}
		}
	}


	return 0;
}
