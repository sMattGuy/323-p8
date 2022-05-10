#include <iostream>
#include <fstream>

class AStarNode{
	public:
	int configuration[9] = {0};
	int gStar;
	int hStar;
	int fStar;
	AStarNode* next;
	AStarNode* parent;
	
	AStarNode(){
		this->gStar = 0;
		this->hStar = 0;
		this->fStar = 0;
		this->next = nullptr;
		this->parent = nullptr;
	}
	void printNode(std::ofstream& output){
		output<<"<"<<this->fStar<<"::";
		for(int i=0;i<9;i++){
			output<<this->configuration[i]<<" ";
		}
		if(this->parent == nullptr){
			output<<"::NULL::NULL>\n";
		}
		else{
			output<<"::"<<this->parent->fStar<<"::";
			for(int i=0;i<9;i++){
				output<<this->parent->configuration[i]<<" ";
			}
			output<<">\n";
		}
	}
};

class AStar{
	public:
	AStarNode* startNode;
	AStarNode* goalNode;
	
	AStarNode* open;
	AStarNode* close;
	AStarNode* childList;
	
	int table[9][9] = 
	{
		{0,1,2,1,2,3,2,3,4},
		{1,0,1,2,1,2,3,2,3},
		{2,1,0,3,2,1,4,3,2},
		{1,2,3,0,1,2,1,2,3},
		{2,1,2,1,0,1,2,1,2},
		{3,2,1,2,1,0,3,2,1},
		{2,3,4,1,2,3,0,1,2},
		{3,2,3,2,1,2,1,0,1},
		{4,3,2,3,2,1,2,1,0}
	};
	
	AStar(){
		this->startNode = nullptr;
		this->goalNode = nullptr;
		this->open = nullptr;
		this->close = nullptr;
		this->childList = nullptr;
	}
	
	int computeGstar(AStarNode* node){
		return node->parent->gStar + 1;
	}
	int computeHstar(AStarNode* node){
		int dist = 0;
		for(int i=0;i<9;i++){
			dist += this->table[node->configuration[i]][this->goalNode->configuration[i]];
		}
		return dist;
	}
	bool isGoalNode(AStarNode* node){
		return this->match(node->configuration, this->goalNode->configuration);
	}
	void openInsert(AStarNode* node){
		AStarNode* reader = this->open;
		while(reader->next != nullptr){
			if(reader->next->fStar > node->fStar){
				break;
			}
			reader = reader->next;
		}
		node->next = reader->next;
		reader->next = node;
	}
	void closeInsert(AStarNode* node){
		AStarNode* reader = this->close;
		while(reader->next != nullptr){
			if(reader->next->fStar > node->fStar){
				break;
			}
			reader = reader->next;
		}
		node->next = reader->next;
		reader->next = node;
	}
	AStarNode* remove(AStarNode* listHead){
		if(listHead->next == nullptr){
			return nullptr;
		}
		else{
			AStarNode* temp = listHead->next;
			listHead->next = listHead->next->next;
			temp->next = nullptr;
			return temp;
		}
	}
	AStarNode* findSpot(AStarNode* listHead, AStarNode* node){
		AStarNode* reader = listHead;
		while(reader->next != nullptr){
			if(this->match(node->configuration, reader->next->configuration)){
				return reader;
			}
			reader = reader->next;
		}
		return nullptr;
	}
	void closeDelete(AStarNode* spot){
		AStarNode* temp = spot->next;
		spot->next = spot->next->next;
		delete temp;
	}
	bool match(int* config1, int* config2){
		for(int i=0;i<9;i++){
			if(config1[i] != config2[i]){
				return false;
			}
		}
		return true;
	}
	bool checkAncestors(AStarNode* currentNode, AStarNode* parent){
		if(parent == nullptr){
			return false;
		}
		else if(this->match(currentNode->configuration, parent->configuration)){
			return true;
		}
		else{
			return this->checkAncestors(currentNode, parent->parent);
		}
	}
	AStarNode* createNewChild(int i, int j, AStarNode* currentNode, AStarNode* listHead){
		//create new node to hold next move
		AStarNode* newNode = new AStarNode();
		for(int k=0;k<9;k++){
			newNode->configuration[k] = currentNode->configuration[k];
		}
		//perform the move
		int temp = 0;
		temp = newNode->configuration[i];
		newNode->configuration[i] = newNode->configuration[j];
		newNode->configuration[j] = temp;
		//update new nodes g h f
		newNode->parent = currentNode;
		newNode->gStar = this->computeGstar(newNode);
		newNode->hStar = this->computeHstar(newNode);
		newNode->fStar = newNode->gStar + newNode->hStar;
		//check if to add it to new list
		if(!this->checkAncestors(newNode, currentNode)){
			newNode->next = listHead->next;
			listHead->next = newNode;
		}
		else{
			//node shouldnt be inserted, delete
			delete newNode;
		}
		return listHead;
	}
	AStarNode* constructChildList(AStarNode* currentNode){
		AStarNode* listHead = new AStarNode();
		for(int i=0;i<9;i++){
			if(currentNode->configuration[i] == 0){
				if(i+1<9 && i!=2 && i!=5){
					listHead = createNewChild(i, i+1, currentNode, listHead);
				}
				if(i+3<9){
					listHead = createNewChild(i, i+3, currentNode, listHead);
				}
				if(i-1 >= 0 && i!=6 && i!=3){
					listHead = createNewChild(i, i-1, currentNode, listHead);
				}
				if(i-3 >= 0){
					listHead = createNewChild(i, i-3, currentNode, listHead);
				}
				return listHead;
			}
		}
		return nullptr;
	}
	void printList(AStarNode* listHead, std::ofstream& output){
		AStarNode* reader = listHead;
		while(reader != nullptr){
			reader->printNode(output);
			reader = reader->next;
		}
		output<<"~~~~~~~~~~~~~~~~~~~~~~\n";
	}
	void printSolution(AStarNode* currentNode, std::ofstream& output){
		output<<"Solution Found.\nGoal\n";
		this->printSolutionRecurv(currentNode, output);
	}
	bool printSolutionRecurv(AStarNode* currentNode, std::ofstream& output){
		if(currentNode == nullptr){
			output<<"Start\n";
			return true;
		}
		else{
			for(int i=0;i<9;i++){
				if(i==3 || i==6){
					output<<std::endl;
				}
				output<<currentNode->configuration[i]<<" ";
			}
			output<<"\n\n";
		}
		return printSolutionRecurv(currentNode->parent, output);
	}
};

int main(int argc, char* argv[]){
	if(argc != 5){
		std::cout<<"You must include 4 arguments! (start, goal, out1, out2)\n";
		return -1;
	}
	//create input stream
	std::ifstream startFile(argv[1]);
	std::ifstream goalFile(argv[2]);
	if(!startFile.good() || !goalFile.good()){
		std::cout<<"Failed to read input files! (make sure the names are correct!)\n";
		return -1;
	}
	//outputs
	std::ofstream debug(argv[3]);
	std::ofstream results(argv[4]);
	//create initial configurations
	int startConfig[9] = {};
	int goalConfig[9] = {};
	for(int i=0;i<9;i++){
		startFile >> startConfig[i];
		goalFile >> goalConfig[i];
	}
	startFile.close();
	goalFile.close();
	//create AStar class
	AStar* AStarSearch = new AStar();
	//create init nodes
	AStarNode* startNode = new AStarNode();
	AStarNode* goalNode = new AStarNode();
	//assign configs from files
	for(int i=0;i<9;i++){
		startNode->configuration[i] = startConfig[i];
		goalNode->configuration[i] = goalConfig[i];
	}
	//set search goal and start to new nodes
	AStarSearch->startNode = startNode;
	AStarSearch->goalNode = goalNode;
	//create dummy nodes for open and close
	AStarSearch->open = new AStarNode();
	AStarSearch->close = new AStarNode();
	//step 1
	AStarSearch->startNode->gStar = 0;
	AStarSearch->startNode->hStar = AStarSearch->computeHstar(AStarSearch->startNode);
	AStarSearch->startNode->fStar = AStarSearch->startNode->gStar + AStarSearch->startNode->hStar;
	AStarSearch->openInsert(AStarSearch->startNode);
	AStarNode* currentNode = new AStarNode();
	//loop counter
	int loopCounter = 0;
	//step 13 loop
	while(!AStarSearch->isGoalNode(currentNode) || AStarSearch->open->next != nullptr){
		//step 2
		currentNode = AStarSearch->remove(AStarSearch->open);
		//step 3
		if(currentNode != nullptr && AStarSearch->isGoalNode(currentNode)){
			//solution found
			results<<"A Solution has been found!\n";
			AStarSearch->printSolution(currentNode, results);
			return 0;
		}
		//step 4
		AStarSearch->childList = AStarSearch->constructChildList(currentNode);
		//step 10 loop
		while(AStarSearch->childList->next != nullptr){
			//step 5
			AStarNode* child = AStarSearch->remove(AStarSearch->childList);
			//step 6
			child->gStar = AStarSearch->computeGstar(child);
			child->hStar = AStarSearch->computeHstar(child);
			child->fStar = child->gStar + child->hStar;
			child->parent = currentNode;
			//step 7
			AStarNode* spot = AStarSearch->findSpot(AStarSearch->close, child);
			//step 8
			if(spot != nullptr && spot->next != nullptr && spot->next->fStar > child->fStar){
				AStarSearch->closeDelete(spot);
			}
			//step 9
			AStarSearch->openInsert(child);
		}
		//step 11
		AStarSearch->closeInsert(currentNode);
		//step 12
		if(loopCounter < 20){
			debug<<"Loop:"<<loopCounter<<std::endl;
			debug<<"This is Open list:\n";
			AStarSearch->printList(AStarSearch->open, debug);
			debug<<"This is Close list:\n";
			AStarSearch->printList(AStarSearch->close, debug);
			loopCounter++;
		}
		else if(loopCounter == 20){
			debug<<"Over 20 loops, this is the last that will be printed\n";
			debug<<"Loop:"<<loopCounter<<std::endl;
			debug<<"This is Open list:\n";
			AStarSearch->printList(AStarSearch->open, debug);
			debug<<"This is Close list:\n";
			AStarSearch->printList(AStarSearch->close, debug);
			loopCounter++;
		}
		if(AStarSearch->open->next == nullptr && !AStarSearch->isGoalNode(currentNode)){
			results<<"No solution can be found in the search!\n";
			return 0;
		}
	}
	return 0;
}