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
ENTITY_MESSAGE_TYPE(DRAW, "void OnDraw(float32)", Params(PT_FLOAT32))
/// Sent when the selected entity is to be redrawn.
ENTITY_MESSAGE_TYPE(DRAW_SELECTED, "void OnDrawSelected()", NO_PARAMS)
/// Sent when the logic of the entity is to be updated.
ENTITY_MESSAGE_TYPE(UPDATE_LOGIC, "void OnUpdateLogic(float32)", Params(PT_FLOAT32))
/// Sent when the physics of the entity is going to be updated.
ENTITY_MESSAGE_TYPE(SYNC_PRE_PHYSICS, "void _SyncPrePhysics(float32)", Params(PT_FLOAT32))
/// Sent when the physics of the entity is updating.
ENTITY_MESSAGE_TYPE(SYNC_POST_PHYSICS, "void _SyncPostPhysics(float32)", Params(PT_FLOAT32))
/// Sent after the physics of the entity has been updated.
ENTITY_MESSAGE_TYPE(UPDATE_POST_PHYSICS, "void OnUpdatePostPhysics(float32)", Params(PT_FLOAT32))
/// Sent when the resources of this entity is going to be updated.
ENTITY_MESSAGE_TYPE(RESOURCE_UPDATE, "void OnResourceUpdate()", NO_PARAMS)
/// Sent after a component of the entity was deleted. The component type is passed in the parameter.
ENTITY_MESSAGE_TYPE(COMPONENT_DESTROYED, "void OnComponentDestroyed(uint32)", Params(PT_UINT32))
/// Sent when a component which was just created needs to notify other components of its creation. The component type is passed in the parameter.
ENTITY_MESSAGE_TYPE(COMPONENT_CREATED, "void OnComponentCreated(uint32)", Params(PT_UINT32))
/// Sent in every game loop.
ENTITY_MESSAGE_TYPE(CHECK_ACTION, "void OnAction()", NO_PARAMS)
/// Sent when the user presses a key.
ENTITY_MESSAGE_TYPE(KEY_PRESSED, "void OnKeyPressed(eKeyCode, uint32)", Params(PT_KEYCODE, PT_UINT32))
/// Sent when the user releases a key.
ENTITY_MESSAGE_TYPE(KEY_RELEASED, "void OnKeyReleased(eKeyCode, uint32)", Params(PT_KEYCODE, PT_UINT32))
/// Sent after two entities collide.
ENTITY_MESSAGE_TYPE(COLLISION_STARTED, "void OnCollisionStarted(EntityHandle, Vector2, Vector2)", Params(PT_ENTITYHANDLE, PT_VECTOR2, PT_VECTOR2))
/// Sent after two entities stop to collide.
ENTITY_MESSAGE_TYPE(COLLISION_ENDED, "void OnCollisionEnded(EntityHandle)", Params(PT_ENTITYHANDLE))


#undef Params
#undef NO_PARAMS
