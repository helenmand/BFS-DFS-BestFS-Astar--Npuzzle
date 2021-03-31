#include <iostream>
#include <queue>
#include <unordered_map>
#include <stack>

// matrix dimension are height:3 and width: 3
#define height 3
#define width 3

using namespace std;

// error message if search algorithms fails
void error(){
    cout<<"sorry i could not find a solution"<<endl;
    cout<<"please check your arguments when you create your initial node"<<endl; // most common mistake 
}

// tree node 
struct Node{
    Node* parent; // parent node of the current node
    int puzzle[height][width]; // node puzzle state
    int Bx, By; // black tile coordinates
    int moves; // number of moves so far
    unsigned long key; // key for the unorderd map
    int cost; // the number of misplaced tiles
};

// function to allocate a new node
Node* newNode(int puzzle[height][width], int x, int y, int newX, int newY, int moves, Node* parent){
    Node* node = new Node;

    node->parent = parent;// set pointer for path to root
    memcpy(node->puzzle, puzzle, sizeof node->puzzle); // copy data from parent node to current node
    swap(node->puzzle[x][y], node->puzzle[newX][newY]); // move tile by 1 position
    
    node->cost = INT_MAX; // cost of current state
    node->moves = moves; // set number of moves so far
    
    node->Bx = newX; // update new blank tile cordinates
    node->By = newY;

    // key for unordered map 
    unsigned long key = node->puzzle[0][0]*100000000 +
                        node->puzzle[0][1]*10000000 +
                        node->puzzle[0][2]*1000000 +
                        node->puzzle[1][0]*100000 +
                        node->puzzle[1][1]*10000 +
                        node->puzzle[1][2]*1000 +
                        node->puzzle[2][0]*100 +
                        node->puzzle[2][1]*10 +
                        node->puzzle[2][2]*1 ;
    node->key = key;
    
    return node; // returning new node

}

// down, left, up, right
int row[] = { 1, 0, -1, 0 };
int col[] = { 0, -1, 0, 1 };

// heuristic function for BestFS, calculates the number of misplaced tiles
int calculateBestFScost( int initial[height][width], int final[height][width]){
    int cost = 0;

    for ( int i=0; i<height; i++)
      for (int j=0; j<width; j++)
        
        cost = (initial[i][j] != final[i][j])?++cost:cost;
    return cost;
}

// heuristic function for A* consisted of BestFS heuristic function 
// plus the number of moves so far
int calculateAStarCost(Node* current, int final[height][width]){
    return (calculateBestFScost(current->puzzle, final) + current->moves);
}

// Function to check if (x, y) is a valid puzzle cordinate
int isSafe(int x, int y){
    return (x >= 0 && x < height && y >= 0 && y < width);
}

// checks if the current state is the final state.
bool isFinalState( Node* current, Node* final){
    for( int i = 0; i<height; i++){
        for ( int j = 0; j<width; j++){
            if ((current->puzzle[i][j]) != (final->puzzle[i][j]))
                return false;
        }
    }
    return true;
}

// function to print a puzzle
void printPuzzle(int puzzle[height][width]){
    for ( int i=0; i<height; i++){
        for ( int j=0; j<width; j++)
            cout<<puzzle[i][j]<<" ";
        cout<<endl;
    }
}

// print path from root node to destination node
void printPath(Node* root){
    if (root == NULL)
        return;
    printPath(root->parent);
    printPuzzle(root->puzzle);

    cout<<endl;
}

// comparison object to be used to order the heap
struct comp{
    bool operator()( const Node* a, const Node* b) const{
        return (a->cost) > (b->cost);
    }
};

// BFS algorith for N-puzzle
void solveBFS( Node* initial, Node* final){
    queue< Node*> agenda; // search list
    unordered_map< unsigned long, Node*> closed; // visited nodes 
    agenda.push(initial); 

    int count = 0; // counts how many nodes the algorith visited

    while (!agenda.empty()){ 
        count++;

        Node *n = agenda.front(); 
        agenda.pop(); // removing current node from search list

        if ( closed.count(n->key)==0 ){ // current node does not belong to visited nodes

            if (isFinalState( n, final) == true){ // if current state is the goal state
                cout<<"BFS moves: "<<n->moves<<endl; // printing moves to final
                cout<<"nodes visited :"<<count; // printing total visited nodes                 
                return; // ending search function 
            }

            pair<unsigned long, Node*> k (n->key, n);
            closed.insert(k); // adding current state to visited nodes

            for (int i = 0; i < 4; i++){
                if (isSafe(n->Bx + row[i], n->By + col[i])){ // expanding current state
                    Node* child = newNode(n->puzzle, n->Bx, n->By, n->Bx + row[i], n->By + col[i], n->moves + 1, n);
                    if ( closed.count(child->key)==0) // adding expanded states that do not belong to visited nodes
                        agenda.push(child);
                }
            }

        }
    }
    error();
}

// DFS algorith for N-puzzle
void solveDFS( Node* initial, Node* final){
    stack< Node*> agenda; // search list
    unordered_map< unsigned long long int, Node*> closed; // visited nodes 
    agenda.push(initial);

    int count = 0; // counts how many nodes the algorith visited

    while (!agenda.empty()){
        count++;

        Node *n = agenda.top();
        agenda.pop(); // removing current node from search list

        if ( closed.count(n->key)==0 ){ // current node does not belong to visited nodes

            if (isFinalState( n, final) == true){ // if current state is the goal state 
                cout<<"DFS moves: "<<n->moves<<endl; // printing moves to final
                cout<<endl<<"nodes visited :"<<count; // printing total visited nodes 
                return; // ending search function 
            }

            pair<unsigned long, Node*> k (n->key, n);
            closed.insert(k); // adding current state to visited nodes

            for (int i = 0; i < 4; i++){
                if (isSafe(n->Bx + row[i], n->By + col[i])){ // expanding current state
                    Node* child = newNode(n->puzzle, n->Bx, n->By, n->Bx + row[i], n->By + col[i], n->moves + 1, n);
                    if ( closed.count(child->key)==0) // adding expanded states that do not belong to visited nodes
                        agenda.push(child);
                }
            }
        }
    }
    error();
}

// BestFS algorith for N-puzzle
void solveBestFS( Node* initial, Node* final){
    priority_queue<Node*, vector<Node*>, comp> agenda; // search list
    unordered_map< unsigned long, Node*> closed; // visited nodes 
    
    initial->cost = calculateBestFScost(initial->puzzle, final->puzzle);  // calculating initial node(root) cost
    agenda.push(initial);
    
    int count = 0; // counts how many nodes the algorith visited

    while (!agenda.empty()){
        count++;
        
        Node *n = agenda.top(); 
        agenda.pop(); // removing current node from search list

        if ( closed.count(n->key)==0 ){ // current node does not belong to visited nodes

            if (isFinalState( n, final) == true){ // if current state is the goal state 
                cout<<"BestFS moves: "<<n->moves<<endl; // printing moves to final
                cout<<endl<<"nodes visited :"<<count; // printing total visited nodes                 
                return; // ending search function 
            }

            pair<unsigned long, Node*> k (n->key, n);
            closed.insert(k); // adding current state to visited nodes

            for (int i = 0; i < 4; i++){
                if (isSafe(n->Bx + row[i], n->By + col[i])){
                    
                    Node* child = newNode(n->puzzle, n->Bx, n->By, n->Bx + row[i], n->By + col[i], n->moves + 1, n);
                    child->cost = calculateBestFScost(child->puzzle, final->puzzle); // calculating cost
                    
                    if ( closed.count(child->key)==0) // adding expanded states that do not belong to visited nodes
                        agenda.push(child);
                }
            }

        }
    }
    error();
}

// A* algorith for N-puzzle
void solveAstar(Node* initial, Node* final){
    priority_queue< Node*, std::vector<Node*>, comp> agenda; // search list
    unordered_map< unsigned long, Node*> closed; // visited nodes 
   
    initial->cost = calculateAStarCost(initial, final->puzzle); // calculating initial node(root) cost
    agenda.push(initial);

    int count = 0; // counts how many nodes the algorith visited
   
    while (!agenda.empty()){
        count++;
        
        Node* n = agenda.top();
        agenda.pop(); // removing current node from search list
        
        if ( closed.count(n->key)==0 ){  // current node does not belong to visited nodes
            
            if (isFinalState(n, final) == true){ // if current state is the goal state 
                cout<<"A* moves: "<<n->moves<<endl; // printing moves to final
                cout<<endl<<"nodes visited :"<<count<<endl; // printing total visited nodes    
                return;// ending search function 
            }

            pair<unsigned long, Node*> k (n->key, n);
            closed.insert(k); // adding current state to visited nodes

            for (int i=0; i<4; i++){
                if (isSafe(n->Bx + row[i], n->By + col[i])){
                    
                    Node* child = newNode(n->puzzle, n->Bx, n->By, n->Bx + row[i], n->By + col[i], n->moves + 1, n);
                    child->cost = calculateAStarCost(child, final->puzzle); // calculating cost
                    
                    if ( closed.count(child->key)==0) // adding expanded states that do not belong to visited nodes
                        agenda.push(child);
                }
            }
       }
    }
    error();
}

// main
int main(){

    // starting puzzle
    int initial[height][width] = {
        {3, 6, 0},
        {1, 4, 2},
        {7, 5, 8}
    };

    // goal puzzle
    int final[height][width] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };

    // creating nodes for starting and goal state
    // arguments: puzzle, current cordinates of the blank tile x, y, new cordinates of the blank tile x, y, moves so far, node parent
    Node* initialNode = newNode( initial, 0, 2, 0, 2, 0, NULL);
    Node* finalNode = newNode( final, 2, 2, 2, 2, 0, NULL);
             
    cout<<"the number of moves each algorithms needs to solve the puzzle and the number of nodes he visited to find the solution"<<endl;
    
    solveBFS(initialNode, finalNode);

    cout<<endl<<endl<<"DFS moves: "<<endl;
    solveDFS(initialNode, finalNode);

    cout<<endl<<endl<<"BestFS moves: "<<endl;
    solveBestFS(initialNode, finalNode);
        
    cout<<endl<<endl<<"A* moves: "<<endl;
    solveAstar(initialNode, finalNode);
    
    system("pause");


    return 0;
}
