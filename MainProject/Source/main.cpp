#include "hardtree.h"
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <set>

void print_word_sense(hardTree<char>& Words, std::list<char>& Chars, std::string& CandWord, const size_t&, std::set<std::string>&);
void classic_searcher(std::list<char>& Chars, std::string& CandWord, const size_t& Length, std::set<std::string>& Output);

int main(){
	std::string User;
	while(std::cin >> User){

	if(User == "Write"){
		hardTree<char> Words("Words/");
		std::fstream File("words_alpha.txt");
		std::string Input;
	
	
		while(std::getline(File, Input)){
			Input.pop_back();
			for(size_t i = 0 ; i < Input.size() ; i++){
				if(!Words.is_in_child(Input[i])){
					Words.mkdir(Input[i]);
				}
				Words.cd(Input[i]);
			}
			Words.mkdir('!');
			Words.root();
			std::cout << Input << '\n';
		}
	}else if(User == "Check"){
		hardTree<char> Words("Words/");
		char Input;
		while(std::cin >> Input){
			bool IsExit = false;
			switch(Input){
				case 'l':{
					auto List = Words.ls();
					for(auto each : List){
						std::cout << '[' << each << ']';
					}
					std::cout << '\n';
				} break;
				case 'c':{
					char Data;
					std::cin >> Data;
					if(Data == '.'){
						Words.cd_up();
					}else{
						Words.cd(Data);
					}	
				} break;
				case 'e':{
					IsExit = true;		
	 			} break;
			}
			if(IsExit){
				break;
			}
		}
	}else if(User == "Find"){
		hardTree<char> Words("Words/");
		std::list<char> Scrambled;
		int Count;
		std::cin >> Count;
		for(int i = 0 ; i < Count ; i++){
			char Char;
			std::cin >> Char;
			Scrambled.push_back(Char);
		}
		std::string CandWord = "";
		int Max;
		std::cin >> Max;
//		size_t Count2 = 0;
		std::set<std::string> Output;
		print_word_sense(Words, Scrambled, CandWord, Max, Output);

		for(const auto& Out : Output){
			std::cout << Out << '\n';
		}

	}else if(User == "Search"){
		std::list<char> Scrambled;
		int Count;
		std::cin >> Count;
		for(int i = 0 ; i < Count ; i++){
			char Char;
			std::cin >> Char;
			Scrambled.push_back(Char);
		}
		std::string CandWord = "";
		std::set<std::string> Output;
		classic_searcher(Scrambled, CandWord, Count, Output);
	
		for(const auto& Word : Output){
			std::ifstream File("words_alpha.txt");
			std::string Input;
			while(std::getline(File, Input)){
				Input.pop_back();
				if(Word == Input) std::cout << Word << '\n';
			}
		}
	}else if(User == "Exit"){
		break;
	}
		std::cout << "End of Circle\n";
	}
	std::cout << "End of programming\n";
	return 0;
}
void classic_searcher(std::list<char>& Chars, std::string& CandWord, const size_t& Length, std::set<std::string>& Output){
	if(CandWord.size() == Length){
		Output.insert(CandWord);
	}
	for(auto Char = Chars.begin() ; Char != Chars.end() ; ++Char){
		if(*Char == '?') continue;
		CandWord += *Char;
		const char Temp = *Char;
		*Char = '?';
		classic_searcher(Chars, CandWord, Length, Output);
		CandWord.pop_back();
		*Char = Temp;
	}
}
void print_word_sense(hardTree<char>& Words, std::list<char>& Chars, std::string& CandWord, const size_t& Length, std::set<std::string>& Output){
	for(auto Char = Chars.begin() ; Char != Chars.end() ; ++Char){
		if(*Char == '?') continue;
		if(Words.is_in_child(*Char)){
			CandWord += *Char;
			Words.cd(*Char);
			const char Temp = *Char;
			*Char = '?';
			if(Words.is_in_child('!') && CandWord.size() == Length){
				Output.insert(CandWord);
//				Count++;
			}
			print_word_sense(Words, Chars, CandWord, Length, Output);
			*Char = Temp;
			Words.cd_up();
			CandWord.erase(CandWord.size() - 1, 1);
		}
	}
}

