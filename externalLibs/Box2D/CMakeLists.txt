#builds the rudeconfig library

cmake_minimum_required(VERSION 2.6)

#find_package(GLUT) 
#find_package(GLUI)

set(SRC_FILES
    Source/Collision/b2BroadPhase.cpp
    Source/Collision/b2TimeOfImpact.cpp
    Source/Collision/b2Distance.cpp
    Source/Collision/b2CollideCircle.cpp
    Source/Collision/b2PairManager.cpp
    Source/Collision/b2Collision.cpp
    Source/Collision/b2CollidePoly.cpp
    Source/Collision/Shapes/b2PolygonShape.cpp
    Source/Collision/Shapes/b2Shape.cpp
    Source/Collision/Shapes/b2CircleShape.cpp
    Source/Common/b2StackAllocator.cpp
    Source/Common/b2Math.cpp
    Source/Common/b2BlockAllocator.cpp
    Source/Common/b2Settings.cpp
    Source/Dynamics/b2Island.cpp
    Source/Dynamics/b2WorldCallbacks.cpp
    Source/Dynamics/b2ContactManager.cpp
    Source/Dynamics/b2World.cpp
    Source/Dynamics/Contacts/b2PolyContact.cpp
    Source/Dynamics/Contacts/b2PolyAndCircleContact.cpp
    Source/Dynamics/Contacts/b2ContactSolver.cpp
    Source/Dynamics/Contacts/b2CircleContact.cpp
    Source/Dynamics/Contacts/b2Contact.cpp
    Source/Dynamics/Joints/b2DistanceJoint.cpp
    Source/Dynamics/Joints/b2GearJoint.cpp
    Source/Dynamics/Joints/b2RevoluteJoint.cpp
    Source/Dynamics/Joints/b2MouseJoint.cpp
    Source/Dynamics/Joints/b2PrismaticJoint.cpp
    Source/Dynamics/Joints/b2Joint.cpp
    Source/Dynamics/Joints/b2PulleyJoint.cpp
    Source/Dynamics/b2Body.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Common
    ${CMAKE_SOURCE_DIR}/src
)

add_library( box2d ${SRC_FILES} )
