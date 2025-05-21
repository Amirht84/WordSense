#include "avlset.h"
#include <iostream>

int main(){
	avlSet<int> Set;
	char Input = ' ';
	while(Input != 'f'){
		std::cin >> Input;
		if(Input == 'i'){
			int Value;
			std::cin >> Value;
			Set.insert(Value);
		}else if(Input == 'd'){
			Set.display();
		}else if(Input == 'c'){
			Set.display_tree();
		}
	}
}
