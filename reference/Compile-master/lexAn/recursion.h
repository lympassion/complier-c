#pragma once
#include<iostream>
#include<string>
#include <fstream>
#include <sstream>
#include<Windows.h>
#include "tree.h"
#include<stack>
#include <math.h>
bool success = 1;
using namespace std;
char ch;//�ַ�������������¶�����Դ�����ַ�
char strToken[100];//�ַ����飬��Ź��ɵ��ʷ��ŵ��ַ���
int len = 0;//��¼�ַ����ĳ���
string klabel[29] = { "+","-","*","/","=","#","<","<=",">",">=",",",";",":=","(",")",".","const","var","procedure","begin","end","odd","if","then","call"
,"while","do","read","write" };
string slabel[100] = {};
string  clabel[100] = {};
string  result[1000] = {};//�������е��ʵĽ������
int procedureId = 0;//��¼��ǰ�Ĺ��̲���
int tableId = 0;//��ǰ���ֱ���±�
int dx = 3;//��ǰ������ƫ�Ƶ�ַ
int ip = 0;//ȫ�ַ��ű��ָ��
int slen = 0;//��ʶ������±�
int clen = 0;//��������±�
			 //�ж��Ƿ�Ϊ����
const char *input;
int index = 0;
int codeId = 0;//������±�
bool isDigit()
{
	if (ch < 58 && ch >= 48)
		return true;
	else
		return false;
}
//�ж��Ƿ�ΪСд��ĸ
bool issLetter()
{
	if (ch <= 122 && ch >= 97)
		return true;
	else
		return false;
}
bool isDigit(char ch1)
{
	if (ch1 < 58 && ch1 >= 48)
		return true;
	else
		return false;
}
//���ַ���ת��Ϊ����
int strToint(string str)
{
	int len = str.length();
	int count = 0;
	for (int i = 0; i < len; i++)
	{
		if (str.at(i) <= 57 && str.at(i) >= 48)
			count = count + (str.at(i) - 48)*pow(10, len - i - 1);
		else
			return -1;
	}
	return count;
}
//������ת��Ϊ�ַ���
//string intTostr(int num)
//{

//}
//�ж��Ƿ�ΪСд��ĸ
bool issLetter(char ch1)
{
	if (ch1 <= 122 && ch1 >= 97)
		return true;
	else
		return false;
}
//�ж��Ƿ�Ϊ��д��ĸ
bool islLetter()
{
	if (ch <= 90 && ch >= 65)
		return true;
	else
		return false;
}
//����һ�����ַ�����ch��
void GetChar()
{
	ch = input[index];
	index++;

}
//�ж�ch���Ƿ��ǿհף����ǣ������GetCharֱ��������һ���ǿհ׵��ַ�
void GetBC()
{
	while (ch == 32)
		GetChar();
}
//��ch�е��ַ����ӵ�strToken֮��
void concat()
{
	strToken[len] = ch;
	len++;
}
//��strToken�е��ַ������ұ����ֱ�������һ���������򷵻��򷵻����ı��룬���򷵻�0ֵ
int Reserve()
{

	for (int i = 0; i < 29; i++)
	{
		/*const char *p = klabel[i].data();
		if (strcmp(p,strToken)==0)
		{
		return i;
		}*/
		if (klabel[i] == strToken)
			return i;
	}
	return -1;
}
//������ָʾ���ص�һ���ַ�λ��
void Retract()
{
	index--;
	ch = 32;
}
//��strToken�еı�ʶ�����뵽���ű�
int insertId()
{
	slabel[slen] = strToken;
	slen++;
	return slen - 1;
}
//��strToken�еĳ������뵽������
int insertConst()
{
	clabel[clen] = strToken;
	clen++;
	return clen - 1;
}
string readFileIntoString(char * filename)
{
	ifstream ifile(filename);
	//���ļ����뵽ostringstream����buf��
	ostringstream buf;
	char ch;
	while (buf&&ifile.get(ch))
		buf.put(ch);
	//������������buf�������ַ���
	return buf.str();
}
//����Ƿ��Ѿ����ڴ˱�ʶ��
int isexists(string str)
{
	for (int i = 0; i < slen; i++)
		if (str == slabel[i])
			return 1;

	return 0;
}
//����Ƿ��Ѿ����ڴ˳���
int  isexistc(string con)
{
	for (int i = 0; i < clen; i++)
		if (con == clabel[i])
			return 1;
	return -1;
}
int isexists()
{
	for (int i = 0; i < slen; i++)
		if (strToken == slabel[i])
			return i;

	return -1;
}
//����Ƿ��Ѿ����ڴ˳���
int  isexistc()
{
	for (int i = 0; i < clen; i++)
		if (strToken == clabel[i])
			return i;
	return -1;
}
//���ֱ�Ľṹ��
struct symble
{
	string name;
	string kind;
	int addr;
	int attribute;//val/value 
}symlabel;
//�ж��Ƿ��˳�
symble table[20];
//�ڷ��ű������ĳ����ʶ��
int findSymble(string str)
{
	for (int i = 0; i < tableId; i++)
	{
		if (table[i].name == str)
			return i;
	}
	return -1;
}
//�ڷ��ű�����ݵ�ַ���ұ���������
string findSymble(int addr1)
{
	for (int i = 0; i < tableId; i++)
	{
		if (table[i].addr == addr1)
			return table[i].name;
	}
	return "";
}
//Ŀ��ָ��Ľṹ�壺
struct tarInstruc
{
	string funcCode=" ";
	int levelDiff = 0;
	int displacement = 0;
	string content = " ";
};
tarInstruc code[200];
//�ӹؼ��ֱ���ҹؼ��ֵ��±�
int findKlabel(string str)
{
	for (int i = 0; i < 29; i++)
	{
		if (str == klabel[i])
			return i+1;
	}
	return -1;
}
//���code����������
void codeAdd(string str,int lev,int place,string con)
{
	code[codeId].funcCode = str;
	code[codeId].levelDiff = lev;
	code[codeId].displacement = place;
	code[codeId].content = con;
	codeId++;
}
//���code��������
void codeAdd(string str, int lev, int place)
{
	code[codeId].funcCode = str;
	code[codeId].levelDiff = lev;
	code[codeId].displacement = place;
	codeId++;
}
//������
void lex(string file1)
{
	int code, value, num = 0;
	/*cout << "�����뵥�ʳ���"<<endl;
	cin >> num;
	cout << "�����뵥�ʣ�"<<endl;
	cin >> input;*/
	//char filename[] = "test.txt";
	//const char *file = readFileIntoString(filename).data();
	std::ifstream in(file1);
	std::ostringstream tmp;
	tmp << in.rdbuf();
	std::string str = tmp.str();
	cout << str << endl;
	for (int i = 0; i < str.length(); i++)//����������д�д��ĸ����ת��ΪСд��ĸ
		if (str.at(i) <= 90 && str.at(i) >= 65)
			str.at(i) = str.at(i) + 32;
	const char *file = str.data();
	input = file;

	while (input[index] != 0)
	{
		len = 0;
		//strcpy(strToken,"");
		memset(strToken, 0, sizeof(strToken));
		//cout << strToken << endl;
		//Sleep(1000);
		GetChar();
		GetBC();
		if (issLetter())//�ж��Ƿ�����ĸ
		{
			while (issLetter() || isDigit())
			{
				concat();
				GetChar();
			}
			Retract();
			result[ip] = strToken;
			ip++;
			code = Reserve();
			if (code == -1)//���ڹؼ��ֱ����Ϊ��ʶ��
			{
				
				int index1 = isexists();//���˱�ʶ���Ƿ����
				if (index1 != -1)
					cout << "(" << strToken << ",id," << index1 << ")" << endl;
				else
				{
					value = insertId();
					cout << "(" << strToken << ",id," << value << ")" << endl;//�����ʶ������Ԫ��

				}//cout << strToken<<endl;
			}
			else
			{
				cout << "(" << klabel[code] << "," << klabel[code] << ",-)" << endl;//����ؼ��ֵĶ�Ԫ��
			}
		}
		else if (isDigit())//��������֣���Ϊ����
		{
			while (isDigit())
			{
				concat();
				GetChar();
			}
			Retract();
			result[ip] = strToken;
			ip++;
			int index2 = isexistc();
			if (index2 != -1)
				cout << "(" << strToken << ",int," << index2 << ")" << endl;
			else
			{
				value = insertConst();
				cout << "(" << strToken << ",int," << value << ")" << endl;
			}
		}
		/*else if (islLetter())
		{
		while(islLetter())
		{
		concat();
		GetChar();
		}
		Retract();
		code = Reserve();
		if (code == -1)
		cout << "ƴд����"<<endl;
		else
		cout << "(" << code << ",-)" << endl;//����ؼ��ֵĶ�Ԫ��
		}*/
		else//�������
		{
			switch (ch)
			{
			case '+':
				cout << "(+,+," << "-)" << endl;
				result[ip] = "+";
				ip++;
				break;
			case '-':
				cout << "(-,-," << "-)" << endl;
				result[ip] = "-";
				ip++;
				break;
			case '*':
				cout << "(*,*," << "-)" << endl;
				result[ip] = "*";
				ip++;
				break;
			case '/':
				cout << "(/,/," << "-)" << endl;
				result[ip] = "/";
				ip++;
				break;
			case '=':
				cout << "(=,=," << "-)" << endl;
				result[ip] = "=";
				ip++;
				break;
			case '#':
				cout << "(#,#," << "-)" << endl;
				result[ip] = "#";
				ip++;
				break;
			case '<':
				GetChar();
				if (ch == '=')
				{
					cout << "(<=,<=," << "-)" << endl;
					result[ip] = "<=";
					ip++;
				}
				else
				{
					Retract();
					cout << "(<,<," << "-)" << endl;
					result[ip] = "<";
					ip++;
				}
				break;
			case '>':
				GetChar();
				if (ch == '=')
				{
					cout << "(>=,>=," << "-)" << endl;
					result[ip] = ">=";
					ip++;
				}
				else
				{
					Retract();
					cout << "(>,>," << "-)" << endl;
					result[ip] = ">";
					ip++;
				}
				break;
			case ',':
				cout << "(,,,," << "-)" << endl;
				result[ip] = ",";
				ip++;
				break;
			case ';':
				cout << "(;,;," << "-)" << endl;
				result[ip] = ";";
				ip++;
				break;
			case ':':
				GetChar();
				if (ch == '=')
				{
					cout << "(:=,:=," << "-)" << endl;
					result[ip] = ":=";
					ip++;
				}
				else
				{
					Retract();
					cout << "(:,:," << "-)" << endl;
					result[ip] = ":";
					ip++;
				}
				break;
			case '(':
				cout << "((,(," << "-)" << endl;
				result[ip] = "(";
				ip++;
				break;
			case ')':
				cout << "(),)," << "-)" << endl;
				result[ip] = ")";
				ip++;
				break;
			case '.':
				cout << "(.,.," << "-)" << endl;
				result[ip] = ".";
				ip++;
				break;
			case 10://�ų������з�
				break;
			default:
				cout << "ƴд����:" << ch << endl;
				break;
			}
		}
	}
}
//��������
void subProgram(treeNode<string> *tN);
void conExplain(treeNode<string> *tN);
void varExplain(treeNode<string> *tN);
void processExplain(treeNode<string> *tN);
void constDef(treeNode<string> *tN);
void statement(treeNode<string> *tN);
void varExplain(treeNode<string> *tN);
void id(treeNode<string> *tN);
void unsInt(treeNode<string> *tN);
void dig(treeNode<string> *tN,int index);

void processHead(treeNode<string> *tN);
void AssignmentStatement(treeNode<string> *tN);
void expression(treeNode<string> *tN);
void CompoundStatement(treeNode<string> *tN);
void conditions(treeNode<string> *tN);
void AddAndSubtract(treeNode<string> *tN);
void item(treeNode<string> *tN);
void factor(treeNode<string> *tN);
void MULAndDIV(treeNode<string> *tN);
void ConditionStatement(treeNode<string> *tN);
void relationship(treeNode<string> *tN);
void processCall(treeNode<string> *tN);
void readStatement(treeNode<string> *tN);
void dowhile(treeNode<string> *tN);
void writeStatement(treeNode<string> *tN);
void letter(treeNode<string> *tN,int index);
void digit(treeNode<string> *tN,int index);
treeNode<string> *Tprogram=new treeNode<string>(2,"<����>");
/*treeNode<string> *TsubProgram=new treeNode<string>(4,"<�ֳ���>");
treeNode<string> *TconExplain = new treeNode<string>("<����˵������>");
treeNode<string> *TvarExplain = new treeNode<string>("<����˵������>");
treeNode<string> *TprocessExplain = new treeNode<string>("<����˵������>");
treeNode<string> *Tstatement = new treeNode<string>(1,"<���>");
treeNode<string> *TconstDef = new treeNode<string>(3,"<��������>");
treeNode<string> *TunsInt = new treeNode<string>("<�޷�������>");
treeNode<string> *Tpoint = new treeNode<string>(".");
treeNode<string> *TprocessHead = new treeNode<string>(3,"�����ײ�");
treeNode<string> *Tid = new treeNode<string>("��ʶ��");
*/
void  program()//����
{
	codeAdd("jmp", 0, -1);//��ʱ��һ��
	ip = 0;
	treeNode<string> *TsubProgram = new treeNode<string>(4,"<�ֳ���>");
	subProgram(TsubProgram);
	if (result[ip] == ".")
	{
		Tprogram->child[0] = TsubProgram;
		Tprogram->child[1] = new treeNode<string>(".");
		//**************Ŀ���������ɣ������˳�
		codeAdd("opr", 0, 0,"return");
		//*************
		cout << "�ɹ�";
	}
	else
		exit(0);
}

void subProgram(treeNode<string> *tN)//�ֳ���
{

	int count = 4;
	int tempTableId = tableId-1;
	//����˵������
	if (result[ip] == "const")
	{   
		treeNode<string> *TconExplain = new treeNode<string>("<����˵������>");
		conExplain(TconExplain);
		count++;
		tN->child[0] = TconExplain;
	}
	//����˵������
	if (result[ip] == "var")
	{
		treeNode<string> *TvarExplain = new treeNode<string>("<����˵������>");
		varExplain(TvarExplain);
		count++;
		tN->child[1] = TvarExplain;
	}
	//����˵������
	if (result[ip] == "procedure")
	{
		treeNode<string> *TprocessExplain = new treeNode<string>("<����˵������>");
		processExplain(TprocessExplain);
		count++;
		tN->child[2] = TprocessExplain;
	}
		if (table[tempTableId].kind == "proc")
			table[tempTableId].addr = codeId;
	//**************Ŀ���������ɣ����ٵ�ַ�ռ�
	int varNumber = 0;
	for (int i = 0; i < tableId; i++)
		if (table[i].attribute == procedureId && table[i].kind == "var")
			varNumber++;
	codeAdd("int",0,varNumber+3);
	//*************
	treeNode<string> *Tstatement = new treeNode<string>(1,"<���>");
	//***************Ŀ���������,ֱ����ת��������
	if (procedureId == 0)
		code[0].displacement = codeId-1;
	//**********************
	statement(Tstatement);//���
	
	tN->child[3]=Tstatement;
	
}
void conExplain(treeNode<string> *tN)//����˵������
{
	if (result[ip] == "const")
	{
		ip++;
		tN->child[0] = new treeNode<string>("const");
		treeNode<string> *TconstDef1 = new treeNode<string>(3, "��������");
		tN->child[1] = TconstDef1;
		constDef(TconstDef1);
		int count = 0;
		while (result[ip] == ",")
		{   
			count++;//���ĺ�����
			tN->child[2*count] = new treeNode<string>(",");
			treeNode<string> *TconstDef2 = new treeNode<string>(3,"��������");
			tN->child[2*count+1] = TconstDef2;
			ip++;
			constDef(TconstDef2);
		}
		if (result[ip] == ";")
		{
			ip++;
			//�������Ľṹ
			tN->child[2*count + 2] = new treeNode<string>(";");
		}
		else
			exit(0);
	}
	else
		exit(0);
}
void varExplain(treeNode<string> *tN)//����˵������
{
	if (result[ip] == "var")
	{
		ip++;
		tN->child[0] = new treeNode<string>("var");
		treeNode<string> *Tid1 = new treeNode<string>(3, "<��ʶ��>");
		tN->child[1] = Tid1;
		table[tableId].name = result[ip];
		table[tableId].attribute = procedureId;
		table[tableId].addr = dx;
		table[tableId].kind = "var";
		tableId++;
		dx++;
		id(Tid1);
		int count = 0;
		while (result[ip] == ",")
		{
			ip++;
			count++;
			tN->child[2 * count] = new treeNode<string>(",");
			treeNode<string> *Tid2 = new treeNode<string>(3, "<��ʶ��>");
			tN->child[2 * count + 1] = Tid2;
			table[tableId].name = result[ip];
			table[tableId].attribute = procedureId;
			table[tableId].addr = dx;
			table[tableId].kind = "var";
			tableId++;
			dx++;
			id(Tid2);
		}
	
		if (result[ip] == ";")
		{
			ip++;
			tN->child[count + 1] = new treeNode<string>(";");
		}
	   else
		   exit(0);
	}
	else
		exit(0);
}
void processExplain(treeNode<string> *tN)//����˵������
{
	procedureId++;//������1
	dx = 3;//��ǰ�����ڵ�ַ��Ϊ1
	treeNode<string> *TprocessHead = new treeNode<string>(3,"<�����ײ�>");
	tN->child[0] = TprocessHead;
	treeNode<string> *TsubProgram = new treeNode<string>( "<�ֳ���>");
	tN->child[1] = TsubProgram;
	processHead(TprocessHead);
	subProgram(TsubProgram);  // TODO
	if (result[ip] == ";")
	{   
		tN->child[2] = new treeNode<string>(";");
		//**************Ŀ���������ɣ�����˵������
		codeAdd("opr", 0, 0,"return");
		//*************
		procedureId--;
		ip++;
		int count=0;
		while (result[ip]=="procedure")
		{
			count++;
			treeNode<string> *TprocessExplain = new treeNode<string>("<����˵������>");
			tN->child[count + 2] = TprocessExplain;
			processExplain(TprocessExplain);
		}

	}
	    else
			exit(0);
}
void statement(treeNode<string> *tN)//���  // TODO
{
	if (isexists(result[ip]))//��ֵ���
	{
		treeNode<string> *TAssignmentStatement = new treeNode<string>(3,"<��ֵ���>");
		tN->child[0] = TAssignmentStatement;
		AssignmentStatement(TAssignmentStatement);
	}
	else if (result[ip] == "if")//�������
	{
		treeNode<string> *TConditionStatement = new treeNode<string>(3,"<�������>");
		tN->child[0] = TConditionStatement;
		ConditionStatement(TConditionStatement);
	}
	else if (result[ip] == "while")//����ѭ��
	{
		treeNode<string> *Tdowhile = new treeNode<string>(3,"<����ѭ�����>");
		tN->child[0] = Tdowhile;
		dowhile(Tdowhile);
	}
	else if (result[ip] == "call")//���̵���	
	{
		treeNode<string> *Tcall= new treeNode<string>(2, "<���̵������>");
		tN->child[0] = Tcall;
		processCall(Tcall);
	}
	else if (result[ip] == "read")//�����
	{
		treeNode<string> *Tread = new treeNode<string>(2, "<�����>");
		tN->child[0] = Tread;
		readStatement(Tread);
	}
	else if (result[ip] == "write")//д���	
	{
		treeNode<string> *Twrite = new treeNode<string>(2, "<д���>");
		tN->child[0] = Twrite;
		writeStatement(Twrite);
	}
	else if (result[ip] == "begin")//�������
	{
		treeNode<string> *TCompoundStatement = new treeNode<string>(2, "<�������>");
		tN->child[0] = TCompoundStatement;
		CompoundStatement(TCompoundStatement);
	}
	
}
void constDef(treeNode<string> *tN)//��������
{ 
	treeNode<string> *Tid = new treeNode<string>( "<��ʶ��>");
	tN->child[0] = Tid;
	table[tableId].name = result[ip];
	table[tableId].kind = "const";
	id(Tid);
	if (result[ip] == "=")
	{
		tN->child[1] = new treeNode<string>("=");
		ip++;
		treeNode<string> *TunsInt = new treeNode<string>("<�޷�������>");
		tN->child[2] = TunsInt;
		table[tableId].attribute = strToint(result[ip]);
		tableId++;
		unsInt(TunsInt);
	}
	else
		exit(0);
}

void id(treeNode<string> *tN)//��ʶ��
{
	/*if (isexists(result[ip]))
	{
		tN->child[0] = new treeNode<string>(result[ip]);
		ip++;
	}
	else
		exit(0);
		*/
	int index=0;
	treeNode<string> *Tletter1 = new treeNode<string>("<��ĸ>");
	tN->child[0] = Tletter1;
	letter(Tletter1,0);
	index++;
	int max = result[ip].length();
	
	for (index=1; index < max && ((result[ip].at(index) >= 97 && result[ip].at(index) <= 122) ||
		(result[ip].at(index) >= 48 && result[ip].at(index) <= 57)); index++)
		{
			if (result[ip].at(index) >= 97 && result[ip].at(index) <= 122)
			{
				treeNode<string> *Tletter2 = new treeNode<string>("<��ĸ>");
				tN->child[index] = Tletter2;
				letter(Tletter2, index);
			}
			else
			{
				treeNode<string> *Tdig = new treeNode<string>("<����>");
				tN->child[index] = Tdig;
				dig(Tdig, index);
			}
		}
	ip++;
}
void unsInt(treeNode<string> *tN)//�޷�������
{
	/*if (isexistc(result[ip]))
	{
		tN->child[0] = new treeNode<string>(result[ip]);
		ip++;
	}
	else
		exit(0);*/
	int index = 0;
	treeNode<string> *Tdig1 = new treeNode<string>("<����>");
	tN->child[0] = Tdig1;
	dig(Tdig1, 0);
	index++;
	int max = result[ip].length();

	for (index = 1; index < max && (result[ip].at(index) >= 48 && result[ip].at(index) <= 57); index++)
	{
		if (result[ip].at(index) >= 48 && result[ip].at(index) <= 57)
		{
			treeNode<string> *Tdig2 = new treeNode<string>("<����>");
			tN->child[index] = Tdig2;
			dig(Tdig2, index);
		}
	}
	ip++;
}
void letter(treeNode<string> *tN,int index1)
{   
	if (result[ip].at(index1) >= 97 && result[ip].at(index1) <= 122)
	{
		string str;
		stringstream stream;
		stream << result[ip].at(index1);
		str = stream.str();//��charת��Ϊstring
		tN->child[0] = new treeNode<string>(str);
	}
	else
		exit(0);
}
void dig(treeNode<string> *tN,int index)//����
{
	if (result[ip].at(index) <= 57 && result[ip].at(index) >= 48)
	{
		string str;
		stringstream stream;
		stream << result[ip].at(index);
		str = stream.str();//��charת��Ϊstring
		tN->child[0] = new treeNode<string>(str);
	}
	else
		exit(0);
}
void processHead(treeNode<string> *tN)//�����ײ�
{
	if (result[ip] == "procedure")
	{
		ip++;
		tN->child[0] = new treeNode<string>("procedure");
		treeNode<string> *Tid = new treeNode<string>("<��ʶ��>");
		tN->child[1] = Tid;
		/******/
		table[tableId].kind = "proc";
		table[tableId].name = result[ip];
		table[tableId].addr = codeId;
		table[tableId].attribute = procedureId-1;
		tableId++;
		id(Tid);
		/****************/
		if (result[ip] == ";")
		{
			ip++;
			tN->child[1] = new treeNode<string>(";");
			
		}
		else
			exit(0);
	}
	else
		exit(0);
}
void AssignmentStatement(treeNode<string> *tN)//��ֵ��� TODO
{
	treeNode<string> *Tid = new treeNode<string>("<��ʶ��>");
	tN->child[0] = Tid;
	int tempIp = ip;//�ݴ�ip
	id(Tid);
	if (result[ip] == ":=")
	{
		ip++;
		tN->child[1] = new treeNode<string>(":=");
		treeNode<string> *Texpression = new treeNode<string>("<���ʽ>");
		tN->child[3] = Texpression;
		expression(Texpression);
		//**************Ŀ���������ɣ���ֵ���
	int tableIndex = findSymble(result[tempIp]);
	if (tableIndex == -1)
	{
		cout << "�˱���δ����" << endl;
		exit(0);
	}
	codeAdd("sto", abs(table[tableIndex].attribute - procedureId), table[tableIndex].addr);
	//*************
	}
	else
		exit(0);
}
void expression(treeNode<string> *tN)//���ʽ
{
	if (result[ip] == "+" || result[ip] == "-")
	{
		ip++;
		tN->child[0] = new treeNode<string>(result[ip]);
	}
	treeNode<string> *Titem = new treeNode<string>("<��>");
	tN->child[1]=Titem;
	item(Titem);
	int count = 0;
	while (result[ip] == "+" || result[ip] == "-")
	{
		count++;
		treeNode<string> *TAddAndSubtract = new treeNode<string>("<�Ӽ������>");
		tN->child[2*count] = TAddAndSubtract;
		treeNode<string> *Titem = new treeNode<string>("<��>");
		tN->child[2 * count+1] = Titem;
		int tempIp = ip;//�ݴ�Ip
		AddAndSubtract(TAddAndSubtract);
		item(Titem);
		//**************Ŀ���������ɣ��Ӽ�����
		codeAdd("opr", 0, findKlabel(result[tempIp]), result[tempIp]);
		//*************
	}
	
}
void CompoundStatement(treeNode<string> *tN)//�������
{
	if (result[ip] == "begin")
	{
		tN->child[0] = new treeNode<string>("begin");
		ip++;
		treeNode<string> *Tstatement1 = new treeNode<string>("<���>");
		tN->child[1] = Tstatement1;
		statement(Tstatement1);
		int count = 0;
		while (result[ip] == ";")
		{
			ip++;
			count++;
			tN->child[2 * count] = new treeNode<string>(";");
			treeNode<string> *Tstatement2 = new treeNode<string>("<���>");
			tN->child[2 * count + 1] = Tstatement2;
			statement(Tstatement2);

		}
		if (result[ip] == "end")
		{
			tN->child[2 * count+2] = new treeNode<string>("end");
			ip++;
		}
		else
			exit(0);
	}
	else
		exit(0);

}
void conditions(treeNode<string> *tN)//����
{ 
	
	 if (result[ip] == "odd")
	 {
		 ip++;
		 tN->child[0] =new  treeNode<string>("odd");
		 treeNode<string> *Texpression = new treeNode<string>("<���ʽ>");
		 expression(Texpression);tN->child[1] = Texpression;
		 //***********Ŀ��������ɣ�odd����
		 codeAdd("opr",0,findKlabel("odd"),"odd");
		 //
	}
	 else 
	{
		 treeNode<string> *Texpression1 = new treeNode<string>("<���ʽ>");
		 tN->child[0] = Texpression1;
		expression(Texpression1);
		treeNode<string> *Trealationship = new treeNode<string>("<��ϵ�����>");
		tN->child[1] = Trealationship;
		int tempIp = ip;//����ip
		relationship(Trealationship);
		treeNode<string> *Texpression2 = new treeNode<string>("<���ʽ>");
		tN->child[2] = Texpression2;
		expression(Texpression2);
		//**************Ŀ���������ɣ��Ӽ�����
		codeAdd("opr", 0, findKlabel(result[tempIp]), result[tempIp]);
		//*************
	}
}
void item(treeNode<string> *tN)//��
{
	treeNode<string> *Tfactor1 = new treeNode<string>("<����>");
	tN->child[0] = Tfactor1;
	factor(Tfactor1);
	int count = 0;
	while (result[ip]=="*"|| result[ip] == "/")
	{
		count++;
		treeNode<string> *TMULAndDIV = new treeNode<string>("<�˳������>");
		tN->child[2*count-1] = TMULAndDIV;
		int tempIp = ip;//�ݴ�Ip
		MULAndDIV(TMULAndDIV);
		treeNode<string> *Tfactor2 = new treeNode<string>("<����>");
		tN->child[2*count] = Tfactor2;
		factor(Tfactor2);
		//**************Ŀ���������ɣ��Ӽ�����
		codeAdd("opr", 0, findKlabel(result[tempIp]), result[tempIp]);
		//*************
	}
}
void factor(treeNode<string> *tN)//����
{
	if ((result[ip].at(0) <= 122 && result[ip].at(0) >= 97))
	{
		treeNode<string> *Tid = new treeNode<string>("<��ʶ��>");
		tN->child[0] = Tid;
		//*********�жϱ�ʶ��Ϊ�������ǳ���
		int tableIndex = findSymble(result[ip]);
		if (tableIndex != -1)
		{
			if (table[tableIndex].kind == "const")
			{
				codeAdd("lit", 0, table[tableIndex].attribute);
			}
			else if (table[tableIndex].kind == "var")
			{
				codeAdd("lod", abs(table[tableIndex].attribute - procedureId), table[tableIndex].addr);
			}
		}
		//****************/
		id(Tid);
	}
	else if ((result[ip].at(0) <= 57 && result[ip].at(0) >= 48))
	{
		treeNode<string> *TunsInt = new treeNode<string>("<�޷�������>");
		tN->child[0] = TunsInt;
	    //****************Ŀ��������ɣ�����
		codeAdd("lit",0,strToint(result[ip]));
		//
		unsInt(TunsInt);
	}
	else if ((result[ip] == "("))
	{
		tN->child[0] = new treeNode<string>("(");
		treeNode<string> *Texpression = new treeNode<string>("<���ʽ>");
		tN->child[1] = Texpression;
		expression(Texpression);
		if (result[ip] == ")")
		{
			tN->child[2] = new treeNode<string>(")");
			ip++;
		}
		else
			exit(0);
 	}
	else
		exit(0);

}
void AddAndSubtract(treeNode<string> *tN)//�Ӽ������
{
	if (result[ip] == "+" || result[ip] == "-")
	{
		tN->child[0] = new treeNode<string>(result[ip]);
		ip++;
	}
	else
		exit(0);
}
void MULAndDIV(treeNode<string> *tN)//�˳������
{
	if (result[ip] == "*" || result[ip] == "/")
	{
		tN->child[0] = new treeNode<string>(result[ip]);
		ip++;
	}
	else
		exit(0);
}
void relationship(treeNode<string> *tN)//��ϵ�����
{
	if (result[ip] == "=" || result[ip] == "#"||result[ip]=="<"|| result[ip] == "<="||result[ip] == ">"
		||result[ip] == ">=")
	{
		tN->child[0] = new treeNode<string>(result[ip]);
		ip++;
	}
	else
		exit(0);
}
void ConditionStatement(treeNode<string> *tN)//�������
{
	if (result[ip] == "if")
	{
		tN->child[0] = new treeNode<string>("if");
		ip++;
		treeNode<string> *Tconditions = new treeNode<string>("<����>");
		tN->child[1] = Tconditions;
		conditions(Tconditions);
		//**************Ŀ���������ɣ���������ת
		codeAdd("jpc", 0, 0);//��ʱ�����ŵ�ַ0
		int tempId = codeId - 1;
		//*************
		if (result[ip] == "then")
		{
			ip++;
			tN->child[0] = new treeNode<string>("then");
			treeNode<string> *Tstatement = new treeNode<string>("<���>");
			tN->child[1] = Tstatement;
			statement(Tstatement);
			//����֮ǰ��jpc
			code[tempId].displacement = codeId;
			//*************
		}
		else
			exit(0);
	}
	else
		exit(0);
}
void processCall(treeNode<string> *tN)//���̵������
{
	if (result[ip] == "call")
	{
		ip++;
		treeNode<string> *Tid = new treeNode<string>("<��ʶ��>");
		tN->child[0] = Tid;
		//****************Ŀ��������ɣ����̵���
		int tempAddr = 0;
		int tempId = 0;
		for (int i = 0; i < tableId; i++)
		{
			if (table[i].name == result[ip])
			{
				tempAddr = table[i].addr;
				tempId = i;
				break;
			}
		}
		codeAdd("cal",abs(procedureId-table[tempId].attribute),tempAddr);
		//**********************************8
		id(Tid);
	}
	else
		exit(0);
}
void dowhile(treeNode<string> *tN)//����ѭ��
{
	if (result[ip] == "while")
	{
		tN->child[0] = new treeNode<string>("while");
		ip++;
		int tempAddr = codeId+1;//whileѭ������ڵ�ַ
		treeNode<string> *Tconditions = new treeNode<string>("<����>");
		tN->child[1] = Tconditions;
		conditions(Tconditions);
		//**************Ŀ���������ɣ���������ת
		codeAdd("jpc", 0, 0);//��ʱ�����ŵ�ַ0
		int tempId = codeId-1;
		//*************
		if (result[ip] == "do")
		{
			tN->child[2] = new treeNode<string>("do");
			ip++;
			treeNode<string> *Tstatement = new treeNode<string>("<���>");
			tN->child[3] = Tstatement;
			statement(Tstatement);
			//**************Ŀ���������ɣ���������ת
			codeAdd("jmp", 0, tempAddr);
			//����֮ǰ��jpc
					code[tempId].displacement = codeId;
			//*************
		}
		else
			success = 0;
	}
	else
		exit(0);
}
void readStatement(treeNode<string> *tN)//�����
{
	if (result[ip] == "read")
	{
		tN->child[0] = new treeNode<string>("read");
		//**************Ŀ���������ɣ������
		codeAdd("opr", 0, findKlabel(result[ip]), result[ip]);
		//*************
		ip++;
		if (result[ip] == "(")
		{
			tN->child[1] = new treeNode<string>("��");
			ip++;
			treeNode<string> *Tid1 = new treeNode<string>("<��ʶ��>");
			tN->child[2] = Tid1;
			//*******************Ŀ���������ɣ�����Ĵ���
			int tableIndex = findSymble(result[ip]);
			if (tableIndex == -1)
			{
				cout << "δ����˱���" << endl;
				exit(0);
			}
			codeAdd("sto", abs(table[tableIndex].attribute - procedureId), table[tableIndex].addr);
			//********************
			id(Tid1);
			int count = 0;
			while (result[ip] == ",")
			{
				count++;
				tN->child[2*count+1] = new treeNode<string>(",");
				ip++;
				//**************Ŀ���������ɣ������
				codeAdd("opr", 0, findKlabel("read"), "read");
				//*************
				treeNode<string> *Tid2 = new treeNode<string>("<��ʶ��>");
				tN->child[2*count+2] = Tid2;
				//*******************Ŀ���������ɣ�����Ĵ���
				 tableIndex = findSymble(result[ip]);
				codeAdd("sto", 0, table[tableIndex].addr);
				//********************
				id(Tid2);

			}
			if (result[ip] == ")")
			{
				tN->child[2*count+3] = new treeNode<string>(")");
				ip++;
			}
			else
				exit(0);

		}
		else
			exit(0);
	}
	else
		exit(0);
}
void writeStatement(treeNode<string> *tN)//д���
{
	if (result[ip] == "write")
	{
		tN->child[0] = new treeNode<string>("write");
		ip++;
		if (result[ip] == "(")
		{
			ip++;
			tN->child[1] = new treeNode<string>("(");
			treeNode<string> *Texpression1 = new treeNode<string>("<���ʽ>");
			tN->child[2] = Texpression1;
			expression(Texpression1);
			//**************Ŀ���������ɣ�д���
			codeAdd("opr", 0, findKlabel("write"), "write");
			//*************
			int count = 0;
			while (result[ip] == ",")
			{
				count++;
				ip++;
				
				tN->child[2 * count + 1] = new treeNode<string>(",");
				treeNode<string> *Texpression2 = new treeNode<string>("<���ʽ>");
				tN->child[2 * count + 2] = Texpression2;
				expression(Texpression2);
				//**************Ŀ���������ɣ�д���
				codeAdd("opr", 0, findKlabel("write"), "write");
				//*************
			}
			if (result[ip] == ")")
			{
				tN->child[2 * count + 3] = new treeNode<string>(")");
				ip++;
			}
			else
				exit(0);

		}
		else
			exit(0);
	}
	else
		exit(0);
}


int findSymbleAddr(int addr1)
{
	for (int i = 0; i < tableId; i++)
		if (table[i].kind == "proc"&&table[i].addr == addr1)
			return table[i].attribute;
}
//����ִ�к���
struct var 
{
	string name;
	int value;
};
var vartable[20];
int vartableid=0;

int findVar(string str)
{
	for (int i = 0; i < vartableid; i++)
	{
		if (vartable[i].name == str)
			return vartable[i].value;
	}
	return -1;
}
//���ݱ����������޸�ֵ
void changeVar(string str,int a)
{
	for (int i = 0; i < vartableid; i++)
	{
		if (vartable[i].name == str)
		{
			vartable[i].value = a;
			break;
		}
	}
}
//
struct action
{
	int moveList;
	int backAddr;
	int staticList;
	int varTable[5];//������
};
action Act[10];
int action1[200];
int moveId = 0;
int backId = 0;
int staticId = 0;
int actId = 0;
void explainToPerfrom()//Ŀ�����Ľ���ִ��
{
	for (int i = 0; i < tableId; i++)
	{
		if (table[i].kind == "var")
		{
			vartable[vartableid].name = table[i].name;
			vartable[vartableid].value = 0;
		}
		vartableid++;
	}
	stack<int> com;//����ջ
	stack<action> actStack;//���¼ջ
	int temp = 0;//�û�����
	int cid = 0;//���������±�
	int tempCount = 0;//��ʱ����
	int tempCode = 0;
	int tempBackId = 0;
	int tempMoveId = 0;
	int tempStaticId = 0;
	int lev = 0;//��ǰ����
	while (cid != codeId-1) 
	{
		if (code[cid].funcCode == "jmp")//��ת���
		{
			if (cid == 0)//��ʼ����ջ
			{
				action1[actId] =0;
				moveId = 0;//��ǰ��̬��
				backId = 1;
				staticId = 2;
				actId++;
				action1[actId] = 0;
				actId++;
				action1[actId] = 0;
				actId++;
			}
			cid = code[cid].displacement;
			
		}
	  else if (code[cid].funcCode == "opr")//�������
	  {
		  switch (code[cid].displacement)
		  {
		    case 0://�˳�����
			{
				cid = action1[backId];
				actId = moveId;
				moveId = action1[moveId];
				backId = moveId+1;
				staticId =moveId+2;
				cid--;
				break;
			}
		    case 1://�ӷ�����
				tempCount = com.top();
				com.pop();
				tempCount = tempCount + com.top();
				com.pop();
				com.push(tempCount);
				break;
			case 2://��������
				tempCount = com.top();
				com.pop();
				tempCount = com.top()-tempCount;
				com.pop();
				com.push(tempCount);
				break;
			case 3://�˷�����
				tempCount = com.top();
				com.pop();
				tempCount = com.top()*tempCount;
				com.pop();
				com.push(tempCount);
				break;
			case 4://��������
				tempCount = com.top();
				com.pop();
				tempCount = com.top()/tempCount;
				com.pop();
				com.push(tempCount);
				break;
			case 5://�����ж�
				tempCount = com.top();
				com.pop();
				if (tempCount == com.top())
					tempCount = 1;
				else
					tempCount = 0;
				com.pop();
				com.push(tempCount);
				break;
			case 6://���Ⱥ�����
				tempCount = com.top();
				com.pop();
				if (tempCount == com.top())
					tempCount = 0;
				else
					tempCount = 1;
				com.pop();
				com.push(tempCount);
				break;
			case 7://С��
				tempCount = com.top();
				com.pop();
				if (com.top() < tempCount)
					tempCount = 1;
				else
					tempCount = 0;
				com.pop();
				com.push(tempCount);
				break;
			case 22://�ж��Ƿ�Ϊ����
				if (com.top() % 2 == 1)
					tempCount = 1;
				else
					tempCount = 0;
				com.pop();
				com.push(tempCount);
				break;
			case 28://����
				cout << "�����룺";
				cin >> temp;
				//vartable[vartableid].
				com.push(temp);
				break;
			case 29://���
				cout << com.top()<<"\n";
				break;
		  }
		  cid++;
	  }
	  else if (code[cid].funcCode == "lit")//��������
	  {
		  com.push(code[cid].displacement);
		  cid++;
	  }
	  else if (code[cid].funcCode == "lod")//���ر���
	  {  
		  //string tempName = findSymble(code[cid].displacement);
		  //com.push(findVar(tempName));
		  int tempId = 0;
		  if (code[cid].levelDiff == 0)
			  tempId = moveId;
		  else
		  {
			  tempId = staticId;
			  for (int i = 0; i < code[cid].levelDiff; i++)
			  {

				  tempId = action1[tempId];
			  }
		  }
		  com.push(action1[tempId + code[cid].displacement]);
		  cid++;
	  }
	  else if (code[cid].funcCode == "sto")//��ջ������͸�����
	  {
		  //string tempName = findSymble(code[cid].displacement);
		  //tempCount = findVar(tempName);
		  //changeVar(tempName,com.top());
		  int tempId = 0;
		  if(code[cid].levelDiff==0)
			   tempId = moveId;
		  else
		  { 
			   tempId = staticId;
			  for (int i = 0; i < code[cid].levelDiff; i++)
			  {
				  tempId = action1[tempId];
			  }
		  }
		  action1[tempId + code[cid].displacement] = com.top();
		  cid++;
	  }
	  else if (code[cid].funcCode == "cal")//���̵������
	  {
		  // lev = findSymbleAddr(code[cid].displacement);//���²��
		  action1[actId] = moveId;
		  tempMoveId = moveId;
		  moveId = actId;
		  actId++;
		  action1[actId] = cid + 1;//�޸ķ��ص�ַ
		  tempBackId = backId;
		  backId = actId;
		  actId++;

		  if (code[cid].levelDiff == 0)//���Ϊ0
		  {
			  tempStaticId = staticId;
			  action1[actId] = action1[tempMoveId];//�µľ�̬��
			  staticId = actId;
			  actId++;
		  }
		  if (code[cid].levelDiff == 1)//���Ϊ1
		  {
			  tempStaticId = staticId;
			  tempCount = staticId;
			  //tempCount= action1[tempCount];
			  action1[actId] = action1[tempCount];//�µľ�̬��
			  staticId = actId;
			  actId++;
		  } 
		  cid = code[cid].displacement;
	  }
	  else if (code[cid].funcCode == "jpc")//������ת���
	  {
		  if (com.top() == 0)
			  cid = code[cid].displacement;
		  else
			  cid++;
	  }
	  else if (code[cid].funcCode == "int")//���ٿռ�
	  {
		  for (int i = 0; i < code[cid].displacement - 3; i++)//�������ʼ��
		  {
			  action1[actId] = 0;
			  actId++;
		  }
		  cid++;
	  }
	}
}