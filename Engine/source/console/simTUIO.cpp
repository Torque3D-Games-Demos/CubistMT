//-----------------------------------------------------------------------------
// Torque 3D 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//#define   DEBUG_TOUCH

//#include "tinyxml/tinyxml.h"
//#define _XTREE_
//#define _MAP_

#include "console/simTUIO.h"

//-----------------------------------------------------------------------------
// Console implementation of STL map.
//-----------------------------------------------------------------------------

#include "core/frameAllocator.h"
#include "console/simBase.h"
#include "console/consoleInternal.h"
#include "console/SimXMLDocument.h"
#include "console/engineAPI.h"

#include "TUIO/TuioClient.h"
#include "gui/core/guiCanvas.h"

#include "core/util/journal/process.h"
#include "T3D/gameBase/gameConnection.h"
#include "core/stream/bitStream.h"
#include "math/mathIO.h"
#include "math/mTransform.h"

//using namespace TUIO;

//class TuioClient;
//class TuioListener;

#include "T3D/ShapeBase.h"
#include "T3D/RigidShape.h"





class ShapeBaseTransformEvent : public NetEvent
{
	SimObjectPtr<ShapeBase> mObject;
	TransformF mTransform;
	Point3F mScale;
	Point3F mVelocity;
	bool mReletive;

public:
   typedef NetEvent Parent;
   ShapeBaseTransformEvent() : mObject(), mTransform(), mScale(1.0f,1.0f,1.0f), mReletive(false) {}
   ShapeBaseTransformEvent(ShapeBase* object, const TransformF& transform, const Point3F& scale = Point3F(1.0f), const Point3F& velocity = Point3F::Zero, const bool reletive = false) :
			mObject(object), mTransform(transform), mScale(scale), mVelocity(velocity), mReletive(reletive)
   {
	//   if(message)
	//      msg = dStrdup(message);
	//   else
	//      msg = NULL;

   }

   ~ShapeBaseTransformEvent()
   {
	//   dFree(msg);
   }

   virtual void pack(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mObject) ? con->getGhostIndex(mObject): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

	//   if(bstream->writeFlag(!(mTransform.isIdentity())))
    //      mathWrite(*bstream,mTransform);
       mathWrite(*bstream,mTransform);

	   if(bstream->writeFlag(mScale.x == mScale.y && mScale.z == mScale.x))
          bstream->write(mScale.x);
	   else
          mathWrite(*bstream,mScale);

	   if(bstream->writeFlag(!mVelocity.isZero()))
		   mathWrite(*bstream,mVelocity);

	   bstream->write(mReletive);
   }
   virtual void write(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mObject) ? con->getGhostIndex(mObject): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

	//   if(bstream->writeFlag(!(mTransform.isIdentity())))
       mathWrite(*bstream,mTransform);

	   if(bstream->writeFlag((mScale.x == mScale.y && mScale.z == mScale.x)))
          bstream->write(mScale.x);
	   else
          mathWrite(*bstream,mScale);

	   if(bstream->writeFlag(!mVelocity.isZero()))
		   mathWrite(*bstream,mVelocity);

	   bstream->write(mReletive);
   }
   virtual void unpack(NetConnection* con, BitStream *bstream)
   {
       S32 gIndex = bstream->readInt(NetConnection::GhostIdBitSize);
	   if(con->isGhosting())
			mObject = static_cast<ShapeBase*>(con->resolveObjectFromGhostIndex(gIndex));

   //    if(bstream->readFlag())
	//	  mathRead(*bstream,&mTransform);
	//   else
    //      mTransform.identity();
		mathRead(*bstream,&mTransform);

	   if(bstream->readFlag())
	   {
          bstream->read(&(mScale.x));
		  mScale.y = mScale.x;
		  mScale.z = mScale.x;
	   }
	   else
          mathRead(*bstream,&mScale);

	   if(bstream->readFlag())
		   mathRead(*bstream,&mVelocity);
	   else
		   mVelocity = Point3F::Zero;

	   bstream->read(&mReletive);
   }
   virtual void process(NetConnection *con)
   {
	//   Con::printf("WebViewDataURLEvent: %s, %s, %s;", mWebViewData->getName(), mURL, mFrame);
	   if(mObject.isValid())
	   {
		   Con::printf("ShapeBaseTransformEvent::process: %f, %f, %f", mObject->getPosition().x, mObject->getPosition().y, mObject->getPosition().z);
		   Con::printf("ShapeBaseTransformEvent::process2: %f, %f, %f", mTransform.getPosition().x, mTransform.getPosition().y, mTransform.getPosition().z);
		   if(mReletive)
		   {
			   if(mScale.x!=1.0f || mScale.y!=1.0f || mScale.z!=1.0f)
			   {
				  Point3F scale = mObject->getScale() * mScale;
				  mObject->setScale(scale);
			   }
			//   if(!(mTransform.isIdentity()))
			//   {
				//  MatrixF mat = mTransform;
				//  mat.mul(mObject->getTransform());

				//  mObject->setTransform(mat);
				   mObject->setPosition(mObject->getPosition() + mTransform.getPosition());
				   if(mTransform.hasRotation())
				   {
					  MatrixF mat(true);
					  mTransform.getOrientation().setMatrix(&mat);
					  mat.mulL(mObject->getTransform());
                      mObject->setTransform(mat);
				   }
			//   }
		   }
		   else
		   {
			   if(mObject->getScale() != mScale)
                  mObject->setScale(mScale);
			//   if(!mTransform.hasRotation())
			//   {
			//      Point3F pos = mTransform.getPosition();
			//      mTransform.set(mObject->getTransform());
			//	  mTransform.mPosition = pos;
			//   }
			//   mObject->setTransform(mTransform.getMatrix());
			   if(mTransform.hasRotation())
			      mObject->setTransform(mTransform.getMatrix());
			   else
			      mObject->setPosition(mTransform.getPosition());
		   }

		   if(!mVelocity.isZero())
		   {
			//   mObject->setVelocity(mVelocity);
			   RigidShape* rigid = static_cast<RigidShape*>(mObject.getPointer());
			   if(rigid)
                  rigid->reset();
			   mObject->applyImpulse(mObject->getWorldBox().getCenter(), mVelocity);
		   }
	   }
   }

   DECLARE_CONOBJECT(ShapeBaseTransformEvent);
};

IMPLEMENT_CO_NETEVENT_V1(ShapeBaseTransformEvent);

ConsoleDocClass( ShapeBaseTransformEvent,
	"@brief A.\n\n");


DefineEngineStaticMethod( ShapeBaseTransformEvent, translate, void, (NetConnection* con, ShapeBase* object, TransformF transform, Point3F scale, Point3F velocity, bool reletive),(NULL,NULL,TransformF::Identity,Point3F::One,Point3F::Zero,true),
   "@brief Send a SceneObjectTransformEvent message to the specified connection.\n\n")
{
	//NetConnection *con = (NetConnection *) Sim::findObject(argv[1]);

	if(con && object)
		con->postNetEvent(new ShapeBaseTransformEvent(object, transform, scale, velocity, reletive));
}




class Swizzle_TouchUpEvent : public NetEvent
{
	SimObjectPtr<ShapeBase> mObject;
	TransformF mTransform;
	Point3F mScale;
	Point3F mVelocity;
	bool mReletive;

public:
   typedef NetEvent Parent;
   Swizzle_TouchUpEvent() : mObject(), mTransform(), mScale(1.0f,1.0f,1.0f), mReletive(false) {}
   Swizzle_TouchUpEvent(ShapeBase* object, const TransformF& transform, const Point3F& scale = Point3F(1.0f), const Point3F& velocity = Point3F::Zero, const bool reletive = false) :
			mObject(object), mTransform(transform), mScale(scale), mVelocity(velocity), mReletive(reletive)
   {
   }

   ~Swizzle_TouchUpEvent()
   {
   }

   virtual void pack(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mObject) ? con->getGhostIndex(mObject): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

       mathWrite(*bstream,mTransform);

	   if(bstream->writeFlag(mScale.x == mScale.y && mScale.z == mScale.x))
          bstream->write(mScale.x);
	   else
          mathWrite(*bstream,mScale);

	   if(bstream->writeFlag(!mVelocity.isZero()))
		   mathWrite(*bstream,mVelocity);

	   bstream->write(mReletive);
   }
   virtual void write(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mObject) ? con->getGhostIndex(mObject): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

       mathWrite(*bstream,mTransform);

	   if(bstream->writeFlag((mScale.x == mScale.y && mScale.z == mScale.x)))
          bstream->write(mScale.x);
	   else
          mathWrite(*bstream,mScale);

	   if(bstream->writeFlag(!mVelocity.isZero()))
		   mathWrite(*bstream,mVelocity);

	   bstream->write(mReletive);
   }
   virtual void unpack(NetConnection* con, BitStream *bstream)
   {
       S32 gIndex = bstream->readInt(NetConnection::GhostIdBitSize);
	   if(con->isGhosting())
			mObject = static_cast<ShapeBase*>(con->resolveObjectFromGhostIndex(gIndex));

		mathRead(*bstream,&mTransform);

	   if(bstream->readFlag())
	   {
          bstream->read(&(mScale.x));
		  mScale.y = mScale.x;
		  mScale.z = mScale.x;
	   }
	   else
          mathRead(*bstream,&mScale);

	   if(bstream->readFlag())
		   mathRead(*bstream,&mVelocity);
	   else
		   mVelocity = Point3F::Zero;

	   bstream->read(&mReletive);
   }
   virtual void process(NetConnection *con)
   {
	   if(mObject.isValid())
	   {
		   Con::printf("ShapeBaseTransformEvent::process: %f, %f, %f", mObject->getPosition().x, mObject->getPosition().y, mObject->getPosition().z);
		   Con::printf("ShapeBaseTransformEvent::process2: %f, %f, %f", mTransform.getPosition().x, mTransform.getPosition().y, mTransform.getPosition().z);
		   if(mReletive)
		   {
			   if(mScale.x!=1.0f || mScale.y!=1.0f || mScale.z!=1.0f)
			   {
				  Point3F scale = mObject->getScale() * mScale;
				  mObject->setScale(scale);
			   }
				   mObject->setPosition(mObject->getPosition() + mTransform.getPosition());
				   if(mTransform.hasRotation())
				   {
					  MatrixF mat(true);
					  mTransform.getOrientation().setMatrix(&mat);
					  mat.mulL(mObject->getTransform());
                      mObject->setTransform(mat);
				   }
		   }
		   else
		   {
			   if(mObject->getScale() != mScale)
                  mObject->setScale(mScale);

			   if(mTransform.hasRotation())
			      mObject->setTransform(mTransform.getMatrix());
			   else
			      mObject->setPosition(mTransform.getPosition());
		   }

		   if(!mVelocity.isZero())
		   {
			   RigidShape* rigid = static_cast<RigidShape*>(mObject.getPointer());
			   if(rigid)
                  rigid->reset();
			   mObject->applyImpulse(mObject->getWorldBox().getCenter(), mVelocity);
		   }
	   }
   }

   DECLARE_CONOBJECT(Swizzle_TouchUpEvent);
};

/*
IMPLEMENT_CO_NETEVENT_V1(Swizzle_TouchUpEvent);

ConsoleDocClass( Swizzle_TouchUpEvent,
	"@brief A.\n\n");


DefineEngineStaticMethod( Swizzle_TouchUpEvent, set, void, (NetConnection* con, ShapeBase* object, TransformF transform, Point3F scale, Point3F velocity, bool reletive),(NULL,NULL,TransformF::Identity,Point3F::One,Point3F::Zero,true),
   "@brief Send a SceneObjectTransformEvent message to the specified connection.\n\n")
{
	//NetConnection *con = (NetConnection *) Sim::findObject(argv[1]);

	if(con && object)
		con->postNetEvent(new Swizzle_TouchUpEvent(object, transform, scale, velocity, reletive));
}

class Swizzle_TouchMoveEvent : public NetEvent
{
	SimObjectPtr<ShapeBase> mObject;
	Point3F mTouchPosition;
	TransformF mTransform;
	Point3F mScale;
	Point3F mVelocity;

public:
   typedef NetEvent Parent;
   Swizzle_TouchMoveEvent() : mObject(), mTouchPosition(), mTransform(), mScale(1.0f,1.0f,1.0f), mReletive(false) {}
   Swizzle_TouchMoveEvent(ShapeBase* object, const Point3F& touchPosition = Point3F::Zero, const TransformF& transform = TransformF::Identity, const Point3F& scale = Point3F(1.0f), const Point3F& velocity = Point3F::Zero) :
			mObject(object), mTouchPosition(touchPosition), mTransform(transform), mScale(scale), mVelocity(velocity)
   {
   }

   ~Swizzle_TouchMoveEvent()
   {
   }

   virtual void pack(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mObject) ? con->getGhostIndex(mObject): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

       mathWrite(*bstream,mTouchPosition);
       mathWrite(*bstream,mTransform);

	   if(bstream->writeFlag(mScale.x == mScale.y && mScale.z == mScale.x))
          bstream->write(mScale.x);
	   else
          mathWrite(*bstream,mScale);

	   if(bstream->writeFlag(!mVelocity.isZero()))
		   mathWrite(*bstream,mVelocity);
   }
   virtual void write(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mObject) ? con->getGhostIndex(mObject): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

       mathWrite(*bstream,mTouchPosition);
       mathWrite(*bstream,mTransform);

	   if(bstream->writeFlag((mScale.x == mScale.y && mScale.z == mScale.x)))
          bstream->write(mScale.x);
	   else
          mathWrite(*bstream,mScale);

	   if(bstream->writeFlag(!mVelocity.isZero()))
		   mathWrite(*bstream,mVelocity);
   }
   virtual void unpack(NetConnection* con, BitStream *bstream)
   {
       S32 gIndex = bstream->readInt(NetConnection::GhostIdBitSize);
	   if(con->isGhosting())
			mObject = static_cast<ShapeBase*>(con->resolveObjectFromGhostIndex(gIndex));

		mathRead(*bstream,&mTouchPosition);
		mathRead(*bstream,&mTransform);

	   if(bstream->readFlag())
	   {
          bstream->read(&(mScale.x));
		  mScale.y = mScale.x;
		  mScale.z = mScale.x;
	   }
	   else
          mathRead(*bstream,&mScale);

	   if(bstream->readFlag())
		   mathRead(*bstream,&mVelocity);
	   else
		   mVelocity = Point3F::Zero;
   }
   virtual void process(NetConnection *con)
   {
	   if(mObject.isValid())
	   {
		//   Con::printf("ShapeBaseTransformEvent::process: %f, %f, %f", mObject->getPosition().x, mObject->getPosition().y, mObject->getPosition().z);
		//   Con::printf("ShapeBaseTransformEvent::process2: %f, %f, %f", mTransform.getPosition().x, mTransform.getPosition().y, mTransform.getPosition().z);
		   
		   if(mObject->getScale() != mScale)
              mObject->setScale(mScale);

		   if(mTransform.hasRotation())
		      mObject->setTransform(mTransform.getMatrix());
		   else
		      mObject->setPosition(mTransform.getPosition());
		  

		   if(!mVelocity.isZero())
		   {
			   RigidShape* rigid = static_cast<RigidShape*>(mObject.getPointer());
			   if(rigid)
                  rigid->reset();
			   mObject->applyImpulse(mObject->getWorldBox().getCenter(), mVelocity);
		   }
	   }
   }

   DECLARE_CONOBJECT(Swizzle_TouchMoveEvent);
};

IMPLEMENT_CO_NETEVENT_V1(Swizzle_TouchMoveEvent);

ConsoleDocClass( Swizzle_TouchMoveEvent,
	"@brief A.\n\n");


DefineEngineStaticMethod( Swizzle_TouchMoveEvent, set, void, (NetConnection* con, ShapeBase* object, Point3F touchPoint, TransformF transform, Point3F scale, Point3F velocity),(NULL,NULL,TransformF::Identity,Point3F::One,Point3F::Zero),
   "@brief Send a SceneObjectTransformEvent message to the specified connection.\n\n")
{
	//NetConnection *con = (NetConnection *) Sim::findObject(argv[1]);

	if(con && object)
		con->postNetEvent(new Swizzle_TouchMoveEvent(object, touchPoint, transform, scale, velocity));
}


*/



class guiTuioListener : public TUIO::TuioListener
{ 
	
   class TouchEvent
   {
   public:
      U64 sessionId;
      S32 touchId;
      S32 x;
      S32 y;
      U32 eventType;
      TouchEvent() : sessionId(0), touchId(0), x(0), y(0), eventType(0) {}
      TouchEvent( U64 sessionId, U32 touchId, S32 x, S32 y, U32 eventType ) : touchId(touchId), x(x), y(y), eventType(eventType) {}
      void set( U64 psessionId, U32 ptouchId, S32 px, S32 py, U32 peventType ) { sessionId = psessionId; touchId = ptouchId; x = px; y = py; eventType = peventType; }
   };
//   bool  mTouchCapturedMouse;
//   U32   mTouchCapturedMouseId;

public:
	//guiTuioListener(int port, SimObjectRef<GuiCanvas> canvas);
	guiTuioListener(SimTUIO* simTUIO);
   
	~guiTuioListener() {
		mClient->disconnect();
		delete mClient;
	}
	
	void addTuioObject(TUIO::TuioObject *tobj);
	void updateTuioObject(TUIO::TuioObject *tobj);
	void removeTuioObject(TUIO::TuioObject *tobj);
	
	void addTuioCursor(TUIO::TuioCursor *tcur);
	void updateTuioCursor(TUIO::TuioCursor *tcur);
	void removeTuioCursor(TUIO::TuioCursor *tcur);
	
	void refresh(TUIO::TuioTime frameTime);
	void process();

   SimObjectRef<GuiCanvas> getCanvas() { return mSimTUIO->getCanvas(); }
//	void run();
	
private:
//	void drawObjects();
//	void drawString(char *str);
//	void processEvents();
//	void toggleFullscreen();
//	SDL_Surface *window;
//	bool verbose, fullscreen, running;
	
//	int width, height;
//	int screen_width, screen_height;
//	int window_width, window_height;
   
   TUIO::TuioClient* mClient;
 //  SimObjectRef<GuiCanvas> mCanvas;
   SimTUIO* mSimTUIO;
   GuiTouchEvent mGuiTouchEvent;
   //Vector<TouchEvent*> mEvents;
   //Map<S32,TouchEvent*> mMoveEvents;
   Vector<TouchEvent> mEvents;
//   Map<S32,Pair<long,S32>> mMoveEvents;
};

void guiTuioListener::addTuioObject(TUIO::TuioObject *tobj) {
//	if (verbose)
//		std::cout << "add obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << std::endl;

}

void guiTuioListener::updateTuioObject(TUIO::TuioObject *tobj) {

//	if (verbose) 	
//		std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() 
//		<< " " << tobj->getMotionSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;

}

void guiTuioListener::removeTuioObject(TUIO::TuioObject *tobj) {

//	if (verbose)
//		std::cout << "del obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << ")" << std::endl;
}

void guiTuioListener::addTuioCursor(TUIO::TuioCursor *tcur) {
	
//	if (verbose) 
//		std::cout << "add cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() << std::endl;
   if(mSimTUIO->getCanvas())
   {
//      mGuiTouchEvent.cursor = tcur;
//      mGuiTouchEvent.touchId = tcur->getCursorID();
//      mGuiTouchEvent.touchPosition = mCanvas->getLocalTouchPosition(tcur);
//   //   if(mCanvas->getBounds().pointInRect(mGuiTouchEvent.touchPosition))
//   //   {
//   //      cursorPt.x = getMax(0, getMin(cursorPt.x, bounds.extent.x - 1));
//   //      cursorPt.y = getMax(0, getMin(cursorPt.y, bounds.extent.y - 1));
//      mGuiTouchEvent.client->lockCursorList();
//         mCanvas->rootTouchDown(mGuiTouchEvent);
//      mGuiTouchEvent.client->unlockCursorList();
      Point2F tuioPoint(tcur->getX(),tcur->getY());
      if(mSimTUIO->getRotate() != 0.0f || mSimTUIO->getScale().x != 1.0f || mSimTUIO->getScale().y != 1.0f)
      {
         static Point2F center = Point2F(0.5, 0.5);
         tuioPoint -= center;
         tuioPoint.rotate(mDegToRad(mSimTUIO->getRotate()));
         tuioPoint *= mSimTUIO->getScale();
         tuioPoint += center;
      }
      tuioPoint += mSimTUIO->getPosition();

      Point2I point = mSimTUIO->getCanvas()->getLocalTouchPosition(tuioPoint);
      U32 id = mClamp(tcur->getCursorID() + mSimTUIO->mIdRange.x, mSimTUIO->mIdRange.x, mSimTUIO->mIdRange.y);

   //   PlatformWindow* win = mCanvas->getPlatformWindow();
//      Con::printf("addTuioCursor: %d, %d, %d, %d", tcur->getCursorID(), point.x, point.y, TOUCH_DOWN);
      //win->touchEvent.trigger(win->getWindowId(), tcur->getCursorID(), point.x, point.y, TOUCH_DOWN);
      //win->getWindowInputGenerator()->handleTouchEvent(win->getWindowId(), tcur->getCursorID(), point.x, point.y, TOUCH_DOWN);
//      mEvents.push_back(new TouchEvent(tcur->getSessionID(), id, point.x, point.y, TOUCH_DOWN));
   //   mMoveEvents.insertUnique(id, TouchEvent(id, point.x, point.y, TOUCH_MOVE));
      //mMoveEvents[id].set(tcur->getSessionID(), id, point.x, point.y, TOUCH_MOVE);
//      mMoveEvents.insert(id, new TouchEvent(tcur->getSessionID(), id, point.x, point.y, TOUCH_MOVE));

      mEvents.push_back(TouchEvent(tcur->getSessionID(), id, point.x, point.y, TOUCH_DOWN));

   //   }
 //  Process::processEvents();


   }
}

void guiTuioListener::updateTuioCursor(TUIO::TuioCursor *tcur) {

   /*
//	if (verbose) 	
//		std::cout << "set cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() 
//					<< " " << tcur->getMotionSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;
   if(mSimTUIO->getCanvas())
   {
//      mGuiTouchEvent.cursor = tcur;
//      mGuiTouchEvent.touchId = tcur->getCursorID();
//      mGuiTouchEvent.touchPosition = mCanvas->getLocalTouchPosition(tcur);
//    //  if(mCanvas->getBounds().pointInRect(mGuiTouchEvent.touchPosition))
//      mGuiTouchEvent.client->lockCursorList();
//      mCanvas->rootTouchMove(mGuiTouchEvent);
//      mGuiTouchEvent.client->unlockCursorList();

      Point2F tuioPoint(tcur->getX(),tcur->getY());
      tuioPoint *= mSimTUIO->getScale();
      tuioPoint += mSimTUIO->getPosition();
      Point2I point = mSimTUIO->getCanvas()->getLocalTouchPosition(tuioPoint);
      U32 id = mClamp(tcur->getCursorID() + mSimTUIO->mIdRange.x, mSimTUIO->mIdRange.x, mSimTUIO->mIdRange.y);
  //    PlatformWindow* win = mCanvas->getPlatformWindow();
      Con::printf("updateTuioCursor: %d, %d, %d, %d", tcur->getCursorID(), point.x, point.y, TOUCH_MOVE);
      //win->touchEvent.trigger(win->getWindowId(), tcur->getCursorID(), point.x, point.y, TOUCH_MOVE);
      //win->getWindowInputGenerator()->handleTouchEvent(win->getWindowId(), tcur->getCursorID(), point.x, point.y, TOUCH_MOVE);

      // search for existing update event and update it. Only need one per tick
      // todo: translate all this into good code

 //     mMoveEvents.findOrInsert(id)->value.set(id, point.x, point.y, TOUCH_MOVE);
      Map<S32,TouchEvent*>::Iterator itr = mMoveEvents.find(id);
      if (itr != mMoveEvents.end())
      {
         if(!itr->value)
            Con::warnf("guiTuioListener::updateTuioCursor - Array of touch move has been corrupted!");
         else
            itr->value->set(tcur->getSessionID(), id, point.x, point.y, TOUCH_MOVE);
      }
      else
      {
         // doesn't always come thanks to UDP so handle it anyway
         Con::warnf("guiTuioListener::updateTuioCursor - Touch move with no touch down: faking it!");
         mEvents.push_back(new TouchEvent(tcur->getSessionID(), id, point.x, point.y, TOUCH_DOWN));
         mMoveEvents.insert(id, new TouchEvent(tcur->getSessionID(), id, point.x, point.y, TOUCH_MOVE));
      }
//      mEvents.push_back(TouchEvent(id, point.x, point.y, TOUCH_MOVE));

  //    mMoveEvents.insertUnique(id, TouchEvent(id, point.x, point.y, TOUCH_MOVE));

  //    mMoveEvents.push_back(tcur->getCursorID(), TouchEvent(tcur->getCursorID(), point.x, point.y, TOUCH_MOVE));
      
  // Process::processEvents();

   }
   */
}

void guiTuioListener::removeTuioCursor(TUIO::TuioCursor *tcur) {
	
//	if (verbose)
//		std::cout << "del cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ")" << std::endl;

   if(mSimTUIO->getCanvas())
   {
   //   mGuiTouchEvent.cursor = tcur;
   //   mGuiTouchEvent.touchId = tcur->getCursorID();
   //   mGuiTouchEvent.touchPosition = mCanvas->getLocalTouchPosition(tcur);
   ////   if(mCanvas->getBounds().pointInRect(mGuiTouchEvent.touchPosition))
   //   mGuiTouchEvent.client->lockCursorList();
   //      mCanvas->rootTouchUp(mGuiTouchEvent);
   //   mGuiTouchEvent.client->unlockCursorList();

      Point2F tuioPoint(tcur->getX(),tcur->getY());
      if(mSimTUIO->getRotate() != 0.0f || mSimTUIO->getScale().x != 1.0f || mSimTUIO->getScale().y != 1.0f)
      {
         static Point2F center = Point2F(0.5, 0.5);
         tuioPoint -= center;
         tuioPoint.rotate(mDegToRad(mSimTUIO->getRotate()));
         tuioPoint *= mSimTUIO->getScale();
         tuioPoint += center;
      }
      tuioPoint += mSimTUIO->getPosition();

      Point2I point = mSimTUIO->getCanvas()->getLocalTouchPosition(tuioPoint);
      U32 id = mClamp(tcur->getCursorID() + mSimTUIO->mIdRange.x, mSimTUIO->mIdRange.x, mSimTUIO->mIdRange.y);
  //    PlatformWindow* win = mCanvas->getPlatformWindow();
//      Con::printf("removeTuioCursor: %d, %d, %d, %d", tcur->getCursorID(), point.x, point.y, TOUCH_UP);
      //win->touchEvent.trigger(win->getWindowId(), tcur->getCursorID(), point.x, point.y, TOUCH_UP);
      //win->getWindowInputGenerator()->handleTouchEvent(win->getWindowId(), tcur->getCursorID(), point.x, point.y, TOUCH_UP);
      mEvents.push_back(TouchEvent(tcur->getSessionID(), id, point.x, point.y, TOUCH_UP));
      
 //  Process::processEvents();

   }
}

#ifdef   DEBUG_TOUCH
static   int   touchCount = 0;
#endif
void guiTuioListener::process()
{
//   if(mEvents.size() == 0)// && mMoveEvents.size() == 0)
//      return;

   PlatformWindow* win = mSimTUIO->getCanvas()->getPlatformWindow();
   if(win)
   {
      WindowInputGenerator* inputGen = win->getWindowInputGenerator();
      WindowId did = win->getWindowId();
      if(inputGen)
      {
         inputGen->touchWindowFocus();
         bool isMouseCompatable = bool(mClient->getTuioCursors().size() == 1);
         SimObjectRef<GuiCanvas> canvas = mSimTUIO->getCanvas();

         if(mEvents.size() != 0)
         {
#ifdef   DEBUG_TOUCH
            boolean  eventStart = false;
#endif
            for(Vector<TouchEvent>::iterator iter = mEvents.begin(); iter != mEvents.end(); iter++)
            {
               TouchEvent* i = (iter);
            //   mGuiTouchEvent.sessionId = i->sessionId;
               mGuiTouchEvent.touchId = i->touchId;
               mGuiTouchEvent.touchPosition.x = i->x;
               mGuiTouchEvent.touchPosition.y = i->y;

#ifdef   DEBUG_TOUCH
               //debug log for touch events
               switch(i->eventType)
               {
               case TOUCH_DOWN:
               case TOUCH_UP:
                  if(!eventStart){
                     eventStart = true;
                     Con::printf("--------------------------");
                  }
                  touchCount += (i->eventType==TOUCH_DOWN? +1 : -1);
                  Con::printf("%sTouch %d (%d)", i->eventType==TOUCH_DOWN?"+":"-",   i->touchId, touchCount);
                  if(i->eventType==TOUCH_DOWN && canvas->isLocked(i->touchId)){
                     Con::printf("     Event repeated");
                  }
                  break;
               }
#endif
               switch(i->eventType)
               {
               case TOUCH_DOWN:
                  if(!canvas->rootTouchDown(mGuiTouchEvent) && isMouseCompatable )
                  {
                     if(canvas->isTouchCapturedMouse())
                     {
                        canvas->clearTouchCapturedMouse();
                     }
                     else
                     {
                        inputGen->handleMouseMove(did, 0, i->x, i->y, 0);
                        canvas->setTouchCapturedMouse(i->touchId);
                        inputGen->handleMouseButton(did, 0, IA_MAKE, 0);
                        inputGen->handleMouseMove(did, 0, i->x, i->y, 0);
                     }
                  }
                  break;

            /*
               case TOUCH_MOVE:
                  if(!canvas->rootTouchMove(mGuiTouchEvent) && isMouseCompatable)
                  {
                     //Con::printf("guiTuioListener::process: TOUCH_MOVE, %d, %d, %d", i->touchId, i->x, i->y);
                     if(canvas->isTouchCapturedMouse() && canvas->getTouchCapturedMouse() == i->touchId)
                     {
                        inputGen->handleMouseMove(did, 0, i->x, i->y, 0);
                     }
                     else
                     {
                        inputGen->handleMouseButton(did, 0, IA_BREAK, 0);                   
                        inputGen->handleMouseMove(did, 0, i->x, i->y, 0);

                        canvas->setTouchCapturedMouse(i->touchId);
                        inputGen->handleMouseButton(did, 0, IA_MAKE, 0);
                        inputGen->handleMouseMove(did, 0, i->x, i->y, 0);

                     }

                  }
                  break;
               */

               case TOUCH_UP:
                  //flush unneeded move events
               //   mMoveEvents.erase(i->touchId);
                  /*
                  Map<S32,TouchEvent*>::Iterator iter = mMoveEvents.find(i->touchId);
                  if(iter != mMoveEvents.end())
                  {
                     if(iter->value)
                        delete (iter->value);
                     mMoveEvents.erase(iter);
                  }
                  */

                  if(!canvas->rootTouchUp(mGuiTouchEvent))
                  {
                     //Con::printf("guiTuioListener::process: TOUCH_UP, %d, %d, %d", i->touchId, i->x, i->y);
                     if(canvas->isTouchCapturedMouse() && canvas->getTouchCapturedMouse() == i->touchId)
                     {
                        inputGen->handleMouseMove(did, 0, i->x, i->y, 0);
                        inputGen->handleMouseButton(did, 0, IA_BREAK, 0);
                        canvas->clearTouchCapturedMouse();
                        inputGen->handleMouseMove(did, 0, i->x, i->y, 0);
                     }
                  }
                  break;
               }
            }
            mEvents.clear();
         }

         std::list<TUIO::TuioCursor*> cursors = mClient->getTuioCursors();
         for(std::list<TUIO::TuioCursor*>::iterator cursor = cursors.begin(); cursor != cursors.end(); cursor++)
         {

            Point2F tuioPoint((*cursor)->getX(),(*cursor)->getY());

		      if(mSimTUIO->getRotate() != 0.0f || mSimTUIO->getScale().x != 1.0f || mSimTUIO->getScale().y != 1.0f)
            {
               static Point2F center = Point2F(0.5, 0.5);
               tuioPoint -= center;
               tuioPoint.rotate(mDegToRad(mSimTUIO->getRotate()));
               tuioPoint *= mSimTUIO->getScale();
               tuioPoint += center;
            }
            tuioPoint += mSimTUIO->getPosition();

			   Point2I point = mSimTUIO->getCanvas()->getLocalTouchPosition(tuioPoint);
            U32 id = mClamp((*cursor)->getCursorID() + mSimTUIO->mIdRange.x, mSimTUIO->mIdRange.x, mSimTUIO->mIdRange.y);

            mGuiTouchEvent.touchId = id;
            mGuiTouchEvent.touchPosition.x = point.x;
            mGuiTouchEvent.touchPosition.y = point.y;
            
            if(!canvas->rootTouchMove(mGuiTouchEvent) && isMouseCompatable)
            {
               //Con::printf("guiTuioListener::process: TOUCH_MOVE, %d, %d, %d", i->touchId, i->x, i->y);
               if(canvas->isTouchCapturedMouse() && canvas->getTouchCapturedMouse() == id)
               {
                  inputGen->handleMouseMove(did, 0, point.x, point.y, 0);
               }
               else
               {
                  inputGen->handleMouseButton(did, 0, IA_BREAK, 0);                   
                  inputGen->handleMouseMove(did, 0, point.x, point.y, 0);

                  canvas->setTouchCapturedMouse(id);
                  inputGen->handleMouseButton(did, 0, IA_MAKE, 0);
                  inputGen->handleMouseMove(did, 0, point.x, point.y, 0);

               }

            }
         }

         /*
         if(mMoveEvents.size() != 0)
         {
            for(Map<S32,TouchEvent*>::Iterator i = mMoveEvents.begin(); i != mMoveEvents.end(); i++)
            {
               TouchEvent& touch = *(i->value);
   //            mGuiTouchEvent.sessionId = touch.sessionId;
               mGuiTouchEvent.touchId = touch.touchId;
               mGuiTouchEvent.touchPosition.x = touch.x;
               mGuiTouchEvent.touchPosition.y = touch.y;

               switch(touch.eventType)
               {
               case TOUCH_MOVE:
                  if(!canvas->rootTouchMove(mGuiTouchEvent) && isMouseCompatable)
                  {
                     //Con::printf("guiTuioListener::process: TOUCH_MOVE, %d, %d, %d", i->touchId, i->x, i->y);
                     if(canvas->isTouchCapturedMouse() && canvas->getTouchCapturedMouse() == touch.touchId)
                     {
                        inputGen->handleMouseMove(did, 0, touch.x, touch.y, 0);
                     }
                     else
                     {
                        inputGen->handleMouseButton(did, 0, IA_BREAK, 0);                   
                        inputGen->handleMouseMove(did, 0, touch.x, touch.y, 0);

                        canvas->setTouchCapturedMouse(touch.touchId);
                        inputGen->handleMouseButton(did, 0, IA_MAKE, 0);
                        inputGen->handleMouseMove(did, 0, touch.x, touch.y, 0);

                     }

                  }
                  break;  
               }
            }
            mMoveEvents.clear();
         }
         */
      }
   }
}


//#include <list>
//#include <map>
void guiTuioListener::refresh(TUIO::TuioTime frameTime)
{

   // sanity check our touch arrays and discard expired touch events
/*
   if(mMoveEvents.size() == 0)
      return;

   for(Map<S32,TouchEvent*>::Iterator i = mMoveEvents.begin(); i != mMoveEvents.end(); i++)
   {
      TouchEvent& touch = *(i->value);
      TUIO::TuioCursor* cursor = mClient->getTuioCursor(touch.sessionId);
      if(!cursor)
      {
         Con::warnf("guiTuioListener::refresh - Touch move left over with no touch up: faking it!");
         mEvents.push_back(TouchEvent(touch.sessionId, touch.touchId, touch.x, touch.y, TOUCH_UP));
         mMoveEvents.erase(touch.touchId);
      }
   }

*/


 //  StandardMainLoop::doMainLoop();
 //  Process::get()._signalProcess.trigger();
 //  Process::processEvents();

	//drawObjects();
   /*
   std::list<TUIO::TuioCursor*> cursorList = mClient->getTuioCursors();
   std::map<GuiControl*, std::list<TUIO::TuioCursor*>> controlsTouched;
	mClient->lockCursorList();
	for (std::list<TUIO::TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++)
   {
		TUIO::TuioCursor *tuioCursor = (*iter);
		
      GuiControl *controlHit = mCanvas->findHitControl(mCanvas->getLocalTouchPosition(tuioCursor));
     // controlsTouched.insert(std::make_pair(controlHit, tuioCursor));
      controlsTouched[controlHit].push_back(tuioCursor);
	}
	mClient->unlockCursorList();


	for (std::map<GuiControl*, std::list<TUIO::TuioCursor*>>::iterator iter = controlsTouched.begin(); iter!=controlsTouched.end(); iter++)
   {
      std::list<TUIO::TuioCursor*> tuioCursors = (*iter).second;
      if(tuioCursors.size() == 1)
      {
         // simulate mouse input
         mCanvas->rootMouseDown(mCanvas->mLastEvent);

      }
   }
   */
}
/*

void guiTuioListener::drawObjects() {
	glClear(GL_COLOR_BUFFER_BIT);
	char id[3];

	// draw the cursors
	std::list<TuioCursor*> cursorList = tuioClient->getTuioCursors();
	tuioClient->lockCursorList();
	for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {
		TuioCursor *tuioCursor = (*iter);
		std::list<TuioPoint> path = tuioCursor->getPath();
		if (path.size()>0) {

		TuioPoint last_point = path.front();
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 1.0);
		
		for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
			glVertex3f(last_point.getX()*width, last_point.getY()*height, 0.0f);
			glVertex3f(point->getX()*width, point->getY()*height, 0.0f);
			last_point.update(point->getX(),point->getY());

		} glEnd();

		// draw the finger tip
		glColor3f(0.75, 0.75, 0.75);
		glPushMatrix();
		glTranslatef(last_point.getX()*width, last_point.getY()*height, 0.0);
		glBegin(GL_TRIANGLE_FAN);
		for(double a = 0.0f; a <= 2*M_PI; a += 0.2f) {
			glVertex2d(cos(a) * height/100.0f, sin(a) * height/100.0f);
		} glEnd();
		glPopMatrix();
		
		glColor3f(0.0, 0.0, 0.0);
		glRasterPos2f(tuioCursor->getScreenX(width),tuioCursor->getScreenY(height));
		sprintf(id,"%d",tuioCursor->getCursorID());
		drawString(id);
		}
	}
	tuioClient->unlockCursorList();

	// draw the objects
	std::list<TuioObject*> objectList = tuioClient->getTuioObjects();
	tuioClient->lockObjectList();
	for (std::list<TuioObject*>::iterator iter = objectList.begin(); iter!=objectList.end(); iter++) {
		TuioObject* tuioObject = (*iter);
		int pos_size = height/20.0f;
		int neg_size = -1*pos_size;
		float xpos  = tuioObject->getX()*width;
		float ypos  = tuioObject->getY()*height;
		float angle = tuioObject->getAngle()/M_PI*180;
		
		glColor3f(0.0, 0.0, 0.0);
		glPushMatrix();
		glTranslatef(xpos, ypos, 0.0);
		glRotatef(angle, 0.0, 0.0, 1.0);
		glBegin(GL_QUADS);
			glVertex2f(neg_size, neg_size);
			glVertex2f(neg_size, pos_size);
			glVertex2f(pos_size, pos_size);
			glVertex2f(pos_size, neg_size);
		glEnd();
		glPopMatrix();

		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2f(xpos,ypos+5);
		sprintf(id,"%d",tuioObject->getSymbolID());
		drawString(id);
	}
	tuioClient->unlockObjectList();

	SDL_GL_SwapBuffers();
}

void guiTuioListener::drawString(char *str) {
	if (str && strlen(str)) {
		while (*str) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *str);
			str++;
		}
	}
}

void guiTuioListener::toggleFullscreen() {

        if( !window || SDL_WM_ToggleFullScreen(window)!=1 ) {
                std::cout << "Unable to toggle fullscreen: " << SDL_GetError() << std::endl;
        } else {
                fullscreen = !fullscreen;
                SDL_ShowCursor(!fullscreen);
        }
}

void guiTuioListener::processEvents()
{
    SDL_Event event;

    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
		case SDL_KEYDOWN:
			if( event.key.keysym.sym == SDLK_ESCAPE ){
				running = false;
				SDL_ShowCursor(true);
				SDL_Quit();
			} else if( event.key.keysym.sym == SDLK_F1 ){
				toggleFullscreen();
			} else if( event.key.keysym.sym == SDLK_v ){
				verbose = !verbose;	
			} 

			break;
		case SDL_QUIT:
			running = false;
			SDL_ShowCursor(true);
			SDL_Quit();
			break;
        }
    }
}
*/

guiTuioListener::guiTuioListener(SimTUIO* simTUIO)
            : mSimTUIO(simTUIO), mGuiTouchEvent(), mEvents(1024)//, mMoveEvents(), mTouchCapturedMouse(false), mTouchCapturedMouseId(0)
{


//	verbose = false;
//	fullscreen = false;
//	window_width = 640;
//	window_height = 480;
//	screen_width = 1024;
//	screen_height = 768;
//   mEvents.reserve(1024);
//   mMoveEvents.resize(1024);

	mClient = new TUIO::TuioClient(mSimTUIO->getPort());
	mClient->addTuioListener(this);
	mClient->connect();

	if (!mClient->isConnected()) {
	//	SDL_Quit();
	//	exit(1);
      return;
	}

   mGuiTouchEvent.client = mClient;
   mGuiTouchEvent.canvas = mSimTUIO->getCanvas();

      
   
	/*
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		std::cerr << "SDL could not be initialized: " <<  SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	
	int videoFlags = SDL_OPENGL | SDL_DOUBLEBUF;
	if( fullscreen ) {
		videoFlags |= SDL_FULLSCREEN;
		SDL_ShowCursor(false);
		width = screen_width;
		height = screen_height;
	} else {
		width = window_width;
		height = window_height;
	}
	
	window = SDL_SetVideoMode(width, height, 32, videoFlags);
	if ( window == NULL ) {
		std::cerr << "Could not open window: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();
	gluOrtho2D(0, (float)width,  (float)height, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SDL_WM_SetCaption("guiTuioListener", NULL);
   */
}
/*
void guiTuioListener::run() {
	running=true;
	while (running) {
		drawObjects();
		processEvents();
		SDL_Delay(40);
	} 
}

*/

IMPLEMENT_CONOBJECT(SimTUIO);

SimTUIO::SimTUIO():
         mPort(3333),
         mPosition(0.0f,0.0f),
         mScale(1.0f,1.0f),
         mRotate(0.0f),
         mIdRange(0, S32_MAX)
{
}
SimTUIO::~SimTUIO()
{
}

bool SimTUIO::processArguments(S32 argc, const char** argv)
{
   return argc == 0;
}
bool SimTUIO::onAdd()
{
   if(!Parent::onAdd())
   {
      return false;
   }

   mGuiTuioListener = new guiTuioListener(this);
   Process::notify(this,&SimTUIO::process, PROCESS_INPUT_ORDER);

   return true;
}
void SimTUIO::onRemove()
{
   Process::remove(this,&SimTUIO::process);

   if(mGuiTuioListener)
   {
      delete mGuiTuioListener;
      mGuiTuioListener = NULL;
   }

   Parent::onRemove();
   
}
void SimTUIO::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("TUIO");
   addField( "canvas", TYPEID< SimObjectRef<GuiCanvas> >(), Offset( mCanvas, SimTUIO ), "Canvas to call multitouch gui events on." );
   addField( "port", TypeS32, Offset( mPort, SimTUIO ), "Network socket port number." );
   addField( "position", TypePoint2F, Offset( mPosition, SimTUIO ), "Offset the incomming TUIO events." );
   addField( "scale", TypePoint2F, Offset( mScale, SimTUIO ), "Scale the incomming TUIO events." );
   addField( "rotate", TypeF32, Offset( mRotate, SimTUIO ), "Rotate the incomming TUIO events." );
   addField( "idRange", TypePoint2I, Offset( mIdRange, SimTUIO ), "frame the id values of the incomming TUIO events between the min and max range values." );
   endGroup("TUIO");

}

void SimTUIO::process()
{
   if(mGuiTuioListener)
      mGuiTuioListener->process();
}



// start jc
/*
Point2I MultiTouchEvent::ZeroPoint = Point2I(0,0);
MultiTouchEvent::MultiTouchEvent(): mPosition(ZeroPoint), mStartPosition(ZeroPoint)    {}


IMPLEMENT_CONOBJECT(TCPSocketStream);
//--------------------------------------

TCPSocketStream::TCPSocketStream()
{
   mBufferSave = 0;
}

TCPSocketStream::~TCPSocketStream()
{
   dFree(mBufferSave);
}
//--------------------------------------
/*
/*
void TCPSocketStream::onConnected()
{
   Parent::onConnected();
   char expPath[8192];
   char buffer[8192];

//   if(mQuery)
//   {
//      dSprintf(buffer, sizeof(buffer), "%s?%s", mPath, mQuery);
//      expandPath(expPath, buffer, sizeof(expPath));
//   }
//   else
//      expandPath(expPath, mPath, sizeof(expPath));

   char *pt = dStrchr(mHostName, ':');
   if(pt)
      *pt = 0;
   dSprintf(buffer, sizeof(buffer), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", expPath, mHostName);
   if(pt)
      *pt = ':';

   send((U8*)buffer, dStrlen(buffer));
   mParseState = ParsingStatusLine;
   mChunkedEncoding = false;
}
*/
/*
//--------------------------------------
bool TCPSocketStream::processLine(U8 *line)
{
   if(mParseState == ParsingStatusLine)
   {
      mParseState = ParsingHeader;
   }
   else if(mParseState == ParsingHeader)
   {
      if(!dStricmp((char *) line, "transfer-encoding: chunked"))
         mChunkedEncoding = true;
      if(line[0] == 0)
      {
         if(mChunkedEncoding)
            mParseState = ParsingChunkHeader;
         else
            mParseState = ProcessingBody;
         return true;
      }
   }
   else if(mParseState == ParsingChunkHeader)
   {
      if(line[0]) // strip off the crlf if necessary
      {
         mChunkSize = 0;
         S32 hexVal;
         while((hexVal = getHexVal(*line++)) != -1)
         {
            mChunkSize *= 16;
            mChunkSize += hexVal;
         }
         if(mBufferSave)
         {
            mBuffer = mBufferSave;
            mBufferSize = mBufferSaveSize;
            mBufferSave = 0;
         }
         if(mChunkSize)
            mParseState = ProcessingBody;
         else
         {
            mParseState = ProcessingDone;
            finishLastLine();
         }
      }
   }
   else
   {
      return Parent::processLine((UTF8*)line);
   }
   return true;
}

U32 TCPSocketStream::onDataReceive(U8 *buffer, U32 bufferLen)
{
   U32 start = 0;
   parseLine(buffer, &start, bufferLen);
   return start;
}

//--------------------------------------
U32 TCPSocketStream::onReceive(U8 *buffer, U32 bufferLen)
{
   if(mParseState == ProcessingBody)
   {
      if(mChunkedEncoding && bufferLen >= mChunkSize)
      {
         U32 ret = onDataReceive(buffer, mChunkSize);
         mChunkSize -= ret;
         if(mChunkSize == 0)
         {
            if(mBuffer)
            {
               mBufferSaveSize = mBufferSize;
               mBufferSave = mBuffer;
               mBuffer = 0;
               mBufferSize = 0;
            }
            mParseState = ParsingChunkHeader;
         }
         return ret;
      }
      else
      {
         U32 ret = onDataReceive(buffer, bufferLen);
         mChunkSize -= ret;
         return ret;
      }
   }
   else if(mParseState != ProcessingDone)
   {
      U32 start = 0;
      parseLine(buffer, &start, bufferLen);
      return start;
   }
   return bufferLen;
}

//--------------------------------------
void TCPSocketStream::connect(const char *address)
{
   mHostName = dStrdup(address);
   Parent::connect(mHostName);
}

void TCPSocketStream::onConnected()
{
   Parent::onConnected();
   char buffer[8192];

   char *pt = dStrchr(mHostName, ':');
   if(pt)
      *pt = 0;
  // dSprintf(buffer, sizeof(buffer), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", expPath, mHostName);
   dSprintf(buffer, sizeof(buffer), "<?xml version='1.0'?> <stream to='localhost:5502' xmlns='jabber:client'/>\r\n");
   
   if(pt)
      *pt = ':';

   send((U8*)buffer, dStrlen(buffer));
   mParseState = ParsingStatusLine;
   mChunkedEncoding = false;
}

void TCPSocketStream::onConnectFailed()
{
   dFree(mHostName);
   mHostName = 0;
   Parent::onConnectFailed();
}
void TCPSocketStream::onDisconnect()
{
   dFree(mHostName);
   mHostName = 0;
   Parent::onDisconnect();
}

//--------------------------------------


IMPLEMENT_CONOBJECT(SimMultiTouchXML);
ConsoleDocClass( SimMultiTouchXML,
				"@brief Class used for reciving XML multitouch events and translating them to local gui events\n\n");

SimMultiTouchXML::SimMultiTouchXML():
      mXMLDocument(NULL),
      mTCPSocketStream(NULL)
{
}
SimMultiTouchXML::~SimMultiTouchXML()
{
}

bool SimMultiTouchXML::processArguments(S32 argc, const char** argv)
{
   return argc == 0;
}
bool SimMultiTouchXML::onAdd()
{
   if(!Parent::onAdd())
   {
      return false;
   }

   if(!mTCPSocketStream)
   {
   //   mHTTPObject = new HTTPObject();
   //   mHTTPObject->setPort(5502);
   //   mHTTPObject->get("localhost","","");

   //   if(!mHTTPObject)
      mTCPSocketStream = new TCPSocketStream();
      //mTCPSocketStream->setPort(5502);
      mTCPSocketStream->connect("127.0.0.1:5502");
      mTCPSocketStream->listen(5502);
   //   mTCPSocketStream->connect("apple.com");

         mXMLDocument = new SimXMLDocument();

      while(!mTCPSocketStream->mBufferSave){}

      mXMLDocument->parse(reinterpret_cast<const char *>(mTCPSocketStream->mBufferSave));

      // "Get" inside of the root element, "Students".
      mXMLDocument->pushChildElement(0);

      // "Get" into the first child element 
      if (mXMLDocument->pushFirstChildElement("sample"))
      {
          while (true)
          {
             if (mXMLDocument->pushFirstChildElement("finger"))
             {
                while (true)
                {
                    // 
                    // Here, i read the element's attributes. 
                    // You might want to save these values in an array or call the %xml.getElementValue()
                    // if you have a different XML structure.
                    //
                    mXMLDocument->attribute("id");
                    mXMLDocument->attribute("age");
                    mXMLDocument->pushFirstChildElement("loc");
                    mXMLDocument->attribute("x");
                    mXMLDocument->attribute("y");
                    // now, read the next "finger"
                    if (!mXMLDocument->nextSiblingElement("finger")) break; 
                }

              // now, read the next "Student"
              if (!mXMLDocument->nextSiblingElement("sample")) break; 
          }
      }
   }
   }
   //      mXMLDocument(NULL),
   //   mHTTPObject(NULL)

   return true;
}
void SimMultiTouchXML::onRemove()
{
   Parent::onRemove();
   if(mTCPSocketStream)
   {
      mTCPSocketStream->disconnect();
      delete mTCPSocketStream;
   }
   if(mXMLDocument)
   {
      delete mXMLDocument;
   }
}
void SimMultiTouchXML::initPersistFields()
{
   Parent::initPersistFields();
}



*/
// end jc


/*
Manipulator::Manipulator() :
      mResolution(0,0)
{
   mNetFlags.set(Ghostable | ScopeAlways);
}

bool Manipulator::onAdd()
{
   if(!Parent::onAdd())
      return false;

   return true;
}

void Manipulator::onRemove()
{
   Parent::onRemove();
}

void Manipulator::inspectPostApply()
{
   setMaskBits( 0xFFFFFFFF );
   
   Parent::inspectPostApply();
}

//------------------------------------------------------------------------------


void Manipulator::initPersistFields()
{
   addGroup("Manipulator");
   addField("url",              TypeStringFilename,     Offset(mURL,         WebViewData), "URL to load.");
   addField("resolution",        TypePoint2I,   Offset(mResolution,  WebViewData), "Resolution of texture.");
   endGroup("Manipulator");

   Parent::initPersistFields();
}

U32 Manipulator::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate( conn, mask, stream );

   return retMask;
}

void Manipulator::unpackUpdate(NetConnection* conn, BitStream* stream)
{
   Parent::unpackUpdate(conn, stream);

}
*/

////EOF/////////////////////////////////////////////////////////////////////////
