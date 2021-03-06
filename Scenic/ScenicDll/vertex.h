#pragma once
#include <string>
#include <list>
#include <memory>
#include "edge.h"
using namespace std;
class vertex {
public:
	vertex(string n, unsigned i, unsigned p = 0, string f = "", bool r = true, bool w = true)
		: name(n), number(i), visited(false), info(f),
		edgeAdj(make_shared<list<weak_ptr<edge>>>()),
		popularity(p), restArea(r), WC(w) {}

	void addEdge(const shared_ptr<edge> e) {
		edgeAdj->push_back(e);
	}

	const shared_ptr<list<weak_ptr<edge>>> getEdgeAdj() const {
		return edgeAdj;
	}

	unsigned int getNumber() const {
		return number;
	}

	bool isVisited() const {
		return visited;
	}

	void setVisited(bool bIV) {
		visited = bIV;
	}

	const string& getName() const {
		return name;
	}

	bool isAccessDirect(const shared_ptr<vertex> t) {
		for each (const auto eg in *edgeAdj)
			if (eg.lock()->getTo() == t) //一般认为eg.lock()不是空悬指针
				return true;
		return false;
	}

	const string& getInfo() {
		return info;
	}

	void setInfo(const char* i) {
		info = i;
	}

	void setPopularity(unsigned p) {
		popularity = p;
	}

	unsigned getPopularity() {
		return popularity;
	}

	void setRestArea(bool r) {
		restArea = r;
	}

	bool getRestArea() {
		return restArea;
	}

	void setWC(bool w) {
		WC = w;
	}

	bool getWC() {
		return WC;
	}
private:
	string name;         //景点名字
	unsigned number;     //景点编号
	bool visited;        //景点是否被访问过
	string info;		 //景区简介
	shared_ptr<list<weak_ptr<edge>>> edgeAdj;
	//从景点出发的边，使用weak_ptr的原因是
	//防止相互shared_ptr指向而无法释放内存。
	unsigned popularity; //景点欢迎度
	bool restArea;       //有无休息区
	bool WC;			 //有无厕所
};