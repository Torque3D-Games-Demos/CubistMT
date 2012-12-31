
//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "platform/platform.h"
#include "T3D/trafficcontroller.h"



#include "scene/sceneRenderState.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "collision/boxConvex.h"

#include "core/stream/bitStream.h"
#include "math/mathIO.h"
#include "gfx/gfxTransformSaver.h"
#include "renderInstance/renderPassManager.h"
#include "gfx/gfxDrawUtil.h"
#include "T3D/physics/physicsPlugin.h"
#include "T3D/physics/physicsBody.h"
#include "T3D/physics/physicsCollision.h"
#include "T3D/AIFishPlayer.h"


bool TrafficController::smRenderTriggers = false;
static   const float TIME_COLLISION_INTEREST = 3.0f;

//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(TrafficControllerData);

ConsoleDocClass( TrafficControllerData,
   "@brief Defines shared properties for TrafficController objects.\n\n"

   "The primary focus of the TrafficControllerData datablock is the callbacks it provides when an object is "
   "within or leaves the TrafficController bounds.\n"

   "@see TrafficController.\n"
   "@ingroup gameObjects\n"
   "@ingroup Datablocks\n"
);

IMPLEMENT_CALLBACK( TrafficControllerData, onTickTrigger, void, ( TrafficController* trigger ), ( trigger ),
   "@brief Called every tickPeriodMS number of milliseconds (as specified in the TrafficControllerData) whenever "
   "one or more objects are inside the volume of the trigger.\n\n"

   "The TrafficController has methods to retrieve the objects that are within the TrafficController's bounds if you "
   "want to do something with them in this callback.\n"

   "@param trigger the TrafficController instance whose volume the object is inside\n"
   
   "@see tickPeriodMS\n"
   "@see TrafficController::getNumObjects()\n"
   "@see TrafficController::getObject()\n");


TrafficControllerData::TrafficControllerData()
{
   tickPeriodMS = 100;
   isClientSide = false;
}

bool TrafficControllerData::onAdd()
{
   if (!Parent::onAdd())
      return false;

   return true;
}

void TrafficControllerData::initPersistFields()
{
   addGroup("Callbacks");

      addField( "tickPeriodMS",  TypeS32,    Offset( tickPeriodMS, TrafficControllerData ),
         "@brief Time in milliseconds between calls to onTickTrigger() while at least one object is within a TrafficController's bounds.\n\n"
         "@see onTickTrigger()\n");
      addField( "clientSide",    TypeBool,   Offset( isClientSide, TrafficControllerData ),
         "Forces TrafficController callbacks to only be called on clients.");

   endGroup("Callbacks");

   Parent::initPersistFields();
}


//--------------------------------------------------------------------------
void TrafficControllerData::packData(BitStream* stream)
{
   Parent::packData(stream);
   stream->write(tickPeriodMS);
   stream->write(isClientSide);
}

void TrafficControllerData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   stream->read(&tickPeriodMS);
   stream->read(&isClientSide);
}


//--------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(TrafficController);

ConsoleDocClass( TrafficController,
   "@brief A TrafficController is a volume of space that initiates script callbacks "
   "when objects pass through the TrafficController.\n\n"

   "TrafficControllerData provides the callbacks for the TrafficController when an object enters, stays inside "
   "or leaves the TrafficController's volume.\n\n"

   "@see TrafficControllerData\n"
   "@ingroup gameObjects\n"
);

IMPLEMENT_CALLBACK( TrafficController, onAdd, void, ( U32 objectId ), ( objectId ),
   "@brief Called when the TrafficController is being created.\n\n"
   "@param objectId the object id of the TrafficController being created\n" );

IMPLEMENT_CALLBACK( TrafficController, onRemove, void, ( U32 objectId ), ( objectId ),
   "@brief Called just before the TrafficController is deleted.\n\n"
   "@param objectId the object id of the TrafficController being deleted\n" );

TrafficController::TrafficController()
{
   // Don't ghost by default.
   mNetFlags.set(Ghostable | ScopeAlways);

   mTypeMask |= TrafficControllerObjectType;

   mObjScale.set(1, 1, 1);
   mObjToWorld.identity();
   mWorldToObj.identity();

   mDataBlock = NULL;

   mLastThink = 0;
   mCurrTick  = 0;
}

TrafficController::~TrafficController()
{
}

//-----------------------------------------------------------------------------
void TrafficController::consoleInit()
{
   Con::addVariable( "$TrafficController::renderTriggers", TypeBool, &smRenderTriggers,
      "@brief Forces all TrafficController's to render.\n\n"
      "Used by the Tools and debug render modes.\n"
      "@ingroup gameObjects" );
}

void TrafficController::initPersistFields()
{

   addProtectedField("tickCommand", TypeCommand, Offset(mTickCommand, TrafficController), &setTickCmd, &defaultProtectedGetFn,
      "The command to execute while an object is inside this trigger. Maximum 1023 characters." );

   Parent::initPersistFields();
}


bool TrafficController::setTickCmd(void *object, const char *index, const char *data)
{
   static_cast<TrafficController*>(object)->setMaskBits(TickCmdMask);
   return true; // to update the actual field
}

//--------------------------------------------------------------------------

bool TrafficController::onAdd()
{
   if(!Parent::onAdd())
      return false;

   onAdd_callback( getId() );


   addToScene();

   if (isServerObject())
      scriptOnAdd();
      
   return true;
}

void TrafficController::onRemove()
{
   onRemove_callback( getId() );

   removeFromScene();
   Parent::onRemove();
}

bool TrafficController::onNewDataBlock( GameBaseData *dptr, bool reload )
{
   mDataBlock = dynamic_cast<TrafficControllerData*>( dptr );
   if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
      return false;

   scriptOnNewDataBlock();
   return true;
}

void TrafficController::onDeleteNotify( SimObject *obj )
{
   GameBase* pScene = dynamic_cast<GameBase*>( obj );

   if  ( pScene != NULL && mDataBlock != NULL )
   {
      for ( U32 i = 0; i < mObjects.size(); i++ )
      {
         if ( pScene == mObjects[i].fish )
         {
            mObjects.erase(i);
            break;
         }
      }
   }

   Parent::onDeleteNotify( obj );
}

//------------------------------------------------------------------------------

void TrafficController::setTransform(const MatrixF & mat)
{
   Parent::setTransform(mat);

   if (isServerObject()) {
      MatrixF base(true);
      base.scale(Point3F(1.0/mObjScale.x,
                         1.0/mObjScale.y,
                         1.0/mObjScale.z));
      base.mul(mWorldToObj);

      setMaskBits(TransformMask | ScaleMask);
   }
}

void TrafficController::prepRenderImage( SceneRenderState *state )
{
   // only render if selected or render flag is set
   if ( !smRenderTriggers && !isSelected() )
      return;

   ObjectRenderInst *ri = state->getRenderPass()->allocInst<ObjectRenderInst>();
   ri->renderDelegate.bind( this, &TrafficController::renderObject );
   ri->type = RenderPassManager::RIT_Editor;      
   ri->translucentSort = true;
   ri->defaultKey = 1;
   state->getRenderPass()->addInst( ri );
}

void TrafficController::renderObject( ObjectRenderInst *ri,
                            SceneRenderState *state,
                            BaseMatInstance *overrideMat )
{
}


void TrafficController::processTick(const Move* move)
{
   Parent::processTick(move);

   if( isClientObject() ){
      return;
   }

   clearGrid();

   //*****************************************************
   // assign fish to grid squares
   //*****************************************************
   for(int i=0; i<mObjects.size(); i++){
      FishStatus     *thisFish = &mObjects[i];
      AIFishPlayer   *fish = thisFish->fish;

      //---get state-------------------
      thisFish->position = fish->getPosition();
      thisFish->direction = fish->getHeading();
      thisFish->passingDistance = fish->getPassingDistance();
      thisFish->speed = fish->getSpeed();

      //---figure out grid square-------------------
      Point3F  rel = thisFish->position - mGridLo;
      int      ix = (int)(rel.x/mGridSize);
      int      iy = (int)(rel.y/mGridSize);
      int      iz = (int)(rel.z/mGridSize);
      ix = mClamp(ix, 0, mNx-1);
      iy = mClamp(iy, 0, mNy-1);
      iz = mClamp(iz, 0, mNz-1);
      thisFish->ix = ix;
      thisFish->iy = iy;
      thisFish->iz = iz;

      //---add data to relevant square-------------------
      int   index = ix + iy*mNx + iz*(mNx*mNy); 
      FishStatus  *nextFish = mGrid[index];
      thisFish->last = 0;
      thisFish->next = nextFish;
      if(nextFish){
         nextFish->last = thisFish;
      }
      mGrid[index] = thisFish;
   }
   //*****************************************************
   // do collision tests
   //*****************************************************
   for(int i=0; i<mObjects.size(); i++){
//      FishStatus     *thisFish = &mObjects[i];
//      AIFishPlayer   *o = thisFish->fish;



      //----if recently hit something allow reversing for a short time, so can get out easier
/*

      //---if imminent collision, set new destination ---------------------------
      int   nx0 = mClamp(thisFish->ix-1, 0, mNx-1);
      int   nx1 = mClamp(thisFish->ix+1, 0, mNx-1);

      int   ny0 = mClamp(thisFish->iy-1, 0, mNy-1);
      int   ny1 = mClamp(thisFish->iy+1, 0, mNy-1);

      int   nz0 = mClamp(thisFish->iz-1, 0, mNz-1);
      int   nz1 = mClamp(thisFish->iz+1, 0, mNz-1);

      bool  done = false;
      float bestCollisionTime = FLT_MAX;
      FishStatus  *bestFish = 0;
      for(int ix=nx0; ix<=nx1 && !done ; ix++){
         for(int iy=ny0; iy<=ny1 && !done ; iy++){
            for(int iz=nz0; iz<=nz1 && !done ; iz++){
               FishStatus  *list = mGrid[ix + iy*mNx + iz*(mNx*mNy)];
               for(FishStatus *otherFish = list; otherFish && !done; otherFish=otherFish->next){
                  if(otherFish==thisFish)continue;
                  float time = getCollisionTime(thisFish, otherFish);
                  if( time!=0 && time<bestCollisionTime){
                     bestCollisionTime = time;
                     bestFish = otherFish;
                  }
               }
            }
         }
      }
      if( bestCollisionTime<3.0f ){
         thisFish->setWanderDestination(false, now);
      }
      */
   }
}
//****************************************************************************
// returns time of future collision or zero if there isnt one
// collision with time<=0 or time>=TIME_COLLISION_INTEREST are ignored
//****************************************************************************
float  TrafficController::getCollisionTime(FishStatus *a0, FishStatus *a1)
{
   Point3F  v0 = a0->direction*a0->speed;
   Point3F  v1 = a1->direction*a1->speed;

   Point3F  v01 = v0-v1;
   Point3F  p01 = a0->position - a1->position;

   F32   a = mDot(v01, v01);
   F32   b = 2.0f*mDot(p01, v01);
   F32   d = a0->passingDistance + a1->passingDistance;
   F32   c = mDot(p01,p01) - d*d;
   F32   ff = b*b - 4.0f*a*c;
   F32   collisionTime = 0;
   if(ff>=0){
      ff = sqrtf(ff);

      F32   time1 = (-b + ff) / (2.0f*a);
      if( time1>0 ){
         collisionTime = time1;
      }
      F32   time2 = (-b - ff) / (2.0f*a);
      if(time2>0 && time2<collisionTime ){
         collisionTime = time2;
      }
      return collisionTime<=TIME_COLLISION_INTEREST? collisionTime : 0;
   }
   return collisionTime;
}


//--------------------------------------------------------------------------

U32 TrafficController::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);

   if( stream->writeFlag( mask & TransformMask ) )
   {
      stream->writeAffineTransform(mObjToWorld);
   }

   if( stream->writeFlag( mask & TickCmdMask ) )
   {
      stream->writeLongString(CMD_SIZE-1, mTickCommand.c_str());
   }

   return retMask;
}

void TrafficController::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   // Transform
   if( stream->readFlag() )
   {
      MatrixF temp;
      stream->readAffineTransform(&temp);
      setTransform(temp);
   }

   if( stream->readFlag() )
   {
      char buf[CMD_SIZE];
      stream->readLongString(CMD_SIZE-1, buf);
      mTickCommand = buf;
   }
}
void  TrafficController::addFish(AIFishPlayer *a)
{
   FishStatus  fs;

   fs.fish = a;
   fs.last = 0;
   fs.next = 0;
   mObjects.push_back(fs);
   a->setTrafficController(this);
}
void  TrafficController::clearGrid()
{
   int   count = mNx*mNy*mNz;
   for(int j=0; j<count; j++){
      mGrid[j] = 0;
   }
}
bool  containerRayCast(RayInfo *rInfo, Point3F &start, 
                       Point3F &end, int mask, SceneObject *fish)
{
   if(fish)fish->disableCollision();
   bool  ret = gServerContainer.castRay(start, end, mask, rInfo);
   if(fish)fish->enableCollision();
   return ret;
}
void  TrafficController::setGrid(Point3F _lo, Point3F _hi, F32 _gridSize)
{
   mGridLo = _lo;
   mGridHi = _hi;
   mGridSize = _gridSize;
   Point3F  size = mGridHi - mGridLo;
   mNx = (size.x/_gridSize) + 1;
   mNy = (size.y/_gridSize) + 1;
   mNz = (size.z/_gridSize) + 1;

   mGrid = new FishStatus *[mNx*mNy*mNz];
   clearGrid();
}
AIFishPlayer   *TrafficController::getFishCollision(AIFishPlayer *a)
{
   bool  overlap = false;
   F32   closestOverlap = FLT_MAX;
   AIFishPlayer   *target = 0;
   for(Vector<FishStatus>::iterator i=mObjects.begin(); i!=mObjects.end(); ++i)
   {
      AIFishPlayer   *other = i->fish;
      if(other==a)continue;
      Point3F  separation = other->getPosition() - a->getPosition();
      F32      distanceAway = separation.len();
      F32      passingDistance = a->getPassingDistance() + other->getPassingDistance();
      if( distanceAway < passingDistance ){
         overlap = true;
         if( distanceAway<closestOverlap ){
            closestOverlap = distanceAway;
            target = other;
         }
      }else{
         if( !overlap ){
         }
      }
   }
   return target;
}

//ConsoleMethod( TrafficController, getNumObjects, S32, 2, 2, "")
DefineEngineMethod( TrafficController, getNumObjects, S32, (),,
   "@brief Get the number of objects that are within the TrafficController's bounds.\n\n"
   "@see getObject()\n")
{
   return object->getNumTriggeringObjects();
}

//ConsoleMethod( TrafficController, getObject, S32, 3, 3, "(int idx)")
DefineEngineMethod( TrafficController, getObject, S32, ( S32 index ),,
   "@brief Retrieve the requested object that is within the TrafficController's bounds.\n\n"
   "@param index Index of the object to get (range is 0 to getNumObjects()-1)\n"
   "@returns The SimObjectID of the object, or -1 if the requested index is invalid.\n"
   "@see getNumObjects()\n")
{
   if (index >= object->getNumTriggeringObjects() || index < 0)
      return -1;
   else
      return object->getObject(U32(index))->getId();
}
//pg start
DefineEngineMethod( TrafficController, addFish, void, (AIFishPlayer* _fish),,
			  "(AIFishPlayer* _fish)")
{	
   object->addFish(_fish);
}
DefineEngineMethod( TrafficController, setGrid, void, (Point3F _lo, Point3F _hi, F32 _gridSize),,
			  "(AIFishPlayer* _fish)")
{	
   object->setGrid(_lo,_hi,_gridSize);
}
