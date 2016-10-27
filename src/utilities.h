#pragma once

#include "common.h"
#include "vectors.h"

template <class TYPE>
TYPE min(TYPE a, TYPE b) { if(a < b) return a; return b; }

template <class TYPE>
TYPE max(TYPE a, TYPE b) { if(a > b) return a; return b; }

template <class TYPE>
int sign(const TYPE &x) { if(x<0) return -1; if(x>0) return 1; return 0; }

void skipWhitespace(std::istream &);

std::string readToken(std::istream &);

std::istream& operator>> (std::istream &, vector3 &);
std::istream& operator>> (std::istream &, vector2 &);

