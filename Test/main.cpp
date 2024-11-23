
//class __declspec(dllimport) XMGLIntertpret;
#include <XMGLInterpreter/XMGLIntertpret.h>
#include<fstream>
#include <iostream>
int main() {
	std::string str;
	std::ifstream file("ts.txt");
	if (!file.is_open()) {
		std::cout << "无法打开文件" << std::endl;
		return 0;
	}
	std::string sd;
	while (std::getline(file, sd)) {
		str += sd;
		str += "\r\n";
	}
	XMGL::XMLInterpreter X;
	X.start(str.c_str());
	file.close();
	return 0;
}