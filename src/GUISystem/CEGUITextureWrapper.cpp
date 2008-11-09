#include "CEGUITextureWrapper.h"
#include "../Utility/Settings.h"
#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "GUIMgr.h"
#include "../ResourceSystem/ResourceMgr.h"
#include "../GfxSystem/GfxRenderer.h"
#include "../GfxSystem/Texture.h"
#include "RendererGate.h"

namespace GUISystem {
	void CEGUITextureWrapper::loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup) {
		mTexture.SetNull();
		mTexture = (GfxSystem::TexturePtr)gResourceMgr.GetResource(resourceGroup.c_str(), filename.c_str());
	}

	CEGUITextureWrapper::CEGUITextureWrapper(RendererGate * gate) : CEGUI::Texture(gate) {
	}

	void CEGUITextureWrapper::loadFromMemory(const void* buffPtr, CEGUI::uint buffWidth,
		CEGUI::uint buffHeight, CEGUI::Texture::PixelFormat pixelFormat) {
			/*
			byte pixelsize = (pixelFormat == CEGUI::Texture::PF_RGB)?3:4;
			DYN_DELETE mTexture;
			
			mTexture = TexturePtr( DYN_NEW GfxSystem::Texture( TODO ) );
			*/
			assert(!"WE ARE DOOMED");
	}

	CEGUITextureWrapper::~CEGUITextureWrapper(void) {
		mTexture.SetNull();
	}
}