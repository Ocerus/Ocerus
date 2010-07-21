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
			return false;;
		}
		else
		{
			mModel->normalize();

			// create textures for the meshes
			for (int i=0; i<mModel->getNumberOfMaterials(); ++i)
			{
				const ModelOBJ::Material* objMaterial = &mModel->getMaterial(i);
				if (!objMaterial->colorMapFilename.empty())
				{
					boost::filesystem::path filePath(path);
					string fileDir = filePath.branch_path().native_directory_string();
					gResourceMgr.AddResourceFileToGroup(fileDir + "/" + objMaterial->colorMapFilename, "MeshTextures", ResourceSystem::RESTYPE_TEXTURE, ResourceSystem::BPT_ABSOLUTE);
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
	///TODO the model loader should be rewritten
	string tmpFilePath = GetFilePath() + ".tmp";
	boost::filesystem::ofstream* ofs = new boost::filesystem::ofstream(tmpFilePath, std::ios_base::out | std::ios_base::binary);
	ofs->write((char*)dc.GetData(), dc.GetSize());
	ofs->close();

	mModel = new ModelOBJ();
	if (!LoadModelFromFilePath(tmpFilePath.c_str()))
	{
		delete mModel;

		ocWarning << "Model '" << GetName() << "' cannot be loaded. Loading NullModel instead!";

		ResourceSystem::ResourcePtr nullModHandle = gResourceMgr.GetResource("General", "NullModel");
		string filePath = nullModHandle->GetFilePath();

		mModel = new ModelOBJ();
		if (!LoadModelFromFilePath(filePath.c_str()))
		{
			ocError << "Cannot load NullModel!";
			delete mModel;

			mModel = 0;
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