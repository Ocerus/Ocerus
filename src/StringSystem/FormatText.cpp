#include "Common.h"
#include "FormatText.h"

using namespace StringSystem;

FormatText& FormatText::operator=(const FormatText& rhs)
{
	if (this != &rhs)
	{
		mTextData = rhs.mTextData;
	}
	return *this;
}

TextData::size_type FormatText::FindMinVariable(void)
{
	int32 minimum = 0;
	TextData::size_type from = 0, next = 0, minPos = TextData::npos;

	while ((next = mTextData.find_first_of(WildChar, from)) != TextData::npos)
	{
		if (mTextData.length() > next + 1)
		{
			TextData::const_reference character = mTextData.at(next + 1);
			if (character >= '1' && character <= '9')
			{
				int32 number = character - '0';
				if (minimum == 0 || number < minimum)
				{
					minimum = number;
					minPos = next;
				}
			}
		}
		from = next + 1;
	}

	return minPos;
}

FormatText& FormatText::operator<<(const TextData& newText)
{
	TextData::size_type replacePos = FindMinVariable();
	if (replacePos != TextData::npos)
	{
		mTextData = mTextData.replace(replacePos, 2, newText);
	} else {
		mTextData += newText;
	}

	return *this;
}

FormatText::operator const TextData& (void) const
{
	return mTextData;
}