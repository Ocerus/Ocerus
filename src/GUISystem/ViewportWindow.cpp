#include "Common.h"
#include "ViewportWindow.h"
#include "GfxSystem/GfxRenderer.h"
#include "GfxSystem/GfxWindow.h"
#include "GfxSystem/DragDropCameraMover.h"
#include "CEGUITools.h"

using namespace GUISystem;

const CEGUI::String GUISystem::ViewportWindow::WidgetTypeName("Ocerus/Viewport");

GUISystem::ViewportWindow::ViewportWindow(const CEGUI::String& type, const CEGUI::String& name):
		CEGUI::FrameWindow(type, name), mRenderTarget(GfxSystem::InvalidRenderTargetID),
		mIsMovableContent(0), mCameraMover(0)
{
}

GUISystem::ViewportWindow::~ViewportWindow()
{
	if (mIsMovableContent) DeleteCameraMover();
}

void GUISystem::ViewportWindow::initialiseComponents()
{
	CEGUI::FrameWindow::initialiseComponents();
	setCloseButtonEnabled(false);
	setFrameEnabled(true);
	setMousePassThroughEnabled(false);
}

void GUISystem::ViewportWindow::SetCamera(const EntitySystem::EntityHandle& camera)
{
	if (mRenderTarget != GfxSystem::InvalidRenderTargetID)
	{
		gGfxRenderer.RemoveRenderTarget(mRenderTarget);
	}
	Vector2 position, size;
	GetArea(position, size);
	mRenderTarget = gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(position, size, false, true), camera);
	
	if (mIsMovableContent)
	{
		DeleteCameraMover();
		CreateCameraMover();
	}
}

void GUISystem::ViewportWindow::SetMovableContent(bool isMovableContent)
{
	bool valueChanged = (isMovableContent != mIsMovableContent);
	mIsMovableContent = isMovableContent;
	if (!valueChanged) return;

	if (mIsMovableContent) CreateCameraMover();
	else DeleteCameraMover();
}

void GUISystem::ViewportWindow::onSized(CEGUI::WindowEventArgs& e)
{
	CEGUI::FrameWindow::onSized(e);
	UpdateViewportArea();
}

void GUISystem::ViewportWindow::onMoved(CEGUI::WindowEventArgs& e)
{
	CEGUI::FrameWindow::onMoved(e);
	UpdateViewportArea();
}

void GUISystem::ViewportWindow::UpdateViewportArea()
{
	if (mRenderTarget == GfxSystem::InvalidRenderTargetID) return;
	GfxSystem::GfxViewport* viewport = gGfxRenderer.GetRenderTargetViewport(mRenderTarget);
	OC_DASSERT(viewport != 0);
	Vector2 position, size;
	GetArea(position, size);
	viewport->SetPosition(position);
	viewport->SetSize(size);
}

void GUISystem::ViewportWindow::GetArea(Vector2& position, Vector2& size) const
{
	const CEGUI::Size& windowSize = CEGUI::System::getSingleton().getRenderer()->getDisplaySize();
	const CEGUI::Vector2& ceguiPosition = getPosition().asRelative(windowSize);
	const CEGUI::Vector2& ceguiSize = getSize().asRelative(windowSize);
	position.x = ceguiPosition.d_x;
	position.y = ceguiPosition.d_y;
	size.x = ceguiSize.d_x;
	size.y = ceguiSize.d_y;
}

void GUISystem::ViewportWindow::CreateCameraMover()
{
	OC_DASSERT(mCameraMover == 0);
	mCameraMover = new GfxSystem::DragDropCameraMover(mRenderTarget);
	AddInputListener(mCameraMover);
}

void GUISystem::ViewportWindow::DeleteCameraMover()
{
	OC_DASSERT(mCameraMover != 0);
	RemoveInputListener(mCameraMover);
	delete mCameraMover;
	mCameraMover = 0;
}

EntitySystem::EntityHandle GUISystem::ViewportWindow::GetCamera() const
{
	return gGfxRenderer.GetRenderTargetCamera(mRenderTarget);
}

void GUISystem::ViewportWindow::AddInputListener( InputSystem::IInputListener* listener )
{
	ListenersList::iterator it = find(mListeners.begin(), mListeners.end(), listener);
	if (it == mListeners.end())
	{
		mListeners.push_back(listener);
	}
}

void GUISystem::ViewportWindow::RemoveInputListener( InputSystem::IInputListener* listener )
{
	ListenersList::iterator it = find(mListeners.begin(), mListeners.end(), listener);
	if (it != mListeners.end())
	{
		mListeners.erase(it);
	}
}

void GUISystem::ViewportWindow::onMouseMove( CEGUI::MouseEventArgs& e )
{
	for (ListenersList::iterator it=mListeners.begin(); it!=mListeners.end(); ++it)
	{
		(*it)->MouseMoved(*gGUIMgr.GetCurrentInputEvent().mouseInfo);
	}
}

void GUISystem::ViewportWindow::onMouseButtonDown( CEGUI::MouseEventArgs& e )
{
	for (ListenersList::iterator it=mListeners.begin(); it!=mListeners.end(); ++it)
	{
		(*it)->MouseButtonPressed(*gGUIMgr.GetCurrentInputEvent().mouseInfo, gGUIMgr.GetCurrentInputEvent().mouseButton);
	}
}

void GUISystem::ViewportWindow::onMouseButtonUp( CEGUI::MouseEventArgs& e )
{
	for (ListenersList::iterator it=mListeners.begin(); it!=mListeners.end(); ++it)
	{
		(*it)->MouseButtonReleased(*gGUIMgr.GetCurrentInputEvent().mouseInfo, gGUIMgr.GetCurrentInputEvent().mouseButton);
	}
}

void GUISystem::ViewportWindow::onMouseWheel( CEGUI::MouseEventArgs& e )
{
	onMouseMove(e);
}

void GUISystem::ViewportWindow::onKeyDown( CEGUI::KeyEventArgs& e )
{
	for (ListenersList::iterator it=mListeners.begin(); it!=mListeners.end(); ++it)
	{
		(*it)->KeyPressed(*gGUIMgr.GetCurrentInputEvent().keyInfo);
	}
}

void GUISystem::ViewportWindow::onKeyUp( CEGUI::KeyEventArgs& e )
{
	for (ListenersList::iterator it=mListeners.begin(); it!=mListeners.end(); ++it)
	{
		(*it)->KeyReleased(*gGUIMgr.GetCurrentInputEvent().keyInfo);
	}
}

void GUISystem::ViewportWindow::Activate()
{
	activate();
}