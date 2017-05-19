#pragma warning(disable:4786)
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
#include <lex.h>
#define LAYER_ID		15		//标记%%
#define HEADER_BEGIN	16		//标记%{
#define HEADER_END		17		//标记%}
#define BEGIN			0
#define MYERROR			-11
using namespace std;

ifstream infile;
ofstream outfile;
int linenum;								//标记.lex文件的行号
int returnState;   							//NFA终态标号
int stateIndex = 0;  						//下一个可用的表示状态标号
map<string, string> idreTable;				//id-re映射表，由.lex文件中扫描得到
map<int, string> nfaTerState2Action;  		//NFA终态-规约action映射表
map<int, string> dfaTerState2Action;		//DFA终态-规约action映射表
map<int, string> mindfaTerState2Action;		//最小化DFA的终态-action映射表
vector<NFA> nfaTable;        				//暂存各正规表达式对应的NFA
vector<Node> mindfaNodes;					//最小化DFA的结点集合
set<char> dfaCharSet;    					//DFA中的字符集
NFA finalNFA;								//合并后的大NFA

/***主函数***/
void main()
{
	linenum = 0;

	/* 打开要写入的.cpp文件 */
	outfile.open("yylex.cpp", ios::out);

	/* 打开要分析的.l文件 */

	string filename = "mylex.l";
	infile.open(filename.c_str(), ios::in);
	if (!infile.good())				//文件读取失败
	{
		printError(-1);				//报错，打开文件失败
		return;
	}
	cout << "Open file succeed! Start analysing ........" << endl;

	/* 开始分析 */
	char c = infile.get();			//从输入流读取一个字符
	int state = checkSpec(c);

	//将.lex文件的 %{ ... %} 部分写入.cpp文件
	if (state != HEADER_BEGIN)		//判断lex 程序的开始部分
	{
		printError(-2);				//报错，文件格式有误
		return;
	}
	while (!infile.eof() && state != HEADER_END)		//文件未读取至底部
	{
		c = infile.get();
//		if (c == '\t' || c == ' ')
//			continue;
		if (c == '%')
		{
			state = checkSpec(c);
			continue;
		}
		if (c == '\n')
		{
			linenum++;
		}
		outfile.put(c);
	}

	//扫描.lex文件的 id-re 部分，存入id-re表
//	cout << "Start reading id-re segment!" << endl;		//测试
	infile.get();										//读入换行符
	pair<string, string> idrePair;
	state = BEGIN;
	while (!infile.eof() && state != LAYER_ID)			//文件未读取至底部
	{
		c = infile.get();
//		cout << "读入：" << c << endl;					//测试
		if (c == '%')
		{
			state = checkSpec(c);
			if (state == MYERROR)
			{
				printError(linenum);
				return;
			}
			continue;
		}
		else {
			infile.seekg(-1, ios::cur);					//对输入文件定位
		}
		string id, re;
		infile >> id >> re;
//		cout << "id: " << id << "	re:" << re << endl;	//测试
		idrePair.first = id;
		replaceRE(re);									//用户自定义re的扫描替换
		idrePair.second = re;
		idreTable.insert(idrePair);
		linenum++;
		infile.get();									//读入换行符
	}

	cout << "Finished scanning segment 1 !" << endl << endl;

	//定义段扫描结束，下面开始规则段扫描
	infile.get();										//读入换行符
	state = BEGIN;
	while (!infile.eof() && state != LAYER_ID)			//判断是否到达文件尾部,以防止出现文件读取错误
	{
		c = infile.get();
		if (c == '%')
		{
			state = checkSpec(c);
			if (state == MYERROR)
			{
				printError(linenum);
				return;
			}
			continue;
		}
		else
		{
			infile.seekg(-1, ios::cur);					//对输入文件定位
		}
		string oneline;
		string re, action;
		getline(infile, oneline);
		linenum++;
		string delim = " \t";
		int offset = oneline.find_first_of(delim);
		re = oneline.substr(0, offset);
		if (!replaceRE(re))
		{
			printError(-3);
			return;
		}
		while (oneline[offset] == ' ' || oneline[offset] == '\t')
			offset++;
		action = oneline.substr(offset, oneline.size() - offset + 1);

//		cout << "re:" << re << "	Action:" << action << endl;
		createNFA(re);														//这个函数同时会标记终止状态returnState
		nfaTerState2Action.insert(make_pair(returnState, action));			//填写终止状态-规约动作映射表
//		printNfaTerState2Action();											//测试
	}

	cout << "Finished scanning segment 2 !" << endl << endl;

	joinNFAs();

	cout << "Finished creating NFA ......" << endl << endl;
	cout << "Start creating DFA ......" << endl << endl;

	//根据NFA创建DFA
	DFA myDfa(finalNFA.start);
//	cout << "生成的DFA结点情况：" << endl;							//测试
//	myDfa.printDFANodes();											//测试

//	cout << "打印DFA终结点-action映射表：" << endl;					//测试
//	printdfaTerState2Action();										//测试
//	cout << endl;													//测试

	//最小化DFA
	cout << "Minimizing the DFA ......" << endl << endl;
	getDFACharSet(myDfa.dfaNodes);
	minimizeDFA(myDfa.dfaNodes);
//	cout << "DFA node size:" << myDfa.dfaNodes.size() << endl;		//测试

	//根据DFA将相应代码写入.cpp文件
	cout << "Generating code of segment 2 ......" << endl << endl;
	generateDFACode(mindfaNodes);

	//写入.l文件的最后一部分
	cout << endl;
	while (!infile.eof())
	{
		c = infile.get();
		outfile.put(c);
	}

	cout << "Finished scanning segment 3 !" << endl << endl;

	infile.close();
	outfile.close();
	cout << "------------------------------Finished!" << endl;
	system("pause");
	return;
}

/***DFA的构造函数***/
DFA::DFA(Node *startNode)
{
	cout << "进入DFA构造函数" << endl;
//	cout << "起始结点的后继结点数目：" << startNode->next.size() << endl;
	set<Node*> i0;
	queue< set<Node*> > nodeQueue;

	i0.insert(startNode);
	getEclosure(i0);
	nodeQueue.push(i0);

	start = new Node(0, startNode->terminalTag);

	Node* currentNode = start;

	set<Node*>  subset;
	int setid = 1;
	set<Node*> temp;
	multimap<char, Node*>  row;
	set<char> rowCharSet;
	multimap<set<Node*>, Node*> setNodeTable;

	setNodeTable.insert(pair<set<Node*>, Node*>(i0, start));
	dfaNodes.push_back(*start);

	/***NFA→DFA 利用表格确定化***/
	do {
		row.clear();
		temp = nodeQueue.front();
		getEclosure(temp);
		currentNode = (*(setNodeTable.find(temp))).second;
		rowCharSet.clear();

		for (set<Node*>::iterator nodePointer = temp.begin(); nodePointer != temp.end(); nodePointer++)
		{
			for (multimap<char, Node*>::iterator mit = (*nodePointer)->next.begin(); mit != (*nodePointer)->next.end(); mit++)
			{
				if (((*mit).first) != 1)	//不为ε边，则将当前char-Node对插入row
				{
					row.insert(pair<char, Node*>((*mit).first, (*mit).second));///copy multimap
					rowCharSet.insert((*mit).first);
				}
			}
		}

		
		/***划分子集 ***/             
		multimap<char, Node*>::iterator mit3;
		multimap<char, Node*> m1;
		m1.clear();
		for (set<char>::iterator ziit = rowCharSet.begin(); ziit != rowCharSet.end(); ziit++)
		{
			subset.clear();
			mit3 = row.find(*ziit);
			for (int i = 0; i<row.count(*ziit); i++)
			{
				subset.insert((*mit3).second);
				mit3++;
			}
			getEclosure(subset);

			if (setNodeTable.find(subset) == setNodeTable.end())
			{
				nodeQueue.push(subset);

				bool isaccepted = false;
				int nfaPoint;
				for (set<Node*>::iterator nodePointer = subset.begin(); nodePointer != subset.end(); nodePointer++)
				{
					if ((*nodePointer)->terminalTag)
					{
						//只要一个是终态，子集所确定的新节点就是终态
						isaccepted = true;
						nfaPoint = (*nodePointer)->stateLabel;
						break;
					}
				}

				Node* newNode = new  Node(setid, isaccepted);
				if (isaccepted)
				{
					map<int, string>::iterator mit4 = nfaTerState2Action.find(nfaPoint);
					if (nfaTerState2Action.count(nfaPoint) != 0)
					{
						dfaTerState2Action.insert(pair<int, string>(setid, (*mit4).second));						
					}
				}
				setid++;
				setNodeTable.insert(pair<set<Node*>, Node*>(subset, newNode));
			}

			if (subset.size() != 0)
			{
				m1.insert(pair<char, Node*>(*ziit, (*(setNodeTable.find(subset))).second));
			}
		}

		currentNode->setOutStates(m1);
		multimap<char, Node*>::iterator mit1 = currentNode->next.begin();

		for (int j = 0; j<currentNode->next.size(); j++)
		{
			mit1++;
		}

		dfaNodes.push_back(*currentNode);
		nodeQueue.pop();

	} while (!nodeQueue.empty());
}

/***求DFA结点的ε闭包***/
void DFA::getEclosure(set<Node *> & eSet)
{
	queue<Node*> findQueue;
	for (set<Node *>::iterator iter = eSet.begin(); iter != eSet.end(); iter++)
	{
		findQueue.push(*iter);
	}

	Node* currentNode;
	do
	{
		currentNode = findQueue.front();
		multimap<char, Node*>::iterator iter2 = currentNode->next.find(1);
		for (int i = 0; i < currentNode->next.count(1); i++)
		{
			if (eSet.find((*iter2).second) == eSet.end())
			{
				eSet.insert((*iter2).second);
				findQueue.push((*iter2).second);
			}
			iter2++;
		}
		findQueue.pop();
	} while (!findQueue.empty());
}

/***判断.lex文件中的%%，%{，%}分隔符***/
int checkSpec(char ch)
{
	if (ch == '%')
	{
		char cc = infile.get();
		switch (cc)
		{
		case '%':
			return LAYER_ID;
		case '{':
			return HEADER_BEGIN;
		case '}':
			return HEADER_END;
		default:
			infile.seekg(-1, ios::cur);
			break;
		}
	}
	return MYERROR;
}

/***正则表达式的扫描替换, 即把当中含有用户自定义标识符的地方替换成完整的正则式***/
bool replaceRE(string & re)
{
	//对用户自定义的RE进行处理，使其只有|、*、(、)等特殊符号，代换{}等。假设[]不能嵌套
	int intcount;
	string strRes = "";						//替换后的RE
	string temp;
	map<string, string>::iterator iter;		//idreTable的迭代器
	int i = 0;
	int j;
	int offset;
	char ch = re[i];
	while (ch != '\0')
	{
		switch (ch)
		{
		case '[':
			strRes.append(1, '(');
			ch = re[++i]; continue;
			break;
		case ']':
			strRes.append(1, ')');
			ch = re[++i];
			break;
		case '-':
		{
			char before = re[i - 1];
			char after = re[i + 1];
			strRes.erase(strRes.length() - 1, 1);
			if (isalnum(re[i - 2]))
			{
				strRes.append(1, '|');
			}
			while (before<after)
			{
				strRes.append(1, before);
				strRes.append(1, '|');
				before++;
			}
			strRes.append(1, after);
			ch = re[i + 2];
			i = i + 2;
			break;
		}
		case '{':
			offset = re.find_first_of('}', i);
			for (j = i + 1; j<offset; j++)
			{
				temp.append(1, re[j]);
			}
			iter = idreTable.find(temp);
			intcount = idreTable.count(temp);
			if (intcount <= 0)
			{
				return false;
			}
			if (iter != idreTable.end())
			{
				strRes.append(1, '(');
				strRes.append(iter->second);
				strRes.append(1, ')');
			}
			temp = "";
			i = offset;
			//跳过'}'
			ch = re[++i];
			break;
		case '"':
			offset = re.find_first_of('"', i + 1);
			temp = re.substr(i + 1, offset - i - 1);
			strRes.append(1, '(');
			strRes.append(temp);
			strRes.append(1, ')');
			i = offset;
			ch = re[++i];
			break;
		case '(':
			strRes.append(1, '(');
			ch = re[++i];
			break;
		case ')':
			strRes.append(1, ')');
			ch = re[++i];
			break;
		case '*':
			strRes.append(1, '*');
			ch = re[++i];
			break;
		case '|':
			strRes.append(1, '|');
			ch = re[++i];
			break;
		case '+':
			strRes.append(1, re[i - 1]);	//将a+的形式转为aa*
			strRes.append(1, '*');
			ch = re[++i];
		default:
			strRes.append(1, ch);
			ch = re[++i];
			break;
		}
	}
	re = strRes;
	return true;
}

/***re中缀变后缀，添加连接符'.'和结束符'#'***/
string infix2suffix(string reBefore)
{
	string tempRE = "";								//插入连接符和结束符后的re
	for (int i = 0; i<reBefore.length(); i++)		//插入连接符
	{
		char ch = reBefore[i];
		tempRE.append(1, ch);
		if (ch != '|'&&ch != '(')
		{
			char temp = reBefore[i + 1];
			if ( temp != '\0' && temp != '*' && temp != '|' && temp != ')')
			{
				tempRE.append(1, '.');
			}
		}
	}
	tempRE.append(1, '#');							//插入结束符


	//开始转化为suffix
	string strRes;
	int i = 0, j = 0;
	stack<char> tempS;								//符号栈
	tempS.push('#');
	char ch = tempRE[i];
	while (ch != '#')
	{
		if (ch == ' ')
			ch = tempRE[++i];
		else if ('(' == ch)
		{
			tempS.push(ch);
			ch = tempRE[++i];
		}
		else if (')' == ch)
		{
			if (tempRE.size() == 2)
			{
				strRes.append(1, ch);
			}
			else {
				while (tempS.top() != '(')
				{
					strRes.append(1, tempS.top());
					tempS.pop();
				}
				tempS.pop();
			}
			ch = tempRE[++i];
		}
		else if ('.' == ch || '*' == ch || '|' == ch)
		{
			char w = tempS.top();
			while (getPriority(w) >= getPriority(ch))
			{
				strRes.append(1, w);
				tempS.pop();
				w = tempS.top();
			}
			tempS.push(ch);
			ch = tempRE[++i];
		}
		else
		{
			strRes.append(1, ch);
			ch = tempRE[++i];
		}
	}
	while (tempS.top() != '#')
	{
		strRes.append(1, tempS.top());
		tempS.pop();
	}
	tempS.pop();
	strRes.append(1, '\0');
	return strRes;
}

/***获取符号c的优先级***/
int getPriority(char c)
{
	if ('#' == c)
	{
		return 0;
	}
	else if ('|' == c)
	{
		return 1;
	}
	else if ('.' == c)
	{
		return 2;
	}
	else if ('*' == c)
	{
		return 3;
	}
	else {
		return 0;
	}
}

/***构造某re的NFA***/
void createNFA(string re)
{
	//strsufix 为后缀表达式,0x01表示ε
	string strsufix;
	strsufix = infix2suffix(re);
	stack<NFA> NFAStack;
	NFA nfatemp;
	NFA nfatemp1;
	NFA resnfa;

	int i = 0;
	char ch = strsufix[i];
	while (ch != '\0')
	{
		if ((ch != '|') && (ch != '.') && (ch != '*'))
		{
			Node* temp = new Node(stateIndex++, false);
			Node* temp1 = new Node(stateIndex++, true);
			temp->addOutState(ch, temp1);
			nfatemp.start = temp;
			nfatemp.terminalNodes.clear();
			(nfatemp.terminalNodes).push_back(temp1);
			NFAStack.push(nfatemp);
			returnState = temp1->getStateLabel();
		}
		else
		{
			//处理ch为| . *的情况
			switch (ch)
			{
			case '*':
			{
				nfatemp = NFAStack.top();
				NFAStack.pop();
				Node* temp = new Node(stateIndex++, false);
				Node* temp1 = new Node(stateIndex++, true);
				Node* endNode = nfatemp.terminalNodes[0];
				endNode->setTerminalTag(false);
				endNode->addOutState(0x01, nfatemp.start);
				endNode->addOutState(0x01, temp1);

				temp->addOutState(0x01, nfatemp.start);
				temp->addOutState(0x01, temp1);
				resnfa.start = temp;
				resnfa.terminalNodes.clear();
				resnfa.terminalNodes.push_back(temp1);
				NFAStack.push(resnfa);
				returnState = temp1->getStateLabel();
				break;
			}
			case '|':
			{
				nfatemp = NFAStack.top();
				NFAStack.pop();
				nfatemp1 = NFAStack.top();
				NFAStack.pop();
				Node *temp = new Node(stateIndex++, false);
				Node* temp1 = new Node(stateIndex++, true);
				temp->addOutState(0x01, nfatemp1.start);
				temp->addOutState(0x01, nfatemp.start);
				Node* endNode1 = nfatemp.terminalNodes[0];
				Node* endNode2 = nfatemp1.terminalNodes[0];

				endNode1->setTerminalTag(false);
				endNode2->setTerminalTag(false);
				endNode2->setStateLabel(nfatemp1.terminalNodes[0]->getStateLabel());
				endNode1->setStateLabel(nfatemp.terminalNodes[0]->getStateLabel());
				endNode1->addOutState(0x01, temp1);
				endNode2->addOutState(0x01, temp1);
				resnfa.start = temp;
				resnfa.terminalNodes.clear();
				resnfa.terminalNodes.push_back(temp1);
				NFAStack.push(resnfa);
				returnState = temp1->getStateLabel();
				break;
			}
			case '.':
			{
				nfatemp = NFAStack.top();
				NFAStack.pop();
				nfatemp1 = NFAStack.top();
				NFAStack.pop();
				(nfatemp1.terminalNodes[0])->setTerminalTag(false);
				Node* endNode1 = nfatemp1.terminalNodes[0];
				Node* endNode2 = nfatemp.terminalNodes[0];
				endNode2->setStateLabel(nfatemp.terminalNodes[0]->getStateLabel());
				endNode1->setStateLabel(nfatemp1.terminalNodes[0]->getStateLabel());
				//将第一个nfa的终点与第二个dfa的起始点合并
				multimap<char, Node*> a = nfatemp.start->getOutStates();
				for (multimap<char, Node*>::iterator it = a.begin(); it != a.end(); it++)
				{
					endNode1->addOutState((*it).first, (*it).second);
				}
				resnfa.start = nfatemp1.start;
				resnfa.terminalNodes.clear();
				resnfa.terminalNodes.push_back(endNode2);
				NFAStack.push(resnfa);
				returnState = endNode2->getStateLabel();
				break;
			}
			default:
			{
				cerr << "Create NFA error for string " << re << " !" << endl;
				return;
			}
			}
		}
		ch = strsufix[++i];
		nfatemp.start = NULL;
		nfatemp.terminalNodes.clear();
		nfatemp1.start = NULL;
		nfatemp1.terminalNodes.clear();
		resnfa.start = NULL;
		resnfa.terminalNodes.clear();
	}
	nfaTable.push_back(NFAStack.top());		//将当前re的NFA压入NFA栈
	NFAStack.pop();
}

/***将临时NFA栈中的所有NFA合并为一个大的NFA***/
void joinNFAs()
{
	Node* temp = new Node(stateIndex++, false);			//新的起始结点
	NFA nfa1;
	NFA nfa2;
	if (nfaTable.size() == 0)
	{
		cout << "the NFATable is empty!" << endl;
		return;
	}
	else if (nfaTable.size() == 1)
	{
		finalNFA = (nfaTable[0]);
	}
	else
	{
		for (int i = 0; i<nfaTable.size(); i++)
		{
			nfa1 = nfaTable[i];
			temp->addOutState(0x01, nfa1.start);
			nfa2.terminalNodes.push_back(nfa1.terminalNodes[0]);
		}
		nfa2.start = temp;
		finalNFA = nfa2;
	}
}

/***获得dfa的字符集***/
void getDFACharSet(vector<Node> nodeset)
{
	Node temp;
	for (int i = 1; i<nodeset.size(); i++)
	{
		temp = nodeset[i];
		multimap<char, Node*> mulmap = temp.getOutStates();

		for (multimap<char, Node *>::iterator map_it = mulmap.begin(); map_it != mulmap.end(); map_it++)
		{
			if (map_it->first != 0x01)
			{
				dfaCharSet.insert((map_it->first));
			}
		}
	}
}

/***DFA最小化***/
void minimizeDFA(vector<Node> dfaNodes)
{
	dfaNodes.erase(dfaNodes.begin());					//删除dfaNodes中的第一个元素

	/***结果保存，采用自顶向下的方法进行子集的划分***/
	vector< set<int> > lastset;							//最后最小化之后的划分
	deque< set<int> > flag;
	vector< set<int> > testset;
	set<int> currentset;
	set<int>::iterator setit;
	int start = 0;										//标记起始节点所在的子集在lastset中的索引
	set<int> t;											//起始结点最终所在的集合
	t.insert(start);
	lastset.push_back(t);								//起始结点所在的集合
	testset.push_back(t);
	getNonTerSet(dfaNodes, flag, testset);				//将dfa的非终结点集合插入flag和testset
//	printFT(flag, testset);								//测试
	divideTerNodes(dfaNodes, flag, testset);			//将terminal 按照其返回值不同进行分解成不同的集合
//	printFT(flag, testset);								//测试
	int i = 0;

	while (!flag.empty())
	{
		currentset = flag.front();
		int cursize = currentset.size();
		flag.pop_front();
		if (currentset.size() <= 1)						//size<=1则不能再进行子集的划分
		{
			lastset.push_back(currentset);
			continue;
		}

		bool terminal = false;

		for (set<char>::iterator charit = dfaCharSet.begin(); charit != dfaCharSet.end(); charit++)
		{
			if (terminal)
			{
				break;
			}
			char ch = (*charit);
			map<int, set<int> > tempres;
			tempres.clear();
			int setid = -100;
			for (set<int>::iterator node_it = currentset.begin(); node_it != currentset.end(); node_it++)
			{
				Node curnode;
				curnode = dfaNodes[(*node_it)];
				int curstate = curnode.getStateLabel();
				multimap<char, Node*> mymulmap;
				multimap<char, Node*>::iterator mulmapit;
				mymulmap = curnode.getOutStates();
				mulmapit = mymulmap.find(ch);
				int mycount = mymulmap.count(ch);
				if (mycount<1)
				{
					setid = -1;
					insertNode2Map(tempres, curstate, setid);
				}
				else
				{
					Node* nextnode = mulmapit->second;
					int state1 = nextnode->getStateLabel();
					setid = getSetId(state1, testset, 0);
					insertNode2Map(tempres, curstate, setid);
				}
			}
			
			map<int, set<int> >::iterator int_set_mapit;
			set<int> int_set;
			if (tempres.size()>1)
			{
				removeSetFromVec(testset, currentset);
				for (int_set_mapit = tempres.begin(); int_set_mapit != tempres.end(); int_set_mapit++)
				{
					int_set = int_set_mapit->second;
					int setsize = int_set.size();
					if (int_set.size()>0)
					{
						flag.push_back(int_set);
						testset.push_back(int_set);
						terminal = true;
					}
				}
			}

		}
		if (!terminal)
		{
			lastset.push_back(currentset);
		}
	}
	//for (int p = 0; p<lastset.size(); p++)
	//{
	//	set<int> setint = lastset[p];
	//	cout << "The ";
	//	cout << p;
	//	cout << " subset has nodes:" << endl;
	//	for (set<int>::iterator printit = setint.begin(); printit != setint.end(); printit++)
	//	{
	//		cout << (*printit) << "\t";
	//	}
	//	cout << endl;
	//}
	
	cout << "Finished DFA subset partition ......" << endl << endl;
	createMinDFA(lastset, dfaNodes, start);
}

/***获取DFA的非终结点集合，并加入划分队列中***/
void getNonTerSet(vector<Node> dfanode, deque<set<int> > &mydeque, vector<set<int> > &test)
{
	set<int> res;
	bool acceptted;
	for (int i = 1; i<dfanode.size(); i++)
	{
		acceptted = dfanode[i].getTerminalTag();
		if (!acceptted)
		{
			res.insert(dfanode[i].getStateLabel()); 
		}
	}
	mydeque.push_back(res);
	test.push_back(res);
}

/***划分终结点***/
void divideTerNodes(vector<Node>dfanode, deque<set<int> > &myqueue, vector<set<int> > &test)
{
	map<string, set<int> >::iterator it;
	map<string, set<int> > str_set;
	map<int, string>::iterator int_str;
	for (int i = 0; i<dfanode.size(); i++)
	{
		if (dfanode[i].getTerminalTag())
		{
			int_str = dfaTerState2Action.find(dfanode[i].getStateLabel());
			if (int_str != dfaTerState2Action.end())
			{
				it = str_set.find(int_str->second);
				if (it != str_set.end())
				{
					insertNode2Set(it->second, dfanode[i].getStateLabel());
				}
				else
				{
					set<int> temp;
					temp.insert(dfanode[i].getStateLabel());
					str_set.insert(make_pair(int_str->second, temp));
				}
			}
		}
	}
	//根据返回值分解后插入到队列中
	for (it = str_set.begin(); it != str_set.end(); it++)
	{
		test.push_back(it->second);
		myqueue.push_back(it->second);
	}
}

/***将结点插入结点-集合映射表***/
void insertNode2Map(map<int, set<int> > &par1, int par2, int setid)
{
	map<int, set<int> >::iterator it;
	it = par1.find(setid);
	set<int> *set_int;
	if (it != par1.end())
	{
		set_int = &(it->second);
		set_int->insert(par2);

	}
	else
	{
		set<int> temp;
		temp.clear();
		temp.insert(par2);
		par1.insert(make_pair(setid, temp));
	}
}

/***将结点插入结点集合***/
void insertNode2Set(set<int> &par1, int par2)
{
	par1.insert(par2);
}

/***从第i个元素找出state对应的id***/
int getSetId(int state, vector< set<int> > vecset, int n)
{
	int Intres = -1;
	set<int> current;
	for (int i = n; i<vecset.size(); i++)
	{
		current = vecset[i];
		set<int>::iterator it;
		it = current.find(state);
		if (it != current.end())
		{
			return i;
		}
	}
	return Intres;  // 没有找到
}

/***从子集vector中删除某set***/
void removeSetFromVec(vector<set<int> > &testset, set<int> current)
{
	vector<set<int> >::iterator it;
	for (it = testset.begin(); it != testset.end(); it++)
	{
		if (isSetsEqual((*it), current))
		{
			testset.erase(it);
			return;
		}
	}
}

/***判断两个集合是否相等***/
bool isSetsEqual(set<int>par1, set<int>par2)
{
	set<set<int> > temp;
	temp.insert(par1);
	temp.insert(par2);
	if (temp.size() == 2)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/***构造最小化DFA***/
void createMinDFA(vector< set<int> >vecset, vector<Node> dfanode, int start)
{
	//用vecset的下标表示mindfa的状态，start为DFA起始状态的子集索引号
	bool acceptted;
	Node* tempnode;
	Node my = dfanode[0];

	deque<int> states;	//保存最小化DFA中的状态号
	for (int i = 0; i<vecset.size(); i++)
	{
		int mystate = -100;
		acceptted = isSetATerofMinDFA(vecset[i], dfanode, mystate);
		tempnode = new Node(i, acceptted);
		mindfaNodes.push_back(*tempnode);
		if (mystate != -100)
		{
			states.push_back(mystate);
		}
	}

	//添加发出边和后继结点
	int nextstate;
	int setid;
	for (int k = 0; k<mindfaNodes.size(); k++)
	{
		Node* temp = &(mindfaNodes[k]);
		//对应dfa的返回值与minidfa的返回值
		map<int, string>::iterator mapintstring;
		if (temp->getTerminalTag())
		{
			mapintstring = dfaTerState2Action.find(states.front());

			if (mapintstring != dfaTerState2Action.end())
			{
				mindfaTerState2Action.insert(make_pair(temp->getStateLabel(), mapintstring->second));
			}
			else
			{
				cout << states.front() << endl;
				cout << "dfa terminal error!" << endl;
			}
			states.pop_front();
		}
		for (set<char>::iterator char_it = dfaCharSet.begin(); char_it != dfaCharSet.end(); char_it++)
		{
			char ch = (*char_it);
			set<int> tempset = vecset[k];
			for (set<int>::iterator set_it = tempset.begin(); set_it != tempset.end(); set_it++)
			{
				int myte = *set_it;
				Node mytempnode = dfanode[(*set_it)];
				multimap<char, Node*> mul = mytempnode.next;
				multimap<char, Node*>::iterator mult_mapit = mul.find(ch);
				if (mult_mapit != mul.end())
				{
					nextstate = (mult_mapit->second)->getStateLabel();
					setid = getSetId(nextstate, vecset, 0);
					if (setid<0)
					{
						cout << "the error found in k= " << k << endl;
						cout << "error found in check nextid" << endl;
						return;
					}
					temp->addOutState(ch, &(mindfaNodes[setid]));
					break;
				}
			}
		}
	}
}

/***判断某集合是否为最小化DFA的终结点***/
bool isSetATerofMinDFA(set<int> int_set, vector<Node> dfanode, int &state)
{
	set<int>::iterator it = int_set.begin();
	map<int, string>::iterator mapit;
	for (set<int>::iterator myit = int_set.begin(); myit != int_set.end(); myit++)
	{
		mapit = dfaTerState2Action.find((*myit));
		if (mapit != dfaTerState2Action.end())
		{
			state = mapit->first;
			break;
		}
	}
	if (it != int_set.end())
	{
		return dfanode[(*it)].getTerminalTag();
	}
	else
	{
		return false;
	}
}

/**根据最终的DFA产生规则段的相应代码，写入.cpp文件***/
void generateDFACode(vector<Node> dfanode)
{
	outfile << "using namespace std;" << endl;
	outfile << "const int START=" << dfanode[0].getStateLabel() << ";" << endl;
	outfile << "const int MYERROR=65537;" << endl;
	outfile << endl;
	
	outfile << "int analysis(string yytext)\n";
	outfile << "{\n";
	outfile << "\tint state=START;\n";
	outfile << "\tint i=0;\n";
	outfile << "\tchar ch=yytext[i];\n";

	outfile << "\twhile(i<=yytext.length())\n";
	outfile << "\t{\n";
	outfile << "\t\tswitch(state)\n";
	outfile << "\t\t{\n";
	for (int i = 0; i<dfanode.size(); i++)
	{
		//一个DFA状态对应一个case
		int flagofelseif = 0;
		outfile << "\t\tcase " << dfanode[i].getStateLabel() << ":\n";
		outfile << "\t\t\t{\n";
		if (dfanode[i].getTerminalTag())
		{
			outfile << "\t\t\t\tif(i==yytext.length())\n";
			outfile << "\t\t\t\t{\n";
			map<int, string>::iterator it;
			it = mindfaTerState2Action.find(dfanode[i].getStateLabel());
			if (it != mindfaTerState2Action.end())
			{
				int length = ((*it).second).length();
				outfile << "\t\t\t\t\t" << ((*it).second).substr(1, length - 2) << endl;
				outfile << "\t\t\t\t\tbreak;\n";
				outfile << "\t\t\t\t}\n";
			}

		}
		multimap<char, Node*> mymap = dfanode[i].getOutStates();
		multimap<char, Node*>::iterator tempit;

		for (tempit = mymap.begin(); tempit != mymap.end(); tempit++)
		{
			outfile << "\t\t\t\t";
			if (flagofelseif == 0)
			{
				outfile << "if";
			}
			else
			{
				outfile << "else if";
			}
			outfile << "(ch=='" << tempit->first << "')\n";
			outfile << "\t\t\t\t{\n";
			outfile << "\t\t\t\t\tstate=" << (tempit->second)->getStateLabel() << ";\n";
			outfile << "\t\t\t\t\tbreak;\n";
			outfile << "\t\t\t\t}\n";
		}
		if (mymap.size()>0)
		{
			outfile << "\t\t\t\telse\n";
			outfile << "\t\t\t\t{\n";
			outfile << "\t\t\t\t\treturn MYERROR;\n";
			outfile << "\t\t\t\t}\n";
		}
		outfile << "\t\t\t\tbreak;\n";
		outfile << "\t\t\t}\n";
	}
	outfile << "\t\t\tdefault:\n";
	outfile << "\t\t\t\treturn MYERROR;\n";
	outfile << "\t\t}\n";
	outfile << "\t\tch=yytext[++i];  //ch is the next letter to be recongnized\n";
	outfile << "\t}\n";
	outfile << "\treturn MYERROR;\n";
	outfile << "\n}";
}

/***报错函数***/
void printError(int i)
{
	if (i>0)
	{
		cerr << "There is an error in line " << i << " !" << endl;
		return;
	}
	
	switch (i)
	{
	case -1:
		cout << "Error -1: Open file error!" << endl;
		break;
	case -2:
		cout << "Error -2: Input file must be start with '%{'!" << endl;
		break;
	case -3:
		cout << "Error -3: 替换re失败！" << endl;
		break;
	default:
		break;
	}
}

/***几个测试函数***/
void printNfaTerState2Action()
{
	for (map<int, string>::iterator iter = nfaTerState2Action.begin(); iter != nfaTerState2Action.end(); iter++)
	{
		cout << "NFATerState:" << iter->first << "	action:" << iter->second << endl;
	}
}
void printFT(deque< set<int> > flag, vector< set<int> > testset)
{
	cout << "deque flag" << endl;
	for (int i = 0; i < flag.size(); i++)
	{
		cout << "queue " << i << endl;
		for (set<int>::iterator iter = flag.at(i).begin(); iter!= flag.at(i).end(); iter++)
		{
			cout << *iter << " ";
		}
		cout << endl ;
	}
	cout << endl << endl;
	cout << "vector testset" << endl;
	for (int i = 0; i < testset.size(); i++)
	{
		cout << "testset " << i << endl;
		for (set<int>::iterator iter = testset.at(i).begin(); iter != testset.at(i).end(); iter++)
		{
			cout << *iter << " ";
		}
		cout << endl;
	}
}
void printdfaTerState2Action() 
{
	cout << "node label\t action" << endl;
	for (map<int, string>::iterator iter = dfaTerState2Action.begin(); iter != dfaTerState2Action.end(); iter++)
	{
		cout << iter->first << "\t " << iter->second << endl << endl;
	}
}