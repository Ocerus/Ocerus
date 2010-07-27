/// @file
/// Declares an editor for layers.

#ifndef _LAYEREDITOR_H_
#define _LAYEREDITOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"
#include "GUISystem/CEGUIForwards.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

namespace Editor
{
	/// The LayerEditor class provides a value editor for layers. This editor only displays
	/// the name of the layer and allows to change the layer by drag&dropping a layer from
	/// the layer manager.
	/// @see LayerWindow
	class LayerEditor: public AbstractValueEditor
	{
	public:
		typedef ITypedValueEditorModel<EntitySystem::LayerID> Model;

		/// Constructs a LayerEditor that uses given model.
		LayerEditor(Model* model): mModel(model), mEditboxWidget(0) { }

		/// Destroys the LayerEditor and its model.
		~LayerEditor();

		/// Creates the main widget of this editor and returns it.
		virtual CEGUI::Window* CreateWidget(const CEGUI::String& namePrefix);

		/// Does nothing. LayerEditor is submitted automatically when drag&dropped from resource list.
		virtual void Submit();

		/// Polls the model for current value and updates the editor widget, unless
		/// the editor is locked for updates.
		virtual void Update();

		/// @name CEGUI callbacks
		//@{
			bool OnEventDragDropItemDropped(const CEGUI::EventArgs&);
			bool OnEventButtonRemovePressed(const CEGUI::EventArgs&);
		//@}

	private:
		Model* mModel;
		CEGUI::Window* mEditboxWidget;
	};
}

#endif // _LAYEREDITOR_H_
