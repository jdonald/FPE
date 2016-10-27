#include "model.h"

face::face(unsigned int a, unsigned int b, unsigned int c, const std::vector<vertex> &vList) {
	for(unsigned int t = 0; t < 3; ++t) shadow[t] = NULL;

	i[0] = a; i[1] = b; i[2] = c;
	normal = (vList[c].xyz-vList[b].xyz).cross(vList[a].xyz-vList[b].xyz);
	normal.normalize();
	//planeEq.calcPlane(vList[a].xyz, vList[b].xyz, vList[c].xyz);
}

face::~face() {
	for(unsigned int t = 0; t < 3; ++t)
		if(shadow[t]) {
			shadowWall *s = shadow[t];
			
			if(s->a != this && s->b != this) //one of these two (a/b) should match 'this'
				int a = 3;
			if( s->a->shadow[ s->edgeA ] != shadow[t] && s->b->shadow[ s->edgeB ] != shadow[t])
				int a = 3;
			if( s->a->shadow[ s->edgeA ] != s || s->b->shadow[ s->edgeB ] != s )
				int a = 3;
			
			s->a->shadow[ s->edgeA ] = NULL; //break links to shadow structure
			s->b->shadow[ s->edgeB ] = NULL;
			delete s;
		}
}

void model::addVertex(const vector3 &p) { vertexList.push_back(vertex(p)); }

void model::addFace(unsigned int a, unsigned int b, unsigned int c) { faceList.push_back(new face(a,b,c,vertexList)); }

void model::load(std::istream &in) {
	char ch;
	vector3 v;
	unsigned int a, b, c;
	bool caster = false;
	
	//clear();
	int baseIndex = vertexList.size();

	skipWhitespace(in);
	while(!in.eof() && in.get(ch)) {
		switch(ch) {
			case '/': //comment
				do { in.get(ch); } while(ch != '\n' && !in.eof());
			break;
			case 'v': //vertex
				in >> v;
				addVertex(v);
			break;
			case 't': //triangle
				in >> a >> b >> c;
				a += baseIndex; b += baseIndex; c += baseIndex;
				addFace(a, b, c);
			break;
			case 'q': //quad (triangle-pair)
				in >> a >> b >> c;
				a += baseIndex; b += baseIndex; c += baseIndex;
				addFace(a, b, c);
				in >> b;
				b += baseIndex;
				addFace(c, b, a);
			break;
			case 'c': //color
				in >> clr; clr.x /= 255.0f; clr.y /= 255.0f; clr.z /= 255.0f;
			break;
			case 's': //cast shadow
				caster = true;
			break;
			case '}': //end of model
				if(caster) generateShadowData();
				cache();
				return;
			break;
		}
		skipWhitespace(in);
	}
}

void model::generateShadowData() {
	for(std::vector<face*>::iterator faceA = faceList.begin(); faceA != faceList.end(); ++faceA) {
		for(unsigned int edgeA = 0; edgeA < 3; ++edgeA) {
			if((*faceA)->shadow[edgeA]) continue; //already has shadow data (assumes only one adjacency per edge)
			for(std::vector<face*>::iterator faceB = faceList.begin(); faceB != faceList.end(); ++faceB) {
				for(unsigned int edgeB = 0; edgeB < 3; ++edgeB) { //find shared edge
					if( (*faceA)->i[edgeA] == (*faceB)->i[(edgeB+1)%3] && (*faceA)->i[(edgeA+1)%3] == (*faceB)->i[edgeB] ) {
						if( !(*faceB)->shadow[edgeB] ) { //if doesnt already have shadow data. hmm
							shadowWall *s = new shadowWall();
							s->a = *faceA; s->b = *faceB; //fill in shadow data
							s->edgeA = edgeA; s->edgeB = edgeB;
							(*faceA)->shadow[edgeA] = s; (*faceB)->shadow[edgeB] = s; //link shadow to faces
							
							faceB = --faceList.end(); break; // 'end edge' (assumes only one valid adjacency per edge)
						}
					}
				}
			}
		}
		//if an edge is still shadowless, generate inverse face and link to all shadowless edges
	}
}

void model::clear() {
	for(std::vector<face*>::iterator i = faceList.begin(); i != faceList.end(); ++i)
		delete *i;
	faceList.clear();

	vertexList.clear();

	cache();
}

void model::cache() {
	if(!glListID) glListID = glGenLists(1);

	glNewList(glListID, GL_COMPILE);
		glColor1p(clr);
		glBegin(GL_TRIANGLES);
			for(unsigned int t = 0; t < faceList.size(); ++t) {
				glNormal1p(faceList[t]->normal);
				for(unsigned int i = 0; i < 3; ++i)
					glVertex1p(vertexList[ faceList[t]->i[i] ].xyz);
			}
		glEnd();

		//shadowWalls
		glBegin(GL_QUADS);
			for(std::vector<face*>::iterator f = faceList.begin(); f != faceList.end(); ++f) {
				for(unsigned int e = 0; e < 3; ++e) {
					if( (*f)->shadow[e] && (*f)->shadow[e]->a == (*f) ) {
						shadowWall *s = (*f)->shadow[e];
						glNormal1p( s->a->normal );
						glVertex1p(vertexList[ s->a->i[(s->edgeA+1)%3] ].xyz);
						glVertex1p(vertexList[ s->a->i[s->edgeA] ].xyz);
						glNormal1p( s->b->normal );
						glVertex1p(vertexList[ s->b->i[(s->edgeB+1)%3] ].xyz);
						glVertex1p(vertexList[ s->b->i[s->edgeB] ].xyz);
					}
				}
			}
		glEnd();

		if(drawOutlines) { //draw outlines and normals
			vector3 sum;
			unsigned int index;
			glColor3f(0.1f, 0.1f, 0.1f); //outline color
			for(unsigned int t = 0; t < faceList.size(); ++t)
			{
				sum.zero();
				glNormal1p(faceList[t]->normal);
				glBegin(GL_LINE_LOOP);
				for(unsigned int i = 0; i < 3; ++i)
				{
					index = faceList[t]->i[i];
					glVertex1p(vertexList[index].xyz);
					sum += vertexList[index].xyz;
				}
				glEnd();
				sum /= 3.0f;
				//glBegin(GL_LINES);
				//	glVertex1p(sum);
				//	glVertex1p(sum+faceList[t]->normal);
				//glEnd();
			}
		}
	glEndList();
}

void model::draw() { glCallList(glListID); }
