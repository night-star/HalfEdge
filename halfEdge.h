#pragma once
#ifndef HALFEDGE_H_
#define HALFEDGE_H_
#include <iostream>
#include <vector>
#include <string>
#include <sstream> 
#include <fstream>
#include <cstring>
#include <algorithm> 
#include <map>

void Error() {
	std::cout << "NOT OFF!!!" << std::endl;
	system("pause");
	exit(0);
}
struct vertex
{
	double x, y, z;//坐标
	std::vector<int> neighbor;//该点相邻的点的id
	std::vector<int> faces;//包含该点的面的id
	int opretion;//是否被操作过
	vertex(double a, double b, double c) {
		x = a;
		y = b;
		z = c;
		opretion = 0;
	};
	vertex() { opretion = 0; };
};
struct half_edge
{
	int start, end;//起始，终止点的id
	int opposite, next;//对边以及下一条边的id
	int operation;//是否处理过，-1否 1已经增边 2该边所在的三角形处理完 3处理完缩减
	half_edge() {};
};
struct face
{
	int v1, v2, v3;//关联的三个顶点
	//int e1, e2, e3;//关联的三条边的id
	face() {};
	face(int a,int b,int c) {
		v1 = a;
		v2 = b;
		v3 = c;
	};
};

void insert(std::vector<vertex> &vs, int index,int ele) {
	if (std::find(vs[index].neighbor.begin(), vs[index].neighbor.end(), ele) == vs[index].neighbor.end()) {
		vs[index].neighbor.push_back(ele);
	}
}

void initHalf(std::vector<vertex> &vers, std::vector<half_edge> &hes, const std::vector<face> &faces) {
	int numOfFace = faces.size();
	int numOfVer = vers.size();
	for (int i = 0; i < numOfVer; i++)
	{
		std::vector<int> nbor;
		std::vector<int> fac;
		vers[i].neighbor = nbor;
		vers[i].faces = fac;
	}
	//std::pair <half_edge, std::pair<int, int>> edgeTovex;
	std::map<std::pair<int, int>, int> edgeTovex;
	for (int i = 0; i < numOfFace; i++)
	{
		half_edge halfedge1, halfedge2, halfedge3;
		halfedge1.start = faces[i].v1;
		halfedge1.end = faces[i].v2;
		halfedge2.start = faces[i].v2;
		halfedge2.end = faces[i].v3;
		halfedge3.start = faces[i].v3;
		halfedge3.end = faces[i].v1;

		insert(vers, faces[i].v1, faces[i].v2);
		insert(vers, faces[i].v1, faces[i].v3);
		insert(vers, faces[i].v2, faces[i].v1);
		insert(vers, faces[i].v2, faces[i].v3);
		insert(vers, faces[i].v3, faces[i].v1);
		insert(vers, faces[i].v3, faces[i].v2);


		vers[faces[i].v1].faces.push_back(i);
		vers[faces[i].v2].faces.push_back(i);
		vers[faces[i].v3].faces.push_back(i);

		int id1 = hes.size();
		int id2 = id1 + 1;
		int id3 = id2 + 1;
		halfedge1.next = id2;
		halfedge2.next = id3;
		halfedge3.next = id1;

		std::pair<int, int > p1 = std::make_pair(halfedge1.start, halfedge1.end);
		std::pair<std::pair<int, int>, int> ev1 = std::make_pair(p1, id1);
		edgeTovex.insert(ev1);

		std::pair<int, int > p2 = std::make_pair(halfedge2.start, halfedge2.end);
		std::pair<std::pair<int, int>, int> ev2 = std::make_pair(p2, id2);
		edgeTovex.insert(ev2);

		std::pair<int, int > p3 = std::make_pair(halfedge3.start, halfedge3.end);
		std::pair<std::pair<int, int>, int> ev3 = std::make_pair(p3, id3);
		edgeTovex.insert(ev3);


		halfedge1.opposite = -1;
		halfedge2.opposite = -1;
		halfedge3.opposite = -1;

		halfedge1.operation = -1;
		halfedge2.operation = -1;
		halfedge3.operation = -1;


		hes.push_back(halfedge1);
		hes.push_back(halfedge2);
		hes.push_back(halfedge3);

		//std::cout << hes[0].start << " " << hes[0].end << " " << hes[0].opposite << " "<<hes[0].next << std::endl;
	}
	int length = hes.size() - 1;
	//std::cout << length << std::endl;
	for (int i = 0; i < length; i++)
	{
		if (hes[i].opposite == -1) {
			std::map<std::pair<int, int>, int> ::iterator it;
			it = edgeTovex.find(std::make_pair(hes[i].end, hes[i].start));
			if (it != edgeTovex.end()) {
				int op = it->second;
				hes[i].opposite = op;
				hes[op].opposite = i;
			}
		}
	}
}


class halfEdge {
public:
	std::vector<vertex> vers;
	std::vector<half_edge> hes;
	std::vector<face> faces;
	halfEdge(std::string path);//根据输入的off构建半边结构
private:
	void readOff(std::string path);
};
halfEdge::halfEdge(std::string path) {
	readOff(path);
	initHalf(vers, hes, faces);
}
void halfEdge::readOff(std::string path) {
	std::ifstream fin(path, std::ios::in);
	char line[1024] = { 0 };
	std::string temp = "";
	std::string substr = path.substr(path.length() - 3, path.length());
	std::cout << substr << std::endl;
	if (strcmp(substr.c_str(), "off") != 0 && strcmp(substr.c_str(), "off") != 0)
		Error();
	fin.getline(line, sizeof(line));
	std::stringstream word(line);
	word >> temp;
	if (strcmp(temp.c_str(), "OFF") != 0)
		Error();
	fin.getline(line, sizeof(line));
	word = std::stringstream(line);

	word >> temp;
	int numOfDot = atoi(temp.c_str()) + 1;
	word >> temp;
	int numOfFace = atoi(temp.c_str()) + 1;

	//std::cout << "Dot: " << numOfDot << "Face: " << numOfFace << std::endl;

	vertex vertex;
	face face;
	while (--numOfDot)
	{
		fin.getline(line, sizeof(line));
		word = std::stringstream(line);

		word >> temp;
		vertex.x = atof(temp.c_str());
		word >> temp;
		vertex.y = atof(temp.c_str());
		word >> temp;
		vertex.z = atof(temp.c_str());
		vertex.opretion = 0;
		vers.push_back(vertex);
	}
	while (--numOfFace)
	{
		fin.getline(line, sizeof(line));
		word = std::stringstream(line);

		word >> temp;
		word >> temp;
		face.v1 = atoi(temp.c_str());
		word >> temp;
		face.v2 = atoi(temp.c_str());
		word >> temp;
		face.v3 = atoi(temp.c_str());

		faces.push_back(face);
	}
	fin.clear();
	fin.close();
}
#endif // !HALFEDGE_H_
