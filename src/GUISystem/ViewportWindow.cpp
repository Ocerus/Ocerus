#include "Common.h"
#include "ViewportWindow.h"
#include "GfxSystem/GfxRenderer.h"
#include "GfxSystem/GfxWindow.h"
#include "CEGUITools.h"

using namespace GUISystem;

const CEGUI::String GUISystem::ViewportWindow::WidgetTypeName("Ocerus/Viewport");

GUISystem::ViewportWindow::ViewportWindow(const CEGUI::String& type, const CEGUI::String& name): CEGUI::FrameWindow(type, name), mRenderTarget(GfxSystem::InvalidRenderTargetID)
{
}

GUISystem::ViewportWindow::~ViewportWindow()
{
}

void GUISystem::ViewportWindow::initialiseComponents()
{
	CEGUI::FrameWindow::initialiseComponents();
	setText("Viewport without camera");
	setCloseButtonEnabled(false);
	setFrameEnabled(true);
}

void GUISystem::ViewportWindow::SetCamera(const EntitySystem::EntityHandle& camera)
{
	if (mRenderTarget != GfxSystem::InvalidRenderTargetID)
	{
		gGfxRenderer.RemoveRenderTarget(mRenderTarget);
	}
	Vector2 position, size;
	GetArea(position, size);
	mRenderTarget = gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(position, size, false), camera);
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