#pragma once
#include<string>
#include<map>
#include<vector>
#include<deque>
#include<list>
#include<iostream>
#include"DFA.h"

using namespace std;

/***********NFA边定义***********/
struct Edge
{
	char symbol;				//边上标记
	struct node *nextnode;		//后续状态结点
	//构造函数
	Edge()
	{
		symbol = '\0';
		nextnode = NULL;
	}

	Edge(char ch)
	{
		symbol = ch;
		nextnode = NULL;
	}

	//拷贝构造函数
	Edge(const Edge & E)
	{
		symbol = E.symbol;
		nextnode = E.nextnode;
	}
};

/***********NFA结点定义***********/
struct node
{
	int label;					//状态结点标记名
	int acceptstatetag;			//终结点标记，0为非终结点，1为终结点
	Edge firstEdge, lastEdge;	//由托马森算法知：每个结点最多有两条边
	
	//结点的拷贝构造函数,为了支持vector中相关的插入弹出等操作
	node(const node & n)		
	{
		label = n.label;
		acceptstatetag = n.acceptstatetag;
		firstEdge = n.firstEdge;
		lastEdge = n.lastEdge;
	}

	node(int lab)
	{
		label = lab;
		acceptstatetag = 0;
	}

	node()
	{
		label = 0;
		acceptstatetag = 0;
	}
};

/***********NFA类定义***********/
class NFA
{
public:
	NFA();									//构造函数
	NFA(const NFA& nfa);					//拷贝构造函数
	NFA * reToNFA(string& re);				//将单个正规表达式re转换为NFA
	int priority(string c);					//求某字符的优先级
	void error();							//报错函数
	void change(string & r1, string &r2);	//将中缀表达式转换为后缀表达式
	bool isletter(char c);					//判断c是否为字母表中元素
	bool isletter(string & c);
	void asign(NFA & nfa);					//将本对象赋值给nfa
	NFA& operator= (const NFA& right);		//'='符号重载
	void preProcess(string & str);			//将正规表达式预处理：插入'.'连接符
	void convrtAll(map<string, string> &reTable, map<string, string> &actionTable);	//将存在表reTable中的所有正规表达式装换为一个大NFA
	bool isIn(map<string, string> reTable, string str);								//判断某str是否在reTable中
	void join(NFA & nfa);					//将当前的NFA和nfa合并
	char strToChar(string & str);			//提取某字符串的字符数组并返回第一个字符
	void printTer();						//打印所有终结点
	void preOrder(node *ptr, DFA& dfa);		//对NFA的结点进行深度优先遍历并存储到DFA结点表中
	void nfaTodfa(DFA & dfa);				//将NFA的结点集赋给DFA？
	void getCharSet(map<string, string>reTable, map<string, string>reaction);		//获得NFA的输入字符集
private:
	//由托马森算法构造出的NFA只有一个开始状态和一个结束状态
	node * start;							//指向NFA的开始结点
	node * end;								//指向NFA的终止结点
	int  nodenum;							//结点总数
	vector<node*> nfaNodes;					//存储指向NFA的所有结点的指针
	vector<TerminalNode>  terminalNode;		//存储对应DFA的所有终止结点？
	set<char> charSet;						//存储对应DFA的输入字符集？
};