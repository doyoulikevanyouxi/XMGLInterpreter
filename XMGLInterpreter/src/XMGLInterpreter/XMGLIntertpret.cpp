#include "XMGLIntertpret.h"
#include "XMGLNode/XMGLNode.h"
#include <iostream>
namespace XMGL {
	inline bool XMGLExpression::IsSpaceChar(const char& chr)
	{
		return chr == ' ' || chr == '\t' || chr == '\r' || chr == '\n';
	}
	inline bool XMGLExpression::IsLetter(const char& chr)
	{
		return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z');
	}
	inline bool XMGLExpression::IsNumber(const char& chr)
	{
		return chr >= '0' && chr <= '9';
	}
	void XMGLExpression::SkipSpaceChar(const char*& str, Tag& tag)
	{
		while (*str != '\0') {
			++str;
			tag.column++;
			if (*str == '\r') {
				tag.column = 1;
				continue;
			}
			if (*str == '\n') {
				tag.line++;
				continue;
			}
			else if (!IsSpaceChar(*str)) {
				break;
			}

		}
	}
	void XMGLExpression::RaiseError(const AbstractError& error)
	{
	}

	bool XMLTagNameExpression::Interpret(const char*& str, Tag& tag)
	{
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}
		if (!SetTagType(str, tag)) {
			return false;
		}
		if (!LoadName(str, tag)) {
			return false;
		}
		return true;
	}

	inline bool XMLTagNameExpression::SetTagType(const char*& str, Tag& tag)
	{
		if (IsLetter(*str)) {
			tag.tagType = XMGLTagtType::NormalTag;
			return true;
		}
		switch (*str)
		{
		case '/':
			tag.tagType = XMGLTagtType::CloseTag;
			++str;
			tag.column++;
			return true;
		case '?':
			tag.tagType = XMGLTagtType::Information;
			++str;
			tag.column++;
			return true;
		case '!':
			++str;
			tag.column++;
			if (*str != '-') {
				tag.tagType = XMGLTagtType::InvalidTag;
				return false;
			}
			++str;
			tag.column++;
			if (*str != '-') {
				tag.tagType = XMGLTagtType::InvalidTag;
				return false;
			}
			++str;
			tag.column++;
			return true;
		default:
			tag.tagType = XMGLTagtType::InvalidTag;
			return false;
		}
	}

	/// <summary>
	/// Tag名称字符 验证字符合法性
	/// Tag名称允许字母，数字，下划线和 : 
	/// </summary>
	/// <param name="chr">被验证的字符</param>
	/// <returns>返回是否合法</returns>
	inline bool XMLTagNameExpression::VertifyCharValidation(const char& chr)
	{
		return IsLetter(chr) || IsNumber(chr) || chr == '_' || chr == ':';
	}

	bool XMLTagNameExpression::LoadName(const char*& str, Tag& tag)
	{
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}
		while (*str != '\0') {
			//遇到空字符就退出Tag名称获取阶段
			if (IsSpaceChar(*str)) {
				return true;
			}
			//验证字符的合法性,非法字符，退出名称解析，返回上一级操作，由上级操作解释字符
			if (!VertifyCharValidation(*str)) {
				return false;
			}
			//读取Tag名称
			tag.nodeName += *str;
			++str;
			tag.column++;
		}
		return true;
	}

	bool XMGLPropertyNameExpression::Interpret(const char*& str, Tag& tag)
	{
		//跳出条件为空字符或非法字符
		name.clear();
		while (*str != '\0') {
			if (IsSpaceChar(*str)) {
				return true;
			}
			//验证属性名称合法性
			if (!VertifyCharValidation(*str)) {
				return false;
			}
			//属性名称
			name += *str;
			++str;
			tag.column++;
		}
		return true;
	}

	inline bool XMGLPropertyNameExpression::HandleSpecialChar(const char*& str, Tag& tag)
	{
		if (IsSpaceChar(*str)) {
			return true;
		}
		return false;
	}

	/// <summary>
	/// 属性名称只能由字母，数字，：以及_组成
	/// </summary>
	/// <param name="chr"></param>
	/// <returns></returns>
	inline bool XMGLPropertyNameExpression::VertifyCharValidation(const char& chr)
	{
		return IsLetter(chr) || IsNumber(chr) || chr == ':' || chr == '_';
	}


	bool XMGLPropertyValueExpression::Interpret(const char*& str, Tag& tag)
	{
		value.clear();
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}

		//属性值的第一个非空字符必须为 " 
		if (*str != '"') {
			/*std::string erorrStr = "属性值必须以 \" 为起始字符，而不是" + *str;
			XMLError e(erorrStr.c_str());
			RaiseError(e);
			errorOcurred = true;*/
			std::cout << "属性值必须以\"作为开头,而不是" << *str << std::endl;
			return false;
		}
		++str;
		tag.column++;
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}

		std::string tmp;
		while (*str != '\0') {
			if (*str == '"') {
				++str;
				tag.column++;
				break;
			}
			//验证数据合法性
			if (!VertifyCharValidation(*str)) {
				if (!HandleSpecialChar(str, tag)) {
					return false;
				}
				//只要出现非法数据，都会停止解析
				break;
			}
			//属性值提取
			tmp += *str;
			++str;
			tag.column++;
		}
		//去除属性值中无效空白字符
		while (true) {
			if (IsSpaceChar(tmp.back())) {
				tmp.pop_back();
				continue;
			}
			break;
		}
		if (!tmp.empty()) {
			value = tmp;
		}
		return true;
	}

	/// <summary>
	/// 属性表达式特殊字符处理，只能处理 " 
	/// </summary>
	/// <param name="str"></param>
	/// <param name="tag"></param>
	/// <returns></returns>
	inline bool XMGLPropertyValueExpression::HandleSpecialChar(const char*& str, Tag& tag)
	{

		return false;
	}

	/// <summary>
	/// 属性值数据合法性 ： 只能是字符，数字，空格或Tab ，其他字符无法处理
	/// </summary>
	/// <param name="chr"></param>
	/// <returns></returns>
	inline bool XMGLPropertyValueExpression::VertifyCharValidation(const char& chr)
	{
		if (chr == '<' || chr == '>')
			return false;
		return true;
	}

	/// <summary>
	/// 属性表达式：将字符串分为两个区域 ，属性名称 和 属性值
	/// </summary>
	/// <param name="str"></param>
	/// <param name="tag"></param>
	/// <returns>返回是是良好的处理结果，非良好结果是属性名或属性值非正确解析</returns>
	bool XMGLPropertyExpression::Interpret(const char*& str, Tag& tag)
	{
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}
		//执行属性名称解析
		if (!pNameExpr.Interpret(str, tag)) {
			if (!HandleSpecialChar(str, tag)) {
				//无法处理的特殊字符会导致属性表达式的处理结束
				return false;
			}
		}
		//属性值解析
		if (!pValueExp.Interpret(str, tag)) {
			return false;
		}
		tag.propertys.insert({ pNameExpr.name,pValueExp.value });
		return true;
	}

	/// <summary>
	/// 属性特殊字符处理，该处理只能处理字符 = 和 空字符，如果有其他特殊字符
	/// </summary>
	/// <param name="str"></param>
	/// <param name="tag"></param>
	/// <returns></returns>
	inline bool XMGLPropertyExpression::HandleSpecialChar(const char*& str, Tag& tag)
	{
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}
		//属性表达式必须有符号 =
		if (*str != '=') {
			std::cout << "属性名之后应接 = 而不是" << *str << std::endl;
			return false;
		}
		++str;
		tag.column++;
		return true;
	}

	bool XMGLTagExpression::Interpret(const char*& str, Tag& tag)
	{
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}
		if (*str != '<') {
			std::cout << "标签应该以<为开头，而不是" << *str << std::endl;
			return false;
		}
		++str;
		tag.column++;
		//Tag名称获取:会返回false和true，false代表非正常结束，但不代表此操作就是错误的
		tNameExpr.Interpret(str, tag);
		//检查tag类型，非法类型直接返回
		if (tag.tagType == XMGLTagtType::InvalidTag) {
			return false;
		}
		//关闭标签不需要后续的处理，但是要确定后接非空字符是'>'
		if (tag.tagType == XMGLTagtType::CloseTag) {
			if (IsSpaceChar(*str)) {
				SkipSpaceChar(str, tag);
			}
			if (*str != '>') {
				std::cout << "关闭标签不具备任何属性，应该以>结尾，而不是" << *str << "	" << tag.line << "," << tag.column << std::endl;
				return false;
			}
			++str;
			tag.column++;
			return true;
		}
		//注释节点不需要解析
		if (tag.tagType == XMGLTagtType::NoteTag) {
			while (*str != '\0') {
				if (*str == '\r') {
					tag.column = 1;
				}
				if (*str == '\n') {
					tag.line++;
				}
				if (*str == '-') {
					++str;
					tag.column++;
					if (*str == '-') {
						++str;
						tag.column++;
						if (*str == '>') {
							++str;
							tag.column++;
							return true;
						}
					}
				}
				++str;
				tag.column++;
			}
			return false;
		}
		//该操作是对后续的属性进行解析
		while (*str != '\0') {
			//空字符作为各属性的分割点
			if (IsSpaceChar(*str)) {
				proExpr.Interpret(str, tag);
				continue;
			}
			//属性之间以空字符隔离，所以如果直接是字母为非法字符
			if (IsLetter(*str)) {
				std::cout << "属性因由控制符隔开而不是" << *str << "	" << tag.line << "," << tag.column << std::endl;
				return false;
			}
			//特殊字符处理即非字母字符处理，包括空字符，标点符号，数字等,由于特殊字符意味着开始或结束，所以直接退出该解析
			//这也是Tag所有特殊字符处理的终点
			if (!HandleSpecialChar(str, tag)) {
				return false;
			}
			else {
				break;
			}
		}

		return true;
	}

	/// <summary>
	/// Tag标签特殊字符处理 
	/// </summary>
	/// <param name="str"></param>
	/// <param name="tag"></param>
	/// <returns></returns>
	inline bool XMGLTagExpression::HandleSpecialChar(const char*& str, Tag& tag)
	{
		//以下特殊字符表示者，Tag的解析以及完成
		switch (*str)
		{
		case '>':
			//只能是普通Tag才能直接使用的字符
			if (tag.tagType != XMGLTagtType::NormalTag) {
				return false;
			}
			++str;
			tag.column++;
			return true;
		case '/':
			//只有普通的Tag才能拥有该字符
			if (tag.tagType != XMGLTagtType::NormalTag) {
				return false;
			}
			++str;
			tag.column++;
			if (IsSpaceChar(*str)) {
				SkipSpaceChar(str, tag);
			}
			//且该字符的后接非空字符必须为 >
			if (*str != '>') {
				return false;
			}
			++str;
			tag.column++;
			tag.tagType = XMGLTagtType::SelfCloseTag;
			return true;
		case '?':
			//只有属于系统信息的Tag才能拥有该字符
			if (tag.tagType != XMGLTagtType::Information) {
				return false;
			}
			++str;
			tag.column++;
			if (IsSpaceChar(*str)) {
				SkipSpaceChar(str, tag);
			}
			//且该字符的后接非空字符必须为 >
			if (*str != '>') {
				return false;
			}
			++str;
			tag.column++;
			return true;
		case '-':
			//只有注释节点才拥有该字符
			if (tag.tagType != XMGLTagtType::NoteTag) {
				return false;
			}
			++str;
			tag.column++;
			if (*str != '-') {
				return false;
			}
			++str;
			tag.column++;
			if (*str != '>') {
				return false;
			}
			++str;
			tag.column++;
			return true;
			break;
			//其他特殊字符都属于无法解析的字符
		default:
			return false;
		}
	}
	void XMLInterpreter::start(const char* data)
	{
		Tag tag;
		if (!tExpr.Interpret(data, tag))
			return;
		for (auto& node : tExpr.GetTags()) {
			toString(node);
		}
	}
	void XMLInterpreter::toString(const std::shared_ptr<Tag>& tag)
	{
		std::cout << tag->nodeName <<"  {"<<std::endl;
		if (tag->hasPcData) {
			std::cout << tag->pcData << std::endl;
			return;
		}
		if (tag->hasChilds) {
			for (auto& node : tag->childs) {
				toString(node);
			}
		}
		std::cout << "   }" << std::endl;
	}
	
	bool XMGLInterpret::Interpret(const char*& str, Tag& tag)
	{
		while (*str != '\0') {
			if (IsSpaceChar(*str)) {
				if (*str == '\r') {
					column = 1;

				}
				if (*str == '\n') {
					line++;
				}
				++str;
				continue;
			}

			if (*str == '<') {
				Tag tag;
				tag.line = line;
				tag.column = column;
				if (!tagExpr.Interpret(str, tag)) {
					std::cout << "这不是一个有效的标签 " << tag.line << "," << tag.column << std::endl;
					line = tag.line;
					column = tag.column;
					return false;
				}
				line = tag.line;
				column = tag.column;
				if (!AddTag(tag)) {
					return false;
				}
				continue;
			}
			if (!LoadPcData(str)) {
				return false;
			}
		}
		return true;
	}
	bool XMGLInterpret::AddTag(Tag& tag)
	{
		auto tagPtr = std::make_shared<Tag>(tag);
		switch (tag.tagType)
		{
		case XMGLTagtType::NormalTag:
			if (!unPairTags.empty()) {
				if (unPairTags.top()->hasPcData) {
					std::cout << "节点的pcData与 childs 只能存在一个" << std::endl;
					std::stack<std::shared_ptr<Tag>>().swap(unPairTags);
					return false;
				}
				unPairTags.top()->hasChilds = true;
				unPairTags.top()->childs.push_back(tagPtr);
			}
			unPairTags.push(tagPtr);
			break;
		case XMGLTagtType::CloseTag:
			if (unPairTags.empty()) {
				std::cout << "无效的标签</" << tag.nodeName << ">" << std::endl;
				std::stack<std::shared_ptr<Tag>>().swap(unPairTags);
				return false;
			}
			//非匹配终止标签
			if (!unPairTags.top()->matched(tag)) {
				std::cout << "应该是标签</" << unPairTags.top()->nodeName << ">,而不是</" << tag.nodeName << ">" << std::endl;
				//应该终止解析，并将栈置空
				std::stack<std::shared_ptr<Tag>> ().swap(unPairTags);
				return false;
			}
			if (unPairTags.size() == 1) {
				tags.push_back(unPairTags.top());
			}
			unPairTags.pop();
			break;
		case XMGLTagtType::SelfCloseTag:
			if (unPairTags.empty()) {
				tags.push_back(tagPtr);
				break;
			}
			if (unPairTags.top()->hasPcData) {
				std::cout << "节点的pcData与 childs 只能存在一个" << std::endl;
				std::stack<std::shared_ptr<Tag>>().swap(unPairTags);
				return false;
			}
			unPairTags.top()->hasChilds = true;
			unPairTags.top()->childs.push_back(tagPtr);
			break;
		case XMGLTagtType::NoteTag:
			break;
		default:
			break;
		}
		return true;
	}

	bool XMGLInterpret::LoadPcData(const char*& str)
	{
		/*Tag t;
		t.line = line;
		t.column = column;
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, t);
		}
		line = t.line;
		column = t.column;*/
		if (unPairTags.empty()) {
			std::cout<<"未知的数据"<< line << "," << column << std::endl;
			return false;
		}
		if (unPairTags.top()->hasChilds) {
			std::cout << "节点的pcData与 childs 只能存在一个" << line << "," << column << std::endl;
			std::stack<std::shared_ptr<Tag>>().swap(unPairTags);
			return false;
		}
		std::string tmp;
		while (*str != '\0') {
			if (*str == '\r') {
				column = 1;
				++str;
				continue;
			}	
			if (*str == '\n') {
				line++;
				++str;
				continue;
			}
			if (!VertifyCharValidation(*str)) {
				std::cout<<"非法字符"<<*str<<" "<< line << "," << column << std::endl;
				std::stack<std::shared_ptr<Tag>>().swap(unPairTags);
				return false;
			}
			if (*str == '<')
				break;
			tmp += *str;
			++str;
			column++;
		}
		//去除属性值中无效空白字符
		while (true) {
			if (IsSpaceChar(tmp.back())) {
				tmp.pop_back();
				continue;
			}
			break;
		}
		if (!tmp.empty()) {
			unPairTags.top()->hasPcData = true;
			unPairTags.top()->pcData = tmp;
		}
		return true;
	}
	inline bool XMGLInterpret::VertifyCharValidation(const char& chr)
	{
		return !(chr=='>');
	}
}
