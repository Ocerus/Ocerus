#include "Common.h"
#include "Vector2Editor.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

void Vector2Editor::Submit()
{
	OC_DASSERT(mEditbox1Widget != 0);
	OC_DASSERT(mEditbox2Widget != 0);
	Vector2 newValue;
	newValue.x = Utils::StringConverter::FromString<float32>(mEditbox1Widget->getText().c_str());
	newValue.y = Utils::StringConverter::FromString<float32>(mEditbox2Widget->getText().c_str());
	((Model*)mModel)->SetValue(newValue);
}

void Vector2Editor::Update()
{
	OC_DASSERT(mEditbox1Widget != 0);
	OC_DASSERT(mEditbox2Widget != 0);

	if (UpdatesLocked()) return;

	if (!mModel->IsValid())
	{
		mEditbox1Widget->setText("");
		mEditbox2Widget->setText("");
		return;
	}
	Vector2 value = ((Model*)mModel)->GetValue();
	mEditbox1Widget->setText(Utils::StringConverter::ToString(value.x));
	mEditbox2Widget->setText(Utils::StringConverter::ToString(value.y));
}