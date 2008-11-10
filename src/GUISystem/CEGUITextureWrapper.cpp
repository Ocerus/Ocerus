#include "CEGUITextureWrapper.h"
#include "../Utility/Settings.h"
#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "GUIMgr.h"
#include "../ResourceSystem/ResourceMgr.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/Texture.h"
#include "CEGUIResource.h"
#include "RendererGate.h"
#include <sstream>

namespace GUISystem {

	void CEGUITextureWrapper::loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup) {
		mTexture.SetNull();
		mTexture = (GfxSystem::TexturePtr)gResourceMgr.GetResource(resourceGroup.c_str(), filename.c_str());
	}

	CEGUITextureWrapper::CEGUITextureWrapper(RendererGate * gate) : CEGUI::Texture(gate) {
	}

	void CEGUITextureWrapper::loadFromMemory(const void* buffPtr, CEGUI::uint buffWidth,
		CEGUI::uint buffHeight, CEGUI::Texture::PixelFormat pixelFormat) {

		uint32 bytesize = ((buffWidth * sizeof(uint32)) * buffHeight); 
		std::string name = GetNextTextureName();
		gResourceMgr.AddResourceMemoryToGroup(buffPtr, bytesize, name, mResourceGroupName, ResourceSystem::Resource::TYPE_TEXTURE);
						
		mTexture.SetNull();
		mTexture = (GfxSystem::TexturePtr)gResourceMgr.GetResource(mResourceGroupName, name);		

	}

	CEGUITextureWrapper::~CEGUITextureWrapper(void) {
		mTexture.SetNull();
	}

	std::string CEGUITextureWrapper::GetNextTextureName() {
		std::stringstream sstream("CEGUI_CUSTOM_TEXTURE_");
		sstream << mTextureIndex++;
		return sstream.str();
	}
}