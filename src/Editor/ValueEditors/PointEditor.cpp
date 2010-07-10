#include "Common.h"
#include "PointEditor.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

void PointEditor::Submit()
{
	OC_DASSERT(mEditbox1Widget != 0);
	OC_DASSERT(mEditbox2Widget != 0);
	GfxSystem::Point newValue;
	newValue.x = Utils::StringConverter::FromString<int32>(mEditbox1Widget->getText().c_str());
	newValue.y = Utils::StringConverter::FromString<int32>(mEditbox2Widget->getText().c_str());
	((Model*)mModel)->SetValue(newValue);
}

void PointEditor::Update()
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
	GfxSystem::Point value = ((Model*)mModel)->GetValue();
	mEditbox1Widget->setText(Utils::StringConverter::ToString(value.x));
	mEditbox2Widget->setText(Utils::StringConverter::ToString(value.y));
}