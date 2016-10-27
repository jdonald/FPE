#include "utilities.h"

void skipWhitespace(std::istream &in) {
	char ch;
	in >> ch;
	in.putback(ch);
}

std::string readToken(std::istream &in) {
	char ch;
	std::string token;
	if(in.peek() == '\"' ||in.peek() == '\'')
	{
		in.get(ch);
		getline(in, token, ch);			
	}
	else
		in >> token;

	return token;
}

std::istream& operator>> (std::istream &lhs, vector2 &rhs) {
	lhs >> rhs.x >> rhs.y;
	return lhs;
}

std::istream& operator>> (std::istream &lhs, vector3 &rhs) {
	lhs >> rhs.x >> rhs.y >> rhs.z;
	return lhs;
}
