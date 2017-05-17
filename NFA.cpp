#pragma warning(disable:4786)
#include<iostream>
#include<ctype.h>
#include<stack>
#include<queue>
#include"NFA.h"

using namespace std;

/***NFA构造函数***/
NFA::NFA()
{
	start = NULL;				//起始结点指针，初始化为空（由托马森算法构造出的NFA只有一个开始状态和一个结束状态）
	end = NULL;					//终结点指针，初始化为空
	nodenum = 0;				//结点总数，初始化为空
}

/***NFA拷贝构造函数***/
NFA::NFA(const NFA& nfa)
{
	start = nfa.start;			//拷贝开始结点指针
	end = nfa.end;				//拷贝开始结点指针
	nodenum = nfa.nodenum;		//拷贝结点数量
	nfaNodes = nfa.nfaNodes;	//拷贝所有节点指针
}

/***中缀变后缀***/
void NFA::change(string &r1, string &r2)
{
	//将正规表达式串r1(以'#'作为结束标记)转换为存于r2后缀表达式
	stack<string> opStack;			//用于暂存运算符的栈
	opStack.push("#");				//将‘#’压入栈底
	int i, j;
	i = 0;							//指示r1中字符位置
	j = 0;							//指示r2中字符位置
	string ch;						//暂存扫描到的字符
	r2 = "";						//串r2初始化为空
	ch = r1[i];						//取r1的第一个字符
	while (ch != "#")
	{
		//顺序处理中缀表达式中的每个字符
		if (ch == " ")
			ch = r1[++i];			//对空格字符不做处理，顺序读取下一个字符
		else if (ch == "(")
		{
			opStack.push(ch);		//左括号直接压入栈中
			ch = r1[++i];
		}
		else if (ch == ")")
		{
			//对右括号，使括号内运算符依次出栈，并写入r2中
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
				//若栈顶算符优先级大于当前算符优先级，弹出输出到r2
				r2.append(w);
				opStack.pop();
				w = opStack.top();
			}
			opStack.push(ch);		//将当前的算符压入栈中
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
	while (ch != "#")				//将栈中的元素全部弹出
	{
		r2.append(opStack.top());
		opStack.pop();
		ch = opStack.top();
	}
	opStack.pop();
	r2.append("$");					//结束r2的提取
}

/***构造某re的NFA***/
NFA* NFA::reToNFA(string & r2)
{
	stack<NFA> nfaStack;			//定义暂存中间的生成的NFA栈
	int i;							//用于指示扫描r2串的字符的位置
	i = 0;   
	string ch;						//暂存r2中扫描到的字符
	ch = r2[i];						//取r2的第一个字符
	while (ch != "$")
	{
		//顺序处理后缀表达式的每个字符
		if (isletter(ch))
		{
			//对于字母表中的元素，作如下处理
			node *s1 = new node(++nodenum);			//创建状态号为++nodenum的新结点
			node *s2 = new node(++nodenum);			//创建状态号为++nodenum的新结点
			s1->acceptstatetag = 0;					//设置s1为非终结点	
			(s1->firstEdge).symbol = strToChar(ch);	//设置边1上的符号
			(s1->firstEdge).nextnode = s2;			//边1指向结点s2
			(s1->lastEdge).nextnode = '\0';			//s1无其他发出边

			s2->acceptstatetag = 1;					//s2为终结点
			(s2->firstEdge).nextnode = '\0';		//s2无发出边
			(s2->lastEdge).nextnode = '\0';

			NFA *tempNFA = new NFA();
			tempNFA->start = s1;
			tempNFA->end = s2;
			tempNFA->nodenum = nodenum;
			nfaStack.push(*tempNFA);				//将新生成的临时NFA压入nfaStack栈中

			ch = r2[++i];							//顺序读取下一个字符，跳出本次while循环
		}

		else if (ch == ".")
		{
			//对于'.',先从栈中弹出两个NFA，将其合并，然后压入栈中

			//从栈中弹出两个NFA，分别存于R1和R2中
			NFA *R2 = new NFA();
			*R2 = nfaStack.top();
			nfaStack.pop();

			NFA *R1 = new NFA();
			*R1 = nfaStack.top();
			nfaStack.pop();

			//将R1和R2合并，合并后的NFA为R1
			R1->end->firstEdge.symbol = '@';					//'@'符号表示ε
			(R1->end)->firstEdge.nextnode = R2->start;			//原R1的终结点指向R2的起始结点
			(R1->end->lastEdge).symbol = '\0';					//原R1的终结点无其他发出边
			R1->end->acceptstatetag = 0;						//原R1的终结点设置为非终结点
			R1->end = R2->end;									//原R2的终结点作为新R1的终结点
			R1->nodenum = nodenum;								//保存当前结点的数目

			nfaStack.push(*R1);									//将合并后的NFA压入栈中
			delete R1;											//释放相应内存
			delete R2;
			ch = r2[++i];										//读取下一个字符，跳出本次while循环
		}

		else if (ch == "|")
		{
			//对于'|',先从栈中弹出两个NFA，将其合并，然后压入栈中

			//从栈中弹出两个NFA，分别存于R1和R2中
			NFA *R2 = new NFA();
			*R2 = nfaStack.top();
			nfaStack.pop();

			NFA *R1 = new NFA();
			*R1 = nfaStack.top();
			nfaStack.pop();

			node *s3 = new node(++nodenum);			//创建状态号为++nodenum的新结点，为新的起始结点
			node *s4 = new node(++nodenum);			//创建状态号为++nodenum的新结点，为新的终结点
			s3->acceptstatetag = 0;					//设置s3为非终结点
			s3->firstEdge.symbol = '@';				//添加指向R1的ε边
			s3->firstEdge.nextnode = R1->start;
			s3->lastEdge.symbol = '@';				//添加指向R2的ε边
			s3->lastEdge.nextnode = R2->start;

			s4->acceptstatetag = 1;					//设置s4为终结点

			R1->end->firstEdge.symbol = '@';		//原R1的终结点指向s4
			R1->end->firstEdge.nextnode = s4;
			R1->end->lastEdge.nextnode = '\0';		//原R1的终结点无其他发出边
			R1->end->acceptstatetag = 0;			//原R1的终结点设置为非终结点

			R2->end->firstEdge.symbol = '@';		//原R2的终结点指向s4
			R2->end->firstEdge.nextnode = s4;
			R2->end->lastEdge.nextnode = '\0';		//原R2的终结点无其他发出边
			R2->end->acceptstatetag = 0;			//原R2的终结点设置为非终结点

			NFA *tempNFA = new NFA();				//创建临时NFA
			tempNFA->start = s3;					//临时NFA的起始结点为s3
			tempNFA->end = s4;						//临时NFA的终结点为s4
			tempNFA->nodenum = nodenum;				//保存当前结点数
			nfaStack.push(*tempNFA);				//将临时NFA压入栈中

			delete tempNFA;							//释放相应内存
			delete R1;
			delete R2;

			ch = r2[++i];							//读取下一个字符，跳出本次while循环
		}

		else if (ch == "*")
		{
			//对于'*',先从栈中弹出1个NFA，进行处理，然后压入栈中

			//从栈中弹出1个NFA，存于R1中
			NFA *R1 = new NFA();
			*R1 = nfaStack.top();
			nfaStack.pop();

			node *s3 = new node(++nodenum);			//创建状态号为++nodenum的新结点，为新的初始结点
			node *s4 = new node(++nodenum);			//创建状态号为++nodenum的新结点，为新的初始结点
			s3->acceptstatetag = 0;					//设置s3为非终结点
			s3->firstEdge.symbol = '@';
			s3->firstEdge.nextnode = R1->start;

			s3->lastEdge.symbol = '@';
			s3->lastEdge.nextnode = s4;

			s4->acceptstatetag = 1;					//设置s4为终结点
			R1->end->firstEdge.symbol = '@';		//原R1第一条边指向s4
			R1->end->firstEdge.nextnode = s4;
			R1->end->lastEdge.symbol = '@';			//原R1第二条边指向自己原来的尾
			R1->end->lastEdge.nextnode = R1->start;
			R1->end->acceptstatetag = 0;			//原R1的终结点设置为非终结点
			R1->start = s3;							//设置新R1的起始结点为s3
			R1->end = s4;							//设置新R1的终结点为s4
			R1->nodenum = nodenum;					//保存当前的结点数
			nfaStack.push(*R1);						//将临时NFA压入栈中

			delete  R1;								//释放相应内存

			ch = r2[++i];							//读取下一个字符，跳出本次while循环
		}
		
		//若以上情况都不满足则报错
		else
			error();
	}

	//后缀表达式处理完毕，此时nfaStack顶部的NFA即为当前re的NFA
	NFA *nfa = new NFA();
	*nfa = nfaStack.top();
	nfaStack.pop();
	return nfa;
}

/***将一个NFA合并至调用它的NFA中***/
void NFA::join(NFA & nfa)
{
	if (this->start == NULL)
		this->start = nfa.start;
	else
	{
		node *s3 = new node(++nodenum);		//创建状态号为++nodenum的新结点
		s3->acceptstatetag = 0;
		s3->firstEdge.symbol = '@';			//第一条边指向调用该函数的NFA的起始结点
		s3->firstEdge.nextnode = start;
		s3->lastEdge.symbol = '@';			//第二条边指向第二个NFA的起始结点
		s3->lastEdge.nextnode = nfa.start;
		start = s3;							//设置s3为调用该函数的nfa的新结点
	}
}

/***判断某字符是否为字母、数字或'+','-','/','='运算符***/
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

/***获取string对应的* char数组并返回第一个* char***/
char NFA::strToChar(string & str)
{
	const char*ptr = NULL;
	ptr = str.data();
	return ptr[0];				//此处返回的是指针！
}

/***获取运算符的优先级***/
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

/***NFA之间的赋值***/
void NFA::asign(NFA &nfa)
{
	nfa.end = end;
	nfa.nfaNodes = nfaNodes;
	nfa.nodenum = nodenum;
	nfa.start = start;
}

/***报错函数【暂时为空】***/
void NFA::error()
{
}

/***'='的重载，实现NFA之间的赋值***/
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

/***对正规表达式预处理***/
void NFA::preProcess(string & re)
{
	re.append("#");
	int size = re.length();
	char *ptr = new char[2 * size];
	int j = 0;									//用于指示暂存队列的游标
	char cur = 0;								//暂存当前取得的字符
	char next = 0;								//暂存当前字符的下一个字符

	for (int i = 0; i<size - 1; i++)
	{
		cur = re[i];
		next = re[i + 1];
		ptr[j++] = cur;
		if (isletter(cur) && isletter(next))	//若当前和下一个字符都是字母、数字或运算符，则插入'.'
			ptr[j++] = '.';
		if (cur == ')' && next == '(')
			ptr[j++] = '.';
	}
	ptr[j] = next;
	re = "";									//清空当前正规表达式
	int index = 0;
	cur = ptr[index];
	string temp;
	while (cur != '#')							//将处理后的正规表达式赋给re
	{
		temp = cur;
		re += temp;
		cur = ptr[++index];
	}
	re.append("#");
}

/***产生最终的大NFA***/
void NFA::convrtAll(map<string, string> &reTable, map<string, string> &actionTable)
{
	map<string, string>::iterator iter;					//定义迭代器
	string infix;										//存正规表达式的中缀表达式
	string suffix;										//存正规表达式的后缀表达式
	TerminalNode endNode;								//暂存终止结点
	NFA *tempNFA = NULL;								//暂存从正规表达式转换而来的NFA
	this->start = NULL;									//当前NFA的初始结点指针置空
	this->end = NULL;									//当前NFA的终结点指针置空
	string str;											//暂存从actionTable里读取的string
	for (iter = actionTable.begin(); iter != actionTable.end(); iter++)
	{
		str = iter->first;
		//cout<<iter->first<<" "<<iter->second<<endl;
		if (isIn(reTable, str))							//如果actionTable当前元素在reTable中，从reTable中取出相应的正规表达式
			infix = reTable[str];
		else
			infix = str;								//否则，取出当前actionTable的第一项
		this->preProcess(infix);						//正规表达式预处理（在正规表达式中插入连接运算符'.'）
		this->change(infix, suffix);					//将中缀表达式转为后缀表达式 

		tempNFA = reToNFA(suffix);						//将后缀表达式转换为NFA存入tempNFA中
		endNode.id = (*tempNFA).end->label;				//保存临时NFA的终止结点编号
		endNode.type = iter->first;						//保存临时NFA的终止结点类型
		terminalNode.push_back(endNode);				//将该临时NFA的终结点压入终结点栈
		this->join(*tempNFA);							//将该临时NFA与当前NFA合并
	}
	getCharSet(reTable, actionTable);					//获得输入字符集
}

/***判断actionTable当前元素是否在reTable中***/
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

/***打印当前NFA的所有终结点***/
void NFA::printTer()
{
	vector<TerminalNode>::iterator iter;
	for (iter = terminalNode.begin(); iter != terminalNode.end(); iter++)
		cout << (*iter).id << '\t' << (*iter).type << endl;
}

/***深度优先遍历某NFA的结点并存储到调用该函数的nfaNode结点vector中***/
void NFA::preOrder(node *tree, DFA& dfa)
{
	//采用广度优先算法遍历NFA的结点，将其存储到DFA结点表中
	int n = this->nodenum + 1;							//取当前NFA的结点个数
	int w = 0;
	node * neighbor = NULL;
	bool * visited = new bool[n];						//记录某结点是否被遍历bool数组
	for (int i = 0; i<n; i++)							//初始化该数组
		visited[i] = false;

	int loc = tree->label;								//当前NFA起始结点的标号
	this->nfaNodes.push_back(tree);
	cout << "label : " << loc << endl;					//输出起始结点的标号
	visited[loc] = true;								//标记已访问

	stack<node*> que;
	que.push(tree);

	while (!que.empty())
	{
		node *top = que.top();
		que.pop();
		neighbor = (top->firstEdge).nextnode;

		if (neighbor != NULL)							//判断第一条边是否为空
		{
			w = neighbor->label;
			if (visited[w] == false)					//若未访问过
			{
				this->nfaNodes.push_back(neighbor);		//压结点地址入栈
				loc = neighbor->label;
				//cout<<"label : "<<loc<<endl;			//输出该结点标号，表示已访问过
				que.push(neighbor);
				visited[w] = true;
			}											//第一条边访问结束
			neighbor = (top->lastEdge).nextnode;
			if (neighbor != NULL)						//若第二条边不为空
			{
				w = neighbor->label;
				if (visited[w] == false)				//若未访问过
				{
					this->nfaNodes.push_back(neighbor);	//压结点地址入栈
					loc = neighbor->label;
					//cout<<"label : "<<loc<<endl;		//输出该结点标号，表示已访问过
					que.push(neighbor);
					visited[w] = true;
				}
			}											//第二条边访问结束
		}
	}
	delete[]visited;
}

/***NFA2DFA【将NFA的结点集赋给DFA】***/
void NFA::nfaTodfa(DFA & dfa)
{
	dfa.start = start->label;				//将当前NFA的开始结点标号赋给DFA的开始结点标号
	node* ptr = start;						//指针ptr指向当前NFA的开始结点
	nfaNodes.clear();						//清空当前NFA的node的指针vector
	preOrder(ptr, dfa);						//深度优先遍历，将NFA的结点放入DFA中
	dfaNode *tempNode = new dfaNode();		//新建一个DFA结点

	for (vector<node*>::iterator it = nfaNodes.begin(); it != nfaNodes.end(); it++)
	{
		tempNode->acceptstatetag = (*it)->acceptstatetag;
		tempNode->label = (*it)->label;

		//存储结点发出边
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

	dfa.charSet = charSet;					//将NFA字母表赋给DFA
	dfa.sortNodes();						//将结点按id排序，下号标即为结点id号
	dfa.acceptedNode = this->terminalNode;	//将当前NFA的终结点集合赋给DFA的终结点集合
	vector<TerminalNode>::iterator iter2;	
	for (iter2 = terminalNode.begin(); iter2 != terminalNode.end(); iter2++)
	{
		dfa.terminal[iter2->id] = iter2->type;
	}
	delete tempNode;
}

/***获取输入字符集***/
void NFA::getCharSet(map<string, string>reTable, map<string, string>reaction)
{
	map<string, string>::iterator iter;
	string temp;		//暂存字符串
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