#ifndef _H_TRAFFICCONTROLLER
#define _H_TRAFFICCONTROLLER

#include "T3D/gameBase/gameBase.h"
#include "T3D/aifishplayer.h"
#include "math/mBox.h"

class TrafficController;
class AIFishPlayer;

struct TrafficControllerData: public GameBaseData {
public:
   typedef GameBaseData Parent;
   S32  tickPeriodMS;
   bool isClientSide;

   TrafficControllerData();
   
   DECLARE_CONOBJECT(TrafficControllerData);

   DECLARE_CALLBACK( void, onTickTrigger, ( TrafficController* trigger ) );

   bool onAdd();
   static void initPersistFields();
   virtual void packData  (BitStream* stream);
   virtual void unpackData(BitStream* stream);

};


class FishStatus{
public:
   Point3F        position;
   Point3F        direction;
   FishStatus     *next;
   FishStatus     *last;
   AIFishPlayer   *fish;
   F32            passingDistance;
   F32            speed;
   int            ix, iy, iz;
   //---
};


class TrafficController : public GameBase
{
public:
   typedef GameBase Parent;
   TrafficController();
   ~TrafficController();

   // SimObject
   DECLARE_CONOBJECT(TrafficController);

   DECLARE_CALLBACK( void, onAdd, ( U32 objectId ) );
   DECLARE_CALLBACK( void, onRemove, ( U32 objectId ) );

   static void consoleInit();
   static void initPersistFields();
   bool onAdd();
   void onRemove();
   void onDeleteNotify(SimObject*);

   // NetObject
   U32  packUpdate  (NetConnection *conn, U32 mask, BitStream* stream);
   void unpackUpdate(NetConnection *conn,           BitStream* stream);

   // SceneObject
   void setTransform(const MatrixF &mat);
   void prepRenderImage( SceneRenderState* state );

   // GameBase
   bool onNewDataBlock( GameBaseData *dptr, bool reload );

   // TrafficController
   void  addFish(AIFishPlayer *a);
   U32       getNumTriggeringObjects() const;
   AIFishPlayer* getObject(const U32);   

   void renderObject( ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance *overrideMat );
   void  setGrid(Point3F lo, Point3F hi, F32 gridSize);
   void  initRadiusSearch(Point3F _searchPosition, F32 _searchRadius);
   AIFishPlayer   *nextSearch();
   AIFishPlayer   *getFishCollision(AIFishPlayer *a);
protected:
   float getCollisionTime(FishStatus *a0, FishStatus *a1);
   void  clearGrid();
   static bool smRenderTriggers;
   void processTick(const Move *move);
   static bool setTickCmd(void *object, const char *index, const char *data);

private:
   Point3F                 mSearchPosition;
   F32                     mSearchRadiusSq;
   int                     mSearchIndex;

   Vector<FishStatus>      mObjects;
   TrafficControllerData*  mDataBlock;
   Point3F                 mGridLo, mGridHi;
   F32                     mGridSize;
   FishStatus              **mGrid;
   int                     mNx, mNy, mNz;
   U32                     mLastThink;
   U32                     mCurrTick;
   String                  mTickCommand;
   enum TriggerUpdateBits
   {
      TransformMask = Parent::NextFreeMask << 0,
      TickCmdMask   = Parent::NextFreeMask << 4,
      NextFreeMask  = Parent::NextFreeMask << 5,
   };
   static const U32 CMD_SIZE = 1024;
};

inline 
U32 TrafficController::getNumTriggeringObjects() const
{
   return mObjects.size();
}

inline 
AIFishPlayer* TrafficController::getObject(const U32 index)
{
   AssertFatal(index < getNumTriggeringObjects(), "Error, out of range object index");

   return mObjects[index].fish;
}

inline
void  TrafficController::initRadiusSearch(Point3F _searchPosition, F32 _searchRadius)
{
   mSearchPosition = _searchPosition;
   mSearchRadiusSq = _searchRadius*_searchRadius;
   mSearchIndex = 0;
}
inline
AIFishPlayer   *TrafficController::nextSearch()
{
   const int   count = mObjects.size();
   while( mSearchIndex<count ){
      AIFishPlayer  *f = mObjects[mSearchIndex++].fish;
      Point3F  v = (f->getPosition() - mSearchPosition);
      if( v.lenSquared()<mSearchRadiusSq ){
         return f;
      }
   }
   return 0;
}

#endif

