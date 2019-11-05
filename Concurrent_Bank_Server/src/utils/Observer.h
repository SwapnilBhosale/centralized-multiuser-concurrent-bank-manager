/*
 * Observer.h
 *
 *  Created on: Sep 20, 2019
 *      Author: Swapnil Bhosale
 */

#ifndef SERVER_OBSERVER_H_
#define SERVER_OBSERVER_H_
#include <iostream>
class Observer {


public:
	virtual void notify(char *data, int clientSocket)=0;
	virtual ~Observer() {
		std::cout<<"destructor of observer called"<<std::endl;
	}
};


#endif /* SERVER_OBSERVER_H_ */
