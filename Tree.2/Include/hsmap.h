#ifndef __HSMAP2__
#define __HSMAP2__

#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <stdexcept>
#include <filesystem>
#include "hardmap.h"

template <typename id , typename data>
class hsMap {
	private:
		const char YES = 'Y';
		const char NO = 'N';
		#pragma pack(push, 1)
		struct hardNode{
			char IsUsed = 'Y';
			id Id;
			data Data;
			int Height = 1;
			long long RightOffset = -1;
			long long LeftOffset = -1;
			int BF = 0;
		};
		struct firstKey{
			int Size;
			long long Head;
		};
		#pragma pack(pop)
		struct dataProxy{
			std::fstream* File;
			long long CurrentPos;
			hardNode Current;
			bool IsWrite;
			dataProxy(std::fstream& _File, const long long& _Pos, const hardNode& _Node):
				File(&_File), 
				CurrentPos(_Pos), 
				Current(_Node), 
				IsWrite(false){}

			dataProxy& operator=(const data& Data){
				Current.Data = Data;
				IsWrite = true;
				return *this;
			}
			operator data(){
				return Current.Data;	
			}
			~dataProxy(){
				if(IsWrite){
					File->seekp(CurrentPos);
					File->write(reinterpret_cast<char*>(&Current), sizeof(hardNode));
					if(!(File)) std::cerr << "Err in hsMap<id , data>::dataProxy\n";
				}
			}
		};
		
		int Size;
		long long Head;
		std::fstream File;
		hardMap<int, firstKey> Keys;
		typename hardMap<int, firstKey>::iterator FirstKey;
		long long find_free_pos();
		bool write_key(const firstKey& Key);
		bool write_file(const long long& Pos, const hardNode& Input);
		bool read_file(const long long& Pos, hardNode& Output);
		long long search(const long long& CurrentPos, const id& Id);
		long long put(const long long& , const hardNode&);
		long long successor(const id&);
		long long predecessor(const id&);
		long long rotate_rr(const long long&, hardNode&);
		long long rotate_ll(const long long&, hardNode&);
		long long rotate_rl(const long long&, hardNode&);
		long long rotate_lr(const long long&, hardNode&);
		void update_node(const long long&, hardNode&, const long long&);
	public:

		hsMap(const std::string& _DirName): Size(0), Head(-1), Keys(_DirName + "keys/"){
			const std::string ErrLog = "err in hsMap<id, data>::hsMap";

			std::filesystem::create_directories(_DirName);
			const std::string FileName = _DirName + "data.dat";
			File.open( FileName, std::ios::in | std::ios::out | std::ios::binary );
			if(!File.is_open()){
				std::ofstream CreateFile(FileName , std::ios::binary);
				CreateFile.close();
				File.open(FileName, std::ios::in | std::ios::out | std::ios::binary);
				if(!File) throw std::runtime_error(ErrLog + "can't open the file");
			}
		}
		dataProxy operator[](const id& Id){
			const std::string ErrLog = "err in hsMap<id , data>::operator[]";
			const auto CurrentPos = search(Head, Id);
			if(CurrentPos == -1 ) throw std::runtime_error(ErrLog + "invalid Id");
			hardNode Current;
			if(!read_file(CurrentPos, Current)) throw std::runtime_error(ErrLog + "can't read");
			return dataProxy(File, CurrentPos, Current);
		}

		class iterator;
		inline iterator begin(){
			const std::string ErrLog = "err in hsMap<id, data>::begin";
			if(Size == 0) return iterator(*this, -1);
			long long CurrentPos = Head;
			hardNode CurrentNode;
			if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
			long long PredecessorPos = predecessor(CurrentNode.Id);
			while(PredecessorPos != -1){
				CurrentPos = PredecessorPos;
				if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
				PredecessorPos = predecessor(CurrentNode.Id);
			}
			return iterator(*this, CurrentPos);
		}
		inline iterator end(){return iterator(*this, -1);}
		iterator find(const id& Id){return iterator(*this, search(Head, Id));}
		void insert(const std::pair<id, data>&);
		void change_id(const int&);
		void make_id(const int&);

		~hsMap(){
			if(FirstKey != Keys.end)
				if(!write_key({Size , Head})) std::cerr << "err in hsMap<id , data>::~hsMap\n";

			File.close();
		}
};

template<typename id, typename data>
void hsMap<id, data>::make_id(const int& MapId){
	FirstKey = Keys.find(MapId);
	if(FirstKey != Keys.end()) throw std::runtime_error("err in hsMap<id, data>::make_id repetitive MapId");
	Keys.insert({MapId, {0, -1}});
}

template<typename id, typename data>
void hsMap<id, data>::change_id(const int& MapId){
	FirstKey = Keys.find(MapId);
	if(FirstKey == Keys.end()) throw std::runtime_error("err in hsMap<id, data>::change_id MapId not available");
	firstKey Temp = (*FirstKey).second;
	Size = Temp.Size;
	Head = Temp.Head;
}

template<typename id, typename data>
long long hsMap<id, data>::rotate_rr(const long long& RootPos, hardNode& RootNode){
	const std::string ErrLog = "err in hsMap<id, data>::rotate_rr";
	
	auto get_height = [this, &ErrLog](const long long& InputPos){
		if(InputPos == -1) return 0;
		int Height;
		File.seekg(InputPos + sizeof(char) + sizeof(id) + sizeof(data));
		File.read(reinterpret_cast<char*>(&Height), sizeof(Height));
		if(!File) throw std::runtime_error(ErrLog + "can't read");
		return Height;
	};

	hardNode NewRootNode;
	long long NewRootPos = RootNode.RightOffset;
	if(!read_file(NewRootPos, NewRootNode)) throw std::runtime_error("can't read");

	RootNode.RightOffset = NewRootNode.LeftOffset;
	NewRootNode.LeftOffset = RootPos;

	update_node(get_height(RootNode.LeftOffset) ,RootNode, get_height(RootNode.RightOffset));
	update_node(RootNode.Height, NewRootNode, get_height(NewRootNode.RightOffset));

	if(!write_file(NewRootPos, NewRootNode)) throw std::runtime_error("can't write");

	return NewRootPos;
}
template<typename id, typename data>
long long hsMap<id, data>::rotate_ll(const long long& RootPos, hardNode& RootNode){
	const std::string ErrLog = "err in hsMap<id, data>::rotate_ll";

	auto get_height = [this, &ErrLog](const long long& InputPos){
		if(InputPos == -1) return 0;
		int Height;
		File.seekg(InputPos + sizeof(char) + sizeof(id) + sizeof(data));
		File.read(reinterpret_cast<char*>(&Height), sizeof(Height));
		if(!File) throw std::runtime_error(ErrLog + "can't read");
		return Height;
	};

	hardNode NewRootNode;
	long long NewRootPos = RootNode.LeftOffset;
	if(!read_file(NewRootPos, NewRootNode)) throw std::runtime_error("can't read");

	RootNode.LeftOffset = NewRootNode.RightOffset;
	NewRootNode.RightOffset = RootPos;

	update_node(get_height(RootNode.LeftOffset) ,RootNode, get_height(RootNode.RightOffset));
	update_node(get_height(NewRootNode.LeftOffset), NewRootNode, RootNode.Height);

	if(!write_file(NewRootPos, NewRootNode)) throw std::runtime_error("can't write");

	return NewRootPos;
}
template<typename id, typename data>
long long hsMap<id, data>::rotate_rl(const long long& RootPos, hardNode& RootNode){
	const std::string ErrLog = "err in hsMap<id, data>::rotate_rl";
	hardNode RightNode;
	if(!read_file(RootNode.RightOffset, RightNode)) throw std::runtime_error(ErrLog + "can't read");
	long long Temp = RootNode.RightOffset;
	RootNode.RightOffset = rotate_ll(RootNode.RightOffset, RightNode);
	if(!write_file(Temp, RightNode)) throw std::runtime_error(ErrLog + "can't write");

	return rotate_rr(RootPos, RootNode);
	
}
template<typename id, typename data>
long long hsMap<id, data>::rotate_lr(const long long& RootPos, hardNode& RootNode){
	const std::string ErrLog = "err in hsMap<id, data>::rotate_lr";
	
	hardNode LeftNode;
	if(!read_file(RootNode.LeftOffset, LeftNode)) throw std::runtime_error(ErrLog + "can't read");
	long long Temp = RootNode.LeftOffset;
	RootNode.LeftOffset = rotate_rr(RootNode.LeftOffset, LeftNode);
	if(!write_file(Temp, LeftNode)) throw std::runtime_error(ErrLog + "can't write");
	
	return rotate_ll(RootPos, RootNode);
}


template<typename id, typename data>
void hsMap<id, data>::update_node(const long long& LeftHeight ,hardNode& CurrentNode, const long long& RightHeight){
	const std::string ErrLog = "err in update_node";
	CurrentNode.BF = RightHeight - LeftHeight;
	CurrentNode.Height = 1 + std::max(RightHeight , LeftHeight);
}

template <typename id , typename data>
long long hsMap<id , data>::put(const long long& CurrentPos, const hardNode& New){
	const std::string ErrLog = "err in hsMap<id , data>::put";

	auto get_bf = [this, &ErrLog](const long long& InputPos){
		if(InputPos == -1) return 0;
		int BF;
		File.seekg(InputPos + sizeof(char) + sizeof(id) + sizeof(data) + sizeof(int) + sizeof(long long) + sizeof(long long));
		File.read(reinterpret_cast<char*>(&BF) , sizeof(BF));
		if(!File) throw std::runtime_error(ErrLog + "can't read");
		return BF;
	};
	auto get_height = [this, &ErrLog](const long long& InputPos){
		if(InputPos == -1) return 0;
		int Height;
		File.seekg(InputPos + sizeof(char) + sizeof(id) + sizeof(data));
		File.read(reinterpret_cast<char*>(&Height), sizeof(Height));
		if(!File) throw std::runtime_error(ErrLog + "can't read");
		return Height;
	};

	hardNode CurrentNode;
	if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");

	if(New.Id < CurrentNode.Id){
		if(CurrentNode.LeftOffset == -1){
			long long FreePos = find_free_pos();
			if(!write_file(FreePos, New)) throw std::runtime_error(ErrLog + "can't write");
			CurrentNode.LeftOffset = FreePos;
		}else{
			CurrentNode.LeftOffset = put(CurrentNode.LeftOffset, New);
		}
	}else if(New.Id > CurrentNode.Id){
		if(CurrentNode.RightOffset == -1){
			long long FreePos = find_free_pos();
			if(!write_file(FreePos, New)) throw std::runtime_error(ErrLog + "can't write");
			CurrentNode.RightOffset = FreePos;
		}else{
			CurrentNode.RightOffset = put(CurrentNode.RightOffset, New);
		}
	}
	update_node(get_height(CurrentNode.LeftOffset), CurrentNode, get_height(CurrentNode.RightOffset));
	
	long long NewRootPos;
	if(CurrentNode.BF > 1){
		if(get_bf(CurrentNode.RightOffset) >= 0){
			NewRootPos = rotate_rr(CurrentPos, CurrentNode);
		}else {
			NewRootPos = rotate_rl(CurrentPos, CurrentNode);
		}
	}else if(CurrentNode.BF < -1){
		if(get_bf(CurrentNode.LeftOffset) <= 0){
			NewRootPos = rotate_ll(CurrentPos, CurrentNode);
		}else{
			NewRootPos = rotate_lr(CurrentPos, CurrentNode);
		}
	}else{
		NewRootPos = CurrentPos;
	}

	if(!write_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't write");

	return NewRootPos;
}
template <typename id , typename data>
void hsMap<id , data>::insert(const std::pair<id , data>& Input){
	const std::string ErrLog = "Err in hsMap<id , data>::insert";
	if(Size != 0 && search(Head, Input.first) != -1){
		return;
	}
	hardNode New;
	New.Id = Input.first;
	New.Data = Input.second;

	if(Size == 0){
		Head = find_free_pos();
		if(!write_file(Head, New)) throw std::runtime_error(ErrLog + "can't read");
		Size = 1;
		if(!write_key({Size, Head})) throw std::runtime_error(ErrLog + "can't write");
	}else{
		Head = put(Head, New);
		Size++;
		if(!write_key({Size, Head})) throw std::runtime_error(ErrLog + "can't write");
	}
}

template <typename id , typename data>
long long hsMap<id , data>::search(const long long& CurrentPos, const id& Id){
	const std::string ErrLog = "err in hsMap<id , data>::search";
	if(Size == 0){
		return -1;
	}
	hardNode Current;
	if(!read_file(CurrentPos, Current)) throw std::runtime_error(ErrLog + "can't read 1");
	if(Id == Current.Id){
		return CurrentPos;
	}else if(Id < Current.Id){
		if(Current.LeftOffset != -1){
			return search(Current.LeftOffset, Id);
		}else{
			return -1;
		}
	}else if(Id > Current.Id){
		if(Current.RightOffset != -1){
			return search(Current.RightOffset, Id);
		}else{
			return -1;
		}
	}
	throw std::runtime_error(ErrLog + " not logical act");
	return -1;
}

template <typename id , typename data>
bool hsMap<id , data>::read_file(const long long& Pos, hardNode& Output){
	File.seekg(Pos);
	File.read(reinterpret_cast<char*>(&Output) , sizeof(hardNode));
	if(!File){
		return false;
	}
	return true;
}

template <typename id , typename data>
bool hsMap<id , data>::write_file(const long long& Pos, const hardNode& Input){
	File.seekp(Pos);
	File.write(reinterpret_cast<const char*>(&Input), sizeof(hardNode));
	if(!(File)){
		return false;
	}
	return true;
}
template <typename id , typename data>
long long hsMap<id , data>::find_free_pos(){
	const std::string ErrLog = "Err in hsMap<id , data>::find_free_pos";
	File.seekp(0 , std::ios::end);
	long long End = File.tellp();	

	return End;

	long long Offset = 0;
	char IsUsed;
	while(Offset != End){
		File.seekg(Offset);
		File.read(&IsUsed , 1);
		if(!File ) throw std::runtime_error(ErrLog + "can't read file");
		if(IsUsed == NO){
			return Offset;
		}
		Offset += sizeof(hardNode);
	}
	return Offset;
}

template <typename id , typename data>
bool hsMap<id , data>::write_key(const firstKey& Key){
	try{
		(*FirstKey).second = Key;
	}catch(const std::runtime_error&){
		return false;
	}
	return true;
}

template <typename id, typename data>
long long hsMap<id, data>::predecessor(const id& InputId){
	const std::string ErrLog = "err in hsMap<id, data>::predecessor";
	hardNode CurrentNode;
	long long CurrentPos = Head;
	long long CandPos = -1;
	while(CurrentPos != -1){
		if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
		if(CurrentNode.Id < InputId){
			CandPos = CurrentPos;
			CurrentPos = CurrentNode.RightOffset;
		}else{
			CurrentPos = CurrentNode.LeftOffset;
		}
	}
	return CandPos;
}

template <typename id,typename data>
long long hsMap<id, data>::successor(const id& InputId){
	const std::string ErrLog = "err in hsMap<id , data>::successor";
	hardNode CurrentNode;
	long long CurrentPos = Head;
	long long CandPos = -1;
	while(CurrentPos != -1){
		if(!read_file(CurrentPos, CurrentNode)) throw std::runtime_error(ErrLog + "can't read");
		if(CurrentNode.Id > InputId){
			CandPos = CurrentPos;
			CurrentPos = CurrentNode.LeftOffset;
		}else{
			CurrentPos = CurrentNode.RightOffset;
		}
	}
	return CandPos;
}

template<typename id, typename data>
class hsMap<id , data>::iterator{
	private:
		hsMap<id , data>* Owner;
		long long CurrentPos;
	public:
		iterator():Owner(nullptr), CurrentPos(-1){}
		iterator(hsMap& _Owner, const long long& _CurrentPos): Owner(&_Owner), CurrentPos(_CurrentPos) {}
		iterator(const iterator& _Other): Owner(_Other.Owner), CurrentPos(_Other.CurrentPos){}
		std::pair<const id, dataProxy> operator*(){
			hardNode CurrentNode;
			if(!(Owner->read_file(CurrentPos, CurrentNode))) throw std::runtime_error("err in hsMap<id , data>::iterator::operator*");
			return std::pair<const id, dataProxy>(CurrentNode.Id , dataProxy(Owner->File, CurrentPos, CurrentNode));
		}
		iterator& operator=(const iterator& _Other){
			Owner = _Other.Owner;
			CurrentPos = _Other.CurrentPos;
		}
		iterator& operator++(){
			hardNode CurrentNode;
			if(!Owner->read_file(CurrentPos, CurrentNode)) throw std::runtime_error("can't read");
			CurrentPos = (Owner->successor(CurrentNode.Id));
			return *this;
		}
		iterator& operator++(int){
			auto Temp = *this;
			hardNode CurrentNode;
			if(!Owner->read_file(CurrentPos, CurrentNode)) throw std::runtime_error("can't read");
			CurrentPos = (Owner->successor(CurrentNode.Id));
			return Temp;
		}
		bool operator==(const iterator& _Other){
			return (_Other.CurrentPos == CurrentPos);
		}
		bool operator!=(const iterator& _Other){
			return !(*this == _Other);
		}
};

#endif
