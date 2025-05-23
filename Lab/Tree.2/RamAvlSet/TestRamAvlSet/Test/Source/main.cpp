#include "avlset.h"
#include <iostream>

int main(){
	avlSet<int> Set;
	int Input;
	while(std::cin >> Input){
		Set.insert(Input);
	}
	Set.display();
}
