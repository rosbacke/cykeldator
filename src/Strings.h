/*
 * Strings.h
 *
 *  Created on: 13 apr. 2019
 *      Author: mikaelr
 */

#ifndef SRC_STRINGS_H_
#define SRC_STRINGS_H_

template<typename T>
char* uint2str( char* const str, T value)
{
	static const char chars[] = { '0', '1','2', '3','4', '5','6', '7','8', '9','a', 'b','c', 'd','e', 'f'};

	int noChars = 2 * sizeof value;
	int index = noChars;
	while(--index >= 0)
	{
		str[index] = chars[ value & 0x0f ];
		value >>= 4;
	}
	str[ noChars ] = 0;
	return str + noChars;
}

class Strings
{

};

#endif /* SRC_STRINGS_H_ */
