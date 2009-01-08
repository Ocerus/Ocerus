#ifndef ResourcePointers_h__
#define ResourcePointers_h__

#include "SmartPointer.h"

/// @name This class gathers smart pointers of all resources together so that it can be included with no overhead.

namespace GfxSystem
{
	class ParticleSystem;
	typedef SmartPointer<ParticleSystem> ParticleSystemPtr;

	class Texture;
	typedef SmartPointer<Texture> TexturePtr;
}

namespace StringSystem
{
	class TextResource;
	typedef SmartPointer<TextResource> TextResourcePtr;
}

namespace ResourceSystem
{
	class Resource;
	typedef SmartPointer<Resource> ResourcePtr;

	class XMLResource;
	typedef SmartPointer<XMLResource> XMLResourcePtr;
}

namespace GUISystem
{
	class CEGUIResource;
	typedef SmartPointer<CEGUIResource> CEGUIResourcePtr;
}

#endif // ResourcePointers_h__