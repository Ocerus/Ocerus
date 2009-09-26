#include "Common.h"
#include "WaterSurface.h"

using namespace Core;

#define MAX_HEIGHT 100
#define DAMPING 0.004f
#define TIMESTEP 0.1f

Core::WaterSurface::WaterSurface( const GfxSystem::TexturePtr waterTexture, const float32 texScale, const float32 cellWidth, 
								 const float32 cellHeight, const int32 numCellsX, const int32 numCellsY ):
	mWaterTexture(waterTexture),
	mWaterTextureScale(texScale),
	mCellWidth(cellWidth),
	mCellHeight(cellHeight),
	mCols(numCellsX),
	mRows(numCellsY),
	mHeightmap(0),
	mResidualDelta(0),
	mAmbientWaveTime(0)
{
	BS_ASSERT_MSG(MathUtils::IsPowerOfTwo(mRows), "Number of rows must be a power of two");
	BS_ASSERT_MSG(MathUtils::IsPowerOfTwo(mCols), "Number of columns must be a power of two");
	BS_ASSERT_MSG(!mWaterTexture.IsNull(), "Texture is null");

	mHeightmap = DYN_NEW int32*[mCols];
	mPrevHeightmap = DYN_NEW int32*[mCols];
	for (int32 i=0; i<mCols; ++i)
	{
		mHeightmap[i] = DYN_NEW int32[mRows];
		mPrevHeightmap[i] = DYN_NEW int32[mRows];
		for (int32 j=0; j<mRows; ++j)
		{
			mHeightmap[i][j] = 0;
			mPrevHeightmap[i][j] = 0;
		}
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

void Core::WaterSurface::LowerArea( const Vector2* poly, const int32 polyLen, const Vector2& offPos, const float32 offAngle )
{
	float32 minX=FLT_MAX, minY=FLT_MAX, maxX=FLT_MIN, maxY=FLT_MIN;
	XForm xf(offPos, Matrix22(offAngle));
	for (int32 i=0; i<polyLen; ++i)
	{
		Vector2 v = MathUtils::Multiply(xf, poly[i]);
		if (v.x < minX)
			minX = v.x;
		if (v.y < minY)
			minY = v.y;
		if (v.x > maxX)
			maxX = v.x;
		if (v.y > maxY)
			maxY = v.y;
	}
	int32 minI = MathUtils::Floor(minX / mCellWidth);
	int32 minJ = MathUtils::Floor(minY / mCellHeight);
	int32 maxI = MathUtils::Ceiling(maxX / mCellWidth);
	int32 maxJ = MathUtils::Ceiling(maxY / mCellHeight);

	for (int32 i=minI; i<=maxI; ++i)
		for (int32 j=minJ; j<=maxJ; ++j)
			mHeightmap[GetWrappedIndexX(i)][GetWrappedIndexY(j)] = -MAX_HEIGHT;
}

void Core::WaterSurface::Update( const float32 delta )
{
	mAmbientWaveTime += delta;
	mResidualDelta += delta;

	while (mResidualDelta > TIMESTEP)
	{
		mResidualDelta -= TIMESTEP;

		int32** tmp = mPrevHeightmap;
		mPrevHeightmap = mHeightmap;
		mHeightmap = tmp;

		for (int32 i=0; i<mCols; ++i)
			for (int32 j=0; j<mRows; ++j)
			{
				/*mHeightmap[i][j] = ((	mHeightmap[ GetWrappedIndexX(i-1) ][ j ] +
										mHeightmap[ GetWrappedIndexX(i+1) ][ j ] +
										mHeightmap[ i ][ GetWrappedIndexY(j-1) ] +
										mHeightmap[ i ][ GetWrappedIndexY(j+1) ]
									) / 2 ) - mHeightmap[i][j];*/
				/*mHeightmap[i][j] = ((	mHeightmap[ ((i-1)&(mCols-1) + mCols)&(mCols-1) ][ j ] +
										mHeightmap[ ((i+1)&(mCols-1) + mCols)&(mCols-1) ][ j ] +
										mHeightmap[ i ][ ((j-1)&(mRows-1) + mRows)&(mRows-1) ] +
										mHeightmap[ i ][ ((j+1)&(mRows-1) + mRows)&(mRows-1) ]
									) / 2 ) - mHeightmap[i][j];
				mHeightmap[i][j] -= MathUtils::Floor(DAMPING * mHeightmap[i][j]);*/
				int32 h = ((	mPrevHeightmap[ ((i-1)&(mCols-1) + mCols)&(mCols-1) ][ j ] +
								mPrevHeightmap[ ((i+1)&(mCols-1) + mCols)&(mCols-1) ][ j ] +
								mPrevHeightmap[ i ][ ((j-1)&(mRows-1) + mRows)&(mRows-1) ] +
								mPrevHeightmap[ i ][ ((j+1)&(mRows-1) + mRows)&(mRows-1) ]
							) / 2 ) - mHeightmap[i][j];
				mHeightmap[i][j] = h - MathUtils::Floor(DAMPING * h);
			}
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
			float32 minX = mCellWidth*i;
			minX = (minX - MathUtils::Floor(minX / textureWidth) * textureWidth) / textureWidth;
			float32 minY = mCellHeight*j;
			minY = (minY - MathUtils::Floor(minY / textureHeight) * textureHeight) / textureHeight;
			float32 maxX = mCellWidth*(i+1);
			maxX = (maxX - MathUtils::Floor(maxX / textureWidth) * textureWidth) / textureWidth;
			float32 maxY = mCellHeight*(j+1);
			maxY = (maxY - MathUtils::Floor(maxY / textureHeight) * textureHeight) / textureHeight;
			if (maxX <= minX)
				maxX += 1.0f;
			if (maxY <= minY)
				maxY += 1.0f;
			textureCoords[0].x = minX;
			textureCoords[0].y = minY;
			textureCoords[1].x = maxX;
			textureCoords[1].y = minY;
			textureCoords[2].x = maxX;
			textureCoords[2].y = maxY;
			textureCoords[3].x = minX;
			textureCoords[3].y = maxY;

			for (int32 k=0; k<4; ++k)
			{
				float32 worldPosX = mCellWidth*(i+directions[k][0]);
				float32 worldPosY = mCellHeight*(j+directions[k][1]);
				vertices[k].x = gGfxRenderer.WorldToScreenX(worldPosX);
				vertices[k].y = gGfxRenderer.WorldToScreenY(worldPosY);

				// refraction
				int32 h = mHeightmap[GetWrappedIndexX(i+directions[k][0])][GetWrappedIndexY(j+directions[k][1])] + GetAmbientWaveHeight(i+directions[k][0], j+directions[k][1]);
				int32 dhX = mHeightmap[GetWrappedIndexX(i+directions[k][0]+1)][GetWrappedIndexY(j+directions[k][1])]
					+ GetAmbientWaveHeight(i+directions[k][0]+1, j+directions[k][1]) - h;
				int32 dhY = mHeightmap[GetWrappedIndexX(i+directions[k][0])][GetWrappedIndexY(j+directions[k][1])+1]
					+ GetAmbientWaveHeight(i+directions[k][0], j+directions[k][1]+1) - h;
				float32 xAngle = MathUtils::ArcTan(0.0009f * (float32)dhX);
				float32 yAngle = MathUtils::ArcTan(0.0009f * (float32)dhY);
				float32 xRefraction = MathUtils::ArcSin(0.5f * MathUtils::Sin(xAngle));
				float32 yRefraction = MathUtils::ArcSin(0.5f * MathUtils::Sin(yAngle));
				float32 displaceX = MathUtils::Tan(xRefraction) * dhX;
				float32 displaceY = MathUtils::Tan(yRefraction) * dhY;
				if (displaceX > 0.499f*mCellWidth)
					displaceX = 0.499f*mCellWidth;
				if (displaceX < -0.499f*mCellWidth)
					displaceX = -0.499f*mCellWidth;
				if (displaceY > 0.499f*mCellHeight)
					displaceY = 0.499f*mCellHeight;
				if (displaceY < -0.499f*mCellHeight)
					displaceY = -0.499f*mCellHeight;
				if (dhX < 0)
				{
					if (dhY < 0)
					{
						displaceX = -displaceX;
						displaceY = -displaceY;
					}
					else
					{
						displaceX = -displaceX;
						//displaceY = displaceY;
					}
				}
				else
				{
					if (dhY < 0)
					{
						//displaceX = displaceX;
						displaceY = -displaceY;
					}
					else
					{
						//displaceX = displaceX;
						//displaceY = displaceY;
					}
				}
				textureCoords[k].x += displaceX / textureWidth;
				textureCoords[k].y += displaceY / textureHeight;

				uint32 col = 130 + 120 * (h + MAX_HEIGHT) / (MAX_HEIGHT*2);
				if (col > 255)
					col = 255;
				vertexColors[k].a = 255;
				vertexColors[k].r = (uint8)col;
				vertexColors[k].g = (uint8)col;
				vertexColors[k].b = (uint8)col;
			}
			gGfxRenderer.DrawQuad(vertices, mWaterTexture, textureCoords, vertexColors);
		}

}

int32 Core::WaterSurface::GetAmbientWaveHeight( const int32 i, const int32 j ) const
{
	return MathUtils::Round(30.0f*MathUtils::Sin(2.0f*mAmbientWaveTime + 1.0f*(mCellWidth*i+mCellHeight*j)));
}