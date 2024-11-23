#pragma once
#include "XMGL.h"
#include "XMGLNode/XMGLNode.h"
#include "XMGLAbstractExpression.h"
#include <vector>
#include <stack>
/// <summary>
/// �����xml��ʽ���ݵĽ������üܹ��������ı����ݺ���Ϊ��׼����ͬ�ı���ﲻͬ���� ex:< nodeName  propertyName> �������
/// ����������Ϊ��������Ϊ��ʼ�������������ζ�ȡ �� tag��ǩ---> [tag���� ---> tag ����] ---> [tag���Ա��ʽ--->{tag�������� ---> tag����ֵ}]
/// ������Ϊһ�����壬ÿ�������޷��������ַ������ϴ��ݣ�ÿ���׶��޷��������ַ�����ֹ���׶εĽ������������ַ�ת�����ϸ��׶δ���������н׶�
/// ���޷�������ô�������ڽ��н�������
/// </summary>		
namespace XMGL {
	////char�ַ�������
	//class XMGLCharFilter {
	//public:
	//	enum {
	//		Default = bit(0),
	//		None = bit(1),
	//		Letters = bit(2),
	//		Numbers = bit(3),
	//		SpecificChars = bit(4),
	//	};
	//public:
	//	bool Filt(const char& chr) {
	//		
	//	}
	//public:
	//	std::vector<char> expectChars;

	//};

	class XMGLExpression : public XMLAbstractExpression {
	public:
		inline bool IsSpaceChar(const char& chr);
		inline bool IsLetter(const char& chr);
		inline bool IsNumber(const char& chr);
		bool errorOccurred = false;
	protected:
		void SkipSpaceChar(const char*& str, Tag& tag);
		void RaiseError(const AbstractError& error) override;
	};

	class XMLTagNameExpression : public XMGLExpression {
	public:
		XMLTagNameExpression() = default;
		~XMLTagNameExpression() = default;
		bool Interpret(const char*& str, Tag& tag) override;
	private:
		inline bool SetTagType(const char*& str,Tag& tag);
		inline bool VertifyCharValidation(const char& chr);
		bool LoadName(const char*& str, Tag& tag);
	};

	class XMGLPropertyNameExpression : public XMGLExpression {
	public:
		XMGLPropertyNameExpression() = default;
		~XMGLPropertyNameExpression() = default;
		bool Interpret(const char*& str, Tag& tag) override;
	private:
		inline bool HandleSpecialChar(const char*& str, Tag& tag);
		inline bool VertifyCharValidation(const char& chr);
	public:
		std::string name;
	};

/// <summary>
/// XML˫���Ž���������һ���ս���ʽ
/// </summary>
	class XMGLPropertyValueExpression : public XMGLExpression {
	public:
		XMGLPropertyValueExpression() = default;
		~XMGLPropertyValueExpression() = default;
		// ͨ�� AbstractExpression �̳�
		bool Interpret(const char*& str, Tag& tag) override;
	private:
		inline bool HandleSpecialChar(const char*& str, Tag& tag);
		inline bool VertifyCharValidation(const char& chr);
	public:
		std::string value;
	};

	/// <summary>
	/// XML���Խ���---�ⲻ��һ���ս���ʽ������洢�������ս���ʽ
	/// ���Ա��ʽֻ��������Ч��ʽ   propertyName�հ��ַ�=�հ��ַ�value
	/// </summary>
	class XMGLPropertyExpression : public XMGLExpression {
	public:
		XMGLPropertyExpression() = default;
		~XMGLPropertyExpression() = default;
		// ͨ�� AbstractExpression �̳�
		bool Interpret(const char*& str, Tag& tag) override;
		inline bool HandleSpecialChar(const char*& str, Tag& tag);
	private:
		XMGLPropertyNameExpression pNameExpr;
		XMGLPropertyValueExpression pValueExp;
		//���������ֵ��ַ�
		//std::vector<char> unexpectChars;
	};

	/// <summary>
	/// XML�ڵ����Խ���
	/// </summary>
	class XMGLTagExpression : public XMGLExpression {
	public:

		XMGLTagExpression() = default;
		~XMGLTagExpression() = default;
		// ͨ�� AbstractExpression �̳�
		bool Interpret(const char*& str, Tag& tag) override;
	private:
		//�����ַ�����,Ҳ������ֹ������ֻҪ�������ַ����ǶԱ������Ľ�����ʾ
		inline bool HandleSpecialChar(const char*& str, Tag& tag);
	private:
		XMGLPropertyExpression proExpr;
		XMLTagNameExpression tNameExpr;
		bool matchStatus = false;
		bool hasbeenClose = false;
	};

	class XMGLInterpret : public XMGLExpression {
	public:
		XMGLInterpret() = default;
		~XMGLInterpret() = default;
		// ͨ�� XMGLExpression �̳�
		bool Interpret(const char*& str, Tag& tag) override;
	public:
		inline auto& GetTags() const { return tags; }
	private:
		bool AddTag(Tag& tag);
		bool LoadPcData(const char*& str);
		inline bool VertifyCharValidation(const char& chr);
	private:
		XMGLTagExpression tagExpr;
		std::stack<std::shared_ptr<Tag>> unPairTags;
		std::vector<std::shared_ptr<Tag>> tags;
		size_t line =1;
		size_t column =1;
	};

	/// <summary>
	/// XML�����������ƽ������̣��������
	/// </summary>
	class DLLAPI XMLInterpreter {
	public:
		XMLInterpreter() = default;
		//�������
		//void OnXMLErrorOccured(const AbstractError& e);
		void start(const char* data);
		void toString(const std::shared_ptr<Tag>& tag);
	private:
		XMGLInterpret tExpr;
	};

}
