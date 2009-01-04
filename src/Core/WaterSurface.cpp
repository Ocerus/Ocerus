#include "Common.h"
#include "WaterSurface.h"

using namespace Core;

#define MIN_HEIGHT -100
#define DAMPING 16

Core::WaterSurface::WaterSurface( const GfxSystem::TexturePtr waterTexture, const float32 texScale, const float32 cellWidth, 
								 const float32 cellHeight, const int32 numCellsX, const int32 numCellsY ):
	mWaterTexture(waterTexture),
	mWaterTextureScale(texScale),
	mCellWidth(cellWidth),
	mCellHeight(cellHeight),
	mCols(numCellsX),
	mRows(numCellsY),
	mHeightmap(0)
{
	assert(MathUtils::IsPowerOfTwo(mRows) && "Number of rows must be a power of two");
	assert(MathUtils::IsPowerOfTwo(mCols) && "Number of columns must be a power of two");
	assert(!mWaterTexture.IsNull() && "Texture is null");

	mHeightmap = DYN_NEW int32 *[mCols];
	for (int32 i=0; i<mCols; ++i)
	{
		mHeightmap[i] = DYN_NEW int32[mRows];
		for (int32 j=0; j<mRows; ++j)
			mHeightmap[i][j] = 0;
	}
}

Core::WaterSurface::~WaterSurface( void )
{
	if (mHeightmap)
	{
		for (int32 i=0; i<mCols; ++i)
			DYN_DELETE_ARRAY mHeightmap[i];
		DYN_DELETE_ARRAY mHeightmap;
	}
}

void Core::WaterSurface::LowerArea( const Vector2* poly, const int32 polyLen )
{

}

void Core::WaterSurface::Update( const float32 delta )
{
	// debug
	mHeightmap[0][0] = MIN_HEIGHT;
	mHeightmap[0][1] = MIN_HEIGHT;
	mHeightmap[1][0] = MIN_HEIGHT;

	//TODO nak tu nefunguje inlinovani
	for (int32 i=0; i<mCols; ++i)
		for (int32 j=0; j<mRows; ++j)
		{
			/*mHeightmap[i][j] = ((	mHeightmap[ GetWrappedIndexX(i-1) ][ j ] +
									mHeightmap[ GetWrappedIndexX(i+1) ][ j ] +
									mHeightmap[ i ][ GetWrappedIndexY(j-1) ] +
									mHeightmap[ i ][ GetWrappedIndexY(j+1) ]
								) / 2 ) - mHeightmap[i][j];*/
			mHeightmap[i][j] = ((	mHeightmap[ ((i-1)&(mCols-1) + mCols)&(mCols-1) ][ j ] +
									mHeightmap[ ((i+1)&(mCols-1) + mCols)&(mCols-1) ][ j ] +
									mHeightmap[ i ][ ((j-1)&(mRows-1) + mRows)&(mRows-1) ] +
									mHeightmap[ i ][ ((j+1)&(mRows-1) + mRows)&(mRows-1) ]
								) / 2 ) - mHeightmap[i][j];
			mHeightmap[i][j] -= mHeightmap[i][j] / DAMPING;
		}

}

void Core::WaterSurface::Draw( void )
{
	Vector2 topLeft = gGfxRenderer.GetCameraWorldBoxTopLeft();
	Vector2 botRight = gGfxRenderer.GetCameraWorldBoxBotRight();

	int32 minI = MathUtils::Floor(topLeft.x / mCellWidth);
	int32 minJ = MathUtils::Floor(topLeft.y / mCellHeight);
	int32 maxI = MathUtils::Ceiling(botRight.x / mCellWidth);
	int32 maxJ = MathUtils::Ceiling(botRight.y / mCellHeight);
	float32 textureWidth = mWaterTextureScale * mWaterTexture->GetWidth();
	float32 textureHeight = mWaterTextureScale * mWaterTexture->GetHeight();

	GfxSystem::Point vertices[4];
	Vector2 textureCoords[4];
	GfxSystem::Color vertexColors[4];
	int32 directions[4][4] = {{0,0},{1,0},{1,1},{0,1}};

	//TODO spoustu bodu i a j se pocita dvakrat - nekam to cachovat nebo s tim proste neco udelat
	for (int32 i=minI; i<maxI; ++i)
		for (int32 j=minJ; j<maxJ; ++j)
		{
			for (int32 k=0; k<4; ++k)
			{
				float32 worldPosX = mCellWidth*(i+directions[k][0]);
				float32 worldPosY = mCellHeight*(j+directions[k][1]);
				vertices[k].x = gGfxRenderer.WorldToScreenX(worldPosX);
				vertices[k].y = gGfxRenderer.WorldToScreenY(worldPosY);

				// refraction
				int32 dhX = mHeightmap[GetWrappedIndexX(i+directions[k][0]+1)][GetWrappedIndexY(j+directions[k][1])]
					- mHeightmap[GetWrappedIndexX(i+directions[k][0])][GetWrappedIndexY(j+directions[k][1])];
				int32 dhY = mHeightmap[GetWrappedIndexX(i+directions[k][0])][GetWrappedIndexY(j+directions[k][1])+1]
					- mHeightmap[GetWrappedIndexX(i+directions[k][0])][GetWrappedIndexY(j+directions[k][1])];
				float32 displaceX = MathUtils::Tan(MathUtils::ArcSin(MathUtils::Sin(MathUtils::ArcTan((float32)dhX)) / 2.0f)) * dhX;
				float32 displaceY = MathUtils::Tan(MathUtils::ArcSin(MathUtils::Sin(MathUtils::ArcTan((float32)dhY)) / 2.0f)) * dhY;
				/*if (dhX < 0)
				{
					if (dhY < 0)
					{
						worldPosX -= displaceX;
						worldPosY -= displaceY;
					}
					else
					{
						worldPosX -= displaceX;
						worldPosY += displaceY;
					}
				}
				else
				{
					if (dhY < 0)
					{
						worldPosX += displaceX;
						worldPosY -= displaceY;
					}
					else
					{
						worldPosX += displaceX;
						worldPosY += displaceY;
					}
				}*/

				textureCoords[k].x = (worldPosX - MathUtils::Floor(worldPosX / textureWidth) * textureWidth) / textureWidth;
				textureCoords[k].y = (worldPosY - MathUtils::Floor(worldPosY / textureHeight) * textureHeight) / textureHeight;
				uint8 col = 255 * (mHeightmap[GetWrappedIndexX(i+directions[k][0])][GetWrappedIndexY(j+directions[k][1])] - MIN_HEIGHT) / (-MIN_HEIGHT*2);
				vertexColors[k].a = 255;
				vertexColors[k].r = col;
				vertexColors[k].g = col;
				vertexColors[k].b = col;
			}
			gGfxRenderer.DrawQuad(vertices, mWaterTexture, textureCoords, vertexColors);
		}

}