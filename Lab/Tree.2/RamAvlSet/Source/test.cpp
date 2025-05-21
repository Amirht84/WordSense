#include "avlset.h"
#include <iostream>

int main(){
	avlSet<int> Set;
	int Value;
//	char Input = ' ';
	while(std::cin >> Value){
//		std::cin >> Input;
//		Input = 'i';
//		if(Input == 'i'){
//			int Value;
//			std::cin >> Value;
			Set.insert(Value);
//		}else if(Input == 'd'){
//			Set.display();
//		}else if(Input == 'c'){
//			Set.display_tree();
//		}
	}
}
