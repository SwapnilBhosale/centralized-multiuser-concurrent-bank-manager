/*
 * utils.h
 *
 *  Created on: Nov 4, 2019
 *      Author: lilbase
 */

#ifndef UTILS_CPP_
#define UTILS_CPP_

#define PORT 8080
#define HOST "224.0.0.0"
#define TIME_DRIFT 10
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>


template <size_t N>
void splitString(std::string (&arr)[N], std::string str)
{
	int n = 0;
	std::istringstream iss(str);
	for (auto it = std::istream_iterator<std::string>(iss); it != std::istream_iterator<std::string>() && n < N; ++it, ++n)
		arr[n] = *it;
}


#endif /* UTILS_CPP_ */
