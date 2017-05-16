#pragma warning(disable:4786)
#include<iostream>
#include<ctype.h>
#include<stack>
#include<queue>
using namespace std;
#include"NFA.h"

/***���캯��***/
NFA::NFA()
{
	start = NULL;
	end = NULL;
	nodenum = 0;
}

/***�������캯��***/
NFA::NFA(const NFA& nfa)
{
	start = nfa.start;
	end = nfa.end;
	nodenum = nfa.nodenum;
	nfaNodes = nfa.nfaNodes;

}

/***��׺���׺***/
void NFA::change(string &r1, string &r2)
{
	/***��������ʽ��r1(��'#'��Ϊ�������)ת��Ϊ����r2��׺���ʽ***/
	stack<string> opStack;							//�����ݴ��������ջ
	opStack.push("#");								//����#��ѹ��ջ��
	int i, j;
	i = 0;											//ָʾr1���ַ�λ��
	j = 0;											//ָʾr2���ַ�λ��
	string ch;
	r2 = "";
	ch = r1[i];
	while (ch != "#")								//˳������׺���ʽ�е�ÿ���ַ�
	{
		if (ch == " ")
			ch = r1[++i];							//�Կո��ַ���������˳���ȡ��һ���ַ�
		else if (ch == "(")
		{
			opStack.push(ch);						//������ֱ��ѹ��ջ��
			ch = r1[++i];
		}
		else if (ch == ")")							//�������ţ�ʹ��������������γ�ջ����д��r2��
		{
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
			while (priority(w) >= priority(ch))		//��ջ��������ȼ����ڵ�ǰ������ȼ������������r2
			{
				r2.append(w);
				opStack.pop();
				w = opStack.top();
			}
			opStack.push(ch);						//����ǰ�����ѹ��ջ��
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
	while (ch != "#")								//��ջ�е�Ԫ��ȫ������
	{
		r2.append(opStack.top());
		opStack.pop();
		ch = opStack.top();
	}
	opStack.pop();
	r2.append("$");									//����r2
}

/***����ĳre��NFA***/
NFA* NFA::reToNFA(string & r2)
{
	stack<NFA> nfaStack;		//�����ݴ��м�����ɵ�NFAջ
	int i = 0;					//����ָʾɨ��r2�����ַ���λ��
	char ch = r2[i];			//ch����r2��ɨ�赽���ַ�
	while (ch != '$')
	{
		//˳�����׺���ʽ��ÿ���ַ�
		if (isletter(ch))
		{
			//������ĸ���е�Ԫ�أ������´���
			node *s1 = new node(++nodenum);				//����״̬��Ϊ++nodenum���½��
			node *s2 = new node(++nodenum);				//����״̬��Ϊ++nodenum���½��
			s1 -> acceptstatetag = 0;					//����s1Ϊ���ս��	
			s2->acceptstatetag = 1;						//����s2Ϊ�ս��
			(s1 -> firstEdge).symbol = ch;				//chΪ����ϵķ���
			(s1 -> firstEdge).nextnode = s2;			//s1ͨ��a�����ӵ�s2
			(s1 -> lastEdge).nextnode = '\0';			//s1������������
			(s2 -> firstEdge).nextnode = '\0';			//s2�޷�����
			(s2 -> lastEdge).nextnode = '\0';

			NFA *tempNFA = new NFA();
			tempNFA -> start = s1;
			tempNFA -> end = s2;
			tempNFA -> nodenum = nodenum;

			nfaStack.push(*tempNFA);					//�������ɵ�NFAѹ��nfaStackջ��

			ch = r2[++i];								//˳���ȡ��һ���ַ�
		}

		else if (ch == '.')
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
			R1 -> end -> firstEdge.symbol = '@';	//'@'���ű�ʾ��
			(R1 -> end) -> firstEdge.nextnode = R2 -> start;
			(R1 -> end -> lastEdge).symbol = '\0';
			R1 -> end -> acceptstatetag = 0;		//R1�Ľ���̬��Ϊ���ս��
			R1 -> end = R2 -> end;					//r1��end����Ϊr2��end���
			R1 -> nodenum = nodenum;				//���浱ǰ������Ŀ
			nfaStack.push(*R1);						//���ϲ����NFAѹ��ջ��
			delete R1;								//�ͷ��ڴ�
			delete R2;
			ch = r2[++i];
		}

		else if (ch == '|')
		{
			//����'|',�ȴ�ջ�е�������NFA������ϲ���Ȼ��ѹ��ջ��

			//��ջ�е�������NFA���ֱ����R1��R2��
			NFA *R2 = new NFA();
			*R2 = nfaStack.top();
			nfaStack.pop();

			NFA *R1 = new NFA();
			*R1 = nfaStack.top();
			nfaStack.pop();

			node *s3 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½�㣬Ϊ�µĳ�ʼ���
			node *s4 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½�㣬Ϊ�µ��ս��
			s3 -> acceptstatetag = 0;				//����s3Ϊ���ս��
			s4->acceptstatetag = 1;					//����s4Ϊ�ս��

			s3 -> firstEdge.symbol = '@';			//���ָ��R1�Ħű�
			s3 -> firstEdge.nextnode = R1 -> start;
			s3 -> lastEdge.symbol = '@';			//���ָ��R2�Ħű�
			s3 -> lastEdge.nextnode = R2 -> start;

			R1 -> end -> firstEdge.symbol = '@';
			R1 -> end -> firstEdge.nextnode = s4;
			R1 -> end -> lastEdge.nextnode = '\0';
			R1 -> end -> acceptstatetag = 0;		//��R1->endΪ���ս��
			R2->end->firstEdge.symbol = '@';
			R2->end->firstEdge.nextnode = s4;
			R2->end->lastEdge.nextnode = '\0';
			R2->end->acceptstatetag = 0;			//��R2->endΪ���ս��

			NFA *tempNFA = new NFA();				//�����µ�NFA
			tempNFA -> start = s3;
			tempNFA -> end = s4;
			tempNFA -> nodenum = nodenum;
			nfaStack.push(*tempNFA);

			delete tempNFA;							//�ͷ��ڴ�
			delete R1;
			delete R2;

			ch = r2[++i];
		}

		else if (ch == '*')
		{
			//����'*',�ȴ�ջ�е���1��NFA������Ȼ��ѹ��ջ��

			//��ջ�е���1��NFA������R1��
			NFA *R1 = new NFA();
			*R1 = nfaStack.top();
			nfaStack.pop();

			node *s3 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½�㣬Ϊ�µĳ�ʼ���
			node *s4 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½�㣬Ϊ�µĳ�ʼ���
			s3 -> acceptstatetag = 0;				//����s3Ϊ���ս��
			s4->acceptstatetag = 1;					//����s4Ϊ�ս��

			s3 -> firstEdge.symbol = '@';
			s3 -> firstEdge.nextnode = R1 -> start;
			s3 -> lastEdge.symbol = '@';
			s3 -> lastEdge.nextnode = s4;

			R1 -> end -> firstEdge.symbol = '@';
			R1 -> end -> firstEdge.nextnode = s4;
			R1 -> end -> lastEdge.symbol = '@';
			R1 -> end -> lastEdge.nextnode = R1 -> start;
			R1 -> end -> acceptstatetag = 0;
			R1 -> start = s3;
			R1 -> end = s4;
			R1 -> nodenum = nodenum;
			nfaStack.push(*R1);

			delete  R1;								//�ͷ��ڴ�

			ch = r2[++i];
		}
		else
			error();
	}

	NFA *nfa = new NFA();
	*nfa = nfaStack.top();
	nfaStack.pop();
	return nfa;
}

/***�ϲ�һ��NFA�����յĴ�NFA��***/
void NFA::join(NFA & nfa)
{
	if (this->start == NULL)
		this->start = nfa.start;
	else
	{
		node *s3 = new node(++nodenum);			//����״̬��Ϊ++nodenum���½��
		s3 -> acceptstatetag = 0;
		s3 -> firstEdge.symbol = '@';
		s3 -> firstEdge.nextnode = start;		//��һ����ָ����øú�����NFA�Ŀ�ʼ��
		s3 -> lastEdge.symbol = '@';
		s3 -> lastEdge.nextnode = nfa.start;	//�ڶ�����ָ��ڶ���NFA�Ŀ�ʼ��
		start = s3;								//���õ��øú�����NFA������ʼ���
	}
}

/***�ж�ĳ�ַ�����һ���ַ��Ƿ�Ϊ��ĸ�����ֻ�'+','-','/','='�����***/
bool NFA::isletter(string & c)
{
	const char*ptr = NULL;
	ptr = c.data();
	if (isletter(ptr[0]))
		return true;
	else
		return false;
}

/***�ж�ĳ�ַ��Ƿ�Ϊ��ĸ�����ֻ�'+','-','/','='�����***/
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

/***��ȡstring��Ӧ��char���鲢���ص�һ��char***/
char NFA::strToChar(string & str)
{
	const char *ptr = NULL;
	ptr = str.data();
	return ptr[0];
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
	char cur = 0;								//�浱ǰȡ�õ��ַ�
	char next = 0;								//��ǰ�ַ�����һ���ַ�

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
	char temp;
	while (cur != '#')							//��������������ʽ����re
	{
		temp = cur;
		re += temp;
		cur = ptr[++index];
	}
	re.append("#");
}

/***�������յ�NFA***/
void NFA::convrtAll(map<string, string> &reTable, map<string, string> &actionTable)
{
	map<string, string>::iterator iter;					//���������
	string infix;										//��������ʽ����׺���ʽ
	string suffix;										//��������ʽ�ĺ�׺���ʽ
	TerminalNode endNode;								//�ݴ���ֹ���
	NFA *tempNFA = NULL;								//�ݴ��������ʽת��������NFA
	this -> start = NULL;
	this -> end = NULL;
	string str;											//�ݴ��table���ȡ��string
	for (iter = actionTable.begin(); iter != actionTable.end(); iter++)
	{
		str = iter -> first;
//		cout<<iter->first<<" "<<iter->second<<endl;		//������
		if (isIn(reTable, str))							//���actionTable��ǰԪ����reTable�У����reTable��ȡ����Ӧ��������ʽ
			infix = reTable[str];
		else
			infix = str;								//����ȡ����ǰactionTable�ĵ�һ��
		this -> preProcess(infix);						//������ʽԤ������������ʽ�в������������'.'��
		this -> change(infix, suffix);					//����׺���ʽתΪ��׺���ʽ 

		tempNFA = reToNFA(suffix);						//����׺���ʽת��ΪNFA����tempNFA��
		endNode.id = (*tempNFA).end -> label;			//������ֹ�����
		endNode.type = iter -> first;					//������ֹ�������

		terminalNode.push_back(endNode);				//����ǰNFA���ս��ѹ���ս��ջ

		this -> join(*tempNFA);
	}

	getCharSet(reTable, actionTable);					//��������ַ���
}

/***�ж�actionTable��ǰԪ���Ƿ���reTable��***/
bool NFA::isIn(map<string, string> reTable, string str)
{
	map<string, string>::iterator iter;
	for (iter = reTable.begin(); iter != reTable.end(); iter++)
	{
		if (iter -> first == str)
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

/***������ȱ���NFA�Ľ�㲢�洢��DFA������***/
void NFA::preOrder(node *tree, DFA& dfa)
{
	int n = this -> nodenum + 1;//ȡ������
	int w = 0;
	node *neighbor = NULL;
	bool *visited = new bool[n];
	for (int i = 0; i < n; i++)
		visited[i] = false;
	int loc = tree -> label;
	this -> nfaNodes.push_back(tree);
	cout << "label : " << loc << "	";					//���Դ���
	visited[loc] = true;
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
				cout<<"label : "<< loc <<"	";			//���Դ��롣���ʸýڵ�
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
					cout<<"label : "<<loc << "	";		//���Դ��롣���ʸýڵ�
					que.push(neighbor);
					visited[w] = true;
				}
			}//�ڶ����߷��ʽ���
		}
	}
	delete[]visited;
}

/***NFA2DFA����NFA�Ľ�㼯����DFA��***/
void NFA::nfaTodfa(DFA & dfa)
{
	//��NFA�Ľ�㼯����DFA
	dfa.start = start -> label;				//����ǰNFA�Ŀ�ʼ�ڵ��Ÿ���DFA�Ŀ�ʼ�����
	node *ptr = start;						//ָ��ptrָ��ǰNFA�Ŀ�ʼ���
	nfaNodes.clear();						//��յ�ǰNFA��node��ָ��vector
	preOrder(ptr, dfa);						//������ȱ�������NFA�Ľ�����DFA��
//  cout << endl << endl;
	dfaNode *tempNode = new dfaNode();

//	cout<<"NFA �Ľ��δ����ǰ��"<< endl;

	//�����ǽ�nfa�Ľڵ��洢��dfa�Ľ���
	for (vector<node*>::iterator it = nfaNodes.begin(); it != nfaNodes.end(); it++)
	{
//		cout<<"label"<<'\t'<<(*it)->label<<endl;
		tempNode -> acceptstatetag = (*it) -> acceptstatetag;
		tempNode -> label = (*it) -> label;

		//cout<<"LABEL----"<<tempNode->label
		//<<"\tTER------"<<tempNode->acceptstatetag<<endl;

		//�洢��㷢����
		if (((*it)->firstEdge).nextnode != NULL)
		{
			dfaEdge edge;
			edge.symbol = ((*it)->firstEdge).symbol;
			edge.nextNode = ((*it)->firstEdge).nextnode -> label;
//			cout<<"��һ��㣺"<<edge.nextNode<<'\t';
			(tempNode->edges).push_back(edge);
			if (((*it)->lastEdge).nextnode != NULL)
			{
				edge.symbol = ((*it)->lastEdge).symbol;
				edge.nextNode = ((*it)->lastEdge).nextnode->label;
//				cout<<"�ڶ���㣺"<<edge.nextNode<<'\n';
				(tempNode->edges).push_back(edge);
			}
		}//�洢���߲��ֽ���
		dfa.dfaNodeTable.push_back(*tempNode);
		tempNode->edges.clear();
	}

	dfa.charSet = charSet;			//��NFA��ĸ����DFA
	dfa.sortNodes();				//����㰴id�����ºű꼴Ϊ���id��
	dfa.acceptedNode = this -> terminalNode;
	vector<TerminalNode>::iterator iter2;
	for (iter2 = terminalNode.begin(); iter2 != terminalNode.end(); iter2++)
	{
		dfa.terminal[iter2->id] = iter2->type;
	}
//	dfa.printNode();
//	dfa.epslonClosure(14);
	delete tempNode;
}

/***��ȡ�����ַ���***/
void NFA::getCharSet(map<string, string>reTable, map<string, string>reaction)
{
	map<string, string>::iterator iter;
	string temp;													//�ݴ��ַ���
	
	for (iter = reTable.begin(); iter != reTable.end(); iter++)
	{
//		cout << iter -> first << " " << iter -> second << endl;		//������
		temp = iter -> second;
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
//		cout<<iter->first<<" "<<iter->second<<endl;					//������
		temp = iter->first;
		char ch = 0;
		for (int i = 0; i<temp.length(); i++)
		{
			ch = temp[i];
			if (isletter(ch))
				charSet.insert(ch);
		}
	}

//	std::ostream_iterator<char> output(cout," ");					//������
//	cout<<"charSet contain "<<endl;									//������
//	std::copy(charSet.begin(),charSet.end(),output);				//������
}

/*
**���ʣ�
**(1)	getCharSet�����е����������ֱ���ʲô
**(2)	nfaTodfa�����Ĺ��ܵ�����ʲô
**/