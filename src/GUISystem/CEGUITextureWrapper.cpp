#include "Common.h"
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

namespace GUISystem {

	uint32 CEGUITextureWrapper::mTextureIndex = 0;
	const String CEGUITextureWrapper::mResourceGroupName = "CEGUI_created";

	void CEGUITextureWrapper::loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup) {
		mTexture.SetNull();
		mTexture = (GfxSystem::TexturePtr)gResourceMgr.GetResource(resourceGroup.c_str(), filename.c_str());		
		mOriginalHeight = mTexture->GetHeight();
		mOriginalWidth = mTexture->GetWidth();		
	}

	CEGUITextureWrapper::CEGUITextureWrapper(RendererGate * gate) : CEGUI::Texture(gate) {
	}

	void CEGUITextureWrapper::loadFromMemory(const void* buffPtr, CEGUI::uint buffWidth,
		CEGUI::uint buffHeight, CEGUI::Texture::PixelFormat pixelFormat) {

		mTexture.SetNull();

		uint32 bytesize = ((buffWidth * sizeof(uint32)) * buffHeight);

		String name = GetNextTextureName();
		gResourceMgr.AddManualResourceToGroup(name, mResourceGroupName, ResourceSystem::Resource::TYPE_TEXTURE);
						
		mTexture = (GfxSystem::TexturePtr)gResourceMgr.GetResource(mResourceGroupName, name);
		// warning, medved error, medved thought that position of A doesnt matter :)
/*
		uint32* color_shift_buff = DYN_NEW uint32[bytesize];
		uint32* color_sb_pos = color_shift_buff;
		const uint32* buffPtrCast = (const uint32*)buffPtr;

		for (const uint32* stop = buffPtrCast + buffWidth*buffHeight; buffPtrCast < stop; ++buffPtrCast, ++color_sb_pos) {
			uint32 alpha = (*buffPtrCast) << 24;

			*color_sb_pos = ((*buffPtrCast) >> 8) | alpha;
		}

		mTexture->LoadFromBitmap(color_shift_buff, bytesize, buffWidth, buffHeight, (GfxSystem::Texture::ePixelFormat)pixelFormat);
		DYN_DELETE_ARRAY color_shift_buff;
*/
		mTexture->LoadFromBitmap(buffPtr, bytesize, buffWidth, buffHeight, (GfxSystem::Texture::ePixelFormat)pixelFormat);		
		mOriginalHeight = buffHeight;
		mOriginalWidth = buffWidth;
	}

	CEGUITextureWrapper::~CEGUITextureWrapper(void) {
		mTexture->Unload(true);
		mTexture.SetNull();		
	}

	String CEGUITextureWrapper::GetNextTextureName() {
		std::stringstream sstream;
		sstream << "CEGUI_CUSTOM_TEXTURE_";
		sstream << mTextureIndex++;
		return sstream.str();
	}
}