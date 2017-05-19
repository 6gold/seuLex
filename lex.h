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

/* ������ */
class Node
{
public:
	//���캯��
	Node()
	{
		stateLabel = 0;							//״̬��ų�ʼ��Ϊ0
		terminalTag = false;					//��ʼ��Ϊ���ս��	
	};

	//�������캯��
	Node(int state, bool tag)
	{
		stateLabel = state;
		terminalTag = tag;
	}

	//�����ս����
	void setTerminalTag(bool tag)
	{
		terminalTag = tag;
	}

	//��ȡ�ս����
	bool getTerminalTag()
	{
		return terminalTag;
	}

	//����stateLabel
	void setStateLabel(int state)
	{
		stateLabel = state;
	}

	//��ȡstateLabel
	int getStateLabel()
	{
		return stateLabel;
	}

	//���øý��ķ����ߺͺ���״̬
	void setOutStates(multimap<char, Node*> out)
	{
		next = out;
	}

	//��ȡ�ý��ķ����ߺͺ���״̬
	multimap<char, Node*> getOutStates()
	{
		return next;
	}

	//����ǰ�����ӷ����ߺͺ�̽��
	void addOutState(char ch, Node* node)
	{
		next.insert(make_pair(ch, node));
	}

	//private:
	int stateLabel;								//״̬���
	bool terminalTag;							//�ս���ǣ�true��ʾ���ս�㣩
	multimap<char, Node*> next;					//�ý��ķ����ߺͺ���״̬
};

/* ����NFA */
typedef struct NFA
{
	Node *start;							//��ʼ���
	vector<Node *> terminalNodes;			//�ս�㼯��
} NFA;

/* ����DFA */
typedef struct DFA
{
	DFA() {};
	DFA(Node* start);						//���캯��
	void getEclosure(set<Node *> & set);	//���DFA���Ħűհ�

	Node *start;							//��ʼ���
	vector<Node> dfaNodes;					//��DFA���н��
	vector<Node> dfaTerminalNodes;			//��DFA�����ս��	
	void printDFANodes()
	{
		cout << "node index\tnode label\tnode tag" << endl;
		for (int i = 0; i < dfaNodes.size(); i++)
		{
			cout << i << "\t" << dfaNodes.at(i).stateLabel << "\t" << dfaNodes.at(i).terminalTag << endl;
		}
	}
} DFA;

/***�ж�.lex�ļ��е�%%��%{��%}�ָ���***/
int checkSpec(char ch);

/***������ʽ��ɨ���滻, ���ѵ��к����û��Զ����ʶ���ĵط��滻������������ʽ***/
bool replaceRE(string & re);

/***re��׺���׺��������ӷ�'.'�ͽ�����'#'***/
string infix2suffix(string reBefore);

/***��ȡ����c�����ȼ�***/
int getPriority(char c);

/***����ĳre��NFA***/
void createNFA(string re);

/***����ʱNFAջ�е�����NFA�ϲ�Ϊһ�����NFA***/
void joinNFAs();

/***���dfa���ַ���***/
void getDFACharSet(vector<Node> nodeset);

/***DFA��С��***/
void minimizeDFA(vector<Node> dfanode);

/***��ȡDFA�ķ��ս�㼯�ϣ������뻮�ֶ�����***/
void getNonTerSet(vector<Node> dfanode, deque<set<int> > &mydeque, vector<set<int> > &test);

/***����̬�ڵ���зֽ�***/
void divideTerNodes(vector<Node>dfanode, deque<set<int> > &myqueue, vector<set<int> > &test);

/***???***/
void insertNode2Map(map<int, set<int> > &par1, int par2, int setid);

/***???***/
void insertNode2Set(set<int> &par1, int par2);

/***�ӵ�i��Ԫ���ҳ�state��Ӧ��id***/
int getSetId(int state, vector< set<int> > vecset, int n);

/***ɾ��set������***/
void removeSetFromVec(vector<set<int> > &testset, set<int> current);

/***�ж����������Ƿ����***/
bool isSetsEqual(set<int>par1, set<int>par2);

/***���ݻ��ֵ��Ӽ���dfanode ������С��dfa��startΪdfa��ʼ״̬���Ӽ�������***/
void createMinDFA(vector< set<int> >vecset, vector<Node> dfanode, int start);

/***�ж��Ӽ��Ƿ�����С�����ս��,������̬�����������̬��һ���Ӽ��й�ֻ��֪����һ���������Ϳ�***/
bool isSetATerofMinDFA(set<int> int_set, vector<Node> dfanode, int &state);

/**�������յ�DFA��������ε���Ӧ���룬д��.cpp�ļ�***/
void generateDFACode(vector<Node> dfanode);

/***������***/
void printError(int i);

/***������ʾ����***/
void hint(int i);

//������
void printNfaTerState2Action();
void printFT(deque< set<int> > flag, vector< set<int> > testset);
void printdfaTerState2Action();
#endif // !LEX.H