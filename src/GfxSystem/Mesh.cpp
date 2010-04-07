#include "Common.h"
#include "Mesh.h"
#include "DataContainer.h"
#include "objloader/model_obj.h"

using namespace GfxSystem;

Mesh::Mesh( void ): mModel(0) {}

Mesh::~Mesh( void ) {}

ResourceSystem::ResourcePtr Mesh::CreateMe()
{
	return ResourceSystem::ResourcePtr(new Mesh());
}

size_t Mesh::LoadImpl()
{
	DataContainer dc;
	GetRawInputData(dc);
	size_t dataSize = dc.GetSize();

	// this is not very nice but it's the fastest way to make OBJ loading working
	///TODO the model loader should be rewritten
	string tmpFilePath = GetFilePath() + ".tmp";
	boost::filesystem::ofstream* ofs = new boost::filesystem::ofstream(tmpFilePath, std::ios_base::out | std::ios_base::binary);
	ofs->write((char*)dc.GetData(), dc.GetSize());
	ofs->close();

	mModel = new ModelOBJ();
	if (mModel->import(tmpFilePath.c_str()))
	{
		mModel->normalize();

		// create textures for the meshes
		for (int i=0; i<mModel->getNumberOfMaterials(); ++i)
		{
			const ModelOBJ::Material* objMaterial = &mModel->getMaterial(0);
			if (!objMaterial->colorMapFilename.empty())
			{
				gResourceMgr.AddResourceFileToGroup(GetFileDir() + "/" + objMaterial->colorMapFilename, "MeshTextures", ResourceSystem::RESTYPE_TEXTURE, false);
			}
		}
	}
	else
	{
		delete mModel;
		dataSize = 0;
	}

	boost::filesystem::remove(tmpFilePath);

	return dataSize;
}

bool Mesh::UnloadImpl()
{
	if (mModel)
	{
		delete mModel;	
	}
	return true;
}

MeshHandle Mesh::GetMesh(void)
{
	EnsureLoaded();
	return (MeshHandle)mModel;
}