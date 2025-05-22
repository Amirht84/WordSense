#include "avlset.h"
#include <iostream>

int main(){
	avlSet<int> Set;
	int Value;
	while(std::cin >> Value){
		std::cout << "-----------------------------\n";
		Set.insert(Value);
	}
}
