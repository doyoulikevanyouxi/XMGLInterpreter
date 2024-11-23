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
	/// Tag�����ַ� ��֤�ַ��Ϸ���
	/// Tag����������ĸ�����֣��»��ߺ� : 
	/// </summary>
	/// <param name="chr">����֤���ַ�</param>
	/// <returns>�����Ƿ�Ϸ�</returns>
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
			//�������ַ����˳�Tag���ƻ�ȡ�׶�
			if (IsSpaceChar(*str)) {
				return true;
			}
			//��֤�ַ��ĺϷ���,�Ƿ��ַ����˳����ƽ�����������һ�����������ϼ����������ַ�
			if (!VertifyCharValidation(*str)) {
				return false;
			}
			//��ȡTag����
			tag.nodeName += *str;
			++str;
			tag.column++;
		}
		return true;
	}

	bool XMGLPropertyNameExpression::Interpret(const char*& str, Tag& tag)
	{
		//��������Ϊ���ַ���Ƿ��ַ�
		name.clear();
		while (*str != '\0') {
			if (IsSpaceChar(*str)) {
				return true;
			}
			//��֤�������ƺϷ���
			if (!VertifyCharValidation(*str)) {
				return false;
			}
			//��������
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
	/// ��������ֻ������ĸ�����֣����Լ�_���
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

		//����ֵ�ĵ�һ���ǿ��ַ�����Ϊ " 
		if (*str != '"') {
			/*std::string erorrStr = "����ֵ������ \" Ϊ��ʼ�ַ���������" + *str;
			XMLError e(erorrStr.c_str());
			RaiseError(e);
			errorOcurred = true;*/
			std::cout << "����ֵ������\"��Ϊ��ͷ,������" << *str << std::endl;
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
			//��֤���ݺϷ���
			if (!VertifyCharValidation(*str)) {
				if (!HandleSpecialChar(str, tag)) {
					return false;
				}
				//ֻҪ���ַǷ����ݣ�����ֹͣ����
				break;
			}
			//����ֵ��ȡ
			tmp += *str;
			++str;
			tag.column++;
		}
		//ȥ������ֵ����Ч�հ��ַ�
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
	/// ���Ա��ʽ�����ַ�����ֻ�ܴ��� " 
	/// </summary>
	/// <param name="str"></param>
	/// <param name="tag"></param>
	/// <returns></returns>
	inline bool XMGLPropertyValueExpression::HandleSpecialChar(const char*& str, Tag& tag)
	{

		return false;
	}

	/// <summary>
	/// ����ֵ���ݺϷ��� �� ֻ�����ַ������֣��ո��Tab �������ַ��޷�����
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
	/// ���Ա��ʽ�����ַ�����Ϊ�������� ���������� �� ����ֵ
	/// </summary>
	/// <param name="str"></param>
	/// <param name="tag"></param>
	/// <returns>�����������õĴ������������ý����������������ֵ����ȷ����</returns>
	bool XMGLPropertyExpression::Interpret(const char*& str, Tag& tag)
	{
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}
		//ִ���������ƽ���
		if (!pNameExpr.Interpret(str, tag)) {
			if (!HandleSpecialChar(str, tag)) {
				//�޷�����������ַ��ᵼ�����Ա��ʽ�Ĵ������
				return false;
			}
		}
		//����ֵ����
		if (!pValueExp.Interpret(str, tag)) {
			return false;
		}
		tag.propertys.insert({ pNameExpr.name,pValueExp.value });
		return true;
	}

	/// <summary>
	/// ���������ַ������ô���ֻ�ܴ����ַ� = �� ���ַ�����������������ַ�
	/// </summary>
	/// <param name="str"></param>
	/// <param name="tag"></param>
	/// <returns></returns>
	inline bool XMGLPropertyExpression::HandleSpecialChar(const char*& str, Tag& tag)
	{
		if (IsSpaceChar(*str)) {
			SkipSpaceChar(str, tag);
		}
		//���Ա��ʽ�����з��� =
		if (*str != '=') {
			std::cout << "������֮��Ӧ�� = ������" << *str << std::endl;
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
			std::cout << "��ǩӦ����<Ϊ��ͷ��������" << *str << std::endl;
			return false;
		}
		++str;
		tag.column++;
		//Tag���ƻ�ȡ:�᷵��false��true��false�����������������������˲������Ǵ����
		tNameExpr.Interpret(str, tag);
		//���tag���ͣ��Ƿ�����ֱ�ӷ���
		if (tag.tagType == XMGLTagtType::InvalidTag) {
			return false;
		}
		//�رձ�ǩ����Ҫ�����Ĵ�������Ҫȷ����ӷǿ��ַ���'>'
		if (tag.tagType == XMGLTagtType::CloseTag) {
			if (IsSpaceChar(*str)) {
				SkipSpaceChar(str, tag);
			}
			if (*str != '>') {
				std::cout << "�رձ�ǩ���߱��κ����ԣ�Ӧ����>��β��������" << *str << "	" << tag.line << "," << tag.column << std::endl;
				return false;
			}
			++str;
			tag.column++;
			return true;
		}
		//ע�ͽڵ㲻��Ҫ����
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
		//�ò����ǶԺ��������Խ��н���
		while (*str != '\0') {
			//���ַ���Ϊ�����Եķָ��
			if (IsSpaceChar(*str)) {
				proExpr.Interpret(str, tag);
				continue;
			}
			//����֮���Կ��ַ����룬�������ֱ������ĸΪ�Ƿ��ַ�
			if (IsLetter(*str)) {
				std::cout << "�������ɿ��Ʒ�����������" << *str << "	" << tag.line << "," << tag.column << std::endl;
				return false;
			}
			//�����ַ���������ĸ�ַ������������ַ��������ţ����ֵ�,���������ַ���ζ�ſ�ʼ�����������ֱ���˳��ý���
			//��Ҳ��Tag���������ַ�������յ�
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
	/// Tag��ǩ�����ַ����� 
	/// </summary>
	/// <param name="str"></param>
	/// <param name="tag"></param>
	/// <returns></returns>
	inline bool XMGLTagExpression::HandleSpecialChar(const char*& str, Tag& tag)
	{
		//���������ַ���ʾ�ߣ�Tag�Ľ����Լ����
		switch (*str)
		{
		case '>':
			//ֻ������ͨTag����ֱ��ʹ�õ��ַ�
			if (tag.tagType != XMGLTagtType::NormalTag) {
				return false;
			}
			++str;
			tag.column++;
			return true;
		case '/':
			//ֻ����ͨ��Tag����ӵ�и��ַ�
			if (tag.tagType != XMGLTagtType::NormalTag) {
				return false;
			}
			++str;
			tag.column++;
			if (IsSpaceChar(*str)) {
				SkipSpaceChar(str, tag);
			}
			//�Ҹ��ַ��ĺ�ӷǿ��ַ�����Ϊ >
			if (*str != '>') {
				return false;
			}
			++str;
			tag.column++;
			tag.tagType = XMGLTagtType::SelfCloseTag;
			return true;
		case '?':
			//ֻ������ϵͳ��Ϣ��Tag����ӵ�и��ַ�
			if (tag.tagType != XMGLTagtType::Information) {
				return false;
			}
			++str;
			tag.column++;
			if (IsSpaceChar(*str)) {
				SkipSpaceChar(str, tag);
			}
			//�Ҹ��ַ��ĺ�ӷǿ��ַ�����Ϊ >
			if (*str != '>') {
				return false;
			}
			++str;
			tag.column++;
			return true;
		case '-':
			//ֻ��ע�ͽڵ��ӵ�и��ַ�
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
			//���������ַ��������޷��������ַ�
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
					std::cout << "�ⲻ��һ����Ч�ı�ǩ " << tag.line << "," << tag.column << std::endl;
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
					std::cout << "�ڵ��pcData�� childs ֻ�ܴ���һ��" << std::endl;
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
				std::cout << "��Ч�ı�ǩ</" << tag.nodeName << ">" << std::endl;
				std::stack<std::shared_ptr<Tag>>().swap(unPairTags);
				return false;
			}
			//��ƥ����ֹ��ǩ
			if (!unPairTags.top()->matched(tag)) {
				std::cout << "Ӧ���Ǳ�ǩ</" << unPairTags.top()->nodeName << ">,������</" << tag.nodeName << ">" << std::endl;
				//Ӧ����ֹ����������ջ�ÿ�
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
				std::cout << "�ڵ��pcData�� childs ֻ�ܴ���һ��" << std::endl;
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
			std::cout<<"δ֪������"<< line << "," << column << std::endl;
			return false;
		}
		if (unPairTags.top()->hasChilds) {
			std::cout << "�ڵ��pcData�� childs ֻ�ܴ���һ��" << line << "," << column << std::endl;
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
				std::cout<<"�Ƿ��ַ�"<<*str<<" "<< line << "," << column << std::endl;
				std::stack<std::shared_ptr<Tag>>().swap(unPairTags);
				return false;
			}
			if (*str == '<')
				break;
			tmp += *str;
			++str;
			column++;
		}
		//ȥ������ֵ����Ч�հ��ַ�
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
