#pragma once
#ifndef LOOP_SUBDIVISION_H_
#define LOOP_SUBDIVISION_H_
#include <halfEdge.h>
#include<math.h>
#define PI 3.1415926


class LoopSub {
public:
	std::vector<vertex> vers;
	std::vector<half_edge> hes;
	std::vector<face> faces;

	std::vector<vertex> upvers;//新增点后的半边结构，由于面是一个个生成，为了时间，不存储
	std::vector<half_edge> uphes;
	std::vector<face> upfaces;


	LoopSub(std::vector<vertex> &v,std::vector<half_edge>&e,std::vector<face> &f);
	~LoopSub();
	void Subdivision();
};

LoopSub::LoopSub(std::vector<vertex> &v, std::vector<half_edge>&e, std::vector<face> &f) {
	vers = v;
	hes = e;
	faces = f;
}
LoopSub::~LoopSub() {
	std::cout << "delete" << std::endl;
	vers.clear();
	hes.clear();
	faces.clear();
	upvers.clear();
	uphes.clear();
	upfaces.clear();

}
void LoopSub::Subdivision() { 
	vertex v;
	std::map<int, int> nvToe;//查看边是否对应了新增点,first:边，second:点
	int length = hes.size();


	//增加点，新增点在vers后，主边在nvToe中
	for (int i = 0; i < length; i++)
	{
		if (hes[i].operation == -1) {
			if (hes[i].opposite != -1) {
				int v0 = hes[i].start;
				int v1 = hes[i].end;
				int v3 = hes[hes[i].next].end;
				int v4 = hes[hes[hes[i].opposite].next].end;
				double a = 3.0 / 8.0 * (vers[v0].x + vers[v1].x) + 1.0 / 8.0 * (vers[v3].x + vers[v4].x);
				double b = 3.0 / 8.0 * (vers[v0].y + vers[v1].y) + 1.0 / 8.0 * (vers[v3].y + vers[v4].y);
				double c = 3.0 / 8.0 * (vers[v0].z + vers[v1].z) + 1.0 / 8.0 * (vers[v3].z + vers[v4].z);
				vertex temp(a, b, c);
				v = temp;
				int id = vers.size();
				vers.push_back(v);
				nvToe.insert(std::make_pair(i, id));
				hes[i].operation = 1;
				hes[hes[i].opposite].operation = 1;
				nvToe.insert(std::make_pair(hes[i].opposite, id));
			}
			else {
				int v0 = hes[i].start;
				int v1 = hes[i].end;
				double a = 1.0 / 2.0 * (vers[v0].x + vers[v1].x);
				double b = 1.0 / 2.0 * (vers[v0].y + vers[v1].y);
				double c = 1.0 / 2.0 * (vers[v0].z + vers[v1].z);
				vertex temp(a, b, c);
				v = temp;
				int id = vers.size();
				vers.push_back(v);
				nvToe.insert(std::make_pair(i, id));
				hes[i].operation = 1;
			}
		}

		//一个三角形完全加点
		if (hes[i].operation == 1 && hes[hes[i].next].operation == 1 && hes[hes[hes[i].next].next].operation == 1) {
			std::map<int, int> ::iterator it;
			int v1, v2, v3;
			it = nvToe.find(i);
			if (it != nvToe.end()) v1 = it->second;
			it = nvToe.find(hes[i].next);
			if (it != nvToe.end()) v2 = it->second;
			it = nvToe.find(hes[hes[i].next].next);
			if (it != nvToe.end()) v3 = it->second;

			int a1, a2, a3;
			a1 = hes[i].start;
			a2 = hes[i].end;
			a3 = hes[hes[i].next].end;

			face f1(v1, a2, v2);
			face f2(v3, v2, a3);
			face f3(a1, v1, v3);
			face f4(v2, v3, v1);

			upfaces.push_back(f1);
			upfaces.push_back(f2);
			upfaces.push_back(f3);
			upfaces.push_back(f4);

			hes[i].operation = 2;
			hes[hes[i].next].operation = 2;
			hes[hes[hes[i].next].next].operation = 2;
		}

	}
	initHalf(vers, uphes, upfaces);
	int num = vers.size();
	for (int i = 0; i < num; i++)
	{
		vertex v = vers[i];
		int n = v.neighbor.size();
		double beta = 1 / double(n)*(5.0 / 8.0 - ((3.0 / 8.0 + 1.0 / 4.0*cos(2 * PI / double(n)))*(3.0 / 8.0 + 1.0 / 4.0*cos(2 * PI / double(n)))));
		double a = 0.0, b = 0.0, c = 0.0;
		for (int j = 0; j < n; j++)
		{
			a += vers[v.neighbor[j]].x;
			b += vers[v.neighbor[j]].y;
			c += vers[v.neighbor[j]].z;
		}
		double a1 = (1 - double(n) * beta)*v.x + beta * a;
		double b1 = (1 - double(n) * beta)*v.y + beta * b;
		double c1 = (1 - double(n) * beta)*v.z + beta * c;

		vertex newV(a1, b1, c1);
		upvers.push_back(newV);
	}
	
}
#endif // !LOOP_SUBDIVISION_H_
