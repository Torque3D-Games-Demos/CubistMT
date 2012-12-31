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

#include "T3D/gameTSCtrl.h"
#include "console/consoleTypes.h"
#include "T3D/gameBase/gameBase.h"
#include "T3D/gameBase/gameConnection.h"
#include "T3D/shapeBase.h"
#include "T3D/gameFunctions.h"
#include "console/engineAPI.h"
// start jc
#include "gui/core/guiCanvas.h"
#include "scene/SceneManager.h"
// end jc

//---------------------------------------------------------------------------
// Debug stuff:
Point3F lineTestStart = Point3F(0, 0, 0);
Point3F lineTestEnd =   Point3F(0, 1000, 0);
Point3F lineTestIntersect =  Point3F(0, 0, 0);
bool gSnapLine = false;

//----------------------------------------------------------------------------
// Class: GameTSCtrl
//----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GameTSCtrl);

// See Torque manual (.CHM) for more information
ConsoleDocClass( GameTSCtrl,
	"@brief The main 3D viewport for a Torque 3D game.\n\n"
	"@ingroup Gui3D\n");

GameTSCtrl::GameTSCtrl()
{
}

//---------------------------------------------------------------------------
bool GameTSCtrl::onAdd()
{
   if ( !Parent::onAdd() )
      return false;

#ifdef TORQUE_DEMO_WATERMARK
   mWatermark.init();
#endif

   return true;
}

//---------------------------------------------------------------------------
bool GameTSCtrl::processCameraQuery(CameraQuery *camq)
{
   GameUpdateCameraFov();
   return GameProcessCameraQuery(camq);
}

//---------------------------------------------------------------------------
void GameTSCtrl::renderWorld(const RectI &updateRect)
{
   GameRenderWorld();
}

//---------------------------------------------------------------------------
void GameTSCtrl::makeScriptCall(const char *func, const GuiEvent &evt) const
{
   // write screen position
   char *sp = Con::getArgBuffer(32);
   dSprintf(sp, 32, "%d %d", evt.mousePoint.x, evt.mousePoint.y);

   // write world position
   char *wp = Con::getArgBuffer(32);
   Point3F camPos;
   mLastCameraQuery.cameraMatrix.getColumn(3, &camPos);
   dSprintf(wp, 32, "%g %g %g", camPos.x, camPos.y, camPos.z);

   // write click vector
   char *vec = Con::getArgBuffer(32);
   Point3F fp(evt.mousePoint.x, evt.mousePoint.y, 1.0);
   Point3F ray;
   unproject(fp, &ray);
   ray -= camPos;
   ray.normalizeSafe();
   dSprintf(vec, 32, "%g %g %g", ray.x, ray.y, ray.z);

   Con::executef( (SimObject*)this, func, sp, wp, vec );
}

void GameTSCtrl::onMouseDown(const GuiEvent &evt)
{
   Parent::onMouseDown(evt);
   if( isMethod( "onMouseDown" ) )
      makeScriptCall( "onMouseDown", evt );
}

void GameTSCtrl::onRightMouseDown(const GuiEvent &evt)
{
   Parent::onRightMouseDown(evt);
   if( isMethod( "onRightMouseDown" ) )
      makeScriptCall( "onRightMouseDown", evt );
}

void GameTSCtrl::onMiddleMouseDown(const GuiEvent &evt)
{
   Parent::onMiddleMouseDown(evt);
   if( isMethod( "onMiddleMouseDown" ) )
      makeScriptCall( "onMiddleMouseDown", evt );
}

void GameTSCtrl::onMouseUp(const GuiEvent &evt)
{
   Parent::onMouseUp(evt);
   if( isMethod( "onMouseUp" ) )
      makeScriptCall( "onMouseUp", evt );
}

void GameTSCtrl::onRightMouseUp(const GuiEvent &evt)
{
   Parent::onRightMouseUp(evt);
   if( isMethod( "onRightMouseUp" ) )
      makeScriptCall( "onRightMouseUp", evt );
}

void GameTSCtrl::onMiddleMouseUp(const GuiEvent &evt)
{
   Parent::onMiddleMouseUp(evt);
   if( isMethod( "onMiddleMouseUp" ) )
      makeScriptCall( "onMiddleMouseUp", evt );
}

void GameTSCtrl::onMouseMove(const GuiEvent &evt)
{
   if(gSnapLine)
      return;

   MatrixF mat;
   Point3F vel;
   if ( GameGetCameraTransform(&mat, &vel) )
   {
      Point3F pos;
      mat.getColumn(3,&pos);
      Point3F screenPoint((F32)evt.mousePoint.x, (F32)evt.mousePoint.y, -1.0f);
      Point3F worldPoint;
      if (unproject(screenPoint, &worldPoint)) {
         Point3F vec = worldPoint - pos;
         lineTestStart = pos;
         vec.normalizeSafe();
         lineTestEnd = pos + vec * 1000;
      }
   }
}

void GameTSCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   // check if should bother with a render
   GameConnection * con = GameConnection::getConnectionToServer();
   bool skipRender = !con || (con->getWhiteOut() >= 1.f) || (con->getDamageFlash() >= 1.f) || (con->getBlackOut() >= 1.f);

   if(!skipRender || true)
      Parent::onRender(offset, updateRect);

#ifdef TORQUE_DEMO_WATERMARK
   mWatermark.render(getExtent());
#endif
}

//--------------------------------------------------------------------------

DefineEngineFunction(snapToggle, void, (),,
					 "@brief Prevents mouse movement from being processed\n\n"

					 "In the source, whenever a mouse move event occurs "
					 "GameTSCtrl::onMouseMove() is called. Whenever snapToggle() "
					 "is called, it will flag a variable that can prevent this "
					 "from happening: gSnapLine. This variable is not exposed to "
					 "script, so you need to call this function to trigger it.\n\n"

					 "@tsexample\n"
					 "// Snapping is off by default, so we will toggle\n"
					 "// it on first:\n"
					 "PlayGui.snapToggle();\n\n"
					 "// Mouse movement should be disabled\n"
					 "// Let's turn it back on\n"
					 "PlayGui.snapToggle();\n"
					 "@endtsexample\n\n"

					 "@ingroup GuiGame")
{
	gSnapLine = !gSnapLine;
}
//
//ConsoleFunction( snapToggle, void, 1, 1, "()" )
//{
//   gSnapLine = !gSnapLine;
//}

// start jc
IMPLEMENT_CALLBACK( TouchTSCtrl, onMouseDown, void, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ),
														  ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onMouseUp, void, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ),
													    ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onMouseMove, void, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
												   ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onMouseDragged, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
												   ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onMouseEnter, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
												   ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onMouseLeave, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
												   ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onRightMouseDown, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
												   ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onRightMouseUp, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
												   ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onRightMouseDragged, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
												   ( modifier, mousePoint, mouseClickCount ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onTouchDown, void, (  S32 id, Point2I touchPoint  ),
												   ( id, touchPoint ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onTouchMove, void, (  S32 id, Point2I touchPoint  ),
												   ( id, touchPoint ),
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

IMPLEMENT_CALLBACK( TouchTSCtrl, onTouchUp, void, (  S32 id, Point2I touchPoint  ),
												   ( id, touchPoint ),
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


//----------------------------------------------------------------------------
// Class: TouchTSCtrl
//----------------------------------------------------------------------------
Point3F  TouchTSCtrl::smCamPos;
MatrixF  TouchTSCtrl::smCamMatrix;
bool     TouchTSCtrl::smCamOrtho = false;
F32      TouchTSCtrl::smCamNearPlane;
F32      TouchTSCtrl::smVisibleDistanceScale = 1.0f;

IMPLEMENT_CONOBJECT(TouchTSCtrl);

// See Torque manual (.CHM) for more information
ConsoleDocClass( TouchTSCtrl,
	"@brief The main 3D viewport for a Torque 3D game plus touch and 3d picking.\n\n"
	"@ingroup Gui3D\n");

TouchTSCtrl::TouchTSCtrl() :
         mLastEvent(),
         mLastTouchEvent(),
         mEnable3DInputEvents(true),
         m3DInputEventRange(1000.0f)
{
}
//------------------------------------------------------------------------------

void TouchTSCtrl::make3DMouseEvent(Gui3DMouseEvent & gui3DMouseEvent, const GuiEvent & event)
{
   (GuiEvent&)(gui3DMouseEvent) = event;
   gui3DMouseEvent.mousePoint = event.mousePoint;

//   if(!smCamOrtho)
//   {
      // get the eye pos and the mouse vec from that...
      Point3F screenPoint((F32)gui3DMouseEvent.mousePoint.x, (F32)gui3DMouseEvent.mousePoint.y, 1.0f);

      Point3F wp;
      unproject(screenPoint, &wp);

      gui3DMouseEvent.pos = smCamPos;
      gui3DMouseEvent.vec = wp - smCamPos;
      gui3DMouseEvent.vec.normalizeSafe();
   /*
   }
   else
   {
      // get the eye pos and the mouse vec from that...
      Point3F screenPoint((F32)gui3DMouseEvent.mousePoint.x, (F32)gui3DMouseEvent.mousePoint.y, 0.0f);

      Point3F np, fp;
      unproject(screenPoint, &np);

      gui3DMouseEvent.pos = np;
      smCamMatrix.getColumn( 1, &(gui3DMouseEvent.vec) );
   }
   */
}

void TouchTSCtrl::make3DTouchEvent(Gui3DTouchEvent & gui3DTouchEvent, const GuiTouchEvent & event)
{
   (GuiEvent&)(gui3DTouchEvent) = event;
   gui3DTouchEvent.touchPosition = event.touchPosition;
   gui3DTouchEvent.touchId = event.touchId;

   // get the eye pos and the mouse vec from that...
   Point3F screenPoint((F32)gui3DTouchEvent.touchPosition.x, (F32)gui3DTouchEvent.touchPosition.y, 1.0f);

   Point3F wp;
   unproject(screenPoint, &wp);

   gui3DTouchEvent.pos = smCamPos;
   gui3DTouchEvent.vec = wp - smCamPos;
   gui3DTouchEvent.vec.normalizeSafe();
}


void TouchTSCtrl::onMouseUp(const GuiEvent & event)
{
//   mLeftMouseDown = false;
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DMouseUp(mLastEvent);
   }

   onMouseUp_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onMouseUp(event);
}

void TouchTSCtrl::onMouseDown(const GuiEvent & event)
{
//   mLeftMouseDown = true;
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DMouseDown(mLastEvent);
   }
//   setFirstResponder();

   onMouseDown_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onMouseDown(event);
}

void TouchTSCtrl::onMouseMove(const GuiEvent & event)
{
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DMouseMove(mLastEvent);
   }
//   mLastMousePos = event.mousePoint;

   onMouseMove_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onMouseMove(event);
}

void TouchTSCtrl::onMouseDragged(const GuiEvent & event)
{
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DMouseDragged(mLastEvent);
   }

   onMouseDragged_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onMouseDragged(event);
}

void TouchTSCtrl::onMouseEnter(const GuiEvent & event)
{
//   mMouseLeft = false;
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DMouseEnter(mLastEvent);
   }

   onMouseEnter_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onMouseEnter(event);
}



void TouchTSCtrl::onMouseLeave(const GuiEvent & event)
{
//   mMouseLeft = true;
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DMouseLeave(mLastEvent);
   }

   onMouseLeave_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onMouseLeave(event);
}
void TouchTSCtrl::onRightMouseDown(const GuiEvent & event)
{
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DRightMouseDown(mLastEvent);
   }

   onRightMouseDown_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onRightMouseDown(event);
}
void TouchTSCtrl::onRightMouseUp(const GuiEvent & event)
{
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DRightMouseUp(mLastEvent);
   }

   onRightMouseUp_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onRightMouseUp(event);
}
void TouchTSCtrl::onRightMouseDragged(const GuiEvent & event)
{
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DRightMouseDragged(mLastEvent);
   }

   onRightMouseDragged_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   Parent::Parent::onRightMouseDragged(event);
}
bool TouchTSCtrl::onMouseWheelUp(const GuiEvent & event)
{
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DMouseWheelUp(mLastEvent);
   }

//   onMouseWheelUp_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   return Parent::Parent::onMouseWheelUp(event);
}
bool TouchTSCtrl::onMouseWheelDown(const GuiEvent & event)
{
   if(mEnable3DInputEvents)
   {
      make3DMouseEvent(mLastEvent, event);
      on3DMouseWheelDown(mLastEvent);
   }

//   onMouseWheelDown_callback(event.modifier, event.mousePoint, event.mouseClickCount);

   return Parent::Parent::onMouseWheelDown(event);
}

bool TouchTSCtrl::onTouchUp(const GuiTouchEvent & event)
{
   removeDeadObjects();
   if(mEnable3DInputEvents)
   {
      make3DTouchEvent(mLastTouchEvent, event);
	  if(!on3DTouchUp(mLastTouchEvent))
		onTouchUp_callback(event.touchId, globalToLocalCoord(event.touchPosition));
	  return true;
   }

   return Parent::Parent::onTouchUp(event);
}
bool TouchTSCtrl::onTouchDown(const GuiTouchEvent & event)
{
   removeDeadObjects();
   if(mEnable3DInputEvents)
   {
      make3DTouchEvent(mLastTouchEvent, event);
	  if(!on3DTouchDown(mLastTouchEvent))
         onTouchDown_callback(event.touchId, globalToLocalCoord(event.touchPosition));
	  return true;
   }

   return Parent::Parent::onTouchDown(event);
}
bool TouchTSCtrl::onTouchMove(const GuiTouchEvent & event)
{
   removeDeadObjects();
   if(mEnable3DInputEvents)
   {
      make3DTouchEvent(mLastTouchEvent, event);
	  if(!on3DTouchMove(mLastTouchEvent))
         onTouchMove_callback(event.touchId, globalToLocalCoord(event.touchPosition));
      return true;
   }

   return Parent::Parent::onTouchMove(event);
}

bool collisionAlphaCheck( RayInfo* ri )
{
   return !ri->object->rayCrossesTransparency(ri);
}

bool TouchTSCtrl::_findInputEventObject(const Gui3DMouseEvent &event, ShapeInputEvent &objEvent)
{
   
   objEvent.ri.generateTexCoord  = true;
   objEvent.mousePoint = event.mousePoint;
   objEvent.mouseClickCount = event.mouseClickCount;
   objEvent.pos = event.pos;
   objEvent.vec = event.vec;
 //  objEvent.ri.generateTexCoord = true;

   Point3F rayEnd = (event.vec * m3DInputEventRange) + event.pos;

   bool result = false;
//   result = gClientContainer.castRayRendered(event.pos, rayEnd, InputEventObjectType, &(objEvent.ri), collisionAlphaCheck);
   result = gClientContainer.castRayHybrid(event.pos, rayEnd, InputEventObjectType, &(objEvent.ri), collisionAlphaCheck);
//   result = gClientContainer.castRay(event.pos, rayEnd, InputEventObjectType, &(objEvent.ri));

   if (!result)
      return false;

// todo: why was I doing this?
/*
   SceneObject* hitObject = objEvent.ri.object;
   // clear ri for reuse
   RayInfo newri;
   newri.generateTexCoord = true;

 //  Point3F xformedStart;
 //  hitObject->getWorldTransform().mulP(event.pos, &xformedStart);
 //  xformedStart.convolveInverse(hitObject->getScale());
 //  rayEnd = (event.vec * m3DInputEventRange) + xformedStart;

	Point3F xformedStart, xformedEnd;
	hitObject->getWorldTransform().mulP(event.pos, &xformedStart);
	hitObject->getWorldTransform().mulP(rayEnd,   &xformedEnd);
	xformedStart.convolveInverse(hitObject->mObjScale);
	xformedEnd.convolveInverse(hitObject->mObjScale);

   result = hitObject->castRayRendered(xformedStart, xformedEnd, &(newri));

   if (!result)
      return false;
   // translate hit back to world space
   // todo: see what needs to be done to the normal
   Point3F newPoint;
   newri.point *= hitObject->getScale();
   hitObject->getTransform().mulP(newri.point, &newPoint);
   newri.point = newPoint;
   objEvent.ri = newri;

 //  if(objEvent.ri.texCoord.x == -1 || !objEvent.ri.material)
 //     return false;
*/

   return true;
}

bool TouchTSCtrl::_findInputEventObjectHitPoint(SceneObject *obj, const Gui3DMouseEvent &event, ShapeInputEvent &objEvent)
{
   
   objEvent.ri.generateTexCoord  = true;
   objEvent.mousePoint = event.mousePoint;
   objEvent.mouseClickCount = event.mouseClickCount;
   objEvent.pos = event.pos;
   objEvent.vec = event.vec;

   Point3F rayEnd = (event.vec * m3DInputEventRange) + event.pos;

	Point3F xformedStart, xformedEnd;
	obj->getWorldTransform().mulP(event.pos, &xformedStart);
	obj->getWorldTransform().mulP(rayEnd,   &xformedEnd);
	xformedStart.convolveInverse(obj->mObjScale);
	xformedEnd.convolveInverse(obj->mObjScale);

   bool result = false;
   result = obj->castRayRendered(xformedStart, xformedEnd, &(objEvent.ri));

  // if (!result || collisionAlphaCheck(objEvent.ri))
   if (!result)
      return false;

 //  if(objEvent.ri.texCoord.x == -1 || !objEvent.ri.material)
 //     return false;

   return true;
}

bool TouchTSCtrl::_findTouchEventObject(const Gui3DTouchEvent &event, ShapeTouchEvent &objEvent)
{
   
   objEvent.ri.generateTexCoord  = true;
   objEvent.touchPosition = event.touchPosition;
   objEvent.touchId = event.touchId;
   objEvent.pos = event.pos;
   objEvent.vec = event.vec;

   Point3F rayEnd = (event.vec * m3DInputEventRange) + event.pos;

   bool result = false;
//   result = gClientContainer.castRayRendered(event.pos, rayEnd, InputEventObjectType, &(objEvent.ri), collisionAlphaCheck);
   result = gClientContainer.castRayHybrid(event.pos, rayEnd, InputEventObjectType, &(objEvent.ri), collisionAlphaCheck);
      
//   result = gClientContainer.castRay(event.pos, rayEnd, InputEventObjectType, &(objEvent.ri));

   if (!result)
      return false;
/*
//   if(objEvent.ri.texCoord.x == -1 || !objEvent.ri.material)
//      return false;
   SceneObject* hitObject = objEvent.ri.object;
   // clear ri for reuse
   RayInfo newri;
   newri.generateTexCoord = true;

	Point3F xformedStart, xformedEnd;
	hitObject->getWorldTransform().mulP(event.pos, &xformedStart);
	hitObject->getWorldTransform().mulP(rayEnd,   &xformedEnd);
	xformedStart.convolveInverse(hitObject->mObjScale);
	xformedEnd.convolveInverse(hitObject->mObjScale);

   result = hitObject->castRayRendered(xformedStart, xformedEnd, &(newri));

   if (!result)
      return false;

   // translate hit back to world space
   // todo: see what needs to be done to the normal
   Point3F newPoint;
   newri.point.convolve(hitObject->mObjScale);
   hitObject->getTransform().mulP(newri.point, &newPoint);
   newri.point = newPoint;
   objEvent.ri = newri;

*/
   return true;
}
bool TouchTSCtrl::_findTouchEventObjectHitPoint(SceneObject *obj, const Gui3DTouchEvent &event, ShapeTouchEvent &objEvent)
{
   
   objEvent.ri.generateTexCoord  = true;
   objEvent.touchPosition = event.touchPosition;
   objEvent.touchId = event.touchId;
   objEvent.pos = event.pos;
   objEvent.vec = event.vec;

   Point3F rayEnd = (event.vec * m3DInputEventRange) + event.pos;

	Point3F xformedStart, xformedEnd;
	obj->getWorldTransform().mulP(event.pos, &xformedStart);
	obj->getWorldTransform().mulP(rayEnd,   &xformedEnd);
	xformedStart.convolveInverse(obj->mObjScale);
	xformedEnd.convolveInverse(obj->mObjScale);

   bool result = false;
   result = obj->castRayRendered(xformedStart, xformedEnd, &(objEvent.ri));
//   result = gClientContainer.castRay(event.pos, rayEnd, InputEventObjectType, &(objEvent.ri));

   if (!result)
      return false;

//   if(objEvent.ri.texCoord.x == -1 || !objEvent.ri.material)
//      return false;

   return true;
}

void TouchTSCtrl::on3DMouseUp(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onMouseUp(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseUp(objEvent);
	  */
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onMouseUp(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseUp(objEvent);
}

void TouchTSCtrl::on3DMouseDown(const Gui3DMouseEvent &event)
{
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onMouseDown(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseDown(objEvent);
}
void TouchTSCtrl::on3DMouseMove(const Gui3DMouseEvent &event)
{
/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onMouseMove(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseMove(objEvent);
*/
   ShapeInputEvent objEvent;

   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onMouseMove(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseMove(objEvent);
}
void TouchTSCtrl::on3DMouseDragged(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
    
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onMouseDragged(objEvent);
   }

  if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseDragged(objEvent);
	  */
   ShapeInputEvent objEvent;

   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onMouseDragged(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseDragged(objEvent);
}
void TouchTSCtrl::on3DMouseEnter(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onMouseEnter(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseEnter(objEvent);
	  */

   ShapeInputEvent objEvent;
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onMouseEnter(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseEnter(objEvent);
}
void TouchTSCtrl::on3DMouseLeave(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onMouseLeave(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseLeave(objEvent);
	  */
   ShapeInputEvent objEvent;
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onMouseLeave(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseLeave(objEvent);
}
void TouchTSCtrl::on3DRightMouseDown(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onRightMouseDown(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onRightMouseDown(objEvent);
	  */
   ShapeInputEvent objEvent;
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onRightMouseDown(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onRightMouseDown(objEvent);

}
void TouchTSCtrl::on3DRightMouseUp(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onRightMouseUp(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onRightMouseUp(objEvent);
	  */
   ShapeInputEvent objEvent;
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onRightMouseUp(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onRightMouseUp(objEvent);

}
void TouchTSCtrl::on3DRightMouseDragged(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onRightMouseDragged(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onRightMouseDragged(objEvent);
	  */
   ShapeInputEvent objEvent;
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onRightMouseDragged(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onRightMouseDragged(objEvent);

}
void TouchTSCtrl::on3DMouseWheelUp(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onMouseWheelUp(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseWheelUp(objEvent);
	  */
   ShapeInputEvent objEvent;
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onMouseWheelUp(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseWheelUp(objEvent);

}
void TouchTSCtrl::on3DMouseWheelDown(const Gui3DMouseEvent &event)
{
	/*
   ShapeInputEvent objEvent;
   
   if (bool(mMouseCapturedControl))
   {
   //   objEvent.mousePoint = event.mousePoint;
   //   objEvent.vec = event.vec;
   //   objEvent.pos = event.pos;
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      return mMouseCapturedControl->onMouseWheelDown(objEvent);
   }

   if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseWheelDown(objEvent);
	  */
   ShapeInputEvent objEvent;
   if (bool(mMouseCapturedControl))
   {
      _findInputEventObjectHitPoint(mMouseCapturedControl.getPointer(), event, objEvent);
      mMouseCapturedControl->onMouseWheelDown(objEvent);
   }
   else if(_findInputEventObject(event, objEvent))
      objEvent.ri.object->onMouseWheelDown(objEvent);

}

bool TouchTSCtrl::on3DTouchUp(const Gui3DTouchEvent &event)
{
	/*
   ShapeTouchEvent objEvent;

   // if the touchID is locked by a control give it the event and leave
   if(!mTouchCapturedControl.isEmpty())
   {
      HashTable<U32, SceneObject*>::Iterator iter = mTouchCapturedControl.find( event.touchId );
      for ( ; iter != mTouchCapturedControl.end() && iter->key == event.touchId; iter++ )
      {
         //SceneObject* controlHit = mTouchCapturedControl[event.touchId];
         SceneObject* controlHit = iter->value;
         if(controlHit)
         {
         //   objEvent.touchId = event.touchId;
         //   objEvent.touchPosition = event.touchPosition;
         //   objEvent.pos = event.pos;
         //   objEvent.vec = event.vec;
            _findTouchEventObjectHitPoint(controlHit, event, objEvent);
            return controlHit->onTouchUp(objEvent); 
         }
      }
   }

   if(_findTouchEventObject(event, objEvent))
      return objEvent.ri.object->onTouchUp(objEvent);

   return false;
*/
   ShapeTouchEvent objEvent;

   // if the touchID is locked by a control give it the event and leave
   if(!mTouchCapturedControl.isEmpty())
   {
      HashTable<U32, SceneObject*>::Iterator iter = mTouchCapturedControl.find( event.touchId );
      for ( ; iter != mTouchCapturedControl.end() && iter->key == event.touchId; iter++ )
      {
         //SceneObject* controlHit = mTouchCapturedControl[event.touchId];
         SceneObject* controlHit = iter->value;
		 if (controlHit)
		 {
			_findTouchEventObjectHitPoint(controlHit, event, objEvent);
			return controlHit->onTouchUp(objEvent);
		 }
      }
   }
   else if(_findTouchEventObject(event, objEvent))
      return objEvent.ri.object->onTouchUp(objEvent);

   return false;
}
bool TouchTSCtrl::on3DTouchDown(const Gui3DTouchEvent &event)
{
   ShapeTouchEvent objEvent;
   if(_findTouchEventObject(event, objEvent))
      return objEvent.ri.object->onTouchDown(objEvent);
   return false;
	/*
   ShapeTouchEvent objEvent;
   if(_findTouchEventObject(event, objEvent))
   {
      if(!objEvent.ri.object->onTouchDown(objEvent));
      {  
         
      }
      else
      {
         return true;
      }
   }

   return false;
   */
}
bool TouchTSCtrl::on3DTouchMove(const Gui3DTouchEvent &event)
{
	/*
   ShapeTouchEvent objEvent;

   // if the touchID is locked by a control give it the event and leave
   if(!mTouchCapturedControl.isEmpty())
   {
      HashTable<U32, SceneObject*>::Iterator iter = mTouchCapturedControl.find( event.touchId );
      for ( ; iter != mTouchCapturedControl.end() && iter->key == event.touchId; iter++ )
      {
         //SceneObject* controlHit = mTouchCapturedControl[event.touchId];
         SceneObject* controlHit = iter->value;
         if(controlHit)
         {
         //   objEvent.touchId = event.touchId;
         //   objEvent.touchPosition = event.touchPosition;
         //   objEvent.pos = event.pos;
         //   objEvent.vec = event.vec;
            _findTouchEventObjectHitPoint(controlHit, event, objEvent);
            return controlHit->onTouchMove(objEvent); 
         }
      }
   }

   if(_findTouchEventObject(event, objEvent))
      return objEvent.ri.object->onTouchMove(objEvent);
   return false;
   */

   ShapeTouchEvent objEvent;

   // if the touchID is locked by a control give it the event and leave
   if(!mTouchCapturedControl.isEmpty())
   {
      HashTable<U32, SceneObject*>::Iterator iter = mTouchCapturedControl.find( event.touchId );
      for ( ; iter != mTouchCapturedControl.end() && iter->key == event.touchId; iter++ )
      {
         //SceneObject* controlHit = mTouchCapturedControl[event.touchId];
         SceneObject* controlHit = iter->value; 
		 if (controlHit)
		 {
			_findTouchEventObjectHitPoint(controlHit, event, objEvent);
			return controlHit->onTouchMove(objEvent);
		 }
      }
   }
   else if(_findTouchEventObject(event, objEvent))
      return objEvent.ri.object->onTouchMove(objEvent);

   return false;
}


bool TouchTSCtrl::processCameraQuery(CameraQuery * query)
{
//   if(mDisplayType == DisplayTypePerspective)
//   {
      query->ortho = false;
//   }
//   else
//   {
//      query->ortho = true;
//   }

   if (getCameraTransform(&query->cameraMatrix))
   {
      query->farPlane = gClientSceneGraph->getVisibleDistance() * smVisibleDistanceScale;
      query->nearPlane = gClientSceneGraph->getNearClip();
      query->fov = mDegToRad(75.0f);
  // start jc
	  query->frustumOffset = gClientSceneGraph->getFrustumOffset();
  // end jc

/*
      if(query->ortho)
      {
         MatrixF camRot(true);
         const F32 camBuffer = 1.0f;
         Point3F camPos = query->cameraMatrix.getPosition();

         F32 isocamplanedist = 0.0f;
         if(mDisplayType == DisplayTypeIsometric)
         {
            const RectI& vp = GFX->getViewport();
            isocamplanedist = 0.25 * vp.extent.y * mSin(mIsoCamAngle);
         }

         // Calculate the scene bounds
         Box3F sceneBounds;
         computeSceneBounds(sceneBounds);

         if(!sceneBounds.isValidBox())
         {
            sceneBounds.maxExtents = camPos + smMinSceneBounds;
            sceneBounds.minExtents = camPos - smMinSceneBounds;
         }
         else
         {
            query->farPlane = getMax(smMinSceneBounds.x * 2.0f, (sceneBounds.maxExtents - sceneBounds.minExtents).len() + camBuffer * 2.0f + isocamplanedist);
         }

         mRawCamPos = camPos;
         camPos += mOrthoCamTrans;

         switch(mDisplayType)
         {
            case DisplayTypeTop:
               camRot.setColumn(0, Point3F(1.0, 0.0,  0.0));
               camRot.setColumn(1, Point3F(0.0, 0.0, -1.0));
               camRot.setColumn(2, Point3F(0.0, 1.0,  0.0));
               camPos.z = getMax(camPos.z + smMinSceneBounds.z, sceneBounds.maxExtents.z + camBuffer);
               break;

            case DisplayTypeBottom:
               camRot.setColumn(0, Point3F(1.0,  0.0,  0.0));
               camRot.setColumn(1, Point3F(0.0,  0.0,  1.0));
               camRot.setColumn(2, Point3F(0.0, -1.0,  0.0));
               camPos.z = getMin(camPos.z - smMinSceneBounds.z, sceneBounds.minExtents.z - camBuffer);
               break;

            case DisplayTypeFront:
               camRot.setColumn(0, Point3F(-1.0,  0.0,  0.0));
               camRot.setColumn(1, Point3F( 0.0, -1.0,  0.0));
               camRot.setColumn(2, Point3F( 0.0,  0.0,  1.0));
               camPos.y = getMax(camPos.y + smMinSceneBounds.y, sceneBounds.maxExtents.y + camBuffer);
               break;

            case DisplayTypeBack:
               camRot.setColumn(0, Point3F(1.0,  0.0,  0.0));
               camRot.setColumn(1, Point3F(0.0,  1.0,  0.0));
               camRot.setColumn(2, Point3F(0.0,  0.0,  1.0));
               camPos.y = getMin(camPos.y - smMinSceneBounds.y, sceneBounds.minExtents.y - camBuffer);
               break;

            case DisplayTypeLeft:
               camRot.setColumn(0, Point3F( 0.0, -1.0,  0.0));
               camRot.setColumn(1, Point3F( 1.0,  0.0,  0.0));
               camRot.setColumn(2, Point3F( 0.0,  0.0,  1.0));
               camPos.x = getMin(camPos.x - smMinSceneBounds.x, sceneBounds.minExtents.x - camBuffer);
               break;

            case DisplayTypeRight:
               camRot.setColumn(0, Point3F( 0.0,  1.0,  0.0));
               camRot.setColumn(1, Point3F(-1.0,  0.0,  0.0));
               camRot.setColumn(2, Point3F( 0.0,  0.0,  1.0));
               camPos.x = getMax(camPos.x + smMinSceneBounds.x, sceneBounds.maxExtents.x + camBuffer);
               break;

            case DisplayTypeIsometric:
               camPos.z = sceneBounds.maxExtents.z + camBuffer + isocamplanedist;
               MatrixF angle(EulerF(mIsoCamAngle, 0, 0));
               MatrixF rot(mIsoCamRot);
               camRot.mul(rot, angle);
               break;
         }

         query->cameraMatrix = camRot;
         query->cameraMatrix.setPosition(camPos);
         query->fov = mOrthoFOV;
      }
*/
      smCamMatrix = query->cameraMatrix;
      smCamMatrix.getColumn(3,&smCamPos);
      smCamOrtho = query->ortho;
      smCamNearPlane = query->nearPlane;

      return true;
   }
   return false;
}


void TouchTSCtrl::mouseLock(SceneObject *lockingControl)
{
   if (bool(mMouseCapturedControl))
      return;

   mMouseCapturedControl = lockingControl;

   if(mMouseControl && mMouseControl != mMouseCapturedControl)
   {
      GuiCanvas *canvas = getRoot();
      if(!canvas)
         return;

      GuiEvent evt;
      Gui3DMouseEvent evt3d;
      ShapeInputEvent objEvent;

      evt.mousePoint = canvas->getCanvasCursorPos();
      make3DMouseEvent(evt3d,evt);
      if(_findInputEventObject(evt3d, objEvent) && objEvent.ri.object != mMouseControl)
         mMouseControl->onMouseLeave(objEvent);
      
   }
}

void TouchTSCtrl::mouseUnlock(SceneObject *lockingControl)
{
   if (static_cast<SceneObject*>(mMouseCapturedControl) != lockingControl)
      return;

   GuiCanvas *canvas = getRoot();
   if(!canvas)
      return;

   GuiEvent evt;
   Gui3DMouseEvent evt3d;
   ShapeInputEvent objEvent;

   evt.mousePoint = canvas->getCanvasCursorPos();
   make3DMouseEvent(evt3d,evt);
   if(_findInputEventObject(evt3d, objEvent) && objEvent.ri.object != mMouseControl)
   {
      mMouseControl = objEvent.ri.object;
//      mMouseControlClicked = false;
      if(bool(mMouseControl))
         mMouseControl->onMouseEnter(objEvent);
   }

   mMouseCapturedControl = NULL;
}

void TouchTSCtrl::touchIDLock(U32 touchID, SceneObject *lockingControl)
{
//   Con::printf("TouchTSCtrl::touchIDLock: %d %s", touchID, lockingControl->getIdString());
   // if id is already locked forget it
//   if (bool(mTouchCapturedControl[touchID]))
//      return;

   // store a pointer to the locking object indexed against the id
   mTouchCapturedControl.insertEqual(touchID, lockingControl);
}
void  TouchTSCtrl::removeDeadObjects()
{
   bool changed = false;
   do{
      changed = false;
      HashTable<U32, SceneObject*>::Iterator obj = mTouchCapturedControl.begin();
      for(; obj != mTouchCapturedControl.end(); obj++)
      {
         SceneObject *so = obj->value;
         if( Sim::findObject(so->getId()) == NULL ){
            mTouchCapturedControl.erase(obj->key, obj->value);
            changed = true;
         }
      }
   }while(changed);
}
void TouchTSCtrl::touchIDUnlock(U32 touchID, SceneObject *lockingControl)
{
//   Con::printf("TouchTSCtrl::touchIDUnlock: %d %s", touchID, lockingControl->getIdString());
   // shouldn't happen really but it should be harmless
//   SceneObject* controlHit = mTouchCapturedControl[touchID];

//   if(!controlHit)
//      return;

//   if(controlHit != lockingControl)
//      return;

   // go on off you go, can't have clogging up the system
   mTouchCapturedControl.erase(touchID, lockingControl);
}

void TouchTSCtrl::touchIDUnlockAll(SceneObject *lockingControl)
{
   // go on off you go, can't have clogging up the system
   HashTable<U32, SceneObject*>::Iterator obj = mTouchCapturedControl.begin();
   for(; obj != mTouchCapturedControl.end(); obj++)
      if(obj->value == lockingControl)
         mTouchCapturedControl.erase(obj->key, lockingControl);
   
}




bool TouchTSCtrl::getCameraTransform(MatrixF* cameraMatrix)
{
   GameConnection* connection = dynamic_cast<GameConnection *>(NetConnection::getConnectionToServer());
   return (connection && connection->getControlCameraTransform(0.032f, cameraMatrix));
}



DefineEngineMethod( TouchTSCtrl, mouseLockObject, void, (SceneObject* lockingObject),,
   "mouseLockObject() Lock mouse input to this control.\n" )
{
   object->mouseLock(lockingObject);
}

DefineEngineMethod( TouchTSCtrl, mouseUnlockObject, void, (SceneObject* lockingObject),,
   "mouseUnlockObject() Unlock mouse input from this control.\n" )
{
   object->mouseUnlock(lockingObject);
}

DefineEngineMethod( TouchTSCtrl, isMouseLockedObject, bool, (),,
   "isMouseLockedObject() is mouse input locked to this control.\n" )
{
   return object->isMouseLocked();
}

DefineEngineMethod( TouchTSCtrl, touchIDLockObject, void, (U32 touchID, SceneObject* lockingObject),,
   "touchIDLockObject(U32 touchID) Lock mouse input to this control.\n" )
{
   S32 touchId = touchID;
   if(touchId >= 0)
      object->touchIDLock(touchId, lockingObject);
}

DefineEngineMethod( TouchTSCtrl, touchIDUnlockObject, void, (U32 touchID, SceneObject* lockingObject),,
   "touchIDUnlockObject(U32 touchID) Unlock mouse input from this control.\n" )
{
   object->touchIDUnlock(touchID, lockingObject);
}
DefineEngineMethod( TouchTSCtrl, isTouchIDLockedObject, bool, (U32 touchID),,
   "isTouchIDLockedObject(U32 touchID) is mouse input locked to this control.\n" )
{
//   SceneObject* scene = static_cast<SceneObject*>(Sim::findObject(dAtoi(argv[3])));
   return object->isTouchIDLocked(touchID);
}
DefineEngineMethod( TouchTSCtrl, touchIDUnlockAll, void, (SceneObject* lockingObject),,
   "mouseUnlockObject() Unlock mouse input from this control.\n" )
{
   object->touchIDUnlockAll(lockingObject);
}
// end jc
