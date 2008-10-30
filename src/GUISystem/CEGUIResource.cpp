#include "CEGUIResource.h"
#include <ios>
#include <sstream>

using namespace GUISystem;

ResourceSystem::ResourcePtr CEGUIResource::CreateMe()
{
	return ResourceSystem::ResourcePtr(DYN_NEW CEGUIResource());
}

bool CEGUIResource::LoadImpl()
{
	InputStream& input = OpenInputStream(ISM_BINARY);
	std::stringstream sstr;	
	mDataBlockSize = 0;

	while (input.good()) {
		sstr << input;
		mDataBlockSize += input.gcount();
	}
	
	mDataBlock = DYN_NEW uint8[mDataBlockSize];
	sstr.read((char *)mDataBlock, mDataBlockSize);
	
	CloseInputStream();
	return true;
}

bool CEGUIResource::UnloadImpl()
{
	DYN_DELETE_ARRAY mDataBlock;
	return true;
}
