#pragma once
#include<iostream>
#include<string>
#include <fstream>
#include <sstream>
#include<Windows.h>
#include"recursion.h"
using namespace std;
char ch;//�ַ�������������¶�����Դ�����ַ�
char strToken[100];//�ַ����飬��Ź��ɵ��ʷ��ŵ��ַ���
int len = 0;//��¼�ַ����ĳ���
string klabel[29] = { "+","-","*","/","=","#","<","<=",">",">=",",",";",":=","(",")",".","const","var","procedure","begin","end","odd","if","then","call"
,"while","do","read","write" };
string slabel[100] = {};
string  clabel[100] = {};
string result[200] = {};
int ip = 0;
int slen = 0;//��ʶ������±�
int clen = 0;//��������±�
			 //�ж��Ƿ�Ϊ����
const char *input;
int index = 0;
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
//��strToken�е��ַ������ұ����ֱ�������һ���������򷵻��򷵻����ı��룬���򷵻�-1ֵ
int Reserve()
{

	for (int i = 0; i < 26; i++)
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
struct symbel
{
	string name;
	string kind;
	int level;
	int addr;
	int value;
}symlabel;
//������
int main()
{
	int code, value, num = 0;
	/*cout << "�����뵥�ʳ���"<<endl;
	cin >> num;
	cout << "�����뵥�ʣ�"<<endl;
	cin >> input;*/
	//char filename[] = "test.txt";
	//const char *file = readFileIntoString(filename).data();
	std::ifstream in("test.txt");
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
				insertId();
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
			insertConst();
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
	for (int i = 0; i < 100; i++)
		cout << result[i] << " ";
	cout << endl;
	for (int i = 0; i < 100; i++)
		cout << slabel[i] << " ";
	cout << endl;
	for (int i = 0; i < 100; i++)
		cout << clabel[i] << " ";
	system("pause");
	return 0;
}