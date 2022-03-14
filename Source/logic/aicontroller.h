#ifndef _AICONTROLLER_H
#define _AICONTROLLER_H

// ai controllers using maps to function pointers for easy access and scalability. 

// we put this here so you can assert from all controllers without including everywhere
#include <assert.h>	
#include <string>
#include <map>
#include "logic/Entidad.h"

// states are a map to member function pointers, to 
// be defined on a derived class. 
class aicontroller;

typedef void (aicontroller::*statehandler)(); 

class aicontroller
	{
	
	std::string state;
	// the states, as maps to functions
	std::map<std::string,statehandler>statemap;

	public:
		Entidad* e;
		void ChangeState(std::string);	// state we wish to go to
		virtual void Init();	// resets the controller
		void Recalc();	// recompute behaviour
		void AddState(std::string,statehandler);
		const std::string& getState() const { return state; }
	};

#endif