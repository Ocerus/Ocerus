#include "Texture.h"

using namespace GfxSystem;

ResourceSystem::ResourcePtr Texture::CreateMe()
{
	return ResourceSystem::ResourcePtr(NEW Texture());
}

void Texture::_GetTexture()
{

}

bool Texture::LoadImpl()
{
	return false;
}

bool Texture::UnloadImpl()
{
	return false;
}