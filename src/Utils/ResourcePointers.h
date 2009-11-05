/// @file
/// This file gathers smart pointers of all resources together so that it can be included with no overhead.

#ifndef ResourcePointers_h__
#define ResourcePointers_h__

#include "SmartPointer.h"

namespace GfxSystem
{
	class ParticleSystem;
	class ParticleResource;
	class Texture;

	/// Smart pointer to ParticleSystem.
	typedef SmartPointer<ParticleSystem> ParticleSystemPtr;
	/// Smart pointer to ParticleResource.
	typedef SmartPointer<ParticleResource> ParticleResourcePtr;
	/// Smart pointer to Texture.
	typedef SmartPointer<Texture> TexturePtr;
}

namespace StringSystem
{
	class TextResource;

	/// Smart pointer to TextResource.
	typedef SmartPointer<TextResource> TextResourcePtr;
}

namespace ResourceSystem
{
	class Resource;
	class XMLResource;

	/// Smar pointer to Resource.
	typedef SmartPointer<Resource> ResourcePtr;
	/// Smart pointer to XMLResource.
	typedef SmartPointer<XMLResource> XMLResourcePtr;
}

namespace GUISystem
{
	class CEGUIResource;

	/// Smart pointer to CEGUIResource.
	typedef SmartPointer<CEGUIResource> CEGUIResourcePtr;
}

namespace ScriptSystem
{
	class ScriptResource;

	/// Smart pointer to ScriptResource.
	typedef SmartPointer<ScriptResource> ScriptResourcePtr;
}

#endif // ResourcePointers_h__