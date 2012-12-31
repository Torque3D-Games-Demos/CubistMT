//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
// start jc
#include "gfx/video/webTexture.h"
// end jc
#include "T3D/physics/physicsShape.h"

#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "core/resourceManager.h"
#include "math/mathIO.h"
#include "T3D/physics/physicsPlugin.h"
#include "T3D/physics/physicsBody.h"
#include "T3D/physics/physicsWorld.h"
#include "T3D/physics/physicsCollision.h"
#include "collision/concretePolyList.h"
#include "ts/tsShapeInstance.h"
#include "scene/sceneRenderState.h"
#include "gfx/gfxTransformSaver.h"
#include "T3D/physics/physicsDebris.h"
#include "T3D/fx/explosion.h"
#include "T3D/containerQuery.h"
#include "lighting/lightQuery.h"
#include "console/engineAPI.h"
// start jc
//#include "gui/controls/guiWebViewCtrl.h"
//#include "gfx/video/webTexture.h"
#include "console/simTUIO.h"
#include "collision/earlyOutPolyList.h"
#include "sim/netConnection.h"
#include "T3D/gameBase/gameConnection.h"
//#include "materials/baseMatInstance.h"
#include "materials/matInstance.h"
#include "materials/ProcessedMaterial.h"
// end jc


bool PhysicsShape::smNoCorrections = false;
bool PhysicsShape::smNoSmoothing = false;

ImplementEnumType( PhysicsSimType,
   "How to handle the physics simulation with the client's and server.\n"
   "@ingroup Physics\n\n")
   { PhysicsShapeData::SimType_ClientOnly,    "ClientOnly", "Only handle physics on the client.\n"  },
   { PhysicsShapeData::SimType_ServerOnly,   "ServerOnly", "Only handle physics on the server.\n" },
   { PhysicsShapeData::SimType_ClientServer,  "ClientServer", "Handle physics on both the client and server.\n"   }
EndImplementEnumType;


IMPLEMENT_CO_DATABLOCK_V1( PhysicsShapeData );

ConsoleDocClass( PhysicsShapeData,
   
   "@brief Defines the properties of a PhysicsShape.\n\n"
   "@see PhysicsShape.\n"   
   "@ingroup Physics"
);

// start jc
IMPLEMENT_CALLBACK( PhysicsShapeData, onMouseDown, void, ( PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord ),
														  ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the mouse is pressed down while in this control.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Mouse was pressed down in this control, causing the callback\n"
   "GuiMouseEventCtrl::onMouseDown(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onMouseUp, void, ( PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord ),
													    ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the mouse is released while in this control.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Mouse was released in this control, causing the callback\n"
   "GuiMouseEventCtrl::onMouseUp(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onMouseMove, void, ( PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord  ),
												   ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the mouse is moved (without dragging) while in this control.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Mouse was moved in this control, causing the callback\n"
   "GuiMouseEventCtrl::onMouseMove(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onMouseDragged, void, (  PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord  ),
												   ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the mouse is dragged while in this control.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Mouse was dragged in this control, causing the callback\n"
   "GuiMouseEventCtrl::onMouseDragged(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onMouseEnter, void, (  PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord  ),
												   ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the mouse enters this control.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Mouse entered this control, causing the callback\n"
   "GuiMouseEventCtrl::onMouseEnter(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onMouseLeave, void, (  PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord  ),
												   ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the mouse leaves this control.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Mouse left this control, causing the callback\n"
   "GuiMouseEventCtrl::onMouseLeave(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onRightMouseDown, void, (  PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord  ),
												   ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the right mouse button is pressed while in this control.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Right mouse button was pressed in this control, causing the callback\n"
   "GuiMouseEventCtrl::onRightMouseDown(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onRightMouseUp, void, (  PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord  ),
												   ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the right mouse button is released while in this control.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Right mouse button was released in this control, causing the callback\n"
   "GuiMouseEventCtrl::onRightMouseUp(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onRightMouseDragged, void, (  PhysicsShape* obj, U8 modifier, Point2I mousePoint,U8 mouseClickCount, Point3F pos, Point3F vec, Point2F mouseUVCoord  ),
												   ( obj, modifier, mousePoint, mouseClickCount, pos, vec, mouseUVCoord ),
   "@brief Callback that occurs whenever the mouse is dragged in this control while the right mouse button is pressed.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Right mouse button was dragged in this control, causing the callback\n"
   "GuiMouseEventCtrl::onRightMouseDragged(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onTouchDown, bool, ( PhysicsShape* obj, S32 id, Point2I touchPoint, Point3F pos, Point3F vec, Point2F touchUVCoord  ),
												   ( obj, id, touchPoint, pos, vec, touchUVCoord ),
   "@brief Callback that occurs whenever the mouse is dragged in this control while the right mouse button is pressed.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Right mouse button was dragged in this control, causing the callback\n"
   "GuiMouseEventCtrl::onRightMouseDragged(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onTouchMove, bool, ( PhysicsShape* obj, S32 id, Point2I touchPoint, Point3F pos, Point3F vec, Point2F touchUVCoord  ),
												   ( obj, id, touchPoint, pos, vec, touchUVCoord ),
   "@brief Callback that occurs whenever the mouse is dragged in this control while the right mouse button is pressed.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Right mouse button was dragged in this control, causing the callback\n"
   "GuiMouseEventCtrl::onRightMouseDragged(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onTouchUp, bool, ( PhysicsShape* obj, S32 id, Point2I touchPoint, Point3F pos, Point3F vec, Point2F touchUVCoord  ),
												   ( obj, id, touchPoint, pos, vec, touchUVCoord ),
   "@brief Callback that occurs whenever the mouse is dragged in this control while the right mouse button is pressed.\n\n"
   "@param modifier Key that was pressed during this callback. Values are:\n\n" 
   "$EventModifier::RSHIFT\n\n"
   "$EventModifier::SHIFT\n\n"
   "$EventModifier::LCTRL\n\n"
   "$EventModifier::RCTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::CTRL\n\n"
   "$EventModifier::RALT\n\n"
   "$EventModifier::ALT\n\n"
   "@param mousePoint X/Y location of the mouse point\n"
   "@param mouseClickCount How many mouse clicks have occured for this event\n\n"
   "@tsexample\n"
   "// Right mouse button was dragged in this control, causing the callback\n"
   "GuiMouseEventCtrl::onRightMouseDragged(%this,%modifier,%mousePoint,%mouseClickCount)\n"
   "{\n"
   "	// Code to call when a mouse event occurs.\n"
   "}\n"
   "@endtsexample\n\n"
   "@see GuiControl\n\n"
);
IMPLEMENT_CALLBACK( PhysicsShapeData, onAddClient, void, ( PhysicsShape* obj ), ( obj ),
	"Called when this ScriptGroup is added to the system.\n"
	"@param ID Unique object ID assigned when created (%this in script).\n" 
);

IMPLEMENT_CALLBACK( PhysicsShapeData, onMaterialsLoaded, void, (PhysicsShape* obj), (obj),
	"Called when this ScriptGroup is added to the system.\n"
	"@param ID Unique object ID assigned when created (%this in script).\n" 
);


// end jc

PhysicsShapeData::PhysicsShapeData()
   :  shapeName( NULL ),
      mass( 1.0f ),
      dynamicFriction( 0.0f ),
      staticFriction( 0.0f ),
      restitution( 0.0f ),
      linearDamping( 0.0f ),
      angularDamping( 0.0f ),
      linearSleepThreshold( 1.0f ),
      angularSleepThreshold( 1.0f ),
      waterDampingScale( 1.0f ),
      buoyancyDensity( 0.0f ),
      simType( SimType_ClientServer )      
{
}

PhysicsShapeData::~PhysicsShapeData()
{
}

void PhysicsShapeData::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Media");

      addField( "shapeName", TypeShapeFilename, Offset( shapeName, PhysicsShapeData ),
         "@brief Path to the .DAE or .DTS file to use for this shape.\n\n"
         "Compatable with Live-Asset Reloading. ");

      addField( "debris", TYPEID< SimObjectRef<PhysicsDebrisData> >(), Offset( debris, PhysicsShapeData ),
         "@brief Name of a PhysicsDebrisData to spawn when this shape is destroyed (optional)." );

      addField( "explosion", TYPEID< SimObjectRef<ExplosionData> >(), Offset( explosion, PhysicsShapeData ),
         "@brief Name of an ExplosionData to spawn when this shape is destroyed (optional)." );

      addField( "destroyedShape", TYPEID< SimObjectRef<PhysicsShapeData> >(), Offset( destroyedShape, PhysicsShapeData ),
         "@brief Name of a PhysicsShapeData to spawn when this shape is destroyed (optional)." );

   endGroup("Media");

   addGroup( "Physics" );
      
      addField( "mass", TypeF32, Offset( mass, PhysicsShapeData ),
         "@brief Value representing the mass of the shape.\n\n"
         "A shape's mass influences the magnitude of any force exerted on it. "
         "For example, a PhysicsShape with a large mass requires a much larger force to move than "
         "the same shape with a smaller mass.\n"
         "@note A mass of zero will create a kinematic shape while anything greater will create a dynamic shape.");

      addField( "friction", TypeF32, Offset( dynamicFriction, PhysicsShapeData ),
         "@brief Coefficient of kinetic %friction to be applied to the shape.\n\n" 
         "Kinetic %friction reduces the velocity of a moving object while it is in contact with a surface. "
         "A higher coefficient will result in a larger velocity reduction. "
         "A shape's friction should be lower than it's staticFriction, but larger than 0.\n\n"
         "@note This value is only applied while an object is in motion. For an object starting at rest, see PhysicsShape::staticFriction");

      addField( "staticFriction", TypeF32, Offset( staticFriction, PhysicsShapeData ),
         "@brief Coefficient of static %friction to be applied to the shape.\n\n" 
         "Static %friction determines the force needed to start moving an at-rest object in contact with a surface. "
         "If the force applied onto shape cannot overcome the force of static %friction, the shape will remain at rest. "
         "A larger coefficient will require a larger force to start motion. "
         "This value should be larger than zero and the physicsShape's friction.\n\n"
         "@note This value is only applied while an object is at rest. For an object in motion, see PhysicsShape::friction");

      addField( "restitution", TypeF32, Offset( restitution, PhysicsShapeData ),
         "@brief Coeffecient of a bounce applied to the shape in response to a collision.\n\n"
         "Restitution is a ratio of a shape's velocity before and after a collision. "
         "A value of 0 will zero out a shape's post-collision velocity, making it stop on contact. "
         "Larger values will remove less velocity after a collision, making it \'bounce\' with a greater force. "
         "Normal %restitution values range between 0 and 1.0."
         "@note Values near or equaling 1.0 are likely to cause undesirable results in the physics simulation."
         " Because of this it is reccomended to avoid values close to 1.0");

      addField( "linearDamping", TypeF32, Offset( linearDamping, PhysicsShapeData ),
         "@brief Value that reduces an object's linear velocity over time.\n\n"
         "Larger values will cause velocity to decay quicker.\n\n" );

      addField( "angularDamping", TypeF32, Offset( angularDamping, PhysicsShapeData ),
         "@brief Value that reduces an object's rotational velocity over time.\n\n"
         "Larger values will cause velocity to decay quicker.\n\n" );

      addField( "linearSleepThreshold", TypeF32, Offset( linearSleepThreshold, PhysicsShapeData ),
         "@brief Minimum linear velocity before the shape can be put to sleep.\n\n"
         "This should be a positive value. Shapes put to sleep will not be simulated in order to save system resources.\n\n"
         "@note The shape must be dynamic.");

      addField( "angularSleepThreshold", TypeF32, Offset( angularSleepThreshold, PhysicsShapeData ),
         "@brief Minimum rotational velocity before the shape can be put to sleep.\n\n"
         "This should be a positive value. Shapes put to sleep will not be simulated in order to save system resources.\n\n"
         "@note The shape must be dynamic.");

      addField( "waterDampingScale", TypeF32, Offset( waterDampingScale, PhysicsShapeData ),
         "@brief Scale to apply to linear and angular dampening while underwater.\n\n "
         "Used with the waterViscosity of the  "
         "@see angularDamping linearDamping" );

      addField( "buoyancyDensity", TypeF32, Offset( buoyancyDensity, PhysicsShapeData ),
         "@brief The density of the shape for calculating buoyant forces.\n\n"
         "The result of the calculated buoyancy is relative to the density of the WaterObject the PhysicsShape is within.\n\n"
         "@see WaterObject::density");

   endGroup( "Physics" );   

   addGroup( "Networking" );

      addField( "simType", TYPEID< PhysicsShapeData::SimType >(), Offset( simType, PhysicsShapeData ),
         "@brief Controls whether this shape is simulated on the server, client, or both physics simulations.\n\n" );

   endGroup( "Networking" );   
}

void PhysicsShapeData::packData( BitStream *stream )
{ 
   Parent::packData( stream );

   stream->writeString( shapeName );

   stream->write( mass );
   stream->write( dynamicFriction );
   stream->write( staticFriction );
   stream->write( restitution );
   stream->write( linearDamping );
   stream->write( angularDamping );
   stream->write( linearSleepThreshold );
   stream->write( angularSleepThreshold );
   stream->write( waterDampingScale );
   stream->write( buoyancyDensity );

   stream->writeInt( simType, SimType_Bits );

   stream->writeRangedU32( debris ? debris->getId() : 0, 0, DataBlockObjectIdLast );
   stream->writeRangedU32( explosion ? explosion->getId() : 0, 0, DataBlockObjectIdLast );
   stream->writeRangedU32( destroyedShape ? destroyedShape->getId() : 0, 0, DataBlockObjectIdLast );

// start jc
   //stream->writeFlag( renderInReflections );

   // allow for client side callbacks
   stream->writeString(mSuperClassName);
   stream->writeString(mClassName);
// end jc
}

void PhysicsShapeData::unpackData( BitStream *stream )
{
   Parent::unpackData(stream);

   shapeName = stream->readSTString();

   stream->read( &mass );
   stream->read( &dynamicFriction );
   stream->read( &staticFriction );
   stream->read( &restitution );
   stream->read( &linearDamping );
   stream->read( &angularDamping );
   stream->read( &linearSleepThreshold );
   stream->read( &angularSleepThreshold );
   stream->read( &waterDampingScale );
   stream->read( &buoyancyDensity );

   simType = (SimType)stream->readInt( SimType_Bits );

   debris = stream->readRangedU32( 0, DataBlockObjectIdLast );
   explosion = stream->readRangedU32( 0, DataBlockObjectIdLast );   
   destroyedShape = stream->readRangedU32( 0, DataBlockObjectIdLast );
// start jc
//   renderInReflections = stream->readFlag();
   setSuperClassNamespace(stream->readSTString());
   setClassNamespace(stream->readSTString());
// end jc
}

bool PhysicsShapeData::onAdd()
{
   if ( !Parent::onAdd() )
      return false;

   ResourceManager::get().getChangedSignal().notify( this, &PhysicsShapeData::_onResourceChanged );
   return true;
}

void PhysicsShapeData::onRemove()
{
   ResourceManager::get().getChangedSignal().remove( this, &PhysicsShapeData::_onResourceChanged );
   Parent::onRemove();
}

void PhysicsShapeData::_onResourceChanged( const Torque::Path &path )
{
   if ( path != Path( shapeName ) )
      return;

   // Reload the changed shape.
   Resource<TSShape> reloadShape;
   PhysicsCollisionRef reloadcolShape;

   reloadShape = ResourceManager::get().load( shapeName );
   if ( !bool(reloadShape) )
   {
      Con::warnf( ConsoleLogEntry::General, "PhysicsShapeData::_onResourceChanged: Could not reload %s.", path.getFileName().c_str() );
      return;
   }

   // Reload the collision shape.
   reloadcolShape = shape->buildColShape( false, Point3F::One );

   if ( bool(reloadShape) &&  bool(colShape))
   {
      shape = reloadShape;
      colShape = reloadcolShape;
   }

   mReloadSignal.trigger();
}

bool PhysicsShapeData::preload( bool server, String &errorBuffer )
{
   if ( !Parent::preload( server, errorBuffer ) )
      return false;

   // If we don't have a physics plugin active then
   // we have to fail completely.
   if ( !PHYSICSMGR )
   {
      errorBuffer = "PhysicsShapeData::preload - No physics plugin is active!";
      return false;
   }

   if ( !shapeName || !shapeName[0] ) 
   {
      errorBuffer = "PhysicsShapeData::preload - No shape name defined.";
      return false;
   }

   // Load the shape.
   shape = ResourceManager::get().load( shapeName );
   if ( bool(shape) == false )
   {
      errorBuffer = String::ToString( "PhysicsShapeData::preload - Unable to load shape '%s'.", shapeName );
      return false;
   }

   // Prepare the shared physics collision shape.
   if ( !colShape )
   {
      colShape = shape->buildColShape( false, Point3F::One );

      // If we got here and didn't get a collision shape then
      // we need to fail... can't have a shape without collision.
      if ( !colShape )
      {
         errorBuffer = String::ToString( "PhysicsShapeData::preload - No collision found for shape '%s'.", shapeName );
         return false;
      }
   }   

   // My convex demcomposion test
   /*
   // Get the verts and triangles for the first visible detail.
   ConcretePolyList polyList;
   polyList.setTransform( &MatrixF::Identity, Point3F::One );
   TSShapeInstance shapeInst( shape, false );
   shapeInst.animate(0);
   if ( !shapeInst.buildPolyList( &polyList, 0 ) )
      return false;

   // Gah... Ratcliff's lib works on doubles... why, oh why?
   Vector<F64> doubleVerts;
   doubleVerts.setSize( polyList.mVertexList.size() * 3 );
   for ( U32 i=0; i < polyList.mVertexList.size(); i++ )
   {
      doubleVerts[ ( i * 3 ) + 0 ] = (F64)polyList.mVertexList[i].x;
      doubleVerts[ ( i * 3 ) + 1 ] = (F64)polyList.mVertexList[i].y;
      doubleVerts[ ( i * 3 ) + 2 ] = (F64)polyList.mVertexList[i].z;
   }

   using namespace ConvexDecomposition;

   class ConvexBuilder : public ConvexDecompInterface
   {
   public:

      ConvexBuilder() { }

      ~ConvexBuilder() 
      {
         for ( U32 i=0; i < mHulls.size(); i++ )
            delete mHulls[i];
      }

      virtual void ConvexDecompResult( ConvexResult &result )
      {
         FConvexResult *hull = new FConvexResult( result );
         mHulls.push_back( hull );
      }

      Vector<FConvexResult*> mHulls;
   };

 	DecompDesc d;
   d.mVcount       =	polyList.mVertexList.size();
   d.mVertices     = doubleVerts.address();
   d.mTcount       = polyList.mIndexList.size() / 3;
   d.mIndices      = polyList.mIndexList.address();
   d.mDepth        = 3;
   d.mCpercent     = 20.0f;
   d.mPpercent     = 30.0f;
   d.mMaxVertices  = 32;
   d.mSkinWidth    = 0.05f; // Need to expose this!

   ConvexBuilder builder;
   d.mCallback = &builder;
 
   if ( performConvexDecomposition( d ) < 1 || builder.mHulls.empty() )
      return false;

   // Add all the convex hull results into the collision shape.
   colShape = PHYSICSMGR->createCollision();
   for ( U32 i=0; i < builder.mHulls.size(); i++ )
      colShape->addConvex( (const Point3F*)builder.mHulls[i]->mHullVertices, 
                           builder.mHulls[i]->mHullVcount,
                           MatrixF::Identity );
   */

   return true;
}


IMPLEMENT_CO_NETOBJECT_V1(PhysicsShape);

ConsoleDocClass( PhysicsShape,
   
   "@brief Represents a destructible physical object simulated through the plugin system.\n\n"
   "@see PhysicsShapeData.\n"   
   "@ingroup Physics"
);

PhysicsShape::PhysicsShape()
   :  mPhysicsRep( NULL ),
      mWorld( NULL ),
      mShapeInst( NULL ),
      mResetPos( MatrixF::Identity ),
      mDestroyed( false ),
      mPlayAmbient( false ),
      mAmbientThread( NULL ),
      mAmbientSeq( -1 ),
// start jc
    mWebViewData(NULL),
    mWebViewID(0),
	mEnableInputEvents(false)
//   mGravityMod(1.0)
// end jc
{
   mNetFlags.set( Ghostable | ScopeAlways );
   mTypeMask |= DynamicShapeObjectType;
}

PhysicsShape::~PhysicsShape()
{
}

void PhysicsShape::consoleInit()
{
   Con::addVariable( "$PhysicsShape::noCorrections", TypeBool, &PhysicsShape::smNoCorrections,
     "@brief Determines if the shape will recieve corrections from the server or "
     "will instead be allowed to diverge.\n\n"
     "In the event that the client and server object positions/orientations "
     "differ and if this variable is true, the server will attempt to \'correct\' "
     "the client object to keep it in sync. Otherwise, client and server objects may fall out of sync.\n\n");

   Con::addVariable( "$PhysicsShape::noSmoothing", TypeBool, &PhysicsShape::smNoSmoothing,
     "@brief Determines if client-side shapes will attempt to smoothly transition to "
     "their new position after reciving a correction.\n\n"
     "If true, shapes will immediately render at the position they are corrected to.\n\n");

   Parent::consoleInit();   
}

void PhysicsShape::initPersistFields()
{   
   addGroup( "PhysicsShape" );

      addField( "playAmbient", TypeBool, Offset( mPlayAmbient, PhysicsShape ),
            "@brief Enables or disables playing of an ambient animation upon loading the shape.\n\n"
            "@note The ambient animation must be named \"ambient\"." );
   
   endGroup( "PhysicsShape" );
// start jc
   addGroup("web");

      addField( "webViewData", TYPEID< WebViewData >(), Offset( mWebViewData, PhysicsShape ),
         "Link to a WebViewData object used to render the 'web' material." );
      addField( "enableInputEvents", TypeBool, Offset( mEnableInputEvents, PhysicsShape ),
         "If true recieve 3D input events." );

   endGroup("web");
// end jc
   Parent::initPersistFields();   

// start jc
 //  removeField( "scale" );
// end jc
}

void PhysicsShape::inspectPostApply()
{
   Parent::inspectPostApply();

   setMaskBits( InitialUpdateMask );
}

U32 PhysicsShape::packUpdate( NetConnection *con, U32 mask, BitStream *stream )
{
   U32 retMask = Parent::packUpdate( con, mask, stream );

// start jc
//   if ( stream->writeFlag( mask & InitialUpdateMask ) )
   if ( stream->writeFlag( mask & InitialUpdateMask || mask & SkinMask ) )
// end jc
   {
      stream->writeAffineTransform( getTransform() );
      stream->writeFlag( mPlayAmbient );

      stream->writeFlag( mDestroyed );

   // start jc

	  // allow for client side callbacks
	  if (stream->writeFlag(mask & SkinMask))
      {
         con->packNetStringHandleU(stream, mSkinNameHandle);

        if(stream->writeFlag(bool(mWebViewData)))
         {
            S32 gIndex = con->getGhostIndex(mWebViewData);
            if(stream->writeFlag(bool(gIndex != -1)))
               stream->writeRangedU32( U32(gIndex), 0, NetConnection::MaxGhostCount );
         }
		 stream->writeFlag(mEnableInputEvents);
      }
	  //stream->writeFlag(mGravityMod);

	// end jc

      return retMask;
   }

   // If we got here its not an initial update.  So only send
   // the least amount of data possible.

// start jc
//   if ( stream->writeFlag( mask & StateMask ) )
   if ( stream->writeFlag( mask & StateMask || mask & ScaleMask ) )
// end jc
   {
      // This will encode the position relative to the control
      // object position.  
      //
      // This will compress the position to as little as 6.25
      // bytes if the position is within about 30 meters of the
      // control object.
      //
      // Worst case its a full 12 bytes + 2 bits if the position
      // is more than 500 meters from the control object.
      //
      stream->writeCompressedPoint( mState.position );

      // Use only 3.5 bytes to send the orientation.
      stream->writeQuat( mState.orientation, 9 );

	// start jc
      if ( stream->writeFlag( mask & ScaleMask ) )  
      {
         // Only write one bit if the scale is one.
         if ( stream->writeFlag( mObjScale != Point3F::One ) )
            mathWrite( *stream, mObjScale );   
      }
	// end jc

      // If the server object has been set to sleep then
      // we don't need to send any velocity.
      if ( !stream->writeFlag( mState.sleeping ) )
      {
         // This gives me ~0.015f resolution in velocity magnitude
         // while only costing me 1 bit of the velocity is zero length,
         // <5 bytes in normal cases, and <8 bytes if the velocity is
         // greater than 1000.
         AssertWarn( mState.linVelocity.len() < 1000.0f, 
            "PhysicsShape::packUpdate - The linVelocity is out of range!" );
         stream->writeVector( mState.linVelocity, 1000.0f, 16, 9 );

         // For angular velocity we get < 0.01f resolution in magnitude
         // with the most common case being under 4 bytes.
         AssertWarn( mState.angVelocity.len() < 10.0f, 
            "PhysicsShape::packUpdate - The angVelocity is out of range!" );
         stream->writeVector( mState.angVelocity, 10.0f, 10, 9 );
      }
   }

   if ( stream->writeFlag( mask & DamageMask ) )
      stream->writeFlag( mDestroyed );

   return retMask;
}   

void PhysicsShape::unpackUpdate( NetConnection *con, BitStream *stream )
{
   Parent::unpackUpdate( con, stream );

   if ( stream->readFlag() ) // InitialUpdateMask
   {
      MatrixF mat;
      stream->readAffineTransform( &mat );
      setTransform( mat );
      mPlayAmbient = stream->readFlag();

      if ( isProperlyAdded() )
         _initAmbient();

      if ( stream->readFlag() )
      {
         if ( isProperlyAdded() )
         {
            // Destroy immediately if we've already been added
            // to the scene.
            destroy();
         }
         else
         {
            // Indicate the shape should be destroyed when the
            // shape is added.
            mDestroyed = true;
         }
      }
	  // start jc
	  if (stream->readFlag())
	  {  // SkinMask

         NetStringHandle skinDesiredNameHandle = con->unpackNetStringHandleU(stream);

         if (mSkinNameHandle != skinDesiredNameHandle)
		 {
            mSkinNameHandle = skinDesiredNameHandle;
            reSkinNewPath();
         }
         if( stream->readFlag() )
            if(stream->readFlag())
               mWebViewID = stream->readRangedU32( 0, NetConnection::MaxGhostCount );

		 mEnableInputEvents = stream->readFlag();
		 if(isProperlyAdded())
		    setEnableInputEvents(mEnableInputEvents);
	  }
	  //stream->read(mGravityMod);
     // end jc


      return;
   }

   if ( stream->readFlag() ) // StateMask
   {
      PhysicsState state;
      
      // Read the encoded and compressed position... commonly only 6.25 bytes.
      stream->readCompressedPoint( &state.position );

      // Read the compressed quaternion... 3.5 bytes.
      stream->readQuat( &state.orientation, 9 );

   // start jc
      if ( stream->readFlag() ) 
      {
         if ( stream->readFlag() )
         {
            VectorF scale;
            mathRead( *stream, &scale );
            setScale( scale );
         }
         else
            setScale( Point3F::One );
      }
   // end jc

      state.sleeping = stream->readFlag();
      if ( !state.sleeping )
      {
         stream->readVector( &state.linVelocity, 1000.0f, 16, 9 );
         stream->readVector( &state.angVelocity, 10.0f, 10, 9 );
      }

      if ( !smNoCorrections && mPhysicsRep && mPhysicsRep->isDynamic() && !mDestroyed )
      {
         // Set the new state on the physics object immediately.
         mPhysicsRep->applyCorrection( state.getTransform() );

         mPhysicsRep->setSleeping( state.sleeping );
         if ( !state.sleeping )
         {
            mPhysicsRep->setLinVelocity( state.linVelocity ); 
            mPhysicsRep->setAngVelocity( state.angVelocity ); 
         }

         mPhysicsRep->getState( &mState );
      }

      // If there is no physics object then just set the
      // new state... the tick will take care of the 
      // interpolation and extrapolation.
      if ( !mPhysicsRep || !mPhysicsRep->isDynamic() )
         mState = state;
   }

   if ( stream->readFlag() ) // DamageMask
   {
      if ( stream->readFlag() )
         destroy();
      else
         restore();
   }
}

bool PhysicsShape::onAdd()
{
// start jc
   if(mEnableInputEvents)
      mTypeMask |= InputEventObjectType;
   else
      mTypeMask &= ~InputEventObjectType;
// end jc

   if ( !Parent::onAdd() )
      return false;

   // If we don't have a physics plugin active then
   // we have to fail completely.
   if ( !PHYSICSMGR )
   {
      Con::errorf( "PhysicsShape::onAdd - No physics plugin is active!" );
      return false;
   }

   // 
   if ( !mPhysicsRep && !_createShape() )
   {
      Con::errorf( "PhysicsShape::onAdd() - Shape creation failed!" );
      return false;
   }

   // The reset position is the transform on the server
   // at creation time... its not used on the client.
   if ( isServerObject() )
   {
      storeRestorePos();
      PhysicsPlugin::getPhysicsResetSignal().notify( this, &PhysicsShape::_onPhysicsReset );
   }

   // Register for the resource change signal.
   //ResourceManager::get().getChangedSignal().notify( this, &PhysicsShape::_onResourceChanged );

   // Only add server objects and non-destroyed client objects to the scene.
   if ( isServerObject() || !mDestroyed)
      addToScene();

   if ( isClientObject() && mDestroyed )
   {
      // Disable all simulation of the body... no collision or dynamics.
      if ( mPhysicsRep )
         mPhysicsRep->setSimulationEnabled( false );

      // Stop doing tick processing for this SceneObject.
      setProcessTick( false );
   }
// start jc
   if(isClientObject())
   {
      getDataBlock()->onAddClient_callback(this);

      if(mShapeInst && mShapeInst->getMaterialList())
         getDataBlock()->onMaterialsLoaded_callback(this);
   }
// end jc

   return true;
}

void PhysicsShape::onRemove()
{
   removeFromScene();

   SAFE_DELETE( mPhysicsRep );
   SAFE_DELETE( mShapeInst );
   mAmbientThread = NULL;
   mAmbientSeq = -1;
   mWorld = NULL;

   if ( isServerObject() )
   {
      PhysicsPlugin::getPhysicsResetSignal().remove( this, &PhysicsShape::_onPhysicsReset );

      if ( mDestroyedShape )
		  mDestroyedShape->deleteObject();
   }

   // Remove the resource change signal.
   //ResourceManager::get().getChangedSignal().remove( this, &PhysicsShape::_onResourceChanged );

   Parent::onRemove();
}

bool PhysicsShape::onNewDataBlock( GameBaseData *dptr, bool reload )
{
   if ( !Parent::onNewDataBlock( dptr, reload ) )
      return false;

   if ( !isProperlyAdded() )
      return true;

   // If we don't have a physics plugin active then
   // we have to fail completely.
   if ( !PHYSICSMGR )
   {
      Con::errorf( "PhysicsShape::onNewDataBlock - No physics plugin is active!" );
      return false;
   }

   // 
   if ( !_createShape() )
   {
      Con::errorf( "PhysicsShape::onNewDataBlock() - Shape creation failed!" );
      return false;
   }

   return true;
}

bool PhysicsShape::_createShape()
{
   SAFE_DELETE( mPhysicsRep );
   SAFE_DELETE( mShapeInst );
   mAmbientThread = NULL;
   mWorld = NULL;
   mAmbientSeq = -1;

   PhysicsShapeData *db = getDataBlock();
   if ( !db )
      return false;

   // Set the world box.
   mObjBox = db->shape->bounds;
   resetWorldBox();

   // If this is the server and its a client only simulation
   // object then disable our tick... the server doesn't do 
   // any work for this shape.
   if (  isServerObject() && 
         db->simType == PhysicsShapeData::SimType_ClientOnly )
   {
      setProcessTick( false );
      return true;
   }

   // Create the shape instance.
   mShapeInst = new TSShapeInstance( db->shape, isClientObject() );

   if ( isClientObject() )
   {
      mAmbientSeq = db->shape->findSequence( "ambient" );
      _initAmbient();   
   }

   // If the shape has a mass then its dynamic... else
   // its a kinematic shape.
   //
   // While a kinematic is less optimal than a static body
   // it allows for us to enable/disable collision and having
   // all dynamic actors react correctly... waking up.
   // 
   const bool isDynamic = db->mass > 0.0f;

   // If we aren't dynamic we don't need to tick.   
   setProcessTick( isDynamic || mPlayAmbient );

   // If this is the client and we're a server only object then
   // we don't need any physics representation... we're done.
   if (  isClientObject() && 
         db->simType == PhysicsShapeData::SimType_ServerOnly )
      return true;

   mWorld = PHYSICSMGR->getWorld( isServerObject() ? "server" : "client" );
      
   mPhysicsRep = PHYSICSMGR->createBody();
   mPhysicsRep->init(   db->colShape, 
                        db->mass, 
                        isDynamic ? 0 : PhysicsBody::BF_KINEMATIC,  
                        this, 
                        mWorld );

   mPhysicsRep->setMaterial( db->restitution, db->dynamicFriction, db->staticFriction );
   
   if ( isDynamic )
   {
      mPhysicsRep->setDamping( db->linearDamping, db->angularDamping );
      mPhysicsRep->setSleepThreshold( db->linearSleepThreshold, db->angularSleepThreshold );
   }

   mPhysicsRep->setTransform( getTransform() );

   return true;
}

void PhysicsShape::_initAmbient()
{
   if ( isServerObject() )
      return;

   bool willPlay = mPlayAmbient && mAmbientSeq != -1;

   if ( willPlay )
   {
      // Create thread if we dont already have.
      if ( mAmbientThread == NULL )
         mAmbientThread = mShapeInst->addThread();
    
      // Play the sequence.
      mShapeInst->setSequence( mAmbientThread, mAmbientSeq, 0);

      setProcessTick(true);
   }
   else
   {
      if ( mAmbientThread != NULL )
      {
         mShapeInst->destroyThread( mAmbientThread );
         mAmbientThread = NULL;
      }
   }
}

void PhysicsShape::_onPhysicsReset( PhysicsResetEvent reset )
{
   if ( reset == PhysicsResetEvent_Store )
      mResetPos = getTransform();

   else if ( reset == PhysicsResetEvent_Restore )
   {
      setTransform( mResetPos );

      // Restore to un-destroyed state.
      restore();

      // Cheat and reset the client from here.
      if ( getClientObject() )
      {
         PhysicsShape *clientObj = (PhysicsShape*)getClientObject();
         clientObj->setTransform( mResetPos );
         clientObj->restore();
      }
   }
}

void PhysicsShape::setTransform( const MatrixF &newMat )
{
   Parent::setTransform( newMat );
   
   // This is only called to set an absolute position
   // so we discard the delta state.
   mState.position = getPosition();
   mState.orientation.set( newMat );
   mRenderState[0] = mRenderState[1] = mState;
   setMaskBits( StateMask );

   if ( mPhysicsRep )
      mPhysicsRep->setTransform( newMat );
}

void PhysicsShape::setScale( const VectorF &scale )
{
   // Cannot scale PhysicsShape.
   // start jc
   Parent::setScale( scale );

   if ( mPhysicsRep )
   {
      MatrixF mat;
      mPhysicsRep->getTransform(&mat);
      mat.scale(scale);
      mPhysicsRep->setTransform(mat);
   }
   // end jc
   return;
}

void PhysicsShape::storeRestorePos()
{
   mResetPos = getTransform();
}

F32 PhysicsShape::getMass() const 
{ 
   const PhysicsShapeData *db = const_cast<PhysicsShape*>( this )->getDataBlock();
   return db->mass; 
}

void PhysicsShape::applyImpulse( const Point3F &pos, const VectorF &vec )
{
   if ( mPhysicsRep && mPhysicsRep->isDynamic() )
      mPhysicsRep->applyImpulse( pos, vec );
}

void PhysicsShape::applyRadialImpulse( const Point3F &origin, F32 radius, F32 magnitude )
{
   if ( !mPhysicsRep || !mPhysicsRep->isDynamic() )
      return;

   // TODO: Find a better approximation of the
   // force vector using the object box.

   VectorF force = getWorldBox().getCenter() - origin;
   F32 dist = force.magnitudeSafe();
   force.normalize();

   if ( dist == 0.0f )
      force *= magnitude;
   else
      force *= mClampF( radius / dist, 0.0f, 1.0f ) * magnitude;   

   mPhysicsRep->applyImpulse( origin, force );

   // TODO: There is no simple way to really sync this sort of an 
   // event with the client.
   //
   // The best is to send the current physics snapshot, calculate the
   // time difference from when this event occured and the time when the
   // client recieves it, and then extrapolate where it should be.
   //
   // Even then its impossible to be absolutely sure its synced.
   //
   // Bottom line... you shouldn't use physics over the network like this.
   //

   // Cheat for single player.
   //if ( getClientObject() )
      //((PhysicsShape*)getClientObject())->mPhysicsRep->applyImpulse( origin, force );
}

void PhysicsShape::interpolateTick( F32 delta )
{
   AssertFatal( !mDestroyed, "PhysicsShape::interpolateTick - Shouldn't be processing a destroyed shape!" );

// start jc
   if (isGhost())
   {
      if(mWebViewID && !mWebViewData)
         webSkin();

      if(mWebViewData)
         mWebViewData->refresh();
   }
// end jc
   if ( !mPhysicsRep->isDynamic() )
      return;

   // Interpolate the position and rotation based on the delta.
   PhysicsState state;
   state.interpolate( mRenderState[1], mRenderState[0], delta );

   // Set the transform to the interpolated transform.
   setRenderTransform( state.getTransform() );
}

// start jc
void PhysicsShape::setEnableInputEvents( bool enable )
{
   if ( enable != mEnableInputEvents )
   {
      removeFromScene();
      if(enable)
         mTypeMask |= InputEventObjectType;
      else
         mTypeMask &= ~InputEventObjectType;
      addToScene();
   }
}

void PhysicsShape::onMouseDown(const ShapeInputEvent &event)
{
   getDataBlock()->onMouseDown_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
void PhysicsShape::onMouseUp(const ShapeInputEvent & event)
{
   getDataBlock()->onMouseUp_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
void PhysicsShape::onMouseMove(const ShapeInputEvent & event)
{
   getDataBlock()->onMouseMove_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
void PhysicsShape::onMouseDragged(const ShapeInputEvent & event)
{
   getDataBlock()->onMouseDragged_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
void PhysicsShape::onMouseEnter(const ShapeInputEvent & event)
{
   getDataBlock()->onMouseEnter_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
void PhysicsShape::onMouseLeave(const ShapeInputEvent & event)
{
   getDataBlock()->onMouseLeave_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
void PhysicsShape::onRightMouseDown(const ShapeInputEvent & event)
{
   getDataBlock()->onRightMouseDown_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
void PhysicsShape::onRightMouseUp(const ShapeInputEvent & event)
{
   getDataBlock()->onRightMouseUp_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
void PhysicsShape::onRightMouseDragged(const ShapeInputEvent & event)
{
   getDataBlock()->onRightMouseDragged_callback(this, event.modifier, event.mousePoint, event.mouseClickCount, event.pos, event.vec, event.ri.texCoord);
}
bool PhysicsShape::onTouchDown(const ShapeTouchEvent & event)
{
   return getDataBlock()->onTouchDown_callback(this, event.touchId, event.touchPosition, event.pos, event.vec, event.ri.texCoord);
}
bool PhysicsShape::onTouchMove(const ShapeTouchEvent & event)
{
   return getDataBlock()->onTouchMove_callback(this, event.touchId, event.touchPosition, event.pos, event.vec, event.ri.texCoord);
}
bool PhysicsShape::onTouchUp(const ShapeTouchEvent & event)
{
   return getDataBlock()->onTouchUp_callback(this, event.touchId, event.touchPosition, event.pos, event.vec, event.ri.texCoord);
}

bool PhysicsShape::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
   if (mShapeInst) 
   {
      RayInfo shortest;
      shortest.t = 1e8;

      info->object = NULL;
	  /*
      for (U32 i = 0; i < getDataBlock()->LOSDetails.size(); i++)
      {
         mShapeInst->animate(getDataBlock()->LOSDetails[i]);
         if (mShapeInst->castRay(start, end, info, getDataBlock()->LOSDetails[i]))
         {
            info->object = this;
            if (info->t < shortest.t)
               shortest = *info;
         }
      }
	  */
         mShapeInst->animate(0);
         if (mShapeInst->castRay(start, end, info, 0))
         {
            info->object = this;
            if (info->t < shortest.t)
               shortest = *info;
         }
     // }


      if (info->object == this) 
      {
         // Copy out the shortest time...
         *info = shortest;
         return true;
      }
   }

   return false;
}

bool PhysicsShape::castRayRendered(const Point3F &start, const Point3F &end, RayInfo* info)
{
   if (mShapeInst) 
   {
      RayInfo localInfo;
   // start jc
	  localInfo.generateTexCoord = info->generateTexCoord;
  // end jc
      mShapeInst->animate();
      bool res = mShapeInst->castRayRendered(start, end, &localInfo, mShapeInst->getCurrentDetail());
      if (res)
      {
         *info = localInfo;
         info->object = this;
         return true;
      }
   }

   return false;
}

Point3F PhysicsShape::getCMassPosition(void)
{
   return mPhysicsRep->getCMassPosition();
}
void PhysicsShape::setLinVelocity(const Point3F velocity)
{
   return mPhysicsRep->setLinVelocity(velocity);
}
Point3F PhysicsShape::getLinVelocity() const
{
   return mPhysicsRep->getLinVelocity();
}


void PhysicsShape::moveGlobalPosition(const Point3F& vec)
{
   mPhysicsRep->moveGlobalPosition(vec);
}
  
void PhysicsShape::setSkinName(const char* name)
{
   if (isServerObject()) {
      if (name[0] != '\0') {

         // Use tags for better network performance
         // Should be a tag, but we'll convert to one if it isn't.
         if (name[0] == StringTagPrefixByte) {
            mSkinNameHandle = NetStringHandle(U32(dAtoi(name + 1)));
         }
         else {
            mSkinNameHandle = NetStringHandle(name);
         }
      }
      else {
         mSkinNameHandle = NetStringHandle();
      }
      setMaskBits(SkinMask);
   }
}

void PhysicsShape::reSkin()
{
   if ( isGhost() && mShapeInst && mSkinNameHandle.isValidString() )
   {
      const char* newSkin = mSkinNameHandle.getString();
      mShapeInst->reSkin( newSkin, mAppliedSkinName );
      mAppliedSkinName = newSkin;
      mSkinHash = _StringTable::hashString( newSkin );
   }
}

void PhysicsShape::reSkinNewPath()
{
   if ( isGhost() && mShapeInst && mSkinNameHandle.isValidString() )
   {
      const char* newSkin = mSkinNameHandle.getString();
      mShapeInst->reSkin( newSkin, mAppliedSkinName, true );
      mAppliedSkinName = newSkin;
      mSkinHash = _StringTable::hashString( newSkin );
   }
}
void PhysicsShape::reSkin(const char* oldSkin)
{
   if ( oldSkin && isGhost() && mShapeInst && mSkinNameHandle.isValidString() )
   {
      mAppliedSkinName = oldSkin;
      const char* newSkin = mSkinNameHandle.getString();
      mShapeInst->reSkin( newSkin, mAppliedSkinName );
      mAppliedSkinName = newSkin;
      mSkinHash = _StringTable::hashString( newSkin );
   }
}
/*
void PhysicsShape::webSkin()
{
	if(!mWebViewData)
	{
		GameConnection *con = GameConnection::getConnectionToServer();
		mWebViewData = static_cast<WebViewData*>(con->resolveGhost(mWebViewID));
	}
	if(mWebViewData)
	{
		mShapeInst->webSkin(mWebViewData);
	}
}
*/
void PhysicsShape::webSkin()
{
   if(!mWebViewData)
   {
      GameConnection *con = GameConnection::getConnectionToServer();
      mWebViewData = static_cast<WebViewData*>(con->resolveGhost(mWebViewID));
   }

   if(mWebViewData)
      mShapeInst->webSkin(mWebViewData);
}


Point2I PhysicsShape::getTextureResolution(U32 index)
{
   if(!mShapeInst)
      return Point2I(0, 0);

   TSMaterialList* matList = mShapeInst->getMaterialList();
   if(!matList)
       return Point2I(0, 0);

   MatInstance *matInst = static_cast<MatInstance*>(matList->getMaterialInst(index));
   if(!matInst || !(matInst->getProcessedMaterial()))
	   return Point2I(0, 0);

   const RenderPassData *rpd = matInst->getProcessedMaterial()->getPass(0);
   if(!rpd || !(rpd->mTexSlot[0].texObject))
	   return Point2I(0, 0);

   return rpd->mTexSlot[0].texObject.getWidthHeight();
}
bool PhysicsShape::rayCrossesTransparency(RayInfo* ri)
{
   if(mWebViewData && ri->texCoord.x != -1)
   {
      // todo: use the new filp value on RenderBuffer and fix all the v flips
      return mWebViewData->isSeeThruPixel(Point2F(ri->texCoord.x-0.5f, 0.5f-ri->texCoord.y));
   }

   return false;
}
//----------------------------------------------------------------------------
/*
void PhysicsShape::queueCollision( SceneObject *obj, const VectorF &vec)
{
   // Add object to list of collisions.
   SimTime time = Sim::getCurrentTime();
   S32 num = obj->getId();

   CollisionTimeout** adr = &mTimeoutList;
   CollisionTimeout* ptr = mTimeoutList;
   while (ptr) {
      if (ptr->objectNumber == num) {
         if (ptr->expireTime < time) {
            ptr->expireTime = time + CollisionTimeoutValue;
            ptr->object = obj;
            ptr->vector = vec;
         }
         return;
      }
      // Recover expired entries
      if (ptr->expireTime < time) {
         CollisionTimeout* cur = ptr;
         *adr = ptr->next;
         ptr = ptr->next;
         cur->next = sFreeTimeoutList;
         sFreeTimeoutList = cur;
      }
      else {
         adr = &ptr->next;
         ptr = ptr->next;
      }
   }

   // New entry for the object
   if (sFreeTimeoutList != NULL)
   {
      ptr = sFreeTimeoutList;
      sFreeTimeoutList = ptr->next;
      ptr->next = NULL;
   }
   else
   {
      ptr = sTimeoutChunker.alloc();
   }

   ptr->object = obj;
   ptr->objectNumber = obj->getId();
   ptr->vector = vec;
   ptr->expireTime = time + CollisionTimeoutValue;
   ptr->next = mTimeoutList;

   mTimeoutList = ptr;
}

void PhysicsShape::notifyCollision()
{
   // Notify all the objects that were just stamped during the queueing
   // process.
   SimTime expireTime = Sim::getCurrentTime() + CollisionTimeoutValue;
   for (CollisionTimeout* ptr = mTimeoutList; ptr; ptr = ptr->next)
   {
      if (ptr->expireTime == expireTime && ptr->object)
      {
         SimObjectPtr<SceneObject> safePtr(ptr->object);
         SimObjectPtr<PhysicsShape> safeThis(this);
         onCollision(ptr->object,ptr->vector);
         ptr->object = 0;

         if(!bool(safeThis))
            return;

         if(bool(safePtr))
            safePtr->onCollision(this,ptr->vector);

         if(!bool(safeThis))
            return;
      }
   }
}

void PhysicsShape::onCollision( SceneObject *object, const VectorF &vec )
{
   if (!isGhost())
      getDataBlock()->onCollision_callback( this, object, vec, vec.len() );
}
*/
//--------------------------------------------------------------------------

// end jc


void PhysicsShape::processTick( const Move *move )
{
   AssertFatal( mPhysicsRep && !mDestroyed, "PhysicsShape::processTick - Shouldn't be processing a destroyed shape!" );

   // Note that unlike TSStatic, the serverside PhysicsShape does not
   // need to play the ambient animation because even if the animation were
   // to move collision shapes it would not affect the physx representation.

   if ( !mPhysicsRep->isDynamic() )
      return;

   // SINGLE PLAYER HACK!!!!
   if ( PHYSICSMGR->isSinglePlayer() && isClientObject() && getServerObject() )
   {          
      PhysicsShape *servObj = (PhysicsShape*)getServerObject();
      setTransform( servObj->mState.getTransform() );      
      mRenderState[0] = servObj->mRenderState[0];
      mRenderState[1] = servObj->mRenderState[1];

      return;
   }

   // Store the last render state.
   mRenderState[0] = mRenderState[1];

   // If the last render state doesn't match the last simulation 
   // state then we got a correction and need to 
   Point3F errorDelta = mRenderState[1].position - mState.position;
   const bool doSmoothing = !errorDelta.isZero() && !smNoSmoothing;

   const bool wasSleeping = mState.sleeping;

   // Get the new physics state.
   if ( mPhysicsRep ) 
   {
      mPhysicsRep->getState( &mState );
      _updateContainerForces();
   }
   else
   {
      // This is where we could extrapolate.
   }

   // Smooth the correction back into the render state.
   mRenderState[1] = mState;
   if ( doSmoothing )
   {
      F32 correction = mClampF( errorDelta.len() / 20.0f, 0.1f, 0.9f );
      mRenderState[1].position.interpolate( mState.position, mRenderState[0].position, correction );  
      mRenderState[1].orientation.interpolate( mState.orientation, mRenderState[0].orientation, correction );
   }

   // If we haven't been sleeping then update our transform
   // and set ourselves as dirty for the next client update.
   if ( !wasSleeping || !mState.sleeping )
   {
      // Set the transform on the parent so that
      // the physics object isn't moved.
      Parent::setTransform( mState.getTransform() );

      // If we're doing server simulation then we need
      // to send the client a state update.
      if ( isServerObject() && mPhysicsRep && !smNoCorrections &&
         
         !PHYSICSMGR->isSinglePlayer() // SINGLE PLAYER HACK!!!!
         
         )
         setMaskBits( StateMask );
   }
}

void PhysicsShape::advanceTime( F32 timeDelta )
{
   if ( isClientObject() && mPlayAmbient && mAmbientThread != NULL )
      mShapeInst->advanceTime( timeDelta, mAmbientThread );
}

void PhysicsShape::_updateContainerForces()
{
   PROFILE_SCOPE( PhysicsShape_updateContainerForces );

   // If we're not simulating don't update forces.
   if ( !mWorld->isEnabled() )
      return;

   ContainerQueryInfo info;
   info.box = getWorldBox();
   info.mass = getDataBlock()->mass;

   // Find and retreive physics info from intersecting WaterObject(s)
   getContainer()->findObjects( getWorldBox(), WaterObjectType|PhysicalZoneObjectType, findRouter, &info );

   // Calculate buoyancy and drag
   F32 angDrag = getDataBlock()->angularDamping;
   F32 linDrag = getDataBlock()->linearDamping;
   F32 buoyancy = 0.0f;
   Point3F cmass = mPhysicsRep->getCMassPosition();

   F32 density = getDataBlock()->buoyancyDensity;
   if ( density > 0.0f )
   {
      if ( info.waterCoverage > 0.0f )
      {
         F32 waterDragScale = info.waterViscosity * getDataBlock()->waterDampingScale;
         F32 powCoverage = mPow( info.waterCoverage, 0.25f );

         angDrag = mLerp( angDrag, angDrag * waterDragScale, powCoverage );
         linDrag = mLerp( linDrag, linDrag * waterDragScale, powCoverage );
      }

      buoyancy = ( info.waterDensity / density ) * mPow( info.waterCoverage, 2.0f );
      
      // A little hackery to prevent oscillation
      // Based on this blog post:
      // (http://reinot.blogspot.com/2005/11/oh-yes-they-float-georgie-they-all.html)
      // JCF: disabled!
   // start jc
   //   Point3F buoyancyForce = buoyancy * -mWorld->getGravity() * TickSec * getDataBlock()->mass;
      Point3F buoyancyForce = buoyancy * -mWorld->getGravity() * TickSec * mPhysicsRep->getMass();
      /*
      Point3F buoyancyForce;
      if( info.gravityScale != 1.0 )
      //   buoyancyForce = buoyancy * -mWorld->getGravity() * TickSec * getDataBlock()->mass * (-mWorld->getGravity() * (1.0f - info.gravityScale));
         buoyancyForce = (buoyancy * -mWorld->getGravity() * TickSec * getDataBlock()->mass)*(1.0f - info.gravityScale);
      else
         buoyancyForce = buoyancy * -mWorld->getGravity() * TickSec * getDataBlock()->mass;     
         */
   // end jc
      mPhysicsRep->applyImpulse( cmass, buoyancyForce );      
   }
// start jc
   else if ( info.gravityScale != 1.0 )
      mPhysicsRep->applyImpulse( cmass, -(mWorld->getGravity())*(1.0f - info.gravityScale) * TickSec * mPhysicsRep->getMass());
// end jc

   // Update the dampening as the container might have changed.
   mPhysicsRep->setDamping( linDrag, angDrag );
   
   // Apply physical zone forces.
   if ( !info.appliedForce.isZero() )
      mPhysicsRep->applyImpulse( cmass, info.appliedForce );
}

void PhysicsShape::prepRenderImage( SceneRenderState *state )
{
   AssertFatal( !mDestroyed, "PhysicsShape::prepRenderImage - Shouldn't be processing a destroyed shape!" );

   PROFILE_SCOPE( PhysicsShape_prepRenderImage );

   if( !mShapeInst )
         return;

   Point3F cameraOffset;
   getRenderTransform().getColumn(3,&cameraOffset);
   cameraOffset -= state->getDiffuseCameraPosition();
   F32 dist = cameraOffset.len();
   if (dist < 0.01f)
      dist = 0.01f;

   F32 invScale = (1.0f/getMax(getMax(mObjScale.x,mObjScale.y),mObjScale.z));   
   if ( mShapeInst->setDetailFromDistance( state, dist * invScale ) < 0 )
      return;

   GFXTransformSaver saver;

   // Set up our TS render state.
   TSRenderState rdata;
   rdata.setSceneState( state );
   rdata.setFadeOverride( 1.0f );

   // We might have some forward lit materials
   // so pass down a query to gather lights.
   LightQuery query;
   query.init( getWorldSphere() );
   rdata.setLightQuery( &query );

   MatrixF mat = getRenderTransform();
   mat.scale( mObjScale );
   GFX->setWorldMatrix( mat );

   mShapeInst->animate();
   mShapeInst->render( rdata );
}

void PhysicsShape::destroy()
{
   if ( mDestroyed )
      return;

   mDestroyed = true;
   setMaskBits( DamageMask );

   const Point3F lastLinVel = mPhysicsRep->isDynamic() ? mPhysicsRep->getLinVelocity() : Point3F::Zero;

   // Disable all simulation of the body... no collision or dynamics.
   mPhysicsRep->setSimulationEnabled( false );

   // On the client side we remove it from the scene graph
   // to disable rendering and volume queries.
   if ( isClientObject() )
      removeFromScene();

   // Stop doing tick processing for this SceneObject.
   setProcessTick( false );

   PhysicsShapeData *db = getDataBlock();
   if ( !db )
      return;

   const MatrixF &mat = getTransform();
   if ( isServerObject() )
   {
      // We only create the destroyed object on the server
      // and let ghosting deal with updating the client.

      if ( db->destroyedShape )
      {
         mDestroyedShape = new PhysicsShape();
         mDestroyedShape->setDataBlock( db->destroyedShape );
         mDestroyedShape->setTransform( mat );
         if ( !mDestroyedShape->registerObject() )
            delete mDestroyedShape.getObject();
      }

      return;
   }
   
   // Let the physics debris create itself.
   PhysicsDebris::create( db->debris, mat, lastLinVel );

   if ( db->explosion )
   {
      Explosion *splod = new Explosion();
      splod->setDataBlock( db->explosion );
      splod->setTransform( mat );
      splod->setInitialState( getPosition(), mat.getUpVector(), 1.0f );
      if ( !splod->registerObject() )
         delete splod;
   }   
}

void PhysicsShape::restore()
{
   if ( !mDestroyed )
      return;

   PhysicsShapeData *db = getDataBlock();
   const bool isDynamic = db && db->mass > 0.0f;

   if ( mDestroyedShape )   
      mDestroyedShape->deleteObject();

   // Restore tick processing, add it back to 
   // the scene, and enable collision and simulation.
   setProcessTick( isDynamic || mPlayAmbient );   
   if ( isClientObject() )
      addToScene();
   mPhysicsRep->setSimulationEnabled( true );

   mDestroyed = false;
   setMaskBits( DamageMask );
}

DefineEngineMethod( PhysicsShape, isDestroyed, bool, (),, 
   "@brief Returns if a PhysicsShape has been destroyed or not.\n\n" )
{
   return object->isDestroyed();
}

DefineEngineMethod( PhysicsShape, destroy, void, (),,
   "@brief Disables rendering and physical simulation.\n\n"
   "Calling destroy() will also spawn any explosions, debris, and/or destroyedShape "
   "defined for it, as well as remove it from the scene graph.\n\n"
   "Destroyed objects are only created on the server. Ghosting will later update the client.\n\n"
   "@note This does not actually delete the PhysicsShape." )
{
   object->destroy();
}

DefineEngineMethod( PhysicsShape, restore, void, (),,
   "@brief Restores the shape to its state before being destroyed.\n\n"
   "Re-enables rendering and physical simulation on the object and "
   "adds it to the client's scene graph. "
   "Has no effect if the shape is not destroyed.\n\n")
{
   object->restore();
}
// start jc
DefineEngineMethod( PhysicsShape, getCMassPosition, Point3F, (),,
   "get the world position of the center of mass." )
{
   return object->getCMassPosition();
}
DefineEngineMethod( PhysicsShape, setLinVelocity, void, (Point3F velocity),,
   "get the world position of the center of mass." )
{
   object->setLinVelocity(velocity);
}
DefineEngineMethod( PhysicsShape, getLinVelocity, Point3F, (),,
   "get the world position of the center of mass." )
{
   return object->getLinVelocity();
}
DefineEngineMethod( PhysicsShape, moveGlobalPosition, void, (Point3F pos),,
   "move to world position." )
{
   object->moveGlobalPosition(pos);
}
DefineEngineMethod( PhysicsShape, getTextureResolution, Point2I, (U32 index),,
   "Get the model filename used by this shape.\n"
   "@return the shape filename\n\n" )
{
   return object->getTextureResolution(index);
}

DefineEngineMethod( PhysicsShape, setSkinName, void, ( const char* name ),,
   "@brief Apply a new skin to this shape.\n\n"

   "@param name name of the skin to apply\n\n" 
   
   "@see getSkinName()\n")
{
   object->setSkinName( name );
}
DefineEngineMethod( PhysicsShape, getSkinName, const char*, (),,
   "@brief Get the name of the skin applied to this shape.\n\n"

   "@return the name of the skin\n\n" 
   
   "@see setSkinName()\n")
{
   return object->getSkinName();
}
DefineEngineMethod( PhysicsShape, setWebViewData, void, (WebViewData* vebView),,
   "Apply a new skin to this shape.\n"
   "@param name name of the skin to apply\n\n" )
{ 
   object->setWebViewData( vebView );
}
DefineEngineMethod( PhysicsShape, hasWebViewData, bool, (),,
   "Get the model filename used by this shape.\n"
   "@return the shape filename\n\n" )
{
   return object->hasWebViewData();
}

// end jc



// start jc
/*
PhysicsAnimatedShape::PhysicsAnimatedShape()
   :  mPhysicsRep( NULL ),
      mWorld( NULL ),
      mShapeInst( NULL ),
      mResetPos( MatrixF::Identity ),
      mDestroyed( false ),
      mPlayAmbient( false ),
      mAmbientThread( NULL ),
      mAmbientSeq( -1 ),
// start jc
//   mWebViewData(NULL),
//   mWebViewID(0),
	mEnableInputEvents(false)
//   mGravityMod(1.0)
// end jc
{
   mNetFlags.set( Ghostable | ScopeAlways );
   mTypeMask |= DynamicShapeObjectType;

}

PhysicsAnimatedShape::~PhysicsAnimatedShape()
{
}

void PhysicsAnimatedShape::consoleInit()
{
   Con::addVariable( "$PhysicsAnimatedShape::noCorrections", TypeBool, &PhysicsAnimatedShape::smNoCorrections,
      "If set true, client-side shapes will recieve no corrections from the server and will instead by allowed to diverge.\n"
	  "@ingroup Physics");

   Con::addVariable( "$PhysicsAnimatedShape::noSmoothing", TypeBool, &PhysicsAnimatedShape::smNoSmoothing,
      "If set true, client-side shapes will render immediately at the position they are corrected to when a correction is recieved from the server.\n"
	  "@ingroup Physics" );

   Parent::consoleInit();   
}

void PhysicsAnimatedShape::initPersistFields()
{   
   addGroup( "PhysicsAnimatedShape" );

      addField( "playAmbient", TypeBool, Offset( mPlayAmbient, PhysicsAnimatedShape ),
            "Enables automatic playing of the animation named \"ambient\" (if it exists) when the PhysicsAnimatedShape is loaded.");
   // start jc
      addField( "enableInputEvents", TypeBool, Offset( mEnableInputEvents, PhysicsAnimatedShape ),
         "If true recieve 3D input events." );
   //   addField( "gravityMod", TypeFloat, Offset( mGravityMod, PhysicsAnimatedShape ),
   //      "If true recieve 3D input events." );
   // end jc
   endGroup( "PhysicsAnimatedShape" );

   Parent::initPersistFields();   

   removeField( "scale" );
}

void PhysicsAnimatedShape::inspectPostApply()
{
   Parent::inspectPostApply();

   setMaskBits( InitialUpdateMask );
}

U32 PhysicsAnimatedShape::packUpdate( NetConnection *con, U32 mask, BitStream *stream )
{
   U32 retMask = Parent::packUpdate( con, mask, stream );

   if ( stream->writeFlag( mask & InitialUpdateMask ) )
   {
      stream->writeAffineTransform( getTransform() );
      stream->writeFlag( mPlayAmbient );

      stream->writeFlag( mDestroyed );
	  // start jc
	  stream->writeFlag(mEnableInputEvents);
	  //stream->writeFlag(mGravityMod);
	  // end jc

      return retMask;
   }

   // If we got here its not an initial update.  So only send
   // the least amount of data possible.

   if ( stream->writeFlag( mask & StateMask ) )
   {
      // This will encode the position relative to the control
      // object position.  
      //
      // This will compress the position to as little as 6.25
      // bytes if the position is within about 300 meters of the
      // control object.
      //
      // Worst case its a full 12 bytes + 2 bits if the position
      // is more than 5000 meters from the control object.
      //
      stream->writeCompressedPoint( mState.position );

      // Use only 3.5 bytes to send the orientation.
      stream->writeQuat( mState.orientation, 9 );

      // If the server object has been set to sleep then
      // we don't need to send any velocity.
      if ( !stream->writeFlag( mState.sleeping ) )
      {
         // This gives me ~0.015f resolution in velocity magnitude
         // while only costing me 1 bit of the velocity is zero length,
         // <5 bytes in normal cases, and <8 bytes if the velocity is
         // greater than 1000.
         AssertWarn( mState.linVelocity.len() < 1000.0f, 
            "PhysicsAnimatedShape::packUpdate - The linVelocity is out of range!" );
         stream->writeVector( mState.linVelocity, 1000.0f, 16, 9 );

         // For angular velocity we get < 0.01f resolution in magnitude
         // with the most common case being under 4 bytes.
         AssertWarn( mState.angVelocity.len() < 10.0f, 
            "PhysicsAnimatedShape::packUpdate - The angVelocity is out of range!" );
         stream->writeVector( mState.angVelocity, 10.0f, 10, 9 );
      }
   }

   if ( stream->writeFlag( mask & DamageMask ) )
      stream->writeFlag( mDestroyed );

   return retMask;
}   

void PhysicsAnimatedShape::unpackUpdate( NetConnection *con, BitStream *stream )
{
   Parent::unpackUpdate( con, stream );

   if ( stream->readFlag() ) // InitialUpdateMask
   {
      MatrixF mat;
      stream->readAffineTransform( &mat );
      setTransform( mat );
      mPlayAmbient = stream->readFlag();
		
      if ( isProperlyAdded() )
         _initAmbient();

      if ( stream->readFlag() )
      {
         if ( isProperlyAdded() )
         {
            // Destroy immediately if we've already been added
            // to the scene.
            destroy();
         }
         else
         {
            // Indicate the shape should be destroyed when the
            // shape is added.
            mDestroyed = true;
         }
      }
	  // start jc
	  mEnableInputEvents = stream->readFlag();
	  if(isProperlyAdded())
		setEnableInputEvents(mEnableInputEvents);

	  //stream->read(mGravityMod);
     // end jc


      return;
   }

   if ( stream->readFlag() ) // StateMask
   {
      PhysicsState state;
      
      // Read the encoded and compressed position... commonly only 6.25 bytes.
      stream->readCompressedPoint( &state.position );

      // Read the compressed quaternion... 3.5 bytes.
      stream->readQuat( &state.orientation, 9 );

      state.sleeping = stream->readFlag();
      if ( !state.sleeping )
      {
         stream->readVector( &state.linVelocity, 1000.0f, 16, 9 );
         stream->readVector( &state.angVelocity, 10.0f, 10, 9 );
      }

      if ( !smNoCorrections && mPhysicsRep && mPhysicsRep->isDynamic() && !mDestroyed )
      {
         // Set the new state on the physics object immediately.
         mPhysicsRep->applyCorrection( state.getTransform() );

         mPhysicsRep->setSleeping( state.sleeping );
         if ( !state.sleeping )
         {
            mPhysicsRep->setLinVelocity( state.linVelocity ); 
            mPhysicsRep->setAngVelocity( state.angVelocity ); 
         }

         mPhysicsRep->getState( &mState );
      }

      // If there is no physics object then just set the
      // new state... the tick will take care of the 
      // interpolation and extrapolation.
      if ( !mPhysicsRep || !mPhysicsRep->isDynamic() )
         mState = state;
   }

   if ( stream->readFlag() ) // DamageMask
   {
      if ( stream->readFlag() )
         destroy();
      else
         restore();
   }
}

bool PhysicsAnimatedShape::onAdd()
{
// start jc
   if(mEnableInputEvents)
      mTypeMask |= InputEventObjectType;
   else
      mTypeMask &= ~InputEventObjectType;
// end jc

   if ( !Parent::onAdd() )
      return false;

   // If we don't have a physics plugin active then
   // we have to fail completely.
   if ( !PHYSICSMGR )
   {
      Con::errorf( "PhysicsAnimatedShape::onAdd - No physics plugin is active!" );
      return false;
   }

   // 
   if ( !mPhysicsRep && !_createShape() )
   {
      Con::errorf( "PhysicsAnimatedShape::onAdd() - Shape creation failed!" );
      return false;
   }

   // The reset position is the transform on the server
   // at creation time... its not used on the client.
   if ( isServerObject() )
   {
      storeRestorePos();
      PhysicsPlugin::getPhysicsResetSignal().notify( this, &PhysicsAnimatedShape::_onPhysicsReset );
   }

   // Register for the resource change signal.
   //ResourceManager::get().getChangedSignal().notify( this, &PhysicsAnimatedShape::_onResourceChanged );

   // Only add server objects and non-destroyed client objects to the scene.
   if ( isServerObject() || !mDestroyed)
      addToScene();

   if ( isClientObject() && mDestroyed )
   {
      // Disable all simulation of the body... no collision or dynamics.
      if ( mPhysicsRep )
         mPhysicsRep->setSimulationEnabled( false );

      // Stop doing tick processing for this SceneObject.
      setProcessTick( false );
   }
// start jc
   if(isClientObject())
   {
      getDataBlock()->onAddClient_callback(this);

      if(mShapeInst && mShapeInst->getMaterialList())
         getDataBlock()->onMaterialsLoaded_callback(this);
   }
// end jc

   return true;
}

void PhysicsAnimatedShape::onRemove()
{
   removeFromScene();

   SAFE_DELETE( mPhysicsRep );
   SAFE_DELETE( mShapeInst );
   mAmbientThread = NULL;
   mAmbientSeq = -1;
   mWorld = NULL;

   if ( isServerObject() )
   {
      PhysicsPlugin::getPhysicsResetSignal().remove( this, &PhysicsAnimatedShape::_onPhysicsReset );

      if ( mDestroyedShape )
		  mDestroyedShape->deleteObject();
   }

   // Remove the resource change signal.
   //ResourceManager::get().getChangedSignal().remove( this, &PhysicsAnimatedShape::_onResourceChanged );

   Parent::onRemove();
}
*/
// end jc