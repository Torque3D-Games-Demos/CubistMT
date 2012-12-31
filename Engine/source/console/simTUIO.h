//-----------------------------------------------------------------------------
// Torque 3D 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Console implementation of STL map.
//-----------------------------------------------------------------------------

#ifndef _SIMTUIO_H_
#define _SIMTUIO_H_

#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif

#ifndef _TVECTOR_H_
#include "core/util/tVector.h"
#endif // _TVECTOR_H_


// start jc
#ifndef _HTTPOBJECT_H_
#include "app/net/httpObject.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _XMLDOC_H_
#include "console/SimXMLDocument.h"
#endif

#ifndef _GUITYPES_H_
#include "gui/core/GuiTypes.h"
#endif

#ifndef _SIMOBJECTREF_H_
#include "console/simObjectRef.h"
#endif

#ifndef _TDICTIONARY_H_
#include "core/util/tDictionary.h"
#endif

//#ifndef _GUICANVAS_H_
//#include "gui/core/guiCanvas.h"
//#endif

//#ifndef INCLUDED_TUIOCLIENT_H
//#include "TUIO/TuioClient.h"
//#endif

class guiTuioListener;
class GuiCanvas;

namespace TUIO {
   class TuioClient;
   class TuioCursor;
};

struct GuiTouchEvent : public GuiEvent
{
   enum EventType {
      TouchDown,
      TouchMove,
      TouchUp
   };
   TUIO::TuioClient* client;
   TUIO::TuioCursor* cursor;
   SimObjectRef<GuiCanvas> canvas;

   S32 touchId;
   Point2I touchPosition;

  // GuiTouchEvent(TUIO::TuioClient* client, TUIO::TuioCursor* cursor)
   GuiTouchEvent()
      : client( NULL ),
        cursor( NULL ) {}
};
struct Gui3DTouchEvent : public GuiTouchEvent
{
   Point3F     vec;
   Point3F     pos;
   
   Gui3DTouchEvent()
      : vec( 0.f, 0.f, 0.f ),
        pos( 0.f, 0.f, 0.f ) {}
};
struct ShapeTouchEvent : public Gui3DTouchEvent
{
   RayInfo     ri;
   
   ShapeTouchEvent()
      : ri() {}
};



class SimTUIO : public SimObject
{
   friend class guiTuioListener;
   // This typedef is required for tie ins with the script language.
   // --------------------------------------------------------------------------
	protected:
      typedef SimObject Parent;
   // --------------------------------------------------------------------------

   public:
      SimTUIO();
      ~SimTUIO();

      // These are overloaded functions from SimObject that we handle for
      // tie in to the script language.  The .cc file has more in depth
      // comments on these.
      //-----------------------------------------------------------------------
      bool processArguments(S32 argc, const char** argv);
      bool onAdd();
      void onRemove();
      static void initPersistFields();
      void process();
      SimObjectRef<GuiCanvas> getCanvas() { return mCanvas; }

   private:
      guiTuioListener* mGuiTuioListener;

      SimObjectRef<GuiCanvas> mCanvas; 
      U16 mPort;
      Point2F mPosition;
      Point2F mScale;
      F32 mRotate;
      Point2I mIdRange;

   public:
      Point2F getPosition()   { return mPosition; }
      Point2F getScale()   { return mScale; }
      F32 getRotate()   { return mRotate; }
      Point2I getIdRange()   { return mIdRange; }
      U16 getPort()   { return mPort; }
     


      DECLARE_CONOBJECT(SimTUIO);
};
/*
class MultiTouchState
{

   public:
      class Finger
      {
         U32 id;
         U32 age;
         Point2F position;
      };
      class Hand : Finger
      {
         Vector<StrongRefPtr<Finger>> fingers;
      };
      class Gesture : Hand
      {
         Vector<StrongRefPtr<Hand)> hands;
         U32 type;
      };

      class FingerHistory
      {
         Vector<StrongRefPtr<Finger>> mHistory;
      };
      class HandHistory
      {
         Vector<StrongRefPtr<Hand>> mHistory;
      };
      class GestureHistory
      {
         Vector<StrongRefPtr<Gesture>> mHistory;
      };

   private:
      Vector<StrongRefPtr<Finger>> mFingers;
      Vector<StrongRefPtr<Hand>> mHands;
      Vector<StrongRefPtr<Gesture>> mGestures;

      Vector<StrongRefPtr<FingerHistory>> mFingersHistory;
      Vector<StrongRefPtr<HandHistory>> mHandsHistory;
      Vector<StrongRefPtr<GestureHistory>> mGesturesHistory;

};

class MultiTouchEvent
{
   public:
      
      enum MultiTouchEventType {
         TouchDown = 0,
         TouchMove,
         TouchUp,
         HandDown,
         HandMove,
         HandUp,
         GestureDown,
         GestureMove,
         GestureUp
      };

   private:
      static Point2I ZeroPoint;

      const Point2I &mStartPosition;
      const Point2I &mPosition;
      MultiTouchEventType mEventType;
      MultiTouchState *mState;

   public:
      MultiTouchEvent();
      ~MultiTouchEvent()   {}

      const Point2I &getPosition()        { return mPosition;        }
      const Point2I &getStartPosition()   { return mStartPosition;   }
};

class TCPSocketStream : public TCPObject
{
private:
   typedef TCPObject Parent;
protected:
   enum ParseState {
      ParsingStatusLine,
      ParsingHeader,
      ParsingChunkHeader,
      ProcessingBody,
      ProcessingDone,
   };
   ParseState mParseState;
//   U32 mTotalBytes;
//   U32 mBytesRemaining;
 public:
   U32 mStatus;
   F32 mVersion;
//   U32 mContentLength;
   bool mChunkedEncoding;
   U32 mChunkSize;
   const char *mContentType;
   char *mHostName;
//   char *mPath;
//   char *mQuery;
//   char *mPost;
   U8 *mBufferSave;
   U32 mBufferSaveSize;
public:
//   static void expandPath(char *dest, const char *path, U32 destSize);
//   void get(const char *hostName, const char *urlName, const char *query);
//   void post(const char *host, const char *path, const char *query, const char *post);
   TCPSocketStream();
   ~TCPSocketStream();

   //static HTTPObject *find(U32 tag);

   void connect(const char *address);
   virtual U32 onDataReceive(U8 *buffer, U32 bufferLen);
   virtual U32 onReceive(U8 *buffer, U32 bufferLen); // process a buffer of raw packet data
   virtual void onConnected();
   virtual void onConnectFailed();
   virtual void onDisconnect();
   bool processLine(U8 *line);

   DECLARE_CONOBJECT(TCPSocketStream);
};

class SimMultiTouchXML : public SimObject
{
   // This typedef is required for tie ins with the script language.
   // --------------------------------------------------------------------------
	protected:
      typedef SimObject Parent;
   // --------------------------------------------------------------------------

   public:
      SimMultiTouchXML();
      ~SimMultiTouchXML();

      // These are overloaded functions from SimObject that we handle for
      // tie in to the script language.  The .cc file has more in depth
      // comments on these.
      //-----------------------------------------------------------------------
      bool processArguments(S32 argc, const char** argv);
      bool onAdd();
      void onRemove();
      static void initPersistFields();

   private:
      SimXMLDocument *mXMLDocument;
      TCPSocketStream *mTCPSocketStream;
   //   HTTPObject *mHTTPObject;
   //   SimXMLDocument *mLastXML;

   public:
      DECLARE_CONOBJECT(SimMultiTouchXML);
};
*/
// end jc



/*
class Manipulator
{

public:

   Manipulator();

   virtual void pack(BitStream* stream) = 0;
   virtual void unpack(BitStream* stream) = 0;

   DECLARE_CONOBJECT(Manipulator);

};

class SwizzleManipulator : public Manipulator
{
private:

   typedef Manipulator Parent;

public:

   SwizzleManipulator();

   virtual void pack(BitStream* stream);
   virtual void unpack(BitStream* stream);

   DECLARE_CONOBJECT(SwizzleManipulator);
};
*/
#endif // _SIMTUIO_H_
////EOF/////////////////////////////////////////////////////////////////////////
