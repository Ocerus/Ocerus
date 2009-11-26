#include "Common.h"
#include "CEGUITextureWrapper.h"
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

	uint32 CEGUITextureWrapper::mTextureIndex = 0;
	const string CEGUITextureWrapper::mResourceGroupName = "CEGUI_created";

	void CEGUITextureWrapper::loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup) {
		///@todo This is really strange on linux. It compiles only if tmp variable is used.
		//GfxSystem::TexturePtr t = gResourceMgr.GetResource(resourceGroup.c_str(), filename.c_str());
		//mTexture = t;
		mTexture = (GfxSystem::TexturePtr)(gResourceMgr.GetResource(resourceGroup.c_str(), filename.c_str()));
		mOriginalHeight = mTexture->GetHeight();
		mOriginalWidth = mTexture->GetWidth();
	}

	CEGUITextureWrapper::CEGUITextureWrapper(RendererGate * gate) : CEGUI::Texture(gate) {
	}

	void CEGUITextureWrapper::loadFromMemory(const void* buffPtr, CEGUI::uint buffWidth,
		CEGUI::uint buffHeight, CEGUI::Texture::PixelFormat pixelFormat) {

		mTexture.reset();

		uint32 bytesize = ((buffWidth * sizeof(uint32)) * buffHeight);

		string name = GetNextTextureName();
		gResourceMgr.AddManualResourceToGroup(name, mResourceGroupName, ResourceSystem::RESTYPE_TEXTURE);

		///@todo This is really strange on linux. It compiles only if tmp variable is used.
		//GfxSystem::TexturePtr t = gResourceMgr.GetResource(mResourceGroupName, name);
		//mTexture = t;
		mTexture = (GfxSystem::TexturePtr)(gResourceMgr.GetResource(mResourceGroupName, name));

		// warning, medved error, medved thought that position of A doesnt matter :)
/*
		uint32* color_shift_buff = new uint32[bytesize];
		uint32* color_sb_pos = color_shift_buff;
		const uint32* buffPtrCast = (const uint32*)buffPtr;

		for (const uint32* stop = buffPtrCast + buffWidth*buffHeight; buffPtrCast < stop; ++buffPtrCast, ++color_sb_pos) {
			uint32 alpha = (*buffPtrCast) << 24;

			*color_sb_pos = ((*buffPtrCast) >> 8) | alpha;
		}

		mTexture->LoadFromBitmap(color_shift_buff, bytesize, buffWidth, buffHeight, (GfxSystem::Texture::ePixelFormat)pixelFormat);
		delete[] color_shift_buff;
*/
		mTexture->LoadFromBitmap(buffPtr, bytesize, buffWidth, buffHeight, (GfxSystem::Texture::ePixelFormat)pixelFormat);
		mOriginalHeight = buffHeight;
		mOriginalWidth = buffWidth;
	}

	CEGUITextureWrapper::~CEGUITextureWrapper(void) {
		mTexture->Unload(true);
		mTexture.reset();
	}

	string CEGUITextureWrapper::GetNextTextureName() {
		std::stringstream sstream;
		sstream << "CEGUI_CUSTOM_TEXTURE_";
		sstream << mTextureIndex++;
		return sstream.str();
	}
}