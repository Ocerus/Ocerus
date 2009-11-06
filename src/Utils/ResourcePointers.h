/// @file
/// This file gathers smart pointers of all resources together so that it can be included with no overhead.

#ifndef ResourcePointers_h__
#define ResourcePointers_h__

#include <boost/shared_ptr.hpp>

namespace GfxSystem
{
	class ParticleSystem;
	class ParticleResource;
	class Texture;

	/// Smart pointer to ParticleSystem.
	typedef boost::shared_ptr<ParticleSystem> ParticleSystemPtr;
	/// Smart pointer to ParticleResource.
    typedef boost::shared_ptr<ParticleResource> ParticleResourcePtr;
	/// Smart pointer to Texture.
    typedef boost::shared_ptr<Texture> TexturePtr;
}

namespace StringSystem
{
	class TextResource;

	/// Smart pointer to TextResource.
    typedef boost::shared_ptr<TextResource> TextResourcePtr;
}

namespace ResourceSystem
{
	class Resource;
	class XMLResource;

	/// Smar pointer to Resource.
    typedef boost::shared_ptr<Resource> ResourcePtr;
	/// Smart pointer to XMLResource.
    typedef boost::shared_ptr<XMLResource> XMLResourcePtr;
}

namespace GUISystem
{
	class CEGUIResource;

	/// Smart pointer to CEGUIResource.
    typedef boost::shared_ptr<CEGUIResource> CEGUIResourcePtr;
}

namespace ScriptSystem
{
	class ScriptResource;

	/// Smart pointer to ScriptResource.
    typedef boost::shared_ptr<ScriptResource> ScriptResourcePtr;
}

#endif // ResourcePointers_h__