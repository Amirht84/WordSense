#include <iostream>
#include "hardtree.h"
using namespace std;

int main(){
	hardTree<char> Test("Test/");
	char Input = ' ';
	while(Input != 'f'){
		cin >> Input;
		switch(Input){
		case 'm':{
			char Dir;
			cin >> Dir;
			Test.mkdir(Dir);
		break;}case 'c':{
			char Dir;
			cin >> Dir;
			if(Dir == '.'){
				Test.cd_up();
			}else{
				Test.cd(Dir);
			}
		break;}case 'l':{
			auto List = Test.ls();
			for(auto Dir : List){
				cout << Dir << ' ';
			}
			cout << endl;
		break;}	
		}		
	}
}
