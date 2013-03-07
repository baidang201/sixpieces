#include "xmlHandler.h"

string MXMLHandler::CreateNode(string name, const string& value)
{
	Escape(name);
	if (value == "")
		return "<"+name+"/>\n";

	return "<"+name+">" + value + "</"+name+">\n";
}

string MXMLHandler::GetNode(const string& xml, string name)
{
	unsigned long noUse = 0;
	return GetNode(xml, name, noUse);
}

string MXMLHandler::GetNode(const string& xml, string name, unsigned long& start)
{
	Escape(name);
	string::size_type begin = xml.find("<"+name+">", start);
	if (begin == string::npos)
		return "";

	begin += name.length() + 2;

	string::size_type end = xml.find("</"+name+">", begin);

	if (end == string::npos)
		return "";

	start = end + name.length() + 3;
	return xml.substr(begin, end - begin);
}

string MXMLHandler::GetNodeWithTag(const string& xml, string name)
{
	Escape(name);
	string::size_type begin = xml.find("<"+name+">");
	if (begin == string::npos)
		return "";

	string::size_type end = xml.find("</"+name+">", begin);
	if (end == string::npos)
		return "";

	end += name.length() + 3;
	return xml.substr(begin, end - begin);
}

bool MXMLHandler::isExistNode(const string& xml, string name) 
{
	string::size_type begin = xml.find("<"+name+"/>");
	if (begin != string::npos )
		return true;

	begin = xml.find("<"+name+">");
	if (begin == string::npos)
		return false;

	begin += name.length() + 2;

	if (xml.find("</"+name+">", begin) == string::npos)
		return false;

	return true;
}
//取回从start位置开始的第一个节点的name和value，并将start设置为该节点的下一个位置
bool MXMLHandler::GetNextNode(const string& xml, string& name, string& value, unsigned long& start)
{
	if (start >= xml.length())
		return false;

	string::size_type begin = xml.find("<", start);
	if (begin == string::npos)
		return false;

	string::size_type end = xml.find(">", begin);
	if (end == string::npos)
		return false;

	name = xml.substr(begin + 1, end - begin - 1);
	if ((name.length() > 0) && (name[name.length() - 1] == '/'))
	{
		name = name.substr(0, name.length() - 1);
		value = "";
		start = end + 1;
		return true;
	}

	begin = end + 1;
	end = xml.find ("</"+name+">", begin);
	if (end == string::npos) 
		return false;

	value = xml.substr(begin, end - begin);
	start = end + name.length() + 3;
	deEscape(name);
	return true;
}
//取回xml中下一个名为name的节点，并将xml修改为余下的xml字符串
string MXMLHandler::GetNextNode(string& xml, string name) 
{
	string::size_type begin = xml.find("<"+name+">");
	if (begin == string::npos)
		return "";

	begin += name.length() + 2;
	string::size_type end = xml.find("</"+name+">", begin);
	if (end == string::npos)
		return "";

	string rt = xml.substr(begin, end - begin);
	end += name.length() + 3;
	xml = xml.substr(end, xml.length() - end);
	return rt;
}

bool MXMLHandler::SetNode(string& xml, string name, string value)
{
	Escape(name);
	if (name.length() == 0)
		return false;

	if (value.length() == 0)
	{
		RemoveNode(xml, name);
		return true;
	}

	string::size_type begin = xml.find("<"+name+">");
	string::size_type end = xml.find("</"+name+">", begin + name.length() + 2);
	if ((begin != string::npos) && (end != string::npos))
	{
		begin += name.length() + 2;
		xml = xml.substr(0, begin) + value + xml.substr(end, xml.length() - end);
		return true;
	}

	string newnode = CreateNode(name, value);
	begin = xml.find("<"+name+"/>");
	if (begin != string::npos)
	{
		string left = xml.substr(0, begin);

		begin += name.length() + 3;
		while ((begin < xml.length()) && ((xml[begin] == '\n') || (xml[begin] == '\r')))
			begin++;

		string right = xml.substr (begin, xml.length());
		xml = left + newnode + right;
		return true;
	}
	else{
		xml += newnode;
		return false;
	}
}

bool MXMLHandler::SetNode(string& xml, string name, string value, unsigned long& start)
{
	Escape(name);
	if (name.length() == 0)
		return false;

	if (value.length() == 0)
	{
		MXMLHandler::RemoveNode(xml, name);
		return true;
	}

	string::size_type begin = xml.find("<"+name+">",start);
	string::size_type end = xml.find("</"+name+">", begin + name.length() + 2);

	if ((begin != string::npos) && (end != string::npos))
	{
		begin += name.length() + 2;
		xml = xml.substr(0, begin) + value + xml.substr(end, xml.length() - end);
		start = begin + value.length() + name.length() + 3;
		return true;
	}
	return false;
}

bool MXMLHandler::RemoveNode(string& xml, string name)
{
	Escape(name);
	if (name.length() == 0)
		return false;

	string::size_type begin = xml.find("<"+name+"/>");
	string::size_type end;
	string right;
	if (begin != string::npos ) 
	{
		end = begin + name.length() + 3;
		while(end != name.length() && (xml[end] == '\r' || xml[end] == '\n'))
			end++;
		right = xml.substr(end, xml.length() - end);
		xml = xml.substr(0, begin) + right;
		return true;
	}

	begin = xml.find("<"+name+">");
	if (begin == string::npos)
		return false;

	end = xml.find("</"+name+">", begin + name.length() + 2);
	if (end == string::npos)
		return false;

	end += name.length() + 3;
	while(end != name.length() && (xml[end] == '\r' || xml[end] == '\n'))
		end++;
	right = xml.substr(end, xml.length() - end);
	xml = xml.substr(0, begin) + right;
	return true;
}

void MXMLHandler::RemoveAllNode(string& xml, string name)
{
	while(MXMLHandler::RemoveNode(xml, name))
		;
}
//将字符串中的&、<、>、"字符进行转义
//  & -> &amp,  < -> &lt
//  > -> &gt,  " -> &quot,  ' -> &apos 
string MXMLHandler::Escape(string s) 
{
	string str = s;
	int size = str.size();
	for ( int i=0; i<size; i++ ) {
		switch (str[i]) {
		case '&':
			str = str.substr(0,i) + "&amp;" + str.substr(i+1);
			i += 4;
			size += 4;
			break;
		case '<':
			str = str.substr(0,i) + "&lt;" + str.substr(i+1);    //原来中间的转义字符写的是 “&it”  modified by tang hanhong 2011-8-4
			i += 3;
			size += 3;
			break;
		case '>':
			str = str.substr(0,i) + "&gt;" + str.substr(i+1); 
			i += 3;
			size += 3;
			break;
		case '"':
			str = str.substr(0,i) + "&quot;" + str.substr(i+1);
			i += 5;
			size += 5;
			break;
		case '\'':
			str = str.substr(0,i) + "&apos;" + str.substr(i+1);
			i += 5;
			size += 5;
			break;
		}
	}
	return str;
}

void MXMLHandler::deEscapeHelp(string& str, string old, char newChar) 
{
	int index = 0;
	while (index != -1){
		index = str.find(old, index);
		if (index != -1){
			str = str.substr(0, index) + newChar + str.substr(index + old.size());
		}
	}
}

string MXMLHandler::deEscape(string s)
{
	string str = s;
	deEscapeHelp(str, "&amp;", '&');
	deEscapeHelp(str, "&it;", '<');
	deEscapeHelp(str, "&gt;", '>');
	deEscapeHelp(str, "&quot;", '"');
	deEscapeHelp(str, "&apos;", '\'');
	return str;
}
