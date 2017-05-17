#pragma once
#include<string>
#include<map>
#include<vector>
#include<deque>
#include<list>
#include<iostream>
#include"DFA.h"

using namespace std;

/***********NFA�߶���***********/
struct Edge
{
	char symbol;				//���ϱ��
	struct node *nextnode;		//����״̬���
	//���캯��
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

	//�������캯��
	Edge(const Edge & E)
	{
		symbol = E.symbol;
		nextnode = E.nextnode;
	}
};

/***********NFA��㶨��***********/
struct node
{
	int label;					//״̬�������
	int acceptstatetag;			//�ս���ǣ�0Ϊ���ս�㣬1Ϊ�ս��
	Edge firstEdge, lastEdge;	//������ɭ�㷨֪��ÿ����������������
	
	//���Ŀ������캯��,Ϊ��֧��vector����صĲ��뵯���Ȳ���
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

/***********NFA�ඨ��***********/
class NFA
{
public:
	NFA();									//���캯��
	NFA(const NFA& nfa);					//�������캯��
	NFA * reToNFA(string& re);				//������������ʽreת��ΪNFA
	int priority(string c);					//��ĳ�ַ������ȼ�
	void error();							//������
	void change(string & r1, string &r2);	//����׺���ʽת��Ϊ��׺���ʽ
	bool isletter(char c);					//�ж�c�Ƿ�Ϊ��ĸ����Ԫ��
	bool isletter(string & c);
	void asign(NFA & nfa);					//��������ֵ��nfa
	NFA& operator= (const NFA& right);		//'='��������
	void preProcess(string & str);			//��������ʽԤ��������'.'���ӷ�
	void convrtAll(map<string, string> &reTable, map<string, string> &actionTable);	//�����ڱ�reTable�е�����������ʽװ��Ϊһ����NFA
	bool isIn(map<string, string> reTable, string str);								//�ж�ĳstr�Ƿ���reTable��
	void join(NFA & nfa);					//����ǰ��NFA��nfa�ϲ�
	char strToChar(string & str);			//��ȡĳ�ַ������ַ����鲢���ص�һ���ַ�
	void printTer();						//��ӡ�����ս��
	void preOrder(node *ptr, DFA& dfa);		//��NFA�Ľ�����������ȱ������洢��DFA������
	void nfaTodfa(DFA & dfa);				//��NFA�Ľ�㼯����DFA��
	void getCharSet(map<string, string>reTable, map<string, string>reaction);		//���NFA�������ַ���
private:
	//������ɭ�㷨�������NFAֻ��һ����ʼ״̬��һ������״̬
	node * start;							//ָ��NFA�Ŀ�ʼ���
	node * end;								//ָ��NFA����ֹ���
	int  nodenum;							//�������
	vector<node*> nfaNodes;					//�洢ָ��NFA�����н���ָ��
	vector<TerminalNode>  terminalNode;		//�洢��ӦDFA��������ֹ��㣿
	set<char> charSet;						//�洢��ӦDFA�������ַ�����
};