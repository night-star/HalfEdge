#pragma once
#ifndef COLLAPSE_H_
#define COLLAPSE_H_
#include <halfEdge.h>
#include <math.h>
#include <algorithm>


void equation2(double *x, double *y, double a1, double b1, double c1, double a2, double b2, double c2)
{
	*x = (c1 * b2 - c2 * b1) / (a1 * b2 - a2 * b1);
	*y = (c1 - a1 * *x) / b1;
}
void equation3(double *x, double *y, double *z, double a1, double b1, double c1, double d1, double a2, double b2, double c2, double d2, double a3, double b3, double c3, double d3)
{
	equation2(x, y, a1 * c2 - a2 * c1, b1 * c2 - b2 * c1, d1 * c2 - d2 * c1, a1 * c3 - a3 * c1, b1 * c3 - b3 * c1, d1 * c3 - d3 * c1);
	*z = (d1 - a1 * *x - b1 * *y) / c1;
}
void calCost(double *cost, double **Q, double x, double y, double z) {
	double cost1 = Q[0][0] * x + Q[1][0] * y + Q[2][0] * z + Q[3][0];
	double cost2 = Q[0][1] * x + Q[1][1] * y + Q[2][1] * z + Q[3][1];
	double cost3 = Q[0][2] * x + Q[1][2] * y + Q[2][2] * z + Q[3][2];
	double cost4 = Q[0][3] * x + Q[1][3] * y + Q[2][3] * z + Q[3][3];
	*cost = cost1 * x + cost2 * y + cost3 * z + cost4;

}
struct Massage
{
	double x, y, z;//更新后的坐标
	double cost;//更新的损失
	int s, e;//合并的两个顶点
	bool operator < ( const Massage &m2) {
		if (this->cost < m2.cost) return true;
		else return false;
	}
	bool operator <= (const Massage &m2) {
		if (this->cost <= m2.cost) return true;
		else return false;
	}
	bool operator > (const Massage &m2) {
		if (this->cost > m2.cost) return true;
		else return false;
	}
	bool operator >= (const Massage &m2) {
		if (this->cost >= m2.cost) return true;
		else return false;
	}
	bool operator == (const Massage &m2) {
		if (this->cost == m2.cost) return true;
		else return false;
	}
};
class Collapse
{
public:
	std::vector<vertex> vers;
	std::vector<half_edge> hes;
	std::vector<face> faces;
	std::vector<Massage> mass;

	Collapse(std::vector<vertex> &v, std::vector<half_edge>&e, std::vector<face> &f);
	~Collapse();
	void coll();
};

Collapse::Collapse(std::vector<vertex> &v, std::vector<half_edge>&e, std::vector<face> &f)
{
	vers = v;
	hes = e;
	faces = f;

}

Collapse::~Collapse()
{
	vers.clear();
	hes.clear();
	faces.clear();
}
void Collapse::coll() {
	int numOfver = vers.size();
	double ***Q = new double**[numOfver];
	for (int i = 0; i < numOfver; i++)
	{
		Q[i] = new double*[4];
		//定义Q
		for (int j = 0; j < 4; j++)
		{
			Q[i][j] = new double[4];
		}
		//初始化
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				Q[i][j][k] = 0;
			}
		}
		int numf = vers[i].faces.size();

		//计算平面方程，以及各点的初始Q
		for (int j = 0; j < numf; j++)
		{
			std::vector<int> temp = vers[i].faces;
			double x1 = vers[faces[temp[j]].v1].x;
			double y1 = vers[faces[temp[j]].v1].y;
			double z1 = vers[faces[temp[j]].v1].z;

			double x2 = vers[faces[temp[j]].v2].x;
			double y2 = vers[faces[temp[j]].v2].y;
			double z2 = vers[faces[temp[j]].v2].z;

			double x3 = vers[faces[temp[j]].v3].x;
			double y3 = vers[faces[temp[j]].v3].y;
			double z3 = vers[faces[temp[j]].v3].z;

			double a = y1 * z2 - y1 * z3 - y2 * z1 + y2 * z3 + y3 * z1 - y3 * z2;
			double b = -x1 * z2 + x1 * z3 + x2 * z1 - x2 * z3 - x3 * z1 + x3 * z2;
			double c = x1 * y2 - x1 * y3 - x2 * y1 + x2 * y3 + x3 * y1 - x3 * y2;
			double d = -x1 * y2*z3 + x1 * y3*z2 + x2 * y1*z3 - x2 * y3*z1 - x3 * y1*z2 + x3 * y2*z1;
			double leng = a * a + b * b + c * c + d * d;
			double leng1 = sqrt(leng);
			a = a / leng1;
			b = b / leng1;
			c = c / leng1;
			d = d / leng1;

			//得到了每个平面的abcd，下面计算每个平面的Q0
			Q[i][0][0] += a * a;
			Q[i][0][1] += a * b;
			Q[i][0][2] += a * c;
			Q[i][0][3] += a * d;

			Q[i][1][0] += a * b;
			Q[i][1][1] += b * b;
			Q[i][1][2] += b * c;
			Q[i][1][3] += b * d;

			Q[i][2][0] += a * c;
			Q[i][2][1] += b * c;
			Q[i][2][2] += c * c;
			Q[i][2][3] += c * d;

			Q[i][3][0] += a * d;
			Q[i][3][1] += b * d;
			Q[i][3][2] += c * d;
			Q[i][3][3] += d * d;
		}
	}

	double **Q0 = new double*[4];
	//定义Q
	for (int j = 0; j < 4; j++)
	{
		Q0[j] = new double[4];
	}
	//初始化
	for (int j = 0; j < 4; j++)
	{
		for (int k = 0; k < 4; k++)
		{
			Q0[j][k] = 0;
		}
	}
	int numOfhes = hes.size();
	for (int i = 0; i < numOfhes; i++)
	{
		if (hes[i].operation != 3) {
			for (int k = 0; k < 4; k++)
			{
				for (int p = 0; p < 4; p++)
				{
					Q0[k][p] = Q[hes[i].start][k][p] + Q[hes[i].end][k][p];
				}
			}
			double vx, vy, vz;
			equation3(&vx, &vy, &vz, Q0[0][0], Q0[0][1], Q0[0][2], -Q0[0][3], Q0[0][1], Q0[1][1], Q0[1][2], -Q0[1][3], Q0[0][2], Q0[1][2], Q0[2][2], -Q0[2][3]);
			double cost;
			int flag = 0;
			if (isnan(vx) || isnan(vy) || isnan(vz)) {
				//flag = 5;
				double cost1, cost2, cost3;
				double x1 = vers[hes[i].start].x, y1 = vers[hes[i].start].y, z1 = vers[hes[i].start].z;
				double x2 = vers[hes[i].end].x, y2 = vers[hes[i].end].y, z2 = vers[hes[i].end].z;
				calCost(&cost1, Q0,x1, y1, z1);
				calCost(&cost2, Q0, x2,y2, z2);
				calCost(&cost3, Q0, (x1 + x2) / 2, (y1 + y1) / 2, (z1 + z2) / 2);
				if (cost1 <= cost2 && cost1 <= cost3) {
					cost = cost1;
					vx = x1; vy = y1; vz = z1;
					flag = 1;
				}
				else if (cost2 <= cost1 && cost2 <= cost3) {
					cost = cost2;
					vx = x2; vy = y2; vz = z2;
					flag = 2;
				}
				else if (cost3 <= cost1 && cost3 <= cost2) {
					cost = cost3;
					vx = (x1 + x2) / 2;
					vy = (y1 + y2) / 2;
					vz = (z1 + z2) / 2;
					flag = 3;
				}
			}
			else {
				calCost(&cost, Q0, vx, vy, vz);
				flag = 4;
			}
			if (flag == 0) {
				cost = 1.7976931348623158e+308;
			}
			Massage m;
			m.cost = cost;
			m.s = hes[i].start;
			m.e = hes[i].end;
			m.x = vx;
			m.y = vy;
			m.z = vz;
			hes[i].operation = 3;
			hes[hes[i].opposite].operation = 3;
			mass.push_back(m);
		}
	}
	std::sort(mass.begin(), mass.end());
	int numOfedge = mass.size();
	int numOfcut = 0.05 * numOfedge + 1;//每次删除5%的边，不更新损失
	for (int j = 0; j < numOfcut; j++)
	{
		Massage m = mass[j];
		int start = m.s;
		int end = m.e;
		vertex newV;
		newV.x = m.x;
		newV.y = m.y;
		newV.z = m.z;
		int id = vers.size();
		vers.push_back(newV);
		std::vector<int> fs =  vers[start].faces;
		int n = fs.size();
		for (int k = 0; k < n; k++)
		{
			if (faces[fs[k]].v1 == start) {
				faces[fs[k]].v1 = id;
			}
			if (faces[fs[k]].v2 == start) {
				faces[fs[k]].v2 = id;
			}
			if (faces[fs[k]].v3 == start) {
				faces[fs[k]].v3 = id;
			}
		}

		fs = vers[end].faces;
		n = fs.size();
		for (int k = 0; k < n; k++)
		{
			if (faces[fs[k]].v1 == end) {
				faces[fs[k]].v1 = id;
			}
			if (faces[fs[k]].v2 == end) {
				faces[fs[k]].v2 = id;
			}
			if (faces[fs[k]].v3 == end) {
				faces[fs[k]].v3 = id;
			}
		}

	}
	for (std::vector<face>::iterator it = faces.begin(); it != faces.end();)//删除
	{
		if ((*it).v1== (*it).v2|| (*it).v1== (*it).v3|| (*it).v2== (*it).v3)
		{
			it = faces.erase(it);
		}
		else
			it++;
	}

	initHalf(vers, hes, faces);
}

#endif // !COLLAPSE_H_
