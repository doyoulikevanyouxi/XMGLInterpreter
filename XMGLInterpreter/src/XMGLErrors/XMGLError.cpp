#include "XMGLError.h"
#include<iostream>

XMGL::XMGLInvalidString::XMGLInvalidString(unsigned int line, unsigned int column) :AbstractError(0xff, line, column)
{
}

XMGL::XMGLInvalidString::XMGLInvalidString(const char* invalidStr, unsigned int line, unsigned int column) :AbstractError(0xff, line, column)
{
	if (invalidStr != nullptr) {
		message = invalidStr;
	}
}

void XMGL::XMGLInvalidString::ErrorMessage() const
{
	std::cout << "无效的字符串 " << message << "	" << line << "," << column << std::endl;
}

XMGL::XMGLUnexpectedSymbol::XMGLUnexpectedSymbol(unsigned int line, unsigned int column) :AbstractError(0xff00, line, column), symbol(0)
{
}

XMGL::XMGLUnexpectedSymbol::XMGLUnexpectedSymbol(const char symbol, unsigned int line, unsigned int column) :AbstractError(0xff00, line, column), symbol(symbol)
{
}

void XMGL::XMGLUnexpectedSymbol::ErrorMessage() const
{
	if (symbol == 0)
		return;
	std::cout << "意外的字符 " << symbol << "	" << line << "," << column << std::endl;
}

XMGL::XMGLError::XMGLError(const char* str, unsigned int line, unsigned int column)
{
	if (str != nullptr) {
		message = str;
	}
}

void XMGL::XMGLError::ErrorMessage() const
{
	std::cout << message << "	" << line << "," << column << std::endl;
}
