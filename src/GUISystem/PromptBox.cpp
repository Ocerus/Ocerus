#include "Common.h"
#include "PromptBox.h"

#include "CEGUICommon.h"

using namespace GUISystem;

const float32 INNER_FRAME_OFFSET = 20.0f;
const float32 BUTTON_MARGIN = 10.0f;

PromptBox::PromptBox(int32 tag): mTag(tag), mPromptBox(0), mMinWidth(0)
{
	CEGUI::String windowName;
	static int i = 0;
	do
	{
		i++;
		windowName = "PromptBox" + StringConverter::ToString(i);
	}
	while (gGUIMgr.WindowExists(windowName.c_str()));
	mPromptBox = gGUIMgr.LoadSystemLayout("PromptBox.layout", windowName);
	mPromptBox->setModalState(true);

	CEGUI::Window* btnOK = mPromptBox->getChild(mPromptBox->getName() + "/ButtonOK");
	btnOK->setID(0);
	btnOK->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUISystem::PromptBox::OnButtonClicked, this));
	CEGUI::Window* btnCancel = mPromptBox->getChild(mPromptBox->getName() + "/ButtonCancel");
	btnCancel->setID(1);
	btnCancel->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUISystem::PromptBox::OnButtonClicked, this));
}

PromptBox::~PromptBox()
{
	gGUIMgr.DestroyWindowDirectly(mPromptBox);
}

void PromptBox::SetText(const CEGUI::String& text)
{
	CEGUI_TRY;
	{
		CEGUI::Window* messageText = mPromptBox->getChild(mPromptBox->getName() + "/MessageText");
		CEGUI::Window* editbox = mPromptBox->getChild(mPromptBox->getName() + "/Editbox");
		const float32 offset = 10;
		float32 buttonHeight = mPromptBox->getChild(mPromptBox->getName() + "/ButtonOK")->getPixelSize().d_height;
		float32 editboxHeight = editbox->getPixelSize().d_height;
	
		messageText->setText(text);
		float32 textWidth = StringConverter::FromString<float32>(messageText->getProperty("HorzExtent").c_str());
		float32 textHeight = StringConverter::FromString<float32>(messageText->getProperty("VertExtent").c_str());

		editbox->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&PromptBox::OnEditboxKeyDown, this));

		messageText->setArea(CEGUI::UDim(0,offset), CEGUI::UDim(0,0), CEGUI::UDim(1, -2.0f*offset), CEGUI::UDim(1, -buttonHeight-editboxHeight - offset));
		mPromptBox->setWidth(CEGUI::UDim(0, textWidth + 2.0f*offset + INNER_FRAME_OFFSET));
		mPromptBox->setHeight(CEGUI::UDim(0, textHeight + buttonHeight + editboxHeight + offset + INNER_FRAME_OFFSET));
		mPromptBox->setXPosition(CEGUI::UDim(0.5f, -0.5f*mPromptBox->getPixelSize().d_width));
		mPromptBox->setYPosition(CEGUI::UDim(0.5f, -0.5f*mPromptBox->getPixelSize().d_height));

		EnsureWindowIsWideEnough();
	}
	CEGUI_CATCH;
}

void PromptBox::Show()
{	
	gGUIMgr.GetGUISheet()->addChildWindow(mPromptBox);
	mPromptBox->getChild(mPromptBox->getName() + "/Editbox")->activate();
}

bool PromptBox::OnButtonClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	bool clickedOK = (args.window->getID() == 0);
	SendPrompt(clickedOK);
	return true;
}

bool PromptBox::OnEditboxKeyDown(const CEGUI::EventArgs& e)
{
	const CEGUI::KeyEventArgs& args = static_cast<const CEGUI::KeyEventArgs&>(e);
	switch (args.scancode)
	{
		case CEGUI::Key::Return:
		case CEGUI::Key::NumpadEnter:
			SendPrompt(true);
			return true;
		case CEGUI::Key::Escape:
			SendPrompt(false);
			return true;
		default:
			break;
	}
	return false;
}

void PromptBox::SendPrompt(bool clickedOK)
{
	if (mCallback.IsSet())
	{
		CEGUI::Window* editbox = mPromptBox->getChild(mPromptBox->getName() + "/Editbox");
		string text = editbox->getText().c_str();
		mCallback.Call(clickedOK, text, mTag);
	}
	delete this;
}

void GUISystem::PromptBox::EnsureWindowIsWideEnough()
{
	if (mMinWidth > mPromptBox->getPixelSize().d_width)
	{
		mPromptBox->setWidth(CEGUI::UDim(0, mMinWidth + INNER_FRAME_OFFSET));
	}
}

void GUISystem::ShowPromptBox(const CEGUI::String& text, PromptBox::Callback callback, int32 tag)
{
	PromptBox* promptBox = new PromptBox(tag);
	promptBox->SetText(text);
	promptBox->RegisterCallback(callback);
	promptBox->Show();
}
