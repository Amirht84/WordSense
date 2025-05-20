#ifndef __AVLSET__
#define __AVLSET__

#include <algorithm>
#include <iostream>

template<typename val>
class avlSet{
	private:
		struct node{
			val Value;
			int Height = 1;
			node *Right = nullptr;
			node *Left = nullptr;
		};
		int Size;
		node* Head;

		node* rotate_rr(node*);
		node* rotate_ll(node*);
		node* rotate_rl(node*);
		node* rotate_lr(node*);
		node* put(node*, const val&);
		inline int height(node* Node){if(Node == nullptr){ return 0; } else { return Node->Height; } }
		inline void update_height(node* Node){Node->Height = 1 + std::max(height(Node->Right), height(Node->Left));}
		node* successor(const val&);
		node* predecessor(const val&);
	public:
		avlSet(): Size(0), Head(nullptr){}
		void insert(const val&);
		void display();
		void display_tree();
};
template<typename val>
void avlSet<val>::display_tree(){
	auto main_engine = [](auto self, node* Root) -> void {
		std::cout << Root->Value << '\n';
		if(Root->Right != nullptr){
			std::cout << "right child of " << Root->Value << ": ";
			self(self, Root->Right);
		}
		if(Root->Left != nullptr){
			std::cout << "left child of " << Root->Value << ": ";
			self(self, Root->Left);
		}
	};
	
	if(Head != nullptr){
		main_engine(main_engine, Head);
	}
}

template<typename val>
void avlSet<val>::display(){
	node* Current;
	for(node* i = Head ; i != nullptr ; i = predecessor(i->Value)){
		Current = i;
	}
	
	while(Current != nullptr){
		std::cout << Current->Value << '\n';
		Current = successor(Current->Value);
	}
}

template<typename val>
typename avlSet<val>::node* avlSet<val>::successor(const val& InputVal){
	node* Current = Head;
	node* CandPtr = nullptr;
	while(Current != nullptr){
		if(Current->Value > InputVal){
			CandPtr = Current;
			Current = Current->Left;
		}else{
			Current = Current->Right;
		}
	}
	return CandPtr;
}

template<typename val>
typename avlSet<val>::node* avlSet<val>::predecessor(const val& InputVal){
	node* Current = Head;
	node* CandPtr = nullptr;
	while(Current != nullptr){
		if(Current->Value < InputVal){
			CandPtr = Current;
			Current = Current->Right;
		}else{
			Current = Current->Left;
		}
	}
	return CandPtr;
}

template<typename val>
void avlSet<val>::insert(const val& Value){
	if(Head == nullptr){
		Head = new node;
		Head->Value = Value;
	}else{
		Head = put(Head, Value);
	}
	Size++;
	return;
}

template<typename val>
typename avlSet<val>::node* avlSet<val>::put(node* Root, const val& Value){
	if(Value == Root->Value){
		return Root;
	}else if(Value < Root->Value){
		if(Root->Left == nullptr){
			Root->Left = new node;
			Root->Left->Value = Value;
		}else{
			Root->Left = put(Root->Left, Value);
		}
	}else if(Value > Root->Value){
		if(Root->Right == nullptr){
			Root->Right = new node;
			Root->Right->Value = Value;
		}else{
			Root->Right = put(Root->Right, Value);
		}
	}
	update_height(Root);
	int BF = height(Root->Left) - height(Root->Right);
	if(BF > 1){
		if(height(Root->Left->Left) - height(Root->Left->Right) >= 0){
			return rotate_ll(Root);
		}else{
			return rotate_lr(Root);
		}
	}else if(BF < -1){
		if(height(Root->Right->Left) - height(Root->Right->Right) <= 0){
			return rotate_rr(Root);
		}else{
			return rotate_rl(Root);
		}
	}else{
		return Root;	
	}
}

template<typename val>
typename avlSet<val>::node* avlSet<val>::rotate_rr(node* Root){
//	std::cout << "rotate_rr\n";

	node* NewRoot = Root->Right;
	Root->Right = NewRoot->Left;
	NewRoot->Left = Root;

	update_height(Root);
	update_height(NewRoot);

	return NewRoot;
}
template<typename val>
typename avlSet<val>::node* avlSet<val>::rotate_ll(node* Root){
//	std::cout << "rotate_ll\n";

	node* NewRoot = Root->Left;
	Root->Left = NewRoot->Right;
	NewRoot->Right = Root;

	update_height(Root);
	update_height(NewRoot);

	return NewRoot;
}
template<typename val>
typename avlSet<val>::node* avlSet<val>::rotate_rl(node* Root){
//	std::cout << "rotate_rl\n";

	Root->Right = rotate_ll(Root->Right);
	return rotate_rr(Root);
}
template<typename val>
typename avlSet<val>::node* avlSet<val>::rotate_lr(node* Root){
//	std::cout << "rotate_lr\n";

	Root->Left = rotate_rr(Root->Left);
	return rotate_ll(Root);
}

#endif
