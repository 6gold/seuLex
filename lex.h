#pragma warning(disable:4786)
#ifndef LEX_H
#define LEX_H
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <stdlib.h>
#include <malloc.h>
#include <queue>
using namespace std;

/* 定义结点 */
class Node
{
public:
	//构造函数
	Node()
	{
		stateLabel = 0;							//状态标号初始化为0
		terminalTag = false;					//初始化为非终结点	
	};

	//拷贝构造函数
	Node(int state, bool tag)
	{
		stateLabel = state;
		terminalTag = tag;
	}

	//设置终结点标记
	void setTerminalTag(bool tag)
	{
		terminalTag = tag;
	}

	//获取终结点标记
	bool getTerminalTag()
	{
		return terminalTag;
	}

	//设置stateLabel
	void setStateLabel(int state)
	{
		stateLabel = state;
	}

	//获取stateLabel
	int getStateLabel()
	{
		return stateLabel;
	}

	//设置该结点的发出边和后续状态
	void setOutStates(multimap<char, Node*> out)
	{
		next = out;
	}

	//获取该结点的发出边和后续状态
	multimap<char, Node*> getOutStates()
	{
		return next;
	}

	//给当前结点添加发出边和后继结点
	void addOutState(char ch, Node* node)
	{
		next.insert(make_pair(ch, node));
	}

	//private:
	int stateLabel;								//状态标号
	bool terminalTag;							//终结点标记（true表示是终结点）
	multimap<char, Node*> next;					//该结点的发出边和后续状态
};

/* 定义NFA */
typedef struct NFA
{
	Node *start;							//起始结点
	vector<Node *> terminalNodes;			//终结点集合
} NFA;

/* 定义DFA */
typedef struct DFA
{
	DFA() {};
	DFA(Node* start);						//构造函数
	void getEclosure(set<Node *> & set);	//求该DFA结点的ε闭包

	Node *start;							//起始结点
	vector<Node> dfaNodes;					//该DFA所有结点
	vector<Node> dfaTerminalNodes;			//该DFA所有终结点	
	void printDFANodes()
	{
		cout << "node index\tnode label\tnode tag" << endl;
		for (int i = 0; i < dfaNodes.size(); i++)
		{
			cout << i << "\t" << dfaNodes.at(i).stateLabel << "\t" << dfaNodes.at(i).terminalTag << endl;
		}
	}
} DFA;

/***判断.lex文件中的%%，%{，%}分隔符***/
int checkSpec(char ch);

/***正则表达式的扫描替换, 即把当中含有用户自定义标识符的地方替换成完整的正则式***/
bool replaceRE(string & re);

/***re中缀变后缀，添加连接符'.'和结束符'#'***/
string infix2suffix(string reBefore);

/***获取符号c的优先级***/
int getPriority(char c);

/***构造某re的NFA***/
void createNFA(string re);

/***将临时NFA栈中的所有NFA合并为一个大的NFA***/
void joinNFAs();

/***获得dfa的字符集***/
void getDFACharSet(vector<Node> nodeset);

/***DFA最小化***/
void minimizeDFA(vector<Node> dfanode);

/***获取DFA的非终结点集合，并加入划分队列中***/
void getNonTerSet(vector<Node> dfanode, deque<set<int> > &mydeque, vector<set<int> > &test);

/***对终态节点进行分解***/
void divideTerNodes(vector<Node>dfanode, deque<set<int> > &myqueue, vector<set<int> > &test);

/***???***/
void insertNode2Map(map<int, set<int> > &par1, int par2, int setid);

/***???***/
void insertNode2Set(set<int> &par1, int par2);

/***从第i个元素找出state对应的id***/
int getSetId(int state, vector< set<int> > vecset, int n);

/***删除set？？？***/
void removeSetFromVec(vector<set<int> > &testset, set<int> current);

/***判断两个集合是否相等***/
bool isSetsEqual(set<int>par1, set<int>par2);

/***根据划分的子集和dfanode 构造最小的dfa，start为dfa起始状态的子集索引号***/
void createMinDFA(vector< set<int> >vecset, vector<Node> dfanode, int start);

/***判断子集是否是最小化的终结点,由于终态不可能与非终态在一个子集中故只需知道第一个点的情况就可***/
bool isSetATerofMinDFA(set<int> int_set, vector<Node> dfanode, int &state);

/**根据最终的DFA产生规则段的相应代码，写入.cpp文件***/
void generateDFACode(vector<Node> dfanode);

/***报错函数***/
void printError(int i);

/***进程提示文字***/
void hint(int i);

//测试用
void printNfaTerState2Action();
void printFT(deque< set<int> > flag, vector< set<int> > testset);
void printdfaTerState2Action();
#endif // !LEX.H