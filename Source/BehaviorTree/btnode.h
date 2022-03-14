#ifndef _BTNODE_INC
#define _BTNODE_INC

#include <string>
#include <vector>

using namespace std;

class bt;
class btnode;


#define RANDOM 0		// I could delete these but they are handy
#define SEQUENCE 1
#define SELECTOR 2		// old priority
#define DECORATOR 3		// decorators are no longer conditions on nodes, but full nodes instead
#define PARALLEL 4		// this is the simpleparallel node from Epic: left child cancels right, must have tasks as siblings
#define TASK 5				// old action
#define TASK_SEC 6				// to do parallel nodes: right son of a parallel node

#define IN_PROGRESS 0	// node will keep executing the next frame
#define SUCCESS 1			// node ended the task, we can move on to the next step
#define FAIL 2				// node ended the task with a fail, hence we can abort

class btnode
	{
	string name;
	
	vector<btnode *>children;
	btnode *parent;
	btnode *right;
	public:
		int type;
		int status=1;				// used to hold permanently the status of looping decorators
		btnode(string);
		void create(string);
		bool isRoot();
		void setParent(btnode *);
		void setRight(btnode *);
		int addChild(btnode *);		// returns the number of children after the insertion
		void setType(int );
		int recalc(bt *);		// now returns int so the parents node know what the children say
		string getName();
};

#endif