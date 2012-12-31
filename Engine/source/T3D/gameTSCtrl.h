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

#ifndef _GAMETSCTRL_H_
#define _GAMETSCTRL_H_

#ifndef _GAME_H_
#include "app/game.h"
#endif
#ifndef _GUITSCONTROL_H_
#include "gui/3d/guiTSControl.h"
#endif

#ifdef TORQUE_DEMO_WATERMARK
#ifndef _WATERMARK_H_
#include "demo/watermark/watermark.h"
#endif
#endif

// start jc
#ifndef _SIMTUIO_H_
#include "console/simTUIO.h"
#endif
// end jc

class ProjectileData;
class GameBase;

//----------------------------------------------------------------------------
class GameTSCtrl : public GuiTSCtrl
{
private:
   typedef GuiTSCtrl Parent;

#ifdef TORQUE_DEMO_WATERMARK
   Watermark mWatermark;
#endif

   void makeScriptCall(const char *func, const GuiEvent &evt) const;

public:
   GameTSCtrl();

   DECLARE_CONOBJECT(GameTSCtrl);
   DECLARE_DESCRIPTION( "A control that renders a 3D view from the current control object." );

   bool processCameraQuery(CameraQuery *query);
   void renderWorld(const RectI &updateRect);

   // GuiControl
   virtual void onMouseDown(const GuiEvent &evt);
   virtual void onRightMouseDown(const GuiEvent &evt);
   virtual void onMiddleMouseDown(const GuiEvent &evt);

   virtual void onMouseUp(const GuiEvent &evt);
   virtual void onRightMouseUp(const GuiEvent &evt);
   virtual void onMiddleMouseUp(const GuiEvent &evt);

   void onMouseMove(const GuiEvent &evt);
   void onRender(Point2I offset, const RectI &updateRect);

   virtual bool onAdd();
};

// start jc

//----------------------------------------------------------------------------
//struct Gui3DMouseEvent;
//struct Gui3DTouchEvent;
//struct ShapeInputEvent;
//struct ShapeTouchEvent;
//struct GuiTouchEvent;

class TouchTSCtrl : public GameTSCtrl
{
private:
   typedef GameTSCtrl Parent;

   SimObjectPtr<SceneObject>   mMouseCapturedControl;  ///< All mouse events will go to this ctrl only
   SimObjectPtr<SceneObject>   mMouseControl;          ///< the control the mouse was last seen in unless some other one captured it
   HashTable<U32, SceneObject*> mTouchCapturedControl;

   bool _findInputEventObject(const Gui3DMouseEvent &event, ShapeInputEvent &ri);
   bool _findTouchEventObject(const Gui3DTouchEvent &event, ShapeTouchEvent &ri);
// start jc
   bool _findInputEventObjectHitPoint(SceneObject *obj, const Gui3DMouseEvent &event, ShapeInputEvent &objEvent);
   bool _findTouchEventObjectHitPoint(SceneObject *obj, const Gui3DTouchEvent &event, ShapeTouchEvent &objEvent);
// end jc
// start pg
   void removeDeadObjects();
// end pg

public:
   TouchTSCtrl();

   DECLARE_CONOBJECT(TouchTSCtrl);
   DECLARE_DESCRIPTION( "A control that renders a 3D view from the current control object." );

      Gui3DMouseEvent   mLastEvent;
      Gui3DTouchEvent   mLastTouchEvent;
      bool              mLeftMouseDown;
      bool              mRightMouseDown;
      bool              mMiddleMouseDown;
      bool              mMouseLeft;

      bool              mEnable3DInputEvents;
      F32               m3DInputEventRange;

      Point2I  mLastMousePos;
      bool     mLastMouseClamping;

      void make3DMouseEvent(Gui3DMouseEvent & gui3Devent, const GuiEvent &event);
      void make3DTouchEvent(Gui3DTouchEvent & gui3Devent, const GuiTouchEvent &event);
      bool getCameraTransform(MatrixF* cameraMatrix);

     DECLARE_CALLBACK( void, onMouseDown, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseUp, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseMove, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseDragged, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseEnter, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
  	  DECLARE_CALLBACK( void, onMouseLeave, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
	  DECLARE_CALLBACK( void, onRightMouseDown, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
	  DECLARE_CALLBACK( void, onRightMouseUp, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
	  DECLARE_CALLBACK( void, onRightMouseDragged, ( U8 modifier, Point2I mousePoint,U8 mouseClickCount ));
	  DECLARE_CALLBACK( void, onTouchDown, ( S32 id, Point2I touchPoint ));
	  DECLARE_CALLBACK( void, onTouchMove, ( S32 id, Point2I touchPoint ));
	  DECLARE_CALLBACK( void, onTouchUp, ( S32 id, Point2I touchPoint ));


      virtual void onMouseUp(const GuiEvent & event);
      virtual void onMouseDown(const GuiEvent & event);
      virtual void onMouseMove(const GuiEvent & event);
      virtual void onMouseDragged(const GuiEvent & event);
      virtual void onMouseEnter(const GuiEvent & event);
      virtual void onMouseLeave(const GuiEvent & event);
      virtual void onRightMouseDown(const GuiEvent & event);
      virtual void onRightMouseUp(const GuiEvent & event);
      virtual void onRightMouseDragged(const GuiEvent & event);
      virtual bool onMouseWheelUp(const GuiEvent & event);
      virtual bool onMouseWheelDown(const GuiEvent & event);

      virtual bool onTouchUp(const GuiTouchEvent & event);
      virtual bool onTouchDown(const GuiTouchEvent & event);
      virtual bool onTouchMove(const GuiTouchEvent & event);

      virtual void on3DMouseUp(const Gui3DMouseEvent &);
      virtual void on3DMouseDown(const Gui3DMouseEvent &);
      virtual void on3DMouseMove(const Gui3DMouseEvent &);
      virtual void on3DMouseDragged(const Gui3DMouseEvent &);
      virtual void on3DMouseEnter(const Gui3DMouseEvent &);
      virtual void on3DMouseLeave(const Gui3DMouseEvent &);
      virtual void on3DRightMouseDown(const Gui3DMouseEvent &);
      virtual void on3DRightMouseUp(const Gui3DMouseEvent &);
      virtual void on3DRightMouseDragged(const Gui3DMouseEvent &);
      virtual void on3DMouseWheelUp(const Gui3DMouseEvent &);
      virtual void on3DMouseWheelDown(const Gui3DMouseEvent &);

      virtual bool on3DTouchUp(const Gui3DTouchEvent &);
      virtual bool on3DTouchDown(const Gui3DTouchEvent &);
      virtual bool on3DTouchMove(const Gui3DTouchEvent &);
  //    virtual void get3DCursor(GuiCursor *&cursor, bool &visible, const Gui3DMouseEvent &);
   //   void lockMouse(SceneObject* object);
   //   void lockTouchID(U32 touchID, SceneObject* object);
   //   void unlockMouse(SceneObject* object);
   //   void unlockTouchID(U32 touchID);

      virtual void mouseLock(SceneObject *lockingControl);
      virtual void mouseUnlock(SceneObject *lockingControl);
      virtual SceneObject* getMouseControl()       { return mMouseControl; }
      virtual SceneObject* getMouseLockedControl() { return mMouseCapturedControl; }
      virtual void touchIDLock(U32 touchID, SceneObject *lockingControl);
      virtual void touchIDUnlock(U32 touchID, SceneObject *lockingControl);
      virtual void TouchTSCtrl::touchIDUnlockAll(SceneObject *lockingControl);

      virtual SceneObject* getTouchIDLockedControl(U32 touchID)
      {
         SceneObject* obj;
         if(mTouchCapturedControl.find(touchID, obj))
            return obj;
         return NULL;
      }

      bool processCameraQuery(CameraQuery * query);

      static Point3F    smCamPos;
      static MatrixF    smCamMatrix;
      static bool       smCamOrtho;
      static F32        smCamNearPlane;
      static F32        smVisibleDistanceScale;

};
// end jc

#endif
