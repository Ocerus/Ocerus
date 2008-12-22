#ifndef _CEGUITEXTUREWRAPPER_H
#define _CEGUITEXTUREWRAPPER_H

#include "../Utility/Settings.h"
#include "CEGUIBase.h"
#include "CEGUITexture.h"
#include "../GfxSystem/Texture.h"
#include "RendererGate.h"

/// @name Forward declarations.
//@{
namespace CEGUI { class String; }
//@}

namespace GUISystem {
	class RendererGate;

	//TODO presunou funkce do CPP, at se odstrani zavislosti
	class CEGUITextureWrapper : public CEGUI::Texture
	{
	public:

		// Returns the current pixel width of the texture
		inline virtual CEGUI::ushort getWidth(void) const { return mTexture->GetWidth(); }

		// Returns the current pixel height of the texture
		inline virtual CEGUI::ushort getHeight(void) const { return mTexture->GetHeight(); }

		//TODO hardwire: I removed params from GetWidth/GetHeight as it caused problems inside HGE
		inline virtual CEGUI::ushort getOriginalHeight(void) const { return mOriginalHeight; }

		inline virtual CEGUI::ushort getOriginalWidth(void) const { return mOriginalWidth; }

		// Returns the original pixel width of the data loaded into the texture.
		// virtual ushort getOriginalWidth(void) const { return getWidth(); }

		// Returns the original pixel height of the data loaded into the texture.
		// virtual ushort getOriginalHeight(void) const { return getHeight(); }

		// Returns the current scale used for the width of the texture 
		// virtual float getXScale(void) const { return 1.0f / static_cast<float>(getOriginalWidth()); } 


		// Returns the current scale used for the height of the texture
		// virtual float getYScale(void) const { return 1.0f / static_cast<float>(getOriginalHeight()); }

		// Loads the specified image file into the texture.  The texture is resized as required to hold the image.
		virtual void loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup);

		/** Loads (copies) an image in memory into the texture.  The texture is resized as required to hold the image.
		  * buffPtr
		  * Pointer to the buffer containing the image data
		  *
		  * buffWidth
		  *	Width of the buffer (in pixels as specified by \a pixelFormat )
		  *
		  *	buffHeight
		  *	Height of the buffer (in pixels as specified by \a pixelFormat )
		  *
		  *	pixelFormat
		  * PixelFormat value describing the format contained in \a buffPtr
		  *
		  * return
		  *	Nothing.
		*/

		virtual void loadFromMemory(const void* buffPtr, CEGUI::uint buffWidth,
			CEGUI::uint buffHeight, CEGUI::Texture::PixelFormat pixelFormat);

		inline virtual void setTexture(GfxSystem::TexturePtr p) { mTexture = p; }

		CEGUI::Renderer* getRenderer(void) const {return d_owner;}

		inline virtual GfxSystem::TexturePtr getTexture() const { return mTexture; }

		CEGUITextureWrapper(RendererGate * gate);

		virtual ~CEGUITextureWrapper(void);

	protected:
		GfxSystem::TexturePtr mTexture;

		uint32 mOriginalHeight, mOriginalWidth;

		static uint32 mTextureIndex;
		static const std::string mResourceGroupName;
		static std::string GetNextTextureName();

	private:
		CEGUI::Renderer* d_owner;		//<! Renderer object that created and owns this texture		
	};
}

#endif