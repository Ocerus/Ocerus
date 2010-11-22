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

bool Mesh::LoadModelFromFilePath(const char* path)
{
	if (mModel->import(path))
	{
		if (mModel->getNumberOfVertices() == 0)
		{
			return false;
		}
		else
		{
			mModel->normalize();

			// create textures for the meshes
			for (int i=0; i<mModel->getNumberOfMaterials(); ++i)
			{
				ModelOBJ::Material* objMaterial = &mModel->getMaterial(i);
				if (!objMaterial->colorMapFilename.empty())
				{
					boost::filesystem::path filePath(path);
					string fileDir = GetRelativeFileDir();

					objMaterial->colorMapFilename = fileDir + objMaterial->colorMapFilename;
					gResourceMgr.AddResourceFileToGroup(objMaterial->colorMapFilename, "MeshTextures", ResourceSystem::RESTYPE_TEXTURE, GetBasePathType());
				}
			}
		}
		return true;
	}
	else
		return false;
		
}

size_t Mesh::LoadImpl()
{
	DataContainer dc;
	GetRawInputData(dc);
	size_t dataSize = dc.GetSize();

	// this is not very nice but it's the fastest way to make OBJ loading working
	string tmpFilePath = GetFilePath() + ".tmp";
	boost::filesystem::ofstream* ofs = new boost::filesystem::ofstream(tmpFilePath, std::ios_base::out | std::ios_base::binary);
	ofs->write((char*)dc.GetData(), dc.GetSize());
	ofs->close();
	dc.Release();

	mModel = new ModelOBJ();
	if (!LoadModelFromFilePath(tmpFilePath.c_str()))
	{
		delete mModel;

		ocWarning << "Model '" << GetName() << "' cannot be loaded. Loading NullModel instead!";

		ResourceSystem::ResourcePtr nullModHandle = gResourceMgr.GetResource("General", ResourceSystem::RES_NULL_MODEL);
		string filePath = nullModHandle->GetFilePath();

		// used only for determining size of data
		GetRawInputData(filePath, dc);
		dataSize = dc.GetSize();
		dc.Release();

		mModel = new ModelOBJ();
		if (!LoadModelFromFilePath(filePath.c_str()))
		{
			ocError << "Cannot load NullModel!";
			delete mModel;

			mModel = NULL;
			dataSize = 0;
		}
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