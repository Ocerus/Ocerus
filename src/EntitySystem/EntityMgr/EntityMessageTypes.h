/// @file
/// Here are definitions of types for all entity message in the system.
/// The macro used for definition is inside EntityMessage.h.

/// Sent shortly after the entity is created and all initial properties are set.
ENTITY_MESSAGE_TYPE(INIT, "void OnInit()")
/// Sent shortly after the INIT message is sent. This allows initialization of components which need other
/// components to be inited first.
ENTITY_MESSAGE_TYPE(POST_INIT, "void OnPostInit()")
/// Sent shortly before the entity is destroyed.
ENTITY_MESSAGE_TYPE(DESTROY, "void OnDestroy()")
/// Sent when the entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW, "void OnDraw()")
/// Sent when the underlay under the entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW_UNDERLAY, "void OnDrawUnderlay()")
/// Sent when the overlay above the entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW_OVERLAY, "void OnDrawOverlay()")
/// Sent when the selected entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW_SELECTED, "void OnDrawSelected()")
/// Sent when the logic of the entity is to be updated.
ENTITY_MESSAGE_TYPE(UPDATE_LOGIC, "void OnUpdateLogic()")
/// Sent when the physics of the entity is to be updated.
ENTITY_MESSAGE_TYPE(UPDATE_PHYSICS, "void OnUpdatePhysics()")
/// Sent when the mouse attempts to pick the entity.
ENTITY_MESSAGE_TYPE(MOUSE_PICK, "void OnMousePick()")