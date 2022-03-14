#include "mcv_platform.h"
#include "bt.h"

//void bt::Init()
//{
//}


btnode* bt::findNode(string s)
{
	if (tree.find(s) == tree.end()) return NULL;
	else return tree[s];
}

btnode *bt::createNode(string s)
{
if (findNode(s)!=NULL) 
	{
	printf("Error: node %s already exists\n",s.c_str());
	return NULL;	// error: node already exists
	}
btnode *btn=new btnode(s);
tree[s]=btn;
return btn;
}




void bt::CreateRootNode(string s,int type,bttask btt)
{
btnode *r=createNode(s);
r->setParent(NULL);
root=r;
r->setType(type);
if (btt != NULL) tasks[s] = btt;		// no need to protect this: if the node doesn't exist, the task won't either
current=NULL;
}


void bt::AddCompositeNode(string parent,string son,int type)
{
btnode *p=findNode(parent);
if (p->type==PARALLEL) 
	{
	printf("Error: a Parallel node can't have Composite children\n");
	exit(-1);
	}
btnode *s=createNode(son);
p->addChild(s);
s->setParent(p);
s->setType(type);
}


void bt::AddTaskNode(string parent, string son, bttask btt)
{
btnode* p = findNode(parent);
	if (p->type == TASK || p->type == TASK_SEC)
	{
		printf("Error: a Task node can't have children\n");
		exit(-1);
	}
btnode* s = createNode(son);
int nchildren=p->addChild(s);
s->setParent(p);
s->setType(TASK);

if (p->type == PARALLEL && nchildren == 2) s->setType(TASK_SEC);	// secondary tasks for right kids of parallels

tasks[son] = btt;		// no need to protect this: if the node doesn't exist, the task won't either
}


void bt::AddDecoratorNode(string parent, string son, bttask dec)
{
btnode* p = findNode(parent);
btnode* s = createNode(son);
p->addChild(s);
s->setParent(p);
s->setType(DECORATOR);
tasks[son] = dec;		// no need to protect this: if the node doesn't exist, the decorator won't either
}


void bt::recalc()
{
if (current==NULL) root->recalc(this);	// I'm not in a sequence, start from the root
else current->recalc(this);				// I'm in a sequence. Continue where I left
if (current_secondary != NULL) current_secondary->recalc(this);		// the right child of a parallel node
}

void bt::setCurrent(btnode *nc)
{
current=nc;
}


int bt::execTask(string s)
{
if (tasks.find(s)==tasks.end()) 
	{
	printf("ERROR: Missing node task for node %s\n",s.c_str());
	exit(-1);		// proper error handling goes here
	}
return (this->*tasks[s])();
}


/*int bt::testDecorator(string s)
{
if (decorators.find(s)==decorators.end())
	{
	return SUCCESS;	// error: no specific decorator defined, we assume SUCCESS, there is no decorator
	}
return (this->*decorators[s])();;
}*/
