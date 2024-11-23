#pragma once
#include "XMLGAbstractError.h"

namespace XMGL {
	/// <summary>
/// ��Ч�ַ���
/// </summary>
class XMGLInvalidString :public AbstractError
{
public:  
	XMGLInvalidString(unsigned int line = 0, unsigned int column = 0);
	XMGLInvalidString(const char* invalidStr = nullptr, unsigned int line = 0,unsigned int column = 0);
	// ͨ�� AbstractError �̳�
	void ErrorMessage() const override;
};

/// <summary>
/// ������ַ�
/// </summary>
class XMGLUnexpectedSymbol : public AbstractError {
public:
	XMGLUnexpectedSymbol(unsigned int line = 0, unsigned int column = 0);
	XMGLUnexpectedSymbol(const char symbol, unsigned int line = 0, unsigned int column = 0);
	// ͨ�� AbstractError �̳�
	void ErrorMessage() const override;
private:
	char symbol;
};

class XMGLError : public AbstractError {
public:
	XMGLError(const char* str,unsigned int line = 0, unsigned int column = 0);
	// ͨ�� AbstractError �̳�
	void ErrorMessage() const override;
private:
};
}

