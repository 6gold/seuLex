#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<set>
#include<deque>
#include<stack>
using namespace std;

/***********DFA边定义***********/
struct dfaEdge
{
	char symbol;					//边上标记
	int nextNode;					//后续状态结点标号
	dfaEdge()						//构造函数
	{
		symbol = '\0';
		nextNode = 0;
	}
	dfaEdge(char ch)
	{
		symbol = ch;
		nextNode = 0;
	}
	dfaEdge(const dfaEdge & E)		//拷贝构造函数
	{
		symbol = E.symbol;
		nextNode = E.nextNode;
	}
};

/***********DFA结点定义***********/
struct dfaNode
{
	int label;									//状态结点标记名
	int acceptstatetag;							//终结点标记，0为非终结点，1为终结点
	vector<dfaEdge> edges;						//存该结点的发出边
	map<char, int> hashedges;					//存该点的所有发出边的map
	
	//结点的构造函数
	dfaNode()
	{
		label = 0;
		acceptstatetag = 0;
	}

	dfaNode(int label)
	{
		label = label;
		acceptstatetag = 0;
	}
	
	//结点的拷贝构造函数,为了支持vector中相关的插入弹出等操作
	dfaNode(const dfaNode & n)
	{
		label = n.label;
		acceptstatetag = n.acceptstatetag;
		edges = n.edges;
		hashedges = n.hashedges;
	}

	//从该点的所有发出边中寻找第一个出现ch的边所指向的后续节点的状态号
	int nextId(char ch)
	{
		map<char, int>::iterator iter;
		iter = hashedges.find(ch);
		if (iter != hashedges.end())
			return iter -> second;				//若找到了带ch的边，则返回对应的后续节点状态号
		else
			return -1;							//若没找到则返回-1
	}
};

/***********DFA终结点定义***********/
struct TerminalNode
{
	int id;			//终止结点ID
	string type;	//终止结点类型，例如int，id，num等
};

/***********DFA类定义***********/
class DFA
{
public:
	DFA();
	void minimizeDfa();												//最小化DFA的状态
	set<int> epslonClosure(int s);									//求状态s的epslon闭包
	set<int> epslonClosure(set<int> T);								//求状态集T的epslon闭包
	set<int> move(set<int> T, char a);								//能够从T中某状态出发通过标号为a的转换到达的状态集合
	void subsetCon();												//采用子集构造法生成DFA
	void getCharSet();												//获得DFA的输入字符集
	void printNodes();												//打印DFA结点
	void sortNodes();
	bool isIn(set<int> s, int element);								//判断element是否在集合s中
	bool isTerminate(set<int>, vector<int> &terSet, int &);			//判断结点集是否含有终止结点
	bool getTerType(int num, string & str);							//将终止结点号为num的终止类型赋给str
	friend ostream &operator<<(ostream&, const vector<dfaEdge>);
	void printNode();												//测试用

//private:
	set<char> charSet;												//DFA的输入字符集
	vector<TerminalNode>  acceptedNode;								//DFA终止结点集合
	vector<dfaNode>  dfaNodeTable;									//DFA的结点表
	int start;														//开始结点的标号
	map<int, string> terminal;
	map<int, int>  isAcc;											//结点是否为终止结点（1为终止结点）
};