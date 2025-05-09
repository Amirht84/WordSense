#include "hardTree.h"
using namespace std;


int main(){
	hardTree<char> Test("Files/Tree.bin");
	Test.mkdir('a');
	Test.mkdir('b');
	Test.mkdir('c');
	auto List = Test.ls();
	for(auto Char :List){
		cout << Char << '\t';
	}
	cout << '\n';
}

