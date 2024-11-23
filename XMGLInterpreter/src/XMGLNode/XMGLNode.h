#pragma once
#include<string>
#include<map>
#include<vector>
#include <memory>
namespace XMGL {
	enum XMGLTagtType {
		Information,
		//ע�ͱ�ǩ
		NoteTag,
		//��ͨ��ǩ
		NormalTag,
		//�رձ�ǩ
		CloseTag,
		//�Աձ�ǩ
		SelfCloseTag,
		//��Ч��ǩ
		InvalidTag
	};

	class Tag {
	public:
		Tag() = default;
		Tag(const Tag& other);
	public:
		bool hasChilds = false;
		bool hasPcData = false;
		XMGLTagtType tagType;
		//�ڵ�����
		std::string nodeName;
		std::string pcData;
		std::vector<std::shared_ptr<Tag>> childs;
		size_t line;
		size_t column;
		//�ڵ�����
		std::map<std::string, std::string> propertys;
		bool matched(Tag& other) {
			if (this->nodeName == other.nodeName) {
				if (this->tagType == NormalTag && other.tagType == CloseTag)
					return true;
				if ((this->tagType == CloseTag && other.tagType == NormalTag))
					return true;
				return false;
			}
			return false;
		}
	};
}


