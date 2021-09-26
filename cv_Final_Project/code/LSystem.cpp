#include "LSystem.h"
#include "MyRotate.h"
LSystem::LSystem() {
	dx = 35.0f;
	dz = 35.0f;
	dy = 30.0f;
	length = 20.0;
	radius = 3.0f;
	leafRadius = 3.0f;
	leafLength = 0.2;
	lengthFactor = 0.7;
	radiusFactor = 0.6;
	numTrunks = numLeafs = 0;
	result = grammar.getResult();
}

LSystem::~LSystem() {
	result.clear();
}

void LSystem::generateFractal() {
	trunks.clear();
	leafs.clear();
	curState.pos = glm::vec3(0, 0, 0);
	curState.dir = glm::vec3(0, 1, 0);
	curState.length = length;
	curState.level = 1;
	curState.radius = radius;
	//״̬ջ
	stack<State> stacks;


	for (int i = 0; i < result.size(); i++) {
		char c = result[i];
		Trunk tmp;
		//����
		if (c == 'F') {
			tmp.start = curState.pos;
			curState.pos += curState.dir * (float)curState.length;
			tmp.end = curState.pos;
			tmp.radius = curState.radius;
			trunks.push_back(tmp);
			continue;
		}
		//��X��תdx�Ƕ�
		else if (c == '+') {
			curState.dir = MyRotate::RotateX(curState.dir, dx);
			continue;
		}
		//��X��ת-dx�Ƕ�
		else if (c == '-') {
			curState.dir = MyRotate::RotateX(curState.dir, -dx);
			continue;
		}
		//��X��תdy�Ƕ�
		else if (c == '*') {
			curState.dir = MyRotate::RotateY(curState.dir, dy);
			continue;
		}
		//��X��ת-dy�Ƕ�
		else if (c == '/') {
			curState.dir = MyRotate::RotateY(curState.dir, -dy);
			continue;
		}
		//��X��תdz�Ƕ�
		else if (c == '!') {
			curState.dir = MyRotate::RotateZ(curState.dir, dz);
			continue;
		}
		//��X��ת-dz�Ƕ�
		else if (c == '@') {
			curState.dir = MyRotate::RotateZ(curState.dir, -dz);
			continue;
		}
		//����ǰ��״̬ѹ��ջ�б���
		//����ǰ�ĳ��Ⱥ������������С
		else if (c == '[') {
			stacks.push(curState);
			curState.length *= lengthFactor;
			curState.radius *= radiusFactor;
			continue;
		}
		//��ջ
		else if (c == ']') {
			curState = stacks.top();
			stacks.pop();
			continue;
		}
		//Ҷ�ӽڵ�
		else if (c == 'L') {
			Trunk t = trunks[trunks.size() - 1];
			Leaf l;
			l.dir = t.end - t.start;
			l.dir = glm::normalize(l.dir);
			l.pos = t.end;
			l.end.x = l.pos.x + l.dir.x * leafLength;
			l.end.y = l.pos.y + l.dir.y * leafLength;
			l.end.z = l.pos.z + l.dir.z * leafLength;
			leafs.push_back(l);
		}
		else {
			//DO_NOTHING
		}
	}

	//cout << "trunks.size() = " << trunks.size() << endl;
}