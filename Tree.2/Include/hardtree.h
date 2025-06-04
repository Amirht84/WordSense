#ifndef __HARDTREE2__
#define __HARDTREE2__

#include "hsmap.h"
#include <filesystem>
#include <stdexcept>
#include <vector>

template<typename data>
class hardTree{
	private:
		const char YES = 'Y';
		const char NO = 'N';
		#pragma pack(push, 1)
		struct hardNode{
			char IsUsed = 'Y';
			long long Parent = -1;
			data Data;
			int ChildsMapId = -1;
		};
		struct firstKey{
			int Size;
			int NullCount;
			long long RootPos;
		};
		#pragma pack(pop)
		int Size;
		int NullCount;
		long long RootPos;
		hsMap<data, long long> Adds;
		long long ThisPlace;
		std::fstream File;
		long long find_free_pos();
		bool read_file(const long long&, hardNode&);
		bool write_file(const long long&, const hardNode&);
		bool write_key(const firstKey&);
	public:
		hardTree(const std::string& _DirName): Size(0), Adds(_DirName + "Adds/"){
			std::filesystem::create_directories(_DirName);
			const std::string FileName = _DirName + "hardtree.dat";
			File.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
			if(!File){
				std::fstream CreateFile(FileName, std::ios::out | std::ios::binary);
				CreateFile.close();
				File.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
				if(!File) throw std::runtime_error("err in  hardTree<data>::hardTree invalid file address");

				RootPos = sizeof(firstKey);
				if(!write_key({0, 0, RootPos})) throw std::runtime_error("err in hardTree<data>::hardTree can't write");
				hardNode Root;
				if(!write_file(RootPos, Root)) throw std::runtime_error("err in hardTree<data>::hardTree can't write");
			}else{
				File.seekg(0);
				firstKey FirstKey;
				File.read(reinterpret_cast<char*>(&FirstKey), sizeof(FirstKey));
				Size = FirstKey.Size;
				NullCount = FirstKey.NullCount;
				RootPos = FirstKey.RootPos;
			}
			ThisPlace = RootPos;
		}
		void mkdir(const data& Data);
		void cd(const data& Data){
			hardNode CurrentNode;
			if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in  hardTree<data>::cd can't read");
			Adds.change_id(CurrentNode.ChildsMapId);
			ThisPlace = Adds[Data];
		}
		void cd_up(){
			hardNode CurrentNode;
			if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in  hardTree<data>::cd can't read");
			ThisPlace = CurrentNode.Parent;
		}
		std::vector<data> ls(){
			std::vector<data> Output;
			hardNode CurrentNode;
			if(!read_file(ThisPlace ,CurrentNode)) throw std::runtime_error("err in  hardTree<data>::ls can't read");
			if(CurrentNode.ChildsMapId != -1){
				Adds.change_id(CurrentNode.ChildsMapId);
				for(auto It = Adds.begin(); It != Adds.end() ; ++It){
					Output.push_back((*It).first);
				}
			}
			return Output;
		}
		bool is_in_child(const data&);
		inline void root(){ThisPlace = RootPos;}
		void mknull(const data&);
		bool is_null_in_child(const data&);
		inline int null_count(){ return NullCount; }
		inline int size(){ return Size; }
};
template<typename data>
long long hardTree<data>::find_free_pos(){

	File.seekp(0 , std::ios::end);
	long long End = File.tellp();

	return End;

	long long Offset = sizeof(firstKey);
	char IsUsed;
	while(Offset != End){
		File.seekg(Offset);
		File.read(&IsUsed , 1);
		if(!File) throw std::runtime_error("error in hardTree<data>::find_free_pos can't read");
		if(IsUsed == NO){
			return Offset;
		}
		Offset += sizeof(hardNode);
	}
	return Offset;
}
template<typename data>
bool hardTree<data>::read_file(const long long& Pos, hardNode& Output){
	File.seekg(Pos);
	File.read(reinterpret_cast<char*>(&Output) , sizeof(hardNode));
	if(!File){
		return false;
	}
	return true;
}

template <typename data>
bool hardTree<data>::write_file(const long long& Pos, const hardNode& Input){
	File.seekp(Pos);
	File.write(reinterpret_cast<const char*>(&Input), sizeof(hardNode));
	if(!(File)){
		return false;
	}else{
		return true;
	}
}

template <typename data>
bool hardTree<data>::write_key(const firstKey& FirstKey){
	File.seekp(0);
	File.write(reinterpret_cast<const char*>(&FirstKey), sizeof(FirstKey));
	if(!File){
		return false;
	}else{
		return true;
	}
}

template <typename data>
bool hardTree<data>::is_in_child(const data& Child){
	hardNode CurrentNode;
	if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in hardTree<data>::is_in_child can't read");
	if(CurrentNode.ChildsMapId == -1){
		return false;
	}else{
		Adds.change_id(CurrentNode.ChildsMapId);
		return Adds.find(Child) != Adds.end();
	}
}

template <typename data>
void hardTree<data>::mkdir(const data& Data){
	hardNode CurrentNode;
	if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in  hardTree<data>::mkdir can't read");
	if(CurrentNode.ChildsMapId == -1){
		++Size;	
		Adds.make_id(Size);
		Adds.change_id(Size);
		CurrentNode.ChildsMapId = Size;
		if(!write_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in  hardTree<data>::mkdir can't write");
		if(!write_key({Size, NullCount, RootPos})) throw std::runtime_error("err in hardTree<data>::mkdir can't write");
	}else{
		Adds.change_id(CurrentNode.ChildsMapId);
	}
	long long NewPos = find_free_pos();
	Adds.insert({Data, NewPos});
	hardNode New;
	New.Parent = ThisPlace;
	New.Data = Data;
	if(!write_file(NewPos, New)) throw std::runtime_error("err in  hardTree<data>::mkdir can't write");
}
template <typename data>
void hardTree<data>::mknull(const data& NullData){
	hardNode CurrentNode;
	if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in hardTree<data>::mknull can't read");
	if(CurrentNode.ChildsMapId == -1){
		++Size;
		Adds.make_id(Size);
		Adds.change_id(Size);
		CurrentNode.ChildsMapId = Size;
		if(!write_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in hardTree<data>::mknull can't write");
	}else{
		Adds.change_id(CurrentNode.ChildsMapId);
	}
	Adds.insert({NullData, -1});
	++NullCount;
	if(!write_key({Size, NullCount, RootPos})) throw std::runtime_error("err in hardTree<data>::mknull ca't write");
}
template <typename data>
bool hardTree<data>::is_null_in_child(const data& NullData){
	hardNode CurrentNode;
	if(!read_file(ThisPlace, CurrentNode)) throw std::runtime_error("err in hardTree<data>::is_null_in_child can't read");
	if(CurrentNode.ChildsMapId == -1){
		return false;
	}else{
		Adds.change_id(CurrentNode.ChildsMapId);
		auto It = Adds.find(NullData);
		if(It != Adds.end() && (*It).second == -1){
			return true;
		}else{
			return false;
		}
	}
}

#endif
