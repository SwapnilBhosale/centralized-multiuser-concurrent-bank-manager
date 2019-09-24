/*
 * Observer.cpp
 *
 *  Created on: Sep 20, 2019
 *      Author: lilbase
 */
#include "ObserverPattern.h"

#include <iostream>

ObserverPattern* ObserverPattern::instance = NULL;

ObserverPattern* ObserverPattern::get_instance() {
	if(instance == NULL)
		instance = new ObserverPattern;
	return instance;
}

ObserverPattern::ObserverPattern(){

}

void ObserverPattern::add_observant(Observer *ob){
	vect.push_back(ob);
}

void ObserverPattern::notify_observants(char *data, int client_socket) {
	for(Observer * ob: vect){
		ob -> notify(data, client_socket);
	}
}

