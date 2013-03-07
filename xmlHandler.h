#ifndef _XMLHANDLER_H_
#define _XMLHANDLER_H_

#ifdef XMLHANDLER_EXPORTS // inside DLL
#define XMLHANDLERAPI
#else // outside DLL
#define XMLHANDLERAPI
#endif // XMLHANDLER_EXPORT
#include <sstream>
#include <string>
using namespace std;


//��XML�ĵ����������
class XMLHANDLERAPI MXMLHandler
{
public:
	static string CreateNode(string name, const string& value = "");
	static string GetNode(const string& xml, string name);
	static string GetNode(const string& xml, string name, unsigned long& start);
	static string GetNodeWithTag(const string& xml, string name);
	static bool isExistNode(const string& xml, string name);
	static bool GetNextNode(const string& xml, string& name, string& value, unsigned long& start);
	static string GetNextNode(string& xml, string name);
	static bool SetNode(string& xml, string name, string value);
	static bool SetNode(string& xml, string name, string value, unsigned long& start);
	static bool RemoveNode(string& xml, string name);
	static void RemoveAllNode(string& xml, string name);
	static string Escape(string s);
	static string deEscape(string s);
private:
	static void deEscapeHelp(string& str, string old, char newChar);
};

//���¼���XML������Ƶ��ʹ�ã������Ӧ������������Ϊ��д�ı���
inline XMLHANDLERAPI string createXMLNode(string name, const string& value = "")
{
	return MXMLHandler::CreateNode(name, value);
}

inline XMLHANDLERAPI string createXMLNode(string name, int value)
{
    stringstream ss;
    string str;
    ss << value;
    ss >> str;
    return MXMLHandler::CreateNode(name, str);
}

inline XMLHANDLERAPI string getXMLNode(const string& xml, string name)
{
	return MXMLHandler::GetNode(xml, name);
}

inline XMLHANDLERAPI string getXMLNode(const string& xml, string name, unsigned long& start)
{
	return MXMLHandler::GetNode(xml, name, start);
}

inline XMLHANDLERAPI bool isExistXMLNode(const string& xml, string name)
{
	return MXMLHandler::isExistNode(xml, name);
}

inline XMLHANDLERAPI bool getNextXMLNode(const string& xml, string& name, string& value, unsigned long& start)
{
	return MXMLHandler::GetNextNode(xml, name, value, start);
}

inline XMLHANDLERAPI string getNextXMLNode(string& xml, string name)
{
	return MXMLHandler::GetNextNode(xml, name);
}
#endif
