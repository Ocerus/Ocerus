#include "Texture.h"

using namespace GfxSystem;

ResourceSystem::ResourcePtr Texture::CreateMe()
{
	return ResourceSystem::ResourcePtr(DYN_NEW Texture());
}

void Texture::_GetTexture()
{
	EnsureLoaded();
}

bool Texture::LoadImpl()
{
	return false;
}

bool Texture::UnloadImpl()
{
	return false;
}