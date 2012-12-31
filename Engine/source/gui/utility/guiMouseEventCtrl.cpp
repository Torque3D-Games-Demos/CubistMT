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

#include "gui/utility/guiMouseEventCtrl.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
//start jc
#include "console/simTUIO.h"
// end jc

IMPLEMENT_CONOBJECT(GuiMouseEventCtrl);

ConsoleDocClass( GuiMouseEventCtrl,
   "@brief Used to overlaps a 'hot region' where you want to catch inputs with and have specific events occur based on individual callbacks.\n\n"

   "Mouse event callbacks supported by this control are: onMouseUp, onMouseDown, onMouseMove, onMouseDragged, onMouseEnter, onMouseLeave,\n"
   "onRightMouseDown, onRightMouseUp and onRightMouseDragged.\n\n"

   "@tsexample\n"
   "new GuiMouseEventCtrl()\n"
   "{\n"
   "	lockMouse = \"0\";\n"
   "	//Properties not specific to this control have been omitted from this example.\n"
   "};\n"
   "@endtsexample\n\n"

   "@see GuiControl\n\n"

   "@ingroup GuiCore\n"
);


IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onMouseDown, void, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onMouseUp, void, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onMouseMove, void, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onMouseDragged, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onMouseEnter, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onMouseLeave, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onRightMouseDown, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onRightMouseUp, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onRightMouseDragged, void, (  U8 modifier, Point2I mousePoint,U8 mouseClickCount  ),
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

// start jc

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onTouchDown, void, (  S32 id, Point2I touchPoint  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onTouchMove, void, (  S32 id, Point2I touchPoint  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, onTouchUp, void, (  S32 id, Point2I touchPoint  ),
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

IMPLEMENT_CALLBACK( GuiMouseEventCtrl, childResized, void, (  S32 childId  ),
												   ( childId ),
   "@brief Callback .\n\n"
);
// end jc

GuiMouseEventCtrl::GuiMouseEventCtrl()
{
   mLockMouse = false;
}

//------------------------------------------------------------------------------
// start jc
/*
void GuiMouseEventCtrl::sendMouseEvent(const char * name, const GuiEvent & event)
{
   char buf[3][32];
   dSprintf(buf[0], 32, "%d", event.modifier);
   dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
   dSprintf(buf[2], 32, "%d", event.mouseClickCount);

   if(dStricmp(name,"onMouseDown") == 0)
	   onMouseDown_callback(event.modifier, event.mousePoint, event.mouseClickCount);
   else if(dStricmp(name,"onMouseUp") == 0)
	   onMouseUp_callback(event.modifier, event.mousePoint, event.mouseClickCount);
	else if(dStricmp(name,"onMouseMove") == 0)
		onMouseMove_callback(event.modifier, event.mousePoint, event.mouseClickCount);
	else if(dStricmp(name,"onMouseDragged") == 0)
		onMouseDragged_callback(event.modifier, event.mousePoint, event.mouseClickCount);
	else if(dStricmp(name,"onMouseEnter") == 0)
		onMouseEnter_callback(event.modifier, event.mousePoint, event.mouseClickCount);
	else if(dStricmp(name,"onMouseLeave") == 0)
		onMouseLeave_callback(event.modifier, event.mousePoint, event.mouseClickCount);
	else if(dStricmp(name,"onRightMouseDown") == 0)
		onRightMouseDown_callback(event.modifier, event.mousePoint, event.mouseClickCount);
	else if(dStricmp(name,"onRightMouseUp") == 0)
		onRightMouseUp_callback(event.modifier, event.mousePoint, event.mouseClickCount);
	else if(dStricmp(name,"onRightMouseDragged") == 0)
		onRightMouseDragged_callback(event.modifier, event.mousePoint, event.mouseClickCount);
}
*/
void GuiMouseEventCtrl::sendMouseEvent(const EventId id, const char * name, const GuiEvent & event)
{
//   char buf[3][32];
//   dSprintf(buf[0], 32, "%d", event.modifier);
//   dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
//   dSprintf(buf[2], 32, "%d", event.mouseClickCount);

   switch(id){
      case OnMouseDown:          onMouseDown_callback(event.modifier, event.mousePoint, event.mouseClickCount);         break;         
      case OnMouseUp:            onMouseUp_callback(event.modifier, event.mousePoint, event.mouseClickCount);           break;         
      case OnMouseMove:          onMouseMove_callback(event.modifier, event.mousePoint, event.mouseClickCount);         break;         
      case OnMouseDragged:       onMouseDragged_callback(event.modifier, event.mousePoint, event.mouseClickCount);      break;         
      case OnMouseEnter:         onMouseEnter_callback(event.modifier, event.mousePoint, event.mouseClickCount);        break;         
      case OnMouseLeave:         onMouseLeave_callback(event.modifier, event.mousePoint, event.mouseClickCount);        break;         
      case OnRightMouseDown:     onRightMouseDown_callback(event.modifier, event.mousePoint, event.mouseClickCount);    break;         
      case OnRightMouseUp:       onRightMouseUp_callback(event.modifier, event.mousePoint, event.mouseClickCount);      break;         
      case OnRightMouseDragged:  onRightMouseDragged_callback(event.modifier, event.mousePoint, event.mouseClickCount); break;  
   }
}
bool GuiMouseEventCtrl::sendTouchEvent(const EventId id, const char * name, const GuiEvent & rawEvent)
{
   const GuiTouchEvent* event = static_cast<const GuiTouchEvent*>(&rawEvent);
   if(!event) return false;

//   char buf[2][32];
//   dSprintf(buf[0], 32, "%d %d", event->touchPosition.x, event->touchPosition.y);
//   dSprintf(buf[1], 32, "%d", event->touchId);

   switch(id){
      case OnTouchDown:    onTouchDown_callback(event->touchId, globalToLocalCoord(event->touchPosition));   break;         
      case OnTouchMove:    onTouchMove_callback(event->touchId, globalToLocalCoord(event->touchPosition));   break;         
      case OnTouchUp:      onTouchUp_callback(event->touchId, globalToLocalCoord(event->touchPosition));     break;   
   }
   return true;
}

void GuiMouseEventCtrl::childResized( GuiControl* child )
{
   childResized_callback(child->getId());
}
// end jc
//------------------------------------------------------------------------------
void GuiMouseEventCtrl::initPersistFields()
{
   addGroup( "Input" );
   
      addField("lockMouse", TypeBool, Offset(mLockMouse, GuiMouseEventCtrl),
         "Whether the control should lock the mouse between up and down button events." );
      
   endGroup( "Input" );

   Parent::initPersistFields();

   Con::setIntVariable("$EventModifier::LSHIFT",      SI_LSHIFT);
   Con::setIntVariable("$EventModifier::RSHIFT",      SI_RSHIFT);
   Con::setIntVariable("$EventModifier::SHIFT",       SI_SHIFT);
   Con::setIntVariable("$EventModifier::LCTRL",       SI_LCTRL);
   Con::setIntVariable("$EventModifier::RCTRL",       SI_RCTRL);
   Con::setIntVariable("$EventModifier::CTRL",        SI_CTRL);
   Con::setIntVariable("$EventModifier::LALT",        SI_LALT);
   Con::setIntVariable("$EventModifier::RALT",        SI_RALT);
   Con::setIntVariable("$EventModifier::ALT",         SI_ALT);
}

//------------------------------------------------------------------------------
void GuiMouseEventCtrl::onMouseDown(const GuiEvent & event)
{
   if(mLockMouse)
      mouseLock();
// start jc
//   sendMouseEvent("onMouseDown", event);
   sendMouseEvent(OnMouseDown, "onMouseDown", event);
// end jc
}

void GuiMouseEventCtrl::onMouseUp(const GuiEvent & event)
{
   if(mLockMouse)
      mouseUnlock();
// start jc
//   sendMouseEvent("onMouseUp", event);
   sendMouseEvent(OnMouseUp, "onMouseUp", event);
// end jc
}

void GuiMouseEventCtrl::onMouseMove(const GuiEvent & event)
{
// start jc
//   sendMouseEvent("onMouseMove", event);
   sendMouseEvent(OnMouseMove, "onMouseMove", event);
// end jc
}

void GuiMouseEventCtrl::onMouseDragged(const GuiEvent & event)
{
// start jc
//   sendMouseEvent("onMouseDragged", event);
   sendMouseEvent(OnMouseDragged, "onMouseDragged", event);
// end jc
}

void GuiMouseEventCtrl::onMouseEnter(const GuiEvent & event)
{
// start jc
//   sendMouseEvent("onMouseEnter", event);
   sendMouseEvent(OnMouseEnter, "onMouseEnter", event);
// end jc
}

void GuiMouseEventCtrl::onMouseLeave(const GuiEvent & event)
{
// start jc
//   sendMouseEvent("onMouseLeave", event);
   sendMouseEvent(OnMouseLeave, "onMouseLeave", event);
// end jc
}

void GuiMouseEventCtrl::onRightMouseDown(const GuiEvent & event)
{
   if(mLockMouse)
      mouseLock();
// start jc
//   sendMouseEvent("onRightMouseDown", event);
   sendMouseEvent(OnRightMouseDown, "onRightMouseDown", event);
// end jc
}

void GuiMouseEventCtrl::onRightMouseUp(const GuiEvent & event)
{
   if(mLockMouse)
      mouseUnlock();
// start jc
//   sendMouseEvent("onRightMouseUp", event);
   sendMouseEvent(OnRightMouseUp, "onRightMouseUp", event);
// end jc
}

void GuiMouseEventCtrl::onRightMouseDragged(const GuiEvent & event)
{
// start jc
//   sendMouseEvent("onRightMouseDragged", event);
   sendMouseEvent(OnRightMouseDragged, "onRightMouseDragged", event);
// end jc
}

// start jc
bool GuiMouseEventCtrl::onTouchDown(const GuiEvent & event)
{
   return sendTouchEvent(OnTouchDown, "onTouchDown", event);
}
bool GuiMouseEventCtrl::onTouchMove(const GuiEvent & event)
{
   return sendTouchEvent(OnTouchMove, "onTouchMove", event);
}
bool GuiMouseEventCtrl::onTouchUp(const GuiEvent & event)
{
   return sendTouchEvent(OnTouchUp, "onTouchUp", event);
}

bool GuiMouseEventCtrl::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   if ( !Parent::resize( newPosition, newExtent ) )
      return false;
   return true;
}


ConsoleMethod( GuiMouseEventCtrl, mouseLock, void, 2, 2,
   "mouseLock() Lock mouse input to this control.\n" )
{
   object->mouseLock();
}

ConsoleMethod( GuiMouseEventCtrl, mouseUnlock, void, 2, 2,
   "mouseUnlock() Unlock mouse input from this control.\n" )
{
   object->mouseUnlock();
}

ConsoleMethod( GuiMouseEventCtrl, isMouseLocked, bool, 2, 2,
   "isMouseLocked() is mouse input locked to this control.\n" )
{
   return object->isMouseLocked();
}

ConsoleMethod( GuiMouseEventCtrl, touchIDLock, void, 3, 3,
   "touchIDLock(U32 touchID) Lock mouse input to this control.\n" )
{
   object->touchIDLock(dAtoi(argv[2]));
}

ConsoleMethod( GuiMouseEventCtrl, touchIDUnlock, void, 3, 3,
   "touchIDUnlock(U32 touchID) Unlock mouse input from this control.\n" )
{
   object->touchIDUnlock(dAtoi(argv[2]));
}
ConsoleMethod( GuiMouseEventCtrl, isTouchIDLocked, bool, 3, 3,
   "isTouchIDLocked(U32 touchID) is mouse input locked to this control.\n" )
{
   return object->isTouchIDLocked(dAtoi(argv[2]));
}

// end jc