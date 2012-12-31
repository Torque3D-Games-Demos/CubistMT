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

#ifndef _GUIMOUSEEVENTCTRL_H_
#define _GUIMOUSEEVENTCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif
#ifndef _EVENT_H_
#include "platform/event.h"
#endif


class GuiMouseEventCtrl : public GuiControl
{
   private:
      typedef  GuiControl     Parent;
// start jc
//      void sendMouseEvent(const char * name, const GuiEvent &);
      enum EventId {
         OnMouseDown,
         OnMouseUp,
         OnMouseMove,
         OnMouseDragged,
         OnMouseEnter,
         OnMouseLeave,
         OnRightMouseDown,
         OnRightMouseUp,
         OnRightMouseDragged,
         OnTouchDown,
         OnTouchMove,
         OnTouchUp
      };
      void sendMouseEvent(const EventId id, const char * name, const GuiEvent &);
      bool sendTouchEvent(const EventId id, const char * name, const GuiEvent &);
// end jc


      // field info
      bool        mLockMouse;

   public:

      GuiMouseEventCtrl();

	  DECLARE_CALLBACK( void, onMouseDown, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseUp, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseMove, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseDragged, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseEnter, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseLeave, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
	  DECLARE_CALLBACK( void, onRightMouseDown, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
	  DECLARE_CALLBACK( void, onRightMouseUp, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
	  DECLARE_CALLBACK( void, onRightMouseDragged, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
// start jc
	  DECLARE_CALLBACK( void, onTouchDown, ( S32 id, Point2I touchPoint ));
	  DECLARE_CALLBACK( void, onTouchMove, ( S32 id, Point2I touchPoint ));
	  DECLARE_CALLBACK( void, onTouchUp, ( S32 id, Point2I touchPoint ));

	  DECLARE_CALLBACK( void, childResized, ( S32 child ));
// end jc

      // GuiControl
      void onMouseDown(const GuiEvent & event);
      void onMouseUp(const GuiEvent & event);
      void onMouseMove(const GuiEvent & event);
      void onMouseDragged(const GuiEvent & event);
      void onMouseEnter(const GuiEvent & event);
      void onMouseLeave(const GuiEvent & event);
      void onRightMouseDown(const GuiEvent & event);
      void onRightMouseUp(const GuiEvent & event);
      void onRightMouseDragged(const GuiEvent & event);
// start jc
      bool onTouchDown(const GuiEvent & event);
      bool onTouchMove(const GuiEvent & event);
      bool onTouchUp(const GuiEvent & event);

      bool resize( const Point2I &newPosition, const Point2I &newExtent );
      void childResized( GuiControl* child );
// end jc
      static void initPersistFields();

      DECLARE_CONOBJECT( GuiMouseEventCtrl );
      DECLARE_CATEGORY( "Gui Other Script" );
      DECLARE_DESCRIPTION( "A control that relays all mouse events to script." );
};

#endif
