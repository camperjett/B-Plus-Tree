//Om Namah Shiva
#include <bits/stdc++.h>
using namespace std;
#define el                  '\n'


class Node
{
	// max number of pointers (in internal nodes) pointing downwards i.e., FANOUT
	// max no. of records or keys (in leaf node )
	int SIZE;
	friend class BPTree;
public:
	bool ISLEAF;
	Node* NEXT_BLOCK;
	vector<int> KEYS;
	vector<Node*> PTRS;
	Node(int size, bool isLeaf);
	bool isLeaf();
	pair<int, Node*> insert(int x);
	void print();
};

class BPlusTree
{
private:
	int NODE_SIZE;
	Node* ROOT;
public:
	BPlusTree(int m);
	int insert(int x);
	Node* getROOT();
	void print();
	int remove(int x);
	vector<int> HISTORY;
};

/**Node Functions***/
Node::Node(int size, bool isLeaf){
	this->SIZE = size;
	this->NEXT_BLOCK = NULL;
	this->ISLEAF = isLeaf;
}
bool Node::isLeaf(){
	return this->ISLEAF;
}
/**BPlusTree Functions***/
BPlusTree::BPlusTree(int m){
	this->NODE_SIZE = m;
	this->ROOT = new Node(this->NODE_SIZE, true);
}
Node* BPlusTree::getROOT(){
	return this->ROOT;
}

int BPlusTree::insert(int x){
	this->HISTORY.push_back(x);
	pair<int, Node*> data = this->ROOT->insert(x);
	//    z    *y
	if(data.second == NULL){
		return 1;
	} else{
		/****
		 * TREE before: 
		 * [|5.5|6|11|]
		 * 		 x
		 * 
		 * >INSERT 10
		 * 
		 * TREE after: (z : 6)
		 * [|5.5|6|]	[|10|11|]
		 * 	   y      		x
		 * 
		 * (so, basically the root now should be the one having one 
		 * 	key and two pointers, left one has pointer to y and right
		 * 	one has pointer to x and its only key is 6) see below...
		 * 		 [|6|]				<--- new root
		 *    /         \
		 * [|5.5|6|]	[|10|11|]
		 * 
		 * */
		Node* newROOT = new Node(this->NODE_SIZE, false);
		newROOT->KEYS.push_back(data.first); // set the new key as z
		newROOT->PTRS.push_back(data.second);// set the left pointer as new node
		newROOT->PTRS.push_back(this->ROOT); // set the right pointer as original root
		this->ROOT = newROOT; 		   // set the new root as the root

		return 2;
		// STATUS 2: means GLOBAL level increase
	}
}

pair<int, Node*> Node::insert(int x){

/**
 * [|5|11|^|]
 * [...]	[|5.5|6|11|]	[...]
 * 
 * >INSERT 10
 * 
 * TREE after: (z : 6)
 * [|5.5|6|]	[|10|11|]
 * 	   y      		x
 * 
 * [|5|6|11|]
 * [...]	[|5.5|6|]	[|10|11|]	[...] 
 * 
 * NOTE: ABOVE ONLY WHEN LEAF NODE ...
 * 
 * **/
	int i = 0;
	for (; i < this->KEYS.size(); ++i)
	{
		if(x<=this->KEYS[i]){
			break;
		} else if(i+1 == this->KEYS.size()){
			i++;
			break;
		}
	}
	if(!this->ISLEAF){
		pair<int, Node*> data = this->PTRS[i]->insert(x);
		if(data.second == NULL) return data;
		else {
			i = 0;
			for (; i < this->KEYS.size(); ++i)
			{
				if(data.first<=this->KEYS[i]){
					break;
				} else if(i+1 == this->KEYS.size()){
					i++;
					break;
				}
			}
			// [|5|11|]
			this->KEYS.push_back(-1);
			// [|5|11|-1]
			this->PTRS.push_back(NULL);
			// [|5|11|-1|]
			for (int j = this->KEYS.size()-2; j>=i; --j)
			{
				this->KEYS[j+1] = this->KEYS[j];
			}
			// [|5|11|11|]
			for (int j = this->PTRS.size()-2; j >=i; --j)
			{
				this->PTRS[j+1] = this->PTRS[j];
			}
			// [|5|11|11|]
			this->KEYS[i] = data.first;
			// [|5|6|11|]
			this->PTRS[i] = data.second;
			// [|5|6|11|]

			if(this->PTRS.size() > this->SIZE){
				// [|5|6|11|20|] (MAX FANOUT => 4) (HERE => 5)
				int LHALF = (this->KEYS.size() +1) / 2,
					RHALF = this->KEYS.size() - LHALF;
				//  (z: 6)
				// [|5|]	[|11|20|] 
				//	 y
				Node* newINTERNALNODE = new Node(this->SIZE,false);
				for (int i = 0; i < LHALF-1; ++i)
				{
					newINTERNALNODE->KEYS.push_back(this->KEYS[i]);
				}
				for (int i = 0; i < LHALF; ++i)
				{
					newINTERNALNODE->PTRS.push_back(this->PTRS[i]);
				}
				// new NODE made
				int z = this->KEYS[LHALF-1];
				// z saved
				vector<int> newKEYS;
				vector<Node*> newPTRS;
				for (int i = LHALF; i < this->KEYS.size(); ++i)
				{
					newKEYS.push_back(this->KEYS[i]);
				}
				for (int i = LHALF; i < this->PTRS.size(); ++i)
				{
					newPTRS.push_back(this->PTRS[i]);
				}
				this->PTRS = newPTRS;
				this->KEYS = newKEYS;
				// original node modified
				pair<int, Node*> response = {z, newINTERNALNODE};
				return response;
			} else {
				pair<int, Node*> response = {-1, NULL};
				return response;
			}

		}
	} else {
		i = 0;
		for (; i < this->KEYS.size(); ++i)
		{
			if(x <= this->KEYS[i]){
				break;
			} else if(i+1 == this->KEYS.size()){
				i++;
				break;
			}
		}
		// [|5|11|]
		this->KEYS.push_back(-1);
		// [|5|11|-1]
		this->PTRS.push_back(NULL);
		// [|5|11|-1|]
		for (int j = this->KEYS.size()-2; j>=i; --j)
		{
			this->KEYS[j+1] = this->KEYS[j];
		}
		// [|5|11|11|]
		for (int j = this->PTRS.size()-2; j >=i; --j)
		{
			this->PTRS[j+1] = this->PTRS[j];
		}
		// [|5|11|11|]
		this->KEYS[i] = x;
		// [|5|6|11|]
		this->PTRS[i] = NULL;
		// [|5|6|11|]
		if(this->KEYS.size() > this->SIZE){
			// [|5|6|11|20|] (MAX FANOUT => 4) (HERE => 5)
			int LHALF = (this->KEYS.size()) / 2,
				RHALF = this->KEYS.size() - LHALF;
			//  (z: 6)
			// [|5|6\]	[|11|20|] 
			//	 y
			Node* newINTERNALNODE = new Node(this->SIZE,true);
			for (int i = 0; i < LHALF; ++i)
			{
				newINTERNALNODE->KEYS.push_back(this->KEYS[i]);
			}
			for (int i = 0; i < LHALF; ++i)
			{
				newINTERNALNODE->PTRS.push_back(this->PTRS[i]);
			}
			newINTERNALNODE->PTRS.push_back(NULL);

			// new NODE made
			int z = this->KEYS[LHALF-1];
			// z saved
			vector<int> newKEYS;
			vector<Node*> newPTRS;
			for (int i = LHALF; i < this->KEYS.size(); ++i)
			{
				newKEYS.push_back(this->KEYS[i]);
			}
			for (int i = LHALF; i < this->PTRS.size(); ++i)
			{
				newPTRS.push_back(this->PTRS[i]);
			}
			this->PTRS = newPTRS;
			this->KEYS = newKEYS;
			// original node modified
			pair<int, Node*> response = {z, newINTERNALNODE};
			return response;
		} else {
			pair<int, Node*> response = {-1, NULL};
			return response;
		}
	}

}


void BPlusTree::print(){
	queue<Node*> BUF;
	BUF.push(this->ROOT);
	BUF.push(NULL);
	while(!BUF.empty()){
		Node* TMP = BUF.front();
		BUF.pop();
		if(TMP == NULL) {
			cout<<el;
			if(!BUF.empty()) BUF.push(NULL);
		} else{
			TMP->print();
			cout<<"	";
			if(!TMP->ISLEAF) for(auto& I: TMP->PTRS) BUF.push(I);
		}
		
	}
	cout<<el;
}

void Node::print(){
	cout<<"[|";
	for(auto& I: this->KEYS){
		cout<<I<<"|";
	}
	cout<<"]";
}

int BPlusTree::remove(int x){
	bool ok = false;
	for(auto& I: this->HISTORY){
		if(x == I) ok = true;
	}
	if(ok == false){
		return 0;
	}
	else {
		// delete this->ROOT;
		this->ROOT = new Node(this->NODE_SIZE, true);
		for(auto& I: HISTORY){
			if(I == x || I == 0) continue;
			this->insert(I);
		}
		return 1;
	}
}

void showMenu(){
	cout<<"\nUwU Available options : \n";
	cout<<"* insert <value>"<<el;
	cout<<"* exit"<<el;
	cout<<"* display"<<el;
	cout<<"* remove <value>"<<el;
	// cout<<"* search <value>"<<el;
	cout<<el;
}
int main()
{
    ios_base::sync_with_stdio(0);
    	
    cout<<"Welcome, This is B+ Tree Implementation\n";
    int FANOUT;
    cout<<"Set the Maximum FANOUT : "; cin>>FANOUT;
    	
    BPlusTree* BPT = new BPlusTree(FANOUT);
    cout<<"I made a B+ Tree for you!\nYou can do following queries\n";
    showMenu();
    string choice = "";
    int x;
    int value;
    do{
    	cout<<"> ";
    	cin>>choice;
    	cout<<el;
    	if(choice == "insert"){
			cin>>value;
			int status = BPT->insert(value);
			BPT->print();
    	} else if(choice == "display"){
    		BPT->print();
    		
    	} else if(choice == "remove"){
    		int mode;
    		cin>>value;
    		BPT->remove(value);
    		BPT->print();
    		
    	} else if(choice == "search"){
    		cin>>value;
    		
    	} else if(choice != "exit"){
    		cout<<"*U* INVALID! PLEASE SELECT FROM OPTIONS";
    		showMenu();
    	} 

    } while(choice != "exit");

    
    return 0;
  	
}
