#include "Common.h"
#include "MessageBox.h"

#include "CEGUITools.h"

using namespace GUISystem;

const float32 INNER_FRAME_OFFSET = 20.0f;
const float32 BUTTON_MARGIN = 10.0f;

MessageBox::MessageBox(MessageBox::eMessageBoxType type, int32 tag): mType(type), mTag(tag), mCallback(0), mMessageBox(0), mMinWidth(0)
{
	mMessageBox = gGUIMgr.LoadSystemLayout("gui/layouts/MessageBox.layout", "MessageBox");
	OC_ASSERT(mMessageBox);
	mMessageBox->setModalState(true);

	CEGUI::Window* btnOK = mMessageBox->getChild(mMessageBox->getName() + "/ButtonOK");
	mButtons.push_back(pair<eMessageBoxButton, CEGUI::Window*>(MBB_OK, btnOK));
	
	CEGUI::Window* btnCancel = mMessageBox->getChild(mMessageBox->getName() + "/ButtonCancel");
	mButtons.push_back(pair<eMessageBoxButton, CEGUI::Window*>(MBB_CANCEL, btnCancel));
	
	CEGUI::Window* btnYes = mMessageBox->getChild(mMessageBox->getName() + "/ButtonYes");
	mButtons.push_back(pair<eMessageBoxButton, CEGUI::Window*>(MBB_YES, btnYes));

	CEGUI::Window* btnNo = mMessageBox->getChild(mMessageBox->getName() + "/ButtonNo");
	mButtons.push_back(pair<eMessageBoxButton, CEGUI::Window*>(MBB_NO, btnNo));

	MessageBoxButtons buttons;
	switch(mType)
	{
	case MBT_OK:
		buttons.push_back(MBB_OK);
		break;
	case MBT_OK_CANCEL:
		buttons.push_back(MBB_OK);
		buttons.push_back(MBB_CANCEL);
		break;
	case MBT_YES_NO:
		buttons.push_back(MBB_YES);
		buttons.push_back(MBB_NO);
		break;
	case MBT_YES_NO_CANCEL:
		buttons.push_back(MBB_YES);
		buttons.push_back(MBB_NO);
		buttons.push_back(MBB_CANCEL);
		break;
	default:
		OC_NOT_REACHED();
		break;
	}
	SetButtons(buttons);
}

MessageBox::~MessageBox()
{
	delete mCallback;
	gGUIMgr.DestroyWindowDirectly(mMessageBox);
}

void MessageBox::SetText(const CEGUI::String& text)
{
	CEGUI::Window* messageText = mMessageBox->getChild(mMessageBox->getName() + "/MessageText");
	const float32 offset = 10;
	float32 buttonHeight = mMessageBox->getChild(mMessageBox->getName() + "/ButtonOK")->getPixelSize().d_height;
	messageText->setText(text);
	float32 textWidth = StringConverter::FromString<float32>(messageText->getProperty("HorzExtent").c_str());
	float32 textHeight = StringConverter::FromString<float32>(messageText->getProperty("VertExtent").c_str());

	messageText->setArea(CEGUI::UDim(0,offset), CEGUI::UDim(0,0), CEGUI::UDim(1, -2.0f*offset), CEGUI::UDim(1, -buttonHeight-offset));
	mMessageBox->setWidth(CEGUI::UDim(0, textWidth + 2.0f*offset + INNER_FRAME_OFFSET));
	mMessageBox->setHeight(CEGUI::UDim(0, textHeight + buttonHeight + offset + INNER_FRAME_OFFSET));
	mMessageBox->setXPosition(CEGUI::UDim(0.5f, -0.5f*mMessageBox->getPixelSize().d_width));
	mMessageBox->setYPosition(CEGUI::UDim(0.5f, -0.5f*mMessageBox->getPixelSize().d_height));

	EnsureWindowIsWideEnough();
}

void MessageBox::Show()
{	
	gGUIMgr.GetGUISheet()->addChildWindow(mMessageBox);
}

void MessageBox::RegisterCallback(MessageBox::CallbackBase* callback)
{
	delete mCallback;
	mCallback = callback;
}

void MessageBox::SetButtons(const MessageBoxButtons& buttons)
{
	float32 sumWidth = 0;
	for (MessageBoxButtons::const_iterator it = buttons.begin(); it != buttons.end(); ++it)
	{
		CEGUI::Window* button = GetButton(*it);
		OC_DASSERT(button != 0);
		float32 width = button->getWidth().d_offset;
		sumWidth += width;
	}

	float32 totalWidth = sumWidth + BUTTON_MARGIN * (buttons.size() - 1);

	float32 left = 0;
	for (MessageBoxButtons::const_iterator it = buttons.begin(); it != buttons.end(); ++it)
	{
		CEGUI::Window* button = GetButton(*it);
		OC_DASSERT(button != 0);
		float32 width = button->getWidth().d_offset;

		button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f, -0.5f * totalWidth + left), button->getPosition().d_y));
		button->setWidth(CEGUI::UDim(0, width));
		button->setVisible(true);
		button->setID((CEGUI::uint)*it);
		button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUISystem::MessageBox::OnButtonClicked, this));
		
		left += width + BUTTON_MARGIN;
	}

	mMinWidth = totalWidth + 2.0f * BUTTON_MARGIN;
}

CEGUI::Window* MessageBox::GetButton(eMessageBoxButton button)
{
	for (Buttons::const_iterator it = mButtons.begin(); it != mButtons.end(); ++it)
		if (it->first == button) return it->second;

	return 0;
}

bool MessageBox::OnButtonClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	eMessageBoxButton button = (eMessageBoxButton)args.window->getID();
	if (mCallback != 0)
	{
		mCallback->execute(button, mTag);
	}
	delete this;
	return true;
}

void GUISystem::ShowMessageBox(const CEGUI::String& text, MessageBox::eMessageBoxType type, MessageBox::CallbackBase* callback, int32 tag)
{
	MessageBox* messageBox = new MessageBox(type, tag);
	messageBox->SetText(text);
	messageBox->RegisterCallback(callback);
	messageBox->Show();
}

void GUISystem::MessageBox::EnsureWindowIsWideEnough()
{
	if (mMinWidth > mMessageBox->getPixelSize().d_width)
	{
		mMessageBox->setWidth(CEGUI::UDim(0, mMinWidth + INNER_FRAME_OFFSET));
	}
}