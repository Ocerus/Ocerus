/// @file
/// Defines various data structures used by (not only) renderer.

#ifndef GfxStructures_h__
#define GfxStructures_h__

#include "Base.h"

namespace GfxSystem
{
	/// Platform dependent handle to an arbitrary texture.
	typedef unsigned int TextureHandle;

	/// Platform dependent handle to an arbitrary mesh.
	typedef uintptr_t MeshHandle;

	/// Invalid texture handle.
	const TextureHandle InvalidTextureHandle = 0;

	/// Texture pixel format
	enum ePixelFormat
	{
		PF_AUTO = 0,			//auto
		PF_L = 1,				//grayscale
		PF_LA = 2,				//grayscale with alpha
		PF_RGB = 3,				//RGB
		PF_RGBA = 4				//RGB with alpha
	};

	/// Integer point in the screen or any pixel oriented buffer.
	struct Point
	{
		/// Constructs the point from two integer values.
		Point(int32 _x, int32 _y): x(_x), y(_y) {}

		/// Constructs the point from two float values by rounding them.
		Point(float32 _x, float32 _y): x((int32)_x), y((int32)_y) {}

		/// Constructs the zero point.
		Point(void): x(0), y(0) {}

		int32 x; ///< X coordinate of the point.
		int32 y; ///< Y coordinate of the point.

		/// Sets new integer values to the point.
		inline void Set(const int32 _x, const int32 _y)
		{
			x = _x;
			y = _y;
		}

		/// True if the point is zero.
		inline bool IsZero()
		{
			return x==0 && y==0;
		}

		/// Compares the point to another; coordinate by coordinate.
		inline bool operator==(const Point& p)
		{
			return x==p.x && y==p.y;
		}

		/// Subtracts another point from this one.
		inline void operator-()
		{
			x = -x;
			y = -y;
		}

		/// Zero point.
		static Point Point_Zero;
	};


	/// Integer rectangle in the screen or any pixel oriented buffer.
	struct Rect
	{
		/// Constructs the rectangle from integer position and bounds.
		Rect(int32 _x, int32 _y, int32 _w, int32 _h): x(_x), y(_y), w(_w), h(_h) {}

		/// Constructs zero rectangle.
		Rect(void): x(0), y(y), w(w), h(h) {}

		int32 x; ///< X coordinate.
		int32 y; ///< Y coordinate.
		int32 w; ///< Width.
		int32 h; ///< Height.

		/// Zero rectangle.
		static Rect Null;
	};


	/// Platform independent RGBA color.
	struct Color
	{
		/// Constructs the color from a red, green, blue and alpha components.
		Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255): r(_r), g(_g), b(_b), a(_a) {}

		/// Constructs the color from an integer hexa representation.
		Color(uint32 color) : r((uint8)( (color >> 16)&0x000000FF )), g((uint8)( (color >> 8)&0x000000FF )),
			b((uint8)( color&0x000000FF )), a((uint8)(color >> 24)) {}

		/// Constructs the black color.
		Color(void): r(0), g(0), b(0), a(255) {}
		
		/// Returns the integer hexa representation of the color.
		inline uint32 GetARGB() const
		{
			return a << 24 | r << 16 | g << 8 | b; 
		}

		/// Returns true if the colors are equal.
		inline bool operator==(const Color& rhs)
		{
			return r==rhs.r && g==rhs.g && b==rhs.b && a==rhs.a;
		}

		uint8 r; ///< Red.
		uint8 g; ///< Green.
		uint8 b; ///< Blue.
		uint8 a; ///< Alpha.

		/// Zero (transparent black) color.
		static Color Null;

		/// White opaque color.
		static Color Full;
	};


	/// Quad with a texture attached to it.
	struct TexturedQuad
	{
		/// Constructs the zero quad.
		TexturedQuad(): position(Vector2_Zero), size(Vector2_Zero), frameSize(Vector2(1,1)),
			texOffset(Vector2_Zero), scale(1.0f, 1.0f), angle(0), z(0), texture(0), transparency(0) {}

		/// Constructs the quad filling all attributes.
		TexturedQuad(const Vector2& _position, const Vector2& _size, const Vector2& _frameSize, const Vector2& _offset,
			const Vector2& _scale, const float32 _angle, const float32 _z, const uint32 _texture, const float32 _transparency ):
			position(_position), size(_size), frameSize(_frameSize), texOffset(_offset), scale(_scale),
			angle(_angle), z(_z), texture(_texture), transparency(_transparency) {}

		Vector2 position; ///< Position in the space defined by the renderer.
		Vector2 size; ///< Size in the space defined by the renderer.
		Vector2 frameSize; ///< (0,1) relative to texure size
		Vector2 texOffset; ///< (0,1) relative to texure size
		Vector2 scale; ///< (0,1) scale.
		float32 angle; ///< Angle in radians.
		float32 z; ///< Z order component.
		TextureHandle texture; ///< Reference to the texture it's drawing.
		float32 transparency; ///< (0,1) transparency. 0 means fully opaque.
	};


	/// Mesh with a texture attached to it.
	struct TexturedMesh
	{
		/// Constructs the zero mesh.
		TexturedMesh(): position(Vector2_Zero), scale(Vector2_Zero), angle(0), z(0), 
			yAngle(0), mesh(0), transparency(0) {}

		Vector2 position; ///< Position in the space defined by the renderer.
		Vector2 scale; ///< (0,1) scale.
		float32 angle; ///< Angle in radians.
		float32 z; ///< Z order component.
		float32 yAngle; ///< Angle along the Y axis in radians.
		MeshHandle mesh; ///< Reference to the mesh it's drawing.
		float32 transparency; ///< (0,1) transparency. 0 means fully opaque.
	};


	/// Attributes of the editor grid. Used in viewports.
	struct GridInfo
	{
		/// Constructs a default grid.
		GridInfo(): minorCellSize(1), minorsInMajor(10), minShowMinorsZoom(0.4f), minorColor(Color(255,255,255,50)),
			majorColor(Color(255,255,255,100)), axisXColor(Color(50,50,255,200)), axisYColor(Color(255,50,50,200)) {}

		float32 minorCellSize; ///< Size of the smaller cells.
		int32 minorsInMajor; ///< Number of minor cells in each major cell.
		float32 minShowMinorsZoom; ///< Minimal zoom of the minor cells to be shown. Otherwise they won't be drawn.
		Color minorColor; ///< %Color of the minor cells.
		Color majorColor; ///< %Color of the major cells.
		Color axisXColor; ///< %Color of the X axis.
		Color axisYColor; ///< %Color of the Y axis.
	};
}

#endif // GfxStructures_h__