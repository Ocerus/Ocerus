/// @defgroup globals Global Functions And Variables
/// @ingroup globals
//@{


/// Returns the handle of the entity that is processing the event handler the code is currently in.
EntityHandle this;

/// Global reference to the entity manager singleton.
EntityMgr& gEntityMgr;

/// Global reference to the input manager singleton.
InputMgr& gInputMgr;

/// Global reference to the project singleton.
Project& gProject;

/// Global reference to the game singleton.
Game& game;

/// Global reference to the GUI manager singleton.
GUIMgr& gGUIMgr;






/// Entity identifier. Positive IDs determine actual entities while negative IDs determine entity prototypes.
typedef int32 EntityID;

/// Invalid ID for an entity.
const EntityID INVALID_ENTITY_ID = 0;

/// Component identifier. It is unique only within a single entity (parent of the components).
typedef INT32 ComponentID;

/// User defined tag assigned to entities.
typedef uint16 EntityTag;

/// Type for storing access flags defined by ePropertyAccess.
typedef uint8 PropertyAccessFlags;



/// Enumerates types of messages sent to entities.
/// @see events for the description of the message/event types.
enum eEntityMessageType
{
	INIT,
	POST_INIT,
	DESTROY,
	DRAW,
	DRAW_SELECTED,
	UPDATE_LOGIC,
	SYNC_PRE_PHYSICS,
	SYNC_POST_PHYSICS,
	UPDATE_POST_PHYSICS,
	RESOURCE_UPDATE,
	COMPONENT_DESTROYED,
	COMPONENT_CREATED,
	CHECK_ACTION,
	KEY_PRESSED,
	KEY_RELEASED,
	COLLISION_STARTED,
	COLLISION_ENDED
};

/// Result of the processing of a message.
enum eEntityMessageResult
{
	/// The message was ignored by every entity and component. None of them processed it.
	RESULT_IGNORED,
	/// The message was processed by all entities without any error.
	RESULT_OK,
	/// Some of the entities processing the message produced an error.
	RESULT_ERROR
};

/// Type of the entity being created.
enum eEntityDescriptionKind
{
	/// Common entity.
	EK_ENTITY,
	/// Prototype entity.
	EK_PROTOTYPE
};

/// Type of the component. It directly corresponds to the type name you can see in the entity editor window.
enum eComponentType
{
	Camera,
	DynamicBody,
	GUILayout,
	Script,
	Sprite,
	StaticBody,
	Model,
	PolygonCollider,
	Transform
};

/// Restrictions of access which can be granted to a property.
/// The property can be access from scripts, from editor, during the init, ... By using these values you can
/// define a filter and forbid to access certain properties from certain parts of the code.
///	Note that it is assumed that this type fits into PropertyAccessFlags!
enum ePropertyAccess 
{ 
	/// The property is accessible only from the C++ code. This must be explicitly stated as otherwise the mask would
	/// not work on the property. That's also why zero flag has no meaning.
	PA_NONE=1<<0, 
	/// The property can be read from edit tools.
	PA_EDIT_READ=1<<1, 
	/// The property can be written from edit tools.
	PA_EDIT_WRITE=1<<2, 
	/// The property can be read from scripts.
	PA_SCRIPT_READ=1<<3, 
	/// The property can be written from scripts.
	PA_SCRIPT_WRITE=1<<4,
	/// The property can be written during the component init.
	PA_INIT=1<<5,
	/// The property should not be loaded/saved from/to stream
	PA_TRANSIENT=1<<6,

	/// Full access to the property.
	PA_FULL_ACCESS=(1<<6) - 1
};

/// Type for storing access flags defined by ePropertyAccess.
typedef uint8 PropertyAccessFlags;

/// See <a href="../../doxygen/html/_key_codes_8h_source.html">KeyCodes.h</a> in the engine doxygen
/// for the list of key codes.
enum eKeyCode {};

/// All possible buttons of the mouse device.
enum eMouseButton
{
	MBTN_LEFT,
	MBTN_RIGHT,
	MBTN_MIDDLE,
	MBTN_UNKNOWN
};


//@}