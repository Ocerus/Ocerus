#ifndef WaterSurface_h__
#define WaterSurface_h__

#include "../Utility/Settings.h"
#include "../GfxSystem/Texture.h"

namespace Core
{
	class WaterSurface
	{
	public:
		WaterSurface(const GfxSystem::TexturePtr waterTexture, const float32 texScale, const float32 cellWidth, 
			const float32 cellHeight, const int32 numCellsX, const int32 numCellsY);
		~WaterSurface(void);

		void Update(const float32 delta);
		void Draw(void);
		void LowerArea(const Vector2* poly, const int32 polyLen, const Vector2& offset);


	private:
		GfxSystem::TexturePtr mWaterTexture;
		float32 mWaterTextureScale;
		float32 mCellWidth;
		float32 mCellHeight;
		int32 mRows;
		int32 mCols;
		//TODO mozna by stacilo mit tu jen int8
		int32** mHeightmap;
		int32** mPrevHeightmap;

		float32 mResidualDelta;
		float32 mAmbientWaveTime;

		inline int32 GetWrappedIndexX(const int32 index) const
		{
			// assuming mCols is a power of two
			return (index&(mCols-1) + mCols)&(mCols-1);
		}
		inline int32 GetWrappedIndexY(const int32 index) const
		{
			return (index&(mRows-1) + mRows)&(mRows-1);
		}

		int32 GetAmbientWaveHeight(const int32 i, const int32 j) const;
	};
}

#endif // WaterSurface_h__