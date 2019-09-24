/*
 * Observer.c
 *
 *  Created on: Sep 20, 2019
 *      Author: lilbase
 */

#ifndef SERVER_OBSERVERPATTERN_H_
#define SERVER_OBSERVERPATTERN_H_
#include <vector>

#include "../src/Observer.h"

class ObserverPattern{
public:
	static ObserverPattern* get_instance();
	void add_observant(Observer *ob);
	void notify_observants(char *data, int clientSocket);

private:
	std::vector<class Observer *> vect;
	static ObserverPattern *instance;
	ObserverPattern();

};




#endif /* SERVER_OBSERVERPATTERN_H_ */
