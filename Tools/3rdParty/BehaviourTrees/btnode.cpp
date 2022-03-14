#include <conio.h>
#include <stdio.h>
#include "btnode.h"
#include "bt.h"


btnode::btnode(string s)
{
name=s;
}


void btnode::create(string s)
{
name=s;
}


bool btnode::isRoot()
{
return (parent==NULL);
}


void btnode::setParent(btnode *p)
{
parent=p;
}


void btnode::setRight(btnode *p)
{
right=p;
}


void btnode::setType(int t)
{
type=t;
}


string btnode::getName()
{
return name;
}


int btnode::addChild(btnode *c)
{
if (!children.empty()) // if this node already had children, connect the last one to this
	children.back()->setRight(c);  // new one so the new one is to the RIGHT of the last one

children.push_back(c); // in any case, insert it
c->right=NULL; // as we're adding from the right make sure right points to NULL
return children.size();
}

char* gettype(int i)
{
	char* s=new char[256];
	if (i == 0) s = _strdup("RANDOM");
	if (i == 1) s = _strdup("SEQUENCE");
	if (i == 2) s = _strdup("SELECTOR");
	if (i == 3) s = _strdup("DECORATOR");
	if (i == 4) s = _strdup("PARALLEL");
	if (i == 5) s = _strdup("TASK");
	if (i == 6) s = _strdup("TASK_SEC");
	return s;
}


void DebugRes(string n,int i)
{
if (i == 0) printf("Node %s IN PROGRESS\n",n.c_str());
if (i == 1) printf("Node %s SUCCESS\n", n.c_str());
if (i == 2) printf("Node %s FAIL\n", n.c_str());
}

int btnode::recalc(bt *tree)
{
int res;
// activate the next line to debug
printf("RECALC: node %s, TYPE: %s\n",name.c_str(),gettype(type));
switch (type)
	{
		case TASK_SEC:		// secondary tasks are cancelled by their left brother, so they never return by themselves
		{
			// run the controller of this node
			res = tree->execTask(name);
			tree->current_secondary=this;
			DebugRes(name, res);
			return res;		// this result will be ignored, secondary task results are not used
			break;
		}
		case TASK:	
		{
			// run the controller of this node
			res=tree->execTask(name);
			// now, the next lines compute what's the NEXT node to use in the next frame...
			if (res == IN_PROGRESS) 
				{ 
				tree->setCurrent(this); 
				DebugRes(name, res); 
				return res; // looping vs. on-shot actions
				}

			if (tree->current_secondary!=NULL)
				{ 
				printf("I cancel my secondary task %s\n", tree->current_secondary->getName().c_str());
				tree->current_secondary = NULL;
				// I cancel secondary tasks, if there are any
				}

			//	FIX	
			if (res == FAIL)
				{
				printf("ABORT DUE TO FAIL\n");
				tree->setCurrent(NULL);
				DebugRes(name, res);
				return res; // returning due to abort by fail 
				}

			// climb tree iteratively, look for unfinished SUCCESSFUL sequence to complete or inprogress decorators
			btnode *cand=this;
			while (cand->parent != NULL)
			{
				btnode* daddy = cand->parent;
				switch (daddy->type)
				{
				case SEQUENCE:
				{
					// oh we were doing a sequence. make sure we finished it!!!
					if (cand->right != NULL) // && res=SUCCESS
						{ 
						tree->setCurrent(cand->right); 
						DebugRes(name, res); 
						return SUCCESS; 
						}	// ERROR: partial abortion of sequences due to FAIL on task
					// sequence was finished (there is nobody on right). Go up to daddy.
					else cand = daddy;
					break;
				}
				case DECORATOR:
				{
					// oh we were doing a decorator. make sure we finished it!!!
					if (daddy->status == IN_PROGRESS) 
						{ 
						tree->setCurrent(daddy);
						DebugRes(name, res);
						return IN_PROGRESS; 
						}
					// decorator was finished. Go up to daddy.
					else cand = daddy;
					break;
				}
				default:
				{
					// i'm not on a sequence or decorator, so keep moving up to the root of the BT
					cand = daddy;
					break;
				}
				}
				// if we've reached the root, means we can reset the traversal for next frame.
				if (cand->parent == NULL) { tree->setCurrent(NULL);	return SUCCESS; }
				}
			break;
			}
	case RANDOM:
		{
		int r=rand()%children.size();
		res=children[r]->recalc(tree);
		return res;
		break;
		}
	case SELECTOR:
		{
		res = FAIL;
		for (int i=0;i<children.size();i++)
			{
			if (res=children[i]->recalc(tree) != FAIL)		// in progress or success are GOOD
				{
				return res;
				}
			}
		return res;
		}
	case SEQUENCE:
		{
		// begin the sequence...the inner node (action) will take care of the sequence
		// via the "setCurrent" mechanism
		res=children[0]->recalc(tree);	
		return res;
		break;
		}
	case DECORATOR:
		{
		status = tree->execTask(name);
		DebugRes(name,status);
		if (status != FAIL) res = children[0]->recalc(tree);
		else res = status;
		return res;
		break;
		}
	case PARALLEL:
	{
		// we assume 2 tasks as children. If not, exit
		if (children.size()!=2) 
			{ 
			printf("Error: a Parallel node must have 2 children\n");
			exit(-1);
			}
		res = children[0]->recalc(tree);		
		
		tree->current_secondary = children[1];
		break;
	}
}
DebugRes(name, res);
return res;
}



