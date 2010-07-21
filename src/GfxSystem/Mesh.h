/// @file
/// Derived class from Resource representing texture.

#ifndef _MESH_H_
#define _MESH_H_

#include "Base.h"
#include "ResourceSystem/Resource.h"

class ModelOBJ;

namespace GfxSystem
{
	/// Resource class representing a 3D mesh.
	class Mesh : public ResourceSystem::Resource
	{
	public:

		/// Default constructor.
		Mesh(void);

		/// Default destructor.
		virtual ~Mesh(void);

		/// Factory method.
		static ResourceSystem::ResourcePtr CreateMe(void);

		/// Returns implementation specific pointer to the model object.
		MeshHandle GetMesh(void);

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_MESH; }

	protected:

		virtual size_t LoadImpl(void);
		virtual bool UnloadImpl(void);

	private:
		bool LoadModelFromFilePath(const char* path);

		ModelOBJ* mModel;
	};
}

#endif