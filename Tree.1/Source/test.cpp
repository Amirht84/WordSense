#include <iostream>
#include "hardmap2.h"
using namespace std;

int main(){
	hardMap<int , char> Test("TestMapV2/");
	char Input = ' ';
	while(Input != 'f'){
		cin >> Input;
		switch(Input){
		case 'i':{
			int Id;
			char Data;
			cin >> Id >> Data;
			Test.insert({Id, Data});
		break;}case 'l':{
			for(auto Pair : Test){
				cout << Pair.first << ' ' << Pair.second << ',';
			}
			cout << endl;
		break;}	
		}
	}
}
