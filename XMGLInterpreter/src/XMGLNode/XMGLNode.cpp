#include "XMGLNode.h"
namespace XMGL {
	Tag::Tag(const Tag& other)
	{
		hasChilds = other.hasChilds;
		hasPcData = other.hasChilds;
		tagType = other.tagType;
		nodeName = other.nodeName;
		line = other.line;
		column = other.column;
		if(!other.propertys.empty())
			propertys = other.propertys;
		if (!other.childs.empty())
			childs = other.childs;
		if (!other.pcData.empty())
			pcData = other.pcData;
	}
}
