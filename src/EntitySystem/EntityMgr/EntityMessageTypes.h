/// @file
/// Here are definitions of types for all entity message in the system.

#define Params(...) {__VA_ARGS__, NUM_PROPERTY_TYPES}
#define NO_PARAMS {NUM_PROPERTY_TYPES}

/// Sent shortly after the entity is created and all initial properties are set.
ENTITY_MESSAGE_TYPE(INIT, "void OnInit()", NO_PARAMS)
/// Sent shortly after the INIT message is sent. This allows initialization of components which need other
/// components to be inited first.
ENTITY_MESSAGE_TYPE(POST_INIT, "void OnPostInit()", NO_PARAMS)
/// Sent shortly before the entity is destroyed.
ENTITY_MESSAGE_TYPE(DESTROY, "void OnDestroy()", NO_PARAMS)
/// Sent when the entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW, "void OnDraw()", NO_PARAMS)
/// Sent when the underlay under the entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW_UNDERLAY, "void OnDrawUnderlay()", NO_PARAMS)
/// Sent when the overlay above the entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW_OVERLAY, "void OnDrawOverlay()", NO_PARAMS)
/// Sent when the selected entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW_SELECTED, "void OnDrawSelected()", NO_PARAMS)
/// Sent when the logic of the entity is to be updated.
ENTITY_MESSAGE_TYPE(UPDATE_LOGIC, "void OnUpdateLogic(float32)", Params(PT_FLOAT32))
/// Sent when the physics of the entity is to be updated.
ENTITY_MESSAGE_TYPE(UPDATE_PHYSICS, "void OnUpdatePhysics(float32)", Params(PT_FLOAT32))
/// Sent after the physics of the entity has been updated.
ENTITY_MESSAGE_TYPE(UPDATE_POST_PHYSICS, "void OnUpdatePostPhysics(float32)", Params(PT_FLOAT32))
/// Sent when the resources of this entity is going to be updated.
ENTITY_MESSAGE_TYPE(RESOURCE_UPDATE, "void OnResourceUpdate()", NO_PARAMS)
/// Sent in every game loop.
ENTITY_MESSAGE_TYPE(CHECK_ACTION, "void OnAction()", NO_PARAMS)


#undef Params
#undef NO_PARAMS
