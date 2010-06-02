#include "Common.h"
#include "MessageBox.h"

#include "CEGUITools.h"

using namespace GUISystem;

MessageBox::MessageBox(MessageBox::eMessageBoxType type, int32 tag): mType(type), mTag(tag), mCallback(0), mMessageBox(0)
{
	CEGUI::String windowName;
	int i = 0;
	do
	{
		i++;
		windowName = "MessageBox" + StringConverter::ToString(i);
	}
	while (gCEGUIWM.isWindowPresent(windowName));
	mMessageBox = LoadWindowLayout("MessageBox.layout", windowName);
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
	gCEGUIWM.destroyWindow(mMessageBox);
}

void MessageBox::SetText(const CEGUI::String& text)
{
	CEGUI::Window* messageText = mMessageBox->getChild(mMessageBox->getName() + "/MessageText");
	messageText->setText(text);
}

void MessageBox::Show()
{	
	gGUIMgr.GetRootLayout()->addChildWindow(mMessageBox);
}

void MessageBox::RegisterCallback(MessageBox::CallbackBase* callback)
{
	delete mCallback;
	mCallback = callback;
}

void MessageBox::SetButtons(const MessageBoxButtons& buttons)
{
	float32 step = 1 / (float32)(buttons.size() + 1);
	float32 left = step;
	float32 sumWidth = 0;
	for (MessageBoxButtons::const_iterator it = buttons.begin(); it != buttons.end(); ++it)
	{
		CEGUI::Window* button = GetButton(*it);
		OC_DASSERT(button != 0);
		float32 width = button->getWidth().d_offset;
		sumWidth += width;

		button->setPosition(CEGUI::UVector2(CEGUI::UDim(left, -width / 2), button->getPosition().d_y));
		button->setWidth(CEGUI::UDim(0, width));
		button->setVisible(true);
		button->setID((CEGUI::uint)*it);
		button->subscribeEvent(CEGUI::PushButton::EventClicked,
				CEGUI::Event::Subscriber(&GUISystem::MessageBox::OnButtonClicked, this));

		
		left += step;
	}
	///@todo Make window wider if needed
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

void ShowMessageBox(const CEGUI::String& text, MessageBox::eMessageBoxType type,
	MessageBox::CallbackBase* callback, int32 tag)
{
	MessageBox* messageBox = new MessageBox(type, tag);
	messageBox->SetText(text);
	messageBox->RegisterCallback(callback);
	messageBox->Show();
}

