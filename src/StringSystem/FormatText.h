/// @file
/// 

#ifndef FormatText_h__
#define FormatText_h__

#include "Base.h"
#include "TextData.h"

namespace StringSystem
{
	/// This class represents generic parameters passed to a function accessed through the properties system.
	class FormatText
	{
	public:
		/// Wild character.
		static const char WildChar = '%';

		/// Constructs new empty parameters.
		FormatText(const TextData& text) : mTextData(text) {}

		/// Assignment operator.
		FormatText& operator=(const FormatText& rhs);

		/// This method is provided for convenience and allows to create a PropertyFunctionParameters
		/// object and fill it with parameters with following code:
		///
		/// FormatText(variableTextData) << firstVariable << secondVariable;
		FormatText& operator<<(const TextData& param);

		operator const TextData& (void) const;

	private:
		TextData::size_type FindMinVariable(void);

		TextData mTextData;
		
	};
}

#endif // FormatText_h__