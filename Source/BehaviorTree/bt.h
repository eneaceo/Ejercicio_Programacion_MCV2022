#ifndef _BT_INC
#define _BT_INC

#include <string>
#include <map>
#include "btnode.h"

using namespace std;

// actions are now tasks, as per the Unreal terminology, and return 3 states:
// SUCCESS: i'm done, keep going
// FAIL: i'm done, but didn't succeed, so stop sequences / return FALSE on decorators
// IN_PROGRESS: this is the old "STAY": still working
typedef int (bt::*bttask)(); 

// conditions are now decorators, as per the Unreal terminology, and return 2 states:
// SUCCESS: this is the old "true"
// FAIL: this is the old "false"
// typedef int (bt::*btdecorator)();		


// Implementation of the behavior tree, Unreal Engine 4 - style 
// Adds decorators as conditional nodes & parallel composite node
// uses the BTnode so both work as a system
// tree implemented as a map of btnodes for easy traversal
// behaviours are a map to member function pointers, to 
// be defined on a derived class. 
// BT is thus designed as a pure abstract class, so no 
// instances or modifications to bt / btnode are needed...


class bt
	{
	// the nodes
	map<string,btnode *>tree;
	// the C++ functions that implement node tasks, hence, the behaviours
	map<string,bttask> tasks;
	// the C++ functions that implement conditions
	// map<string,btdecorator> decorators;

	btnode *root=NULL;
	
	// moved to private as really the derived classes do not need to see this, these are the functions with integrity checks
	btnode *createNode(string);
		
	public:
	btnode *findNode(string);
		btnode* current = NULL;
		btnode* current_secondary = NULL;		// for parallel nodes: this stores the right task

		// use a derived create to declare BT nodes for your specific BTs
		//virtual void Init();
		void CreateRootNode(string,int,bttask=NULL);	// the last parameter is in case the root is a task, uncommon but possible
		void AddCompositeNode(string,string,int);
		void AddTaskNode(string,string,bttask);
		void AddDecoratorNode(string,string,bttask);
		
		// internals used by btnode and other bt calls
		
		int execTask(string);
		
		int testDecorator(string);
		void setCurrent(btnode *);

		// call this once per frame to compute the AI. No need to derive this one, 
		// as the behaviours are derived via bttasks and the tree is declared on Init
		void recalc();



	};


#endif