#ifndef  LSYSTEM_H
#define LSYSTEM_H

#include <stack>
#include <GLFW/glfw3.h>
#include <GL/glm/glm.hpp>

#include "Grammar.h"

using namespace std;

struct State
{
	glm::vec3 pos;
	glm::vec3 dir;
	float length;
	float radius;
	int level;
};

struct Trunk
{
	glm::vec3 start;
	glm::vec3 end;
	float radius;
	float length;
	Trunk() {
		start = end = glm::vec3(0.0f);
	}
};

struct Leaf
{
	glm::vec3 pos;
	glm::vec3 end;
	glm::vec3 dir;
};

class LSystem {
public:
	vector<Trunk> trunks;
	vector<Leaf> leafs;
	LSystem();
	~LSystem();
	void generateFractal();
	double getRadiusFactor() { return radiusFactor; }
	double getLengthFactor() { return lengthFactor; }
	double getLeafRadius() { return leafRadius; }

//private:
	int level;
	double dx, dy, dz;
	double leafRadius;
	double leafLength;
	double length, lengthFactor;
	double radius, radiusFactor;
	State curState;
	Grammar grammar;
	int numTrunks, numLeafs;
	string result;
};

#endif