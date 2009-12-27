/// @file
/// Interface allowing us to easily render the physical representation of the scene.

#ifndef PhysicsDraw_h__
#define PhysicsDraw_h__

#include "Base.h"
#include "Dynamics/b2WorldCallbacks.h"

namespace Core
{
	/// Interface allowing us to easily render the physical representation of the scene.
	class PhysicsDraw: public b2DebugDraw
	{
	public:

		/// Default constructor.
		PhysicsDraw(void);

		/// Default destructor.
		virtual ~PhysicsDraw(void) {}

		/// Draw a closed polygon provided in CCW order.
		virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

		/// Draw a solid closed polygon provided in CCW order.
		virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

		/// Draw a circle.
		virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

		/// Draw a solid circle.
		virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

		/// Draw a line segment.
		virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

		/// Draw a transform. Choose your own length scale.
		/// @param xf a transform.
		virtual void DrawXForm(const b2XForm& xf);
	};
}

#endif // PhysicsDraw_h__
