#pragma once
#include <unordered_map>
#include "vertex.h"
#include <iostream>
#include <fstream>
#include <stack>
using namespace std;
class distributionGraph {
public:
	void MiniDistanse();			 //求两景点间的最短路径
	void creatTourSortGraph();		 //输出导游线路图
	void TopoSort();				 //判断导游线路图有无回路
	void outputGraph();				 //输出图的邻接表
	void createGraph();				 //创建图的邻接表存储
	distributionGraph() {}			 //构造函数
	~distributionGraph();			 //析构函数
private:
	unordered_map<string ,vertex*>* vertexAdj; //名字->景点 hash表
	vector<edge*>* pEdgeVec;                   //所有边的集合，方便析构，暂时不用C++11
	vector<string>* vertexNames;               //按景点初始化顺序存储景点的名字
	list<vertex*>* tourGuide;                  //存储导游路线图
	vector<vector<unsigned>>* vertexMtx;       //存储邻接矩阵
	unsigned vertexNumber;                     //景点数量
	unsigned edgeNumber;                       //无向边数量，实际边*2
	void DFSTraverse(list<vertex*>&);		   //深度优先遍历
	bool isEdge(const string&, const string&); //判断两个顶点之间是否有直接相连的边
	vector<string>* toposort(
		vector<vector<unsigned>>*, vector<unsigned>*);
											   //求有无回路,拓扑排序改编
	void findLoopUsingDFS(vector<string>&);    //遍历导游路线图求回路
	void Adj2Mtx(vector<vector<unsigned>>&);   //邻接链表转邻接矩阵
	bool isGraphEmpty();					   //判断图是否空
	void updateVertexVisited();                //更新所有点的访问状态
	void shortedstPathDijkstra(unsigned,
		vector<unsigned>&, vector<unsigned>&); //迪杰斯特拉算法求最短路径
	void outPutShortestPath(unsigned,
		vector<unsigned>&, vector<unsigned>&); //输出所求的最短路径
};

void distributionGraph::MiniDistanse(){

	if (isGraphEmpty())
		return;

	cout << "请输入要查询的两个景点的名称：";
	string from, to;
	cin >> from >> to; //暂时默认输入正确
	if (from == to) {
		cout << from << " -> " << to << endl;
		cout << "最短距离：" << 0 << endl;
		return;
	}

	unsigned int f0 = vertexAdj->at(from)->getNumber();//起点编号
	unsigned int t1 = vertexAdj->at(to)->getNumber();  //终点编号
	vector<unsigned int> shortPathTable(vertexNumber+1); //表示V0到Vw各顶点的最短路径权值和
	vector<unsigned int> path(vertexNumber+1); //表示V0到Vw各顶点的前驱顶点下标值
	shortedstPathDijkstra(f0, path, shortPathTable);   //迪杰斯特拉算法寻找最短路径

	outPutShortestPath(t1, path, shortPathTable);      //输出两景点最短路径和最短距离
}

//输出导游线路图
void distributionGraph::creatTourSortGraph() {
	
	//判断图是否为空
	if (isGraphEmpty())
		return;

	tourGuide = new list<vertex*>();
	auto& rTourGuide = *tourGuide;
	//非递归栈辅助深度优先遍历,并把路线图简单地存储在tourGuide中
	DFSTraverse(rTourGuide);

	auto lvcbg = tourGuide->begin();
	auto lvced = tourGuide->end();
	//打印所有遍历路上经历的节点
	cout << (*lvcbg)->getName();
	while (++lvcbg != lvced)
		cout << " --> " << (*lvcbg)->getName();
	cout << endl;

	//更新所有点的访问状态。
	updateVertexVisited();
}

//深度优先遍历
void distributionGraph::DFSTraverse(list<vertex*>& rTourGuide) {

	stack<vertex*> stackVertex; //存储路径
	size_t         count = 1;   //已发现的景点数量

	//选择一个景点开始，并压入栈,添加进路线图
	vertex*        pCurrentVertex = (*vertexAdj)[(*vertexNames)[1]];
	pCurrentVertex->setVisited(true);
	stackVertex.push(pCurrentVertex);
	rTourGuide.push_back(pCurrentVertex);

	//栈中还有点说明还有些边没有尝试
	while (!stackVertex.empty()) {

		const list<edge*>* pAulEdgeAdj = stackVertex.top()->getEdgeAdj();//以该景点为出度的边
		auto			   ile = pAulEdgeAdj->begin();   //起始边iterator
		auto			   ileEnd = pAulEdgeAdj->end();

		//遍历所有可能经过的边
		while (ile != ileEnd) {
			//判断是否访问过该边入度景点
			//如果该景点没有被访问过，将该景点状态改变，并压入栈，填进路线图
			if (!(*ile)->getTo()->isVisited()) {
				pCurrentVertex = (*ile)->getTo();
				pCurrentVertex->setVisited(true);
				stackVertex.push(pCurrentVertex);
				rTourGuide.push_back(pCurrentVertex);
				
				if (++count >= vertexAdj->size()) //如果访问过所有的景点，退出遍历
					return;

				break;
			}
			else {			//若有访问过这个景点iterator++
				++ile;
			}
		}
		//如果是因为遍历无果退出上边的循环，pop辅助栈的景点
		if (ile == ileEnd) {
			stackVertex.pop();
			if (!stackVertex.empty()) //pop后栈顶的景点将会再次被访问
				rTourGuide.push_back(stackVertex.top());
		}
	}
}

//判断导游线路图有无回路
void distributionGraph::TopoSort() {

	//拓扑排序方法
	//auto size = vertexNames->size();
	////初始化导游图中的景点入度
	//vector<vector<unsigned int>>* map = new vector<vector<unsigned int>>(size, vector<int>(size, 0));
	//vector<unsigned int>*         indegree = new vector<unsigned int>(size, 0);
	//auto                 it = tourGuide->begin();

	//while(it != tourGuide->end()){
	//	auto iy = ++it;
	//	if (iy != tourGuide->end()) {
	//		int & power = (*map)[(*it)->getNumber()][(*iy)->getNumber()];
	//		if (!power) {
	//			power++;
	//			(*indegree)[(*it)->getNumber()]++;
	//		}
	//	}
	//}
	//内部逻辑,返回值为vector<string>*
	//auto result = toposort(map, indegree);
	//if (result->size() != size-1) {
	//	cout << "图中有回路" << endl;
	//}

	vector<string> result;
	findLoopUsingDFS(result);	//遍历导游图得到
	if (!result.empty()) {
		cout << "图中有回路:" << endl;
		for each (string var in result){
			cout << var;
		}
		cout << endl;
	}
	//delete map;
	//delete indegree;
}

vector<string>* distributionGraph::toposort(vector<vector<unsigned>>* map, vector<unsigned>* indegree) {
	unsigned i, j, k, size = indegree->size();
	vector<string>* result = new vector<string>(indegree->size()-1);
	for (i = 1;i<size;i++){ //遍历n次
		for (j = 1;j<size;j++){ //找出入度为0的节点
			if ((*indegree)[j] == 0){
				(*indegree)[j]--;
				result->push_back((*vertexNames)[j]);
				for (k = 1;k<size;k++) //删除与该节点关联的边
					if ((*map)[j][k] == 1)
						(*indegree)[k]--;
				break;
			}
		}
	}
	return result;
}

//使用深度优先方法进行拓扑排序
void distributionGraph::findLoopUsingDFS(vector<string>& loop){//loop存储回路
	//导游图不能是空的
	if (tourGuide->empty()) {
		return;
	}

	auto tourIt = tourGuide->begin();
	auto pCurrentVertex = *tourIt;
	pCurrentVertex->setVisited(true); //设置已经访问的标记
	while (true) {
		//完成导游图的遍历
		if (++tourIt == tourGuide->end())
			break;
		if ((*tourIt)->isVisited()) {
			//如果出现了回路，前探直到未访问节点
			loop.push_back(pCurrentVertex->getName());
			do {
				pCurrentVertex = *tourIt;
				loop.push_back(string("->"));
				loop.push_back(pCurrentVertex->getName());
			} while ((*++tourIt)->isVisited());
		}else {
			pCurrentVertex = *tourIt;
			pCurrentVertex->setVisited(true);
		}
	}
}

//判断是否存在f->t的边
inline bool distributionGraph::isEdge(const string& f, const string& t) {
	return vertexAdj->at(f)->isAccessDirect(vertexAdj->at(t));
}

//输出图的邻接表
void distributionGraph::outputGraph(){

	//判断图是否为空
	if (isGraphEmpty())
		return;
	if (vertexMtx == nullptr) {
		vertexMtx =	new vector<vector<unsigned>>
			(vertexNumber + 1, vector<unsigned>(vertexNumber + 1, 32767));
	}
	auto& rMatrix = *vertexMtx;
	Adj2Mtx(rMatrix);
	auto& rVertexNames = *vertexNames;
	auto size = vertexAdj->size();

	//遍历输出
	for each (string str in rVertexNames)
		printf("%8s", str.c_str());
	cout << endl;

	for(size_t j = 1; j <= size; ++j){
		printf("%8s", rVertexNames[j].c_str());
		for (size_t k = 1; k <= size; ++k)
			printf("%8d", rMatrix[j][k]);
		cout << endl;
	}
}

//创建图的邻接表存储
void distributionGraph::createGraph() {

	vertexNames  = new vector<string>();
	pEdgeVec     = new vector<edge*>();
	vertexAdj    = new unordered_map<string, vertex*>();
	auto& rVertexAdj = *vertexAdj;
	string name;
	unsigned i;
	//打开存有景区信息的文件
	ifstream in("info.txt");

	//读取节点数和边数,字符串转数字
	char c[128];
	memset(c, '\0', 128);
	in.getline(c, 128, ' ');
	vertexNumber = atoi(c);
	memset(c, '\0', 128);
	in.getline(c, 128);
	edgeNumber = atoi(c);
	cout << "请输入定点数和边数：" << vertexNumber << ' ' << edgeNumber << endl;

	//循环读取文件中的景点名字信息
	cout << "\n\t\t\"请输入各顶点的信息\"\n请输入各顶点的名字：";
	
	vertexNames->push_back(string(" "));
	
	for (i = 1; i < vertexNumber; ++i) {
		memset(c, '\0', 128);
		in.getline(c, 128, ' ');
		name = c;
		cout << name << ' ';
		rVertexAdj[name] = new vertex(name, i);
		vertexNames->push_back(name);
	}
	memset(c, '\0', 128);
	in.getline(c, 128);
	name = c;
	cout << name << ' ';
	rVertexAdj[name] = new vertex(name, i);
	vertexNames->push_back(name);
	cout << endl;
	
	//读取边信息
	for (i = 1; i <= edgeNumber; ++i) {
		string f, t;
		int power;
		cout << "请输入第" << i << "条边的两个顶点以及该边的权值：";
		
		//获得边的起点、终点、权值
		memset(c, '\0', 128);
		in.getline(c, 128, ' ');
		f = c;
		cout << f << ' ';
		memset(c, '\0', 128);
		in.getline(c, 128, ' ');
		t = c;
		cout << t << ' ';
		memset(c, '\0', 128);
		in.getline(c, 128);
		power = atoi(c);
		cout << power << endl;

		//为节点和边添加关联关系
		vertex* from = rVertexAdj[f];
		vertex* to = rVertexAdj[t];
		edge* e1 = new edge(from, to, power);
		edge* e2 = new edge(to, from, power);
		pEdgeVec->push_back(e1);
		pEdgeVec->push_back(e2);
		from->addEdge(e1);
		to->addEdge(e2);
	}
	in.close();
}

//迪杰斯特拉算法求最短路径
void distributionGraph::shortedstPathDijkstra( 
	unsigned v0, vector<unsigned>& path, vector<unsigned>& shortPathTable)
{
	if (vertexMtx == nullptr) { //如果邻接矩阵没有初始化
		vertexMtx = new vector<vector<unsigned>>
			(vertexNumber + 1, vector<unsigned>(vertexNumber + 1, 32767));
		Adj2Mtx(*vertexMtx);
	}
	auto& rMatrix = *vertexMtx;
	unsigned v, w, k, min, num = vertexNumber+1;
	vector<int> final(num); //final[k]=1表示求得v0到k点的最短权值了
	for (v = 1; v < num; ++v) {
		final[v] = 0;
		shortPathTable[v] = rMatrix[v0][v];
		path[v] =v0; //初始定义所有点的前驱都是v0，之后逐渐更新
	}
	final[v0] = 1;	//v0到v0不需要求路径
	path[v0] = 0;
	//开始主循环，每次求得v0到某个景点的最短路径
	for (v = 1; v < num; ++v) {
		min = 65535;
		for (w = 1; w < num; ++w) {//寻找离v0最近的景点
			if (!final[w] && shortPathTable[w] < min) {
				k = w;
				min = shortPathTable[w];
			}
		}
		final[k] = 1; //将目前找到的最近的景点置为1,代表找不到从v0到k点更近的路了
		for (w = 1; w <num;++w) {
			//如果离v0点最近的k点距离 + k点到w点距离 小于v0到w点的距离，更新并记录
			if (!final[w] && (min + rMatrix[k][w]) < shortPathTable[w]) {
				shortPathTable[w] = min + rMatrix[k][w];  //更新
				path[w] = k;                                //记录w点的前驱是k点
			}
		}
	}
}

//输出最短路径和最短距离
void distributionGraph::outPutShortestPath(unsigned t1,
	vector<unsigned>& path, vector<unsigned>& shortPathTable)
{
	vector<unsigned int> result;
	auto& rVertexNames = *vertexNames;
	unsigned int v = t1;
	do {
		result.push_back(v);
		v = path[v];
	} while (v != 0);

	v = result.size() - 1;
	while (v > 0) //起点终点不匹配
		cout << rVertexNames[result[v--]] << " -> " << rVertexNames[result[v]] << endl;

	cout << "最短距离：" << shortPathTable[t1] << endl;
}

//判断图是否空
inline bool distributionGraph::isGraphEmpty() {
	//判断若vertexAdj是空的，就退出重新进入控制选项
	if (!vertexAdj->empty())
		return false;

	cout << "错误!\n你还没有创建景区景点分布图，请输入 1 ！" << endl;
	return true;
}

//邻接链表转邻接矩阵
void distributionGraph::Adj2Mtx(vector<vector<unsigned>>& rMatrix){
	size_t         size = vertexAdj->size();
	
	//遍历所有的边，更新邻接矩阵的数据
	for each (edge* eg in *pEdgeVec){
		unsigned int from = eg->getFrom()->getNumber();
		unsigned int to   = eg->getTo()->getNumber();
		rMatrix[from][to] = eg->getPower();
		rMatrix[from][from] = 0;
	}
}

//更新所有点的访问状态
inline void distributionGraph::updateVertexVisited() {
	for each (auto& var in *vertexAdj)
		var.second->setVisited(false);
}

//析构函数
distributionGraph::~distributionGraph() {
	if (vertexAdj != nullptr) {
		for each (const pair<string, vertex*>& var in *vertexAdj)
			delete var.second;
		delete vertexAdj;
	}
	if (pEdgeVec != nullptr) {
		for each (const edge* var in *pEdgeVec)
			delete var;
		delete pEdgeVec;
	}
	if (tourGuide != nullptr) {
		delete tourGuide;
	}
	if (vertexNames != nullptr) {
		delete vertexNames;
	}
	if (vertexMtx != nullptr) {
		delete vertexMtx;
	}
}