#builds the rudeconfig library

cmake_minimum_required(VERSION 2.6)

#find_package(GLUT) 
#find_package(GLUI)

set(SRC_FILES
    Source/Box2D/Collision/b2BroadPhase.cpp
    Source/Box2D/Collision/b2TimeOfImpact.cpp
    Source/Box2D/Collision/b2Distance.cpp
    Source/Box2D/Collision/b2CollideCircle.cpp
    Source/Box2D/Collision/b2Collision.cpp
    Source/Box2D/Collision/b2CollidePolygon.cpp
    Source/Box2D/Collision/b2DynamicTree.cpp
    Source/Box2D/Collision/Shapes/b2PolygonShape.cpp
    Source/Box2D/Collision/Shapes/b2CircleShape.cpp
    Source/Box2D/Common/b2StackAllocator.cpp
    Source/Box2D/Common/b2Math.cpp
    Source/Box2D/Common/b2BlockAllocator.cpp
    Source/Box2D/Common/b2Settings.cpp
    Source/Box2D/Dynamics/b2Island.cpp
    Source/Box2D/Dynamics/b2WorldCallbacks.cpp
    Source/Box2D/Dynamics/b2ContactManager.cpp
    Source/Box2D/Dynamics/b2World.cpp
    Source/Box2D/Dynamics/Contacts/b2TOISolver.cpp
    Source/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
    Source/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
    Source/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
    Source/Box2D/Dynamics/Contacts/b2CircleContact.cpp
    Source/Box2D/Dynamics/Contacts/b2Contact.cpp
    Source/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
    Source/Box2D/Dynamics/Joints/b2GearJoint.cpp
    Source/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
    Source/Box2D/Dynamics/Joints/b2MouseJoint.cpp
    Source/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
    Source/Box2D/Dynamics/Joints/b2Joint.cpp
    Source/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
    Source/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
    Source/Box2D/Dynamics/Joints/b2LineJoint.cpp
    Source/Box2D/Dynamics/Joints/b2WeldJoint.cpp
    Source/Box2D/Dynamics/b2Body.cpp
    Source/Box2D/Dynamics/b2Fixture.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/src
)

add_library( box2d ${SRC_FILES} )
