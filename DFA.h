#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<set>
#include<deque>
#include<stack>
using namespace std;

/***********DFA�߶���***********/
struct dfaEdge
{
	char symbol;					//���ϱ��
	int nextNode;					//����״̬�����
	dfaEdge()						//���캯��
	{
		symbol = '\0';
		nextNode = 0;
	}
	dfaEdge(char ch)
	{
		symbol = ch;
		nextNode = 0;
	}
	dfaEdge(const dfaEdge & E)		//�������캯��
	{
		symbol = E.symbol;
		nextNode = E.nextNode;
	}
};

/***********DFA��㶨��***********/
struct dfaNode
{
	int label;									//״̬�������
	int acceptstatetag;							//�ս���ǣ�0Ϊ���ս�㣬1Ϊ�ս��
	vector<dfaEdge> edges;						//��ý��ķ�����
	map<char, int> hashedges;					//��õ�����з����ߵ�map
	
	//���Ĺ��캯��
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
	
	//���Ŀ������캯��,Ϊ��֧��vector����صĲ��뵯���Ȳ���
	dfaNode(const dfaNode & n)
	{
		label = n.label;
		acceptstatetag = n.acceptstatetag;
		edges = n.edges;
		hashedges = n.hashedges;
	}

	//�Ӹõ�����з�������Ѱ�ҵ�һ������ch�ı���ָ��ĺ����ڵ��״̬��
	int nextId(char ch)
	{
		map<char, int>::iterator iter;
		iter = hashedges.find(ch);
		if (iter != hashedges.end())
			return iter -> second;				//���ҵ��˴�ch�ıߣ��򷵻ض�Ӧ�ĺ����ڵ�״̬��
		else
			return -1;							//��û�ҵ��򷵻�-1
	}
};

/***********DFA�ս�㶨��***********/
struct TerminalNode
{
	int id;			//��ֹ���ID
	string type;	//��ֹ������ͣ�����int��id��num��
};

/***********DFA�ඨ��***********/
class DFA
{
public:
	DFA();
	void minimizeDfa();												//��С��DFA��״̬
	set<int> epslonClosure(int s);									//��״̬s��epslon�հ�
	set<int> epslonClosure(set<int> T);								//��״̬��T��epslon�հ�
	set<int> move(set<int> T, char a);								//�ܹ���T��ĳ״̬����ͨ�����Ϊa��ת�������״̬����
	void subsetCon();												//�����Ӽ����취����DFA
	void getCharSet();												//���DFA�������ַ���
	void printNodes();												//��ӡDFA���
	void sortNodes();
	bool isIn(set<int> s, int element);								//�ж�element�Ƿ��ڼ���s��
	bool isTerminate(set<int>, vector<int> &terSet, int &);			//�жϽ�㼯�Ƿ�����ֹ���
	bool getTerType(int num, string & str);							//����ֹ����Ϊnum����ֹ���͸���str
	friend ostream &operator<<(ostream&, const vector<dfaEdge>);
	void printNode();												//������

//private:
	set<char> charSet;												//DFA�������ַ���
	vector<TerminalNode>  acceptedNode;								//DFA��ֹ��㼯��
	vector<dfaNode>  dfaNodeTable;									//DFA�Ľ���
	int start;														//��ʼ���ı��
	map<int, string> terminal;
	map<int, int>  isAcc;											//����Ƿ�Ϊ��ֹ��㣨1Ϊ��ֹ��㣩
};