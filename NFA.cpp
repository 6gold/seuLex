#pragma warning(disable:4786)
#include<iostream>
#include<ctype.h>
#include<stack>
#include<queue>
#include"NFA.h"

using namespace std;

/***NFA���캯��***/
NFA::NFA()
{
	start = NULL;				//��ʼ���ָ�룬��ʼ��Ϊ�գ�������ɭ�㷨�������NFAֻ��һ����ʼ״̬��һ������״̬��
	end = NULL;					//�ս��ָ�룬��ʼ��Ϊ��
	nodenum = 0;				//�����������ʼ��Ϊ��
}

/***NFA�������캯��***/
NFA::NFA(const NFA& nfa)
{
	start = nfa.start;			//������ʼ���ָ��
	end = nfa.end;				//������ʼ���ָ��
	nodenum = nfa.nodenum;		//�����������
	nfaNodes = nfa.nfaNodes;	//�������нڵ�ָ��
}

/***��׺���׺***/
void NFA::change(string &r1, string &r2)
{
	//��������ʽ��r1(��'#'��Ϊ�������)ת��Ϊ����r2��׺���ʽ
	stack<string> opStack;			//�����ݴ��������ջ
	opStack.push("#");				//����#��ѹ��ջ��
	int i, j;
	i = 0;							//ָʾr1���ַ�λ��
	j = 0;							//ָʾr2���ַ�λ��
	string ch;						//�ݴ�ɨ�赽���ַ�
	r2 = "";						//��r2��ʼ��Ϊ��
	ch = r1[i];						//ȡr1�ĵ�һ���ַ�
	while (ch != "#")
	{
		//˳������׺���ʽ�е�ÿ���ַ�
		if (ch == " ")
			ch = r1[++i];			//�Կո��ַ���������˳���ȡ��һ���ַ�
		else if (ch == "(")
		{
			opStack.push(ch);		//������ֱ��ѹ��ջ��
			ch = r1[++i];
		}
		else if (ch == ")")
		{
			//�������ţ�ʹ��������������γ�ջ����д��r2��
			while (opStack.top() != "(")
			{
				r2.append(opStack.top());
				opStack.pop();
			}
			opStack.pop();
			ch = r1[++i];
		}
		else if (ch == "." || ch == "|" || ch == "*")
		{
			string w = opStack.top();
			while (priority(w) >= priority(ch))
			{
				//��ջ��������ȼ����ڵ�ǰ������ȼ������������r2
				r2.append(w);
				opStack.pop();
				w = opStack.top();
			}
			opStack.push(ch);		//����ǰ�����ѹ��ջ��
			ch = r1[++i];
		}
		else
		{
			r2.append(ch);
			//j++;
			ch = r1[++i];
		}
	}
	ch = opStack.top();
	while (ch != "#")				//��ջ�е�Ԫ��ȫ������
	{
		r2.append(opStack.top());
		opStack.pop();
		ch = opStack.top();
	}
	opStack.pop();
	r2.append("$");					//����r2����ȡ
}

/***����ĳre��NFA***/
NFA* NFA::reToNFA(string & r2)
{
	stack<NFA> nfaStack;			//�����ݴ��м�����ɵ�NFAջ
	int i;							//����ָʾɨ��r2�����ַ���λ��
	i = 0;   
	string ch;						//�ݴ�r2��ɨ�赽���ַ�
	ch = r2[i];						//ȡr2�ĵ�һ���ַ�
	while (ch != "$")
	{
		//˳�����׺���ʽ��ÿ���ַ�
		if (isletter(ch))
		{
			//������ĸ���е�Ԫ�أ������´���
			node *s1 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½��
			node *s2 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½��
			s1->acceptstatetag = 0;					//����s1Ϊ���ս��	
			(s1->firstEdge).symbol = strToChar(ch);	//���ñ�1�ϵķ���
			(s1->firstEdge).nextnode = s2;			//��1ָ����s2
			(s1->lastEdge).nextnode = '\0';			//s1������������

			s2->acceptstatetag = 1;					//s2Ϊ�ս��
			(s2->firstEdge).nextnode = '\0';		//s2�޷�����
			(s2->lastEdge).nextnode = '\0';

			NFA *tempNFA = new NFA();
			tempNFA->start = s1;
			tempNFA->end = s2;
			tempNFA->nodenum = nodenum;
			nfaStack.push(*tempNFA);				//�������ɵ���ʱNFAѹ��nfaStackջ��

			ch = r2[++i];							//˳���ȡ��һ���ַ�����������whileѭ��
		}

		else if (ch == ".")
		{
			//����'.',�ȴ�ջ�е�������NFA������ϲ���Ȼ��ѹ��ջ��

			//��ջ�е�������NFA���ֱ����R1��R2��
			NFA *R2 = new NFA();
			*R2 = nfaStack.top();
			nfaStack.pop();

			NFA *R1 = new NFA();
			*R1 = nfaStack.top();
			nfaStack.pop();

			//��R1��R2�ϲ����ϲ����NFAΪR1
			R1->end->firstEdge.symbol = '@';					//'@'���ű�ʾ��
			(R1->end)->firstEdge.nextnode = R2->start;			//ԭR1���ս��ָ��R2����ʼ���
			(R1->end->lastEdge).symbol = '\0';					//ԭR1���ս��������������
			R1->end->acceptstatetag = 0;						//ԭR1���ս������Ϊ���ս��
			R1->end = R2->end;									//ԭR2���ս����Ϊ��R1���ս��
			R1->nodenum = nodenum;								//���浱ǰ������Ŀ

			nfaStack.push(*R1);									//���ϲ����NFAѹ��ջ��
			delete R1;											//�ͷ���Ӧ�ڴ�
			delete R2;
			ch = r2[++i];										//��ȡ��һ���ַ�����������whileѭ��
		}

		else if (ch == "|")
		{
			//����'|',�ȴ�ջ�е�������NFA������ϲ���Ȼ��ѹ��ջ��

			//��ջ�е�������NFA���ֱ����R1��R2��
			NFA *R2 = new NFA();
			*R2 = nfaStack.top();
			nfaStack.pop();

			NFA *R1 = new NFA();
			*R1 = nfaStack.top();
			nfaStack.pop();

			node *s3 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½�㣬Ϊ�µ���ʼ���
			node *s4 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½�㣬Ϊ�µ��ս��
			s3->acceptstatetag = 0;					//����s3Ϊ���ս��
			s3->firstEdge.symbol = '@';				//���ָ��R1�Ħű�
			s3->firstEdge.nextnode = R1->start;
			s3->lastEdge.symbol = '@';				//���ָ��R2�Ħű�
			s3->lastEdge.nextnode = R2->start;

			s4->acceptstatetag = 1;					//����s4Ϊ�ս��

			R1->end->firstEdge.symbol = '@';		//ԭR1���ս��ָ��s4
			R1->end->firstEdge.nextnode = s4;
			R1->end->lastEdge.nextnode = '\0';		//ԭR1���ս��������������
			R1->end->acceptstatetag = 0;			//ԭR1���ս������Ϊ���ս��

			R2->end->firstEdge.symbol = '@';		//ԭR2���ս��ָ��s4
			R2->end->firstEdge.nextnode = s4;
			R2->end->lastEdge.nextnode = '\0';		//ԭR2���ս��������������
			R2->end->acceptstatetag = 0;			//ԭR2���ս������Ϊ���ս��

			NFA *tempNFA = new NFA();				//������ʱNFA
			tempNFA->start = s3;					//��ʱNFA����ʼ���Ϊs3
			tempNFA->end = s4;						//��ʱNFA���ս��Ϊs4
			tempNFA->nodenum = nodenum;				//���浱ǰ�����
			nfaStack.push(*tempNFA);				//����ʱNFAѹ��ջ��

			delete tempNFA;							//�ͷ���Ӧ�ڴ�
			delete R1;
			delete R2;

			ch = r2[++i];							//��ȡ��һ���ַ�����������whileѭ��
		}

		else if (ch == "*")
		{
			//����'*',�ȴ�ջ�е���1��NFA�����д���Ȼ��ѹ��ջ��

			//��ջ�е���1��NFA������R1��
			NFA *R1 = new NFA();
			*R1 = nfaStack.top();
			nfaStack.pop();

			node *s3 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½�㣬Ϊ�µĳ�ʼ���
			node *s4 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½�㣬Ϊ�µĳ�ʼ���
			s3->acceptstatetag = 0;					//����s3Ϊ���ս��
			s3->firstEdge.symbol = '@';
			s3->firstEdge.nextnode = R1->start;

			s3->lastEdge.symbol = '@';
			s3->lastEdge.nextnode = s4;

			s4->acceptstatetag = 1;					//����s4Ϊ�ս��
			R1->end->firstEdge.symbol = '@';		//ԭR1��һ����ָ��s4
			R1->end->firstEdge.nextnode = s4;
			R1->end->lastEdge.symbol = '@';			//ԭR1�ڶ�����ָ���Լ�ԭ����β
			R1->end->lastEdge.nextnode = R1->start;
			R1->end->acceptstatetag = 0;			//ԭR1���ս������Ϊ���ս��
			R1->start = s3;							//������R1����ʼ���Ϊs3
			R1->end = s4;							//������R1���ս��Ϊs4
			R1->nodenum = nodenum;					//���浱ǰ�Ľ����
			nfaStack.push(*R1);						//����ʱNFAѹ��ջ��

			delete  R1;								//�ͷ���Ӧ�ڴ�

			ch = r2[++i];							//��ȡ��һ���ַ�����������whileѭ��
		}
		
		//������������������򱨴�
		else
			error();
	}

	//��׺���ʽ������ϣ���ʱnfaStack������NFA��Ϊ��ǰre��NFA
	NFA *nfa = new NFA();
	*nfa = nfaStack.top();
	nfaStack.pop();
	return nfa;
}

/***��һ��NFA�ϲ�����������NFA��***/
void NFA::join(NFA & nfa)
{
	if (this->start == NULL)
		this->start = nfa.start;
	else
	{
		node *s3 = new node(++nodenum);		//����״̬��Ϊ++nodenum���½��
		s3->acceptstatetag = 0;
		s3->firstEdge.symbol = '@';			//��һ����ָ����øú�����NFA����ʼ���
		s3->firstEdge.nextnode = start;
		s3->lastEdge.symbol = '@';			//�ڶ�����ָ��ڶ���NFA����ʼ���
		s3->lastEdge.nextnode = nfa.start;
		start = s3;							//����s3Ϊ���øú�����nfa���½��
	}
}

/***�ж�ĳ�ַ��Ƿ�Ϊ��ĸ�����ֻ�'+','-','/','='�����***/
bool NFA::isletter(string & c)
{
	const char*ptr = NULL;
	ptr = c.data();
	if (isletter(ptr[0]))
		return true;
	else
		return false;
}
bool NFA::isletter(char c)
{
	if (isalnum(c))
		return true;
	switch (c)
	{
	case '+':
		return true;
		break;
	case '-':
		return true;
		break;
	case '/':
		return true;
		break;
	case '=':
		return true;
		break;
	default:
		return false;
	}
}

/***��ȡstring��Ӧ��* char���鲢���ص�һ��* char***/
char NFA::strToChar(string & str)
{
	const char*ptr = NULL;
	ptr = str.data();
	return ptr[0];				//�˴����ص���ָ�룡
}

/***��ȡ����������ȼ�***/
int NFA::priority(string c)
{
	char ch = strToChar(c);
	switch (ch)
	{
	case '#':
		return 0;
	case '|':
		return 1;
	case '.':
		return 2;
	case '*':
		return 3;
	default:
		error();
		return -1;
	}
}

/***NFA֮��ĸ�ֵ***/
void NFA::asign(NFA &nfa)
{
	nfa.end = end;
	nfa.nfaNodes = nfaNodes;
	nfa.nodenum = nodenum;
	nfa.start = start;
}

/***����������ʱΪ�ա�***/
void NFA::error()
{
}

/***'='�����أ�ʵ��NFA֮��ĸ�ֵ***/
NFA& NFA::operator =(const NFA& right)
{
	if (&right != this)
	{
		end = right.end;
		nfaNodes = right.nfaNodes;
		nodenum = right.nodenum;
		start = right.start;
	}
	return *this;
}

/***��������ʽԤ����***/
void NFA::preProcess(string & re)
{
	re.append("#");
	int size = re.length();
	char *ptr = new char[2 * size];
	int j = 0;									//����ָʾ�ݴ���е��α�
	char cur = 0;								//�ݴ浱ǰȡ�õ��ַ�
	char next = 0;								//�ݴ浱ǰ�ַ�����һ���ַ�

	for (int i = 0; i<size - 1; i++)
	{
		cur = re[i];
		next = re[i + 1];
		ptr[j++] = cur;
		if (isletter(cur) && isletter(next))	//����ǰ����һ���ַ�������ĸ�����ֻ�������������'.'
			ptr[j++] = '.';
		if (cur == ')' && next == '(')
			ptr[j++] = '.';
	}
	ptr[j] = next;
	re = "";									//��յ�ǰ������ʽ
	int index = 0;
	cur = ptr[index];
	string temp;
	while (cur != '#')							//��������������ʽ����re
	{
		temp = cur;
		re += temp;
		cur = ptr[++index];
	}
	re.append("#");
}

/***�������յĴ�NFA***/
void NFA::convrtAll(map<string, string> &reTable, map<string, string> &actionTable)
{
	map<string, string>::iterator iter;					//���������
	string infix;										//��������ʽ����׺���ʽ
	string suffix;										//��������ʽ�ĺ�׺���ʽ
	TerminalNode endNode;								//�ݴ���ֹ���
	NFA *tempNFA = NULL;								//�ݴ��������ʽת��������NFA
	this->start = NULL;									//��ǰNFA�ĳ�ʼ���ָ���ÿ�
	this->end = NULL;									//��ǰNFA���ս��ָ���ÿ�
	string str;											//�ݴ��actionTable���ȡ��string
	for (iter = actionTable.begin(); iter != actionTable.end(); iter++)
	{
		str = iter->first;
		//cout<<iter->first<<" "<<iter->second<<endl;
		if (isIn(reTable, str))							//���actionTable��ǰԪ����reTable�У���reTable��ȡ����Ӧ��������ʽ
			infix = reTable[str];
		else
			infix = str;								//����ȡ����ǰactionTable�ĵ�һ��
		this->preProcess(infix);						//������ʽԤ������������ʽ�в������������'.'��
		this->change(infix, suffix);					//����׺���ʽתΪ��׺���ʽ 

		tempNFA = reToNFA(suffix);						//����׺���ʽת��ΪNFA����tempNFA��
		endNode.id = (*tempNFA).end->label;				//������ʱNFA����ֹ�����
		endNode.type = iter->first;						//������ʱNFA����ֹ�������
		terminalNode.push_back(endNode);				//������ʱNFA���ս��ѹ���ս��ջ
		this->join(*tempNFA);							//������ʱNFA�뵱ǰNFA�ϲ�
	}
	getCharSet(reTable, actionTable);					//��������ַ���
}

/***�ж�actionTable��ǰԪ���Ƿ���reTable��***/
bool NFA::isIn(map<string, string> reTable, string str)
{
	map<string, string>::iterator iter;
	for (iter = reTable.begin(); iter != reTable.end(); iter++)
	{
		if (iter->first == str)
			return true;
	}
	return false;
}

/***��ӡ��ǰNFA�������ս��***/
void NFA::printTer()
{
	vector<TerminalNode>::iterator iter;
	for (iter = terminalNode.begin(); iter != terminalNode.end(); iter++)
		cout << (*iter).id << '\t' << (*iter).type << endl;
}

/***������ȱ���ĳNFA�Ľ�㲢�洢�����øú�����nfaNode���vector��***/
void NFA::preOrder(node *tree, DFA& dfa)
{
	//���ù�������㷨����NFA�Ľ�㣬����洢��DFA������
	int n = this->nodenum + 1;							//ȡ��ǰNFA�Ľ�����
	int w = 0;
	node * neighbor = NULL;
	bool * visited = new bool[n];						//��¼ĳ����Ƿ񱻱���bool����
	for (int i = 0; i<n; i++)							//��ʼ��������
		visited[i] = false;

	int loc = tree->label;								//��ǰNFA��ʼ���ı��
	this->nfaNodes.push_back(tree);
	cout << "label : " << loc << endl;					//�����ʼ���ı��
	visited[loc] = true;								//����ѷ���

	stack<node*> que;
	que.push(tree);

	while (!que.empty())
	{
		node *top = que.top();
		que.pop();
		neighbor = (top->firstEdge).nextnode;

		if (neighbor != NULL)							//�жϵ�һ�����Ƿ�Ϊ��
		{
			w = neighbor->label;
			if (visited[w] == false)					//��δ���ʹ�
			{
				this->nfaNodes.push_back(neighbor);		//ѹ����ַ��ջ
				loc = neighbor->label;
				//cout<<"label : "<<loc<<endl;			//����ý���ţ���ʾ�ѷ��ʹ�
				que.push(neighbor);
				visited[w] = true;
			}											//��һ���߷��ʽ���
			neighbor = (top->lastEdge).nextnode;
			if (neighbor != NULL)						//���ڶ����߲�Ϊ��
			{
				w = neighbor->label;
				if (visited[w] == false)				//��δ���ʹ�
				{
					this->nfaNodes.push_back(neighbor);	//ѹ����ַ��ջ
					loc = neighbor->label;
					//cout<<"label : "<<loc<<endl;		//����ý���ţ���ʾ�ѷ��ʹ�
					que.push(neighbor);
					visited[w] = true;
				}
			}											//�ڶ����߷��ʽ���
		}
	}
	delete[]visited;
}

/***NFA2DFA����NFA�Ľ�㼯����DFA��***/
void NFA::nfaTodfa(DFA & dfa)
{
	dfa.start = start->label;				//����ǰNFA�Ŀ�ʼ����Ÿ���DFA�Ŀ�ʼ�����
	node* ptr = start;						//ָ��ptrָ��ǰNFA�Ŀ�ʼ���
	nfaNodes.clear();						//��յ�ǰNFA��node��ָ��vector
	preOrder(ptr, dfa);						//������ȱ�������NFA�Ľ�����DFA��
	dfaNode *tempNode = new dfaNode();		//�½�һ��DFA���

	for (vector<node*>::iterator it = nfaNodes.begin(); it != nfaNodes.end(); it++)
	{
		tempNode->acceptstatetag = (*it)->acceptstatetag;
		tempNode->label = (*it)->label;

		//�洢��㷢����
		if (((*it)->firstEdge).nextnode != NULL)
		{
			dfaEdge edge;
			edge.symbol = ((*it)->firstEdge).symbol;
			edge.nextNode = ((*it)->firstEdge).nextnode->label;
			(tempNode->edges).push_back(edge);

			if (((*it)->lastEdge).nextnode != NULL)
			{
				edge.symbol = ((*it)->lastEdge).symbol;
				edge.nextNode = ((*it)->lastEdge).nextnode->label;
				(tempNode->edges).push_back(edge);
			}
		}
		dfa.dfaNodeTable.push_back(*tempNode);
		tempNode->edges.clear();
	}

	dfa.charSet = charSet;					//��NFA��ĸ����DFA
	dfa.sortNodes();						//����㰴id�����ºű꼴Ϊ���id��
	dfa.acceptedNode = this->terminalNode;	//����ǰNFA���ս�㼯�ϸ���DFA���ս�㼯��
	vector<TerminalNode>::iterator iter2;	
	for (iter2 = terminalNode.begin(); iter2 != terminalNode.end(); iter2++)
	{
		dfa.terminal[iter2->id] = iter2->type;
	}
	delete tempNode;
}

/***��ȡ�����ַ���***/
void NFA::getCharSet(map<string, string>reTable, map<string, string>reaction)
{
	map<string, string>::iterator iter;
	string temp;		//�ݴ��ַ���
	for (iter = reTable.begin(); iter != reTable.end(); iter++)
	{
		//cout<<iter->first<<" "<<iter->second<<endl;
		temp = iter->second;
		char ch = 0;
		for (int i = 0; i<temp.length(); i++)
		{
			ch = temp[i];
			if (isletter(ch))
				charSet.insert(ch);
		}
	}
	for (iter = reaction.begin(); iter != reaction.end(); iter++)
	{
		//cout<<iter->first<<" "<<iter->second<<endl;
		temp = iter->first;
		char ch = 0;
		for (int i = 0; i<temp.length(); i++)
		{
			ch = temp[i];
			if (isletter(ch))
				charSet.insert(ch);
		}
	}
	//std::ostream_iterator<char> output(cout," ");
	//cout<<"charSet contain "<<endl;
	//std::copy(charSet.begin(),charSet.end(),output);
}

/*
void NFA::preOrder(node* tree)
{
node *left=NULL;
node *right=NULL;
if(tree!=NULL)
{
nfaNodes.push_back(*tree);
left=(tree->firstEdge).nextnode;
right=(tree->lastEdge).nextnode;
if(left!=NULL)
preOrder(left);
if(right!=NULL)
preOrder(right);
}
return ;
}*/