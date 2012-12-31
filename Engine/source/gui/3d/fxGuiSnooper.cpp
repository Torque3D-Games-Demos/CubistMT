#include "console/consoleTypes.h"
#include "gui/3d/guiTSControl.h"
#include "gfx/gfxDrawUtil.h"
#include "gui/core/guiControl.h"
#include "renderInstance/renderPassManager.h"
#include "gfx/gfxDevice.h"
#include "scene/reflectionManager.h"
#include "T3D/gameBase/gameConnection.h"


class fxGuiSnooper : public GuiTSCtrl
{
private:
	typedef GuiTSCtrl Parent;

	Point3F				mRotateView;					// View Rotation.
	Point3F				mOffsetView;					// Offset Distance.
	F32					mFov;							// Field of View.
	Point3F				mSweepAmplitude;				// Sweep Amplitude.
	Point3F				mSweepTime;						// Sweep Time.

	bool				mUseOverlayBitmap;				// Use Overlay Bitmap Flag.
	bool				mUseOverlayColour;				// Use Overlay Colour Flag.
	bool				mOverlayTile;					// Overlay Tile Flag.
	ColorF				mOverlayColor;					// Filter Colour Vector.
	bool				mOverlayRedMask;				// Overlay Red Mask Flag.
	bool				mOverlayGreenMask;				// Overlay Green Mask Flag.
	bool				mOverlayBlueMask;				// Overlay Blue Mask Flag.

	StringTableEntry	mObjectName;					// Attached Object Name.
	SceneObject*		mAttachedObject;				// Attached Object.

	U32					mLastTimeStamp;					// Last Time Stamp.
	Point3F				mCurrentSweepMagnitude;			// Current Sweep Phase.
	StringTableEntry	mOverlayBitmapName;				// Overlay Bitmap Name.
	GFXTexHandle		mOverlayTextureHandle;			// Overlay Texture Handle.

    GFXStateBlockRef mSnooperClearSB;
    GFXStateBlockRef mDzEnableSB;
	GFXStateBlockRef mEcwSB;
	GFXStateBlockRef mEcwResetSB;

	void renderWorld(const RectI & updateRect);
	void onRender(Point2I offset, const RectI &updateRect);

protected:
	CameraQuery mLastCameraQuery; 

public:
	fxGuiSnooper();

	static void initPersistFields();
	static void consoleInit();

	bool processCameraQuery(CameraQuery * query);

	void setViewObject(const char* ObjectName);
	void setViewRotation(Point3F Rotation);
	void setOverlayBitmap(const char *name);
	void setOverlayColor(ColorF OverlayColor);
	void setOverlayMask(bool RedMask, bool GreenMask, bool BlueMask);

	bool onWake();
	void onSleep();

	DECLARE_CONOBJECT(fxGuiSnooper);
};

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(fxGuiSnooper);

//------------------------------------------------------------------------------


fxGuiSnooper::fxGuiSnooper() :
	mRotateView(0,0,0),
	mOffsetView(0,0,0),
	mFov(60.0),
	mSweepAmplitude(0,0,60),
	mSweepTime(5000,5000,5000),
	mCurrentSweepMagnitude(0,0,0),
	mAttachedObject(NULL),
	mOverlayRedMask(true),
	mOverlayGreenMask(true),
	mOverlayBlueMask(true),
	mOverlayTile(false),
	mUseOverlayBitmap(false),
	mUseOverlayColour(false),
	mLastTimeStamp(Platform::getRealMilliseconds())
{
	// Create Empty Attached Object Name.
	mObjectName = StringTable->insert("");

	// Create Empty Overlay Bitmap Name.
	mOverlayBitmapName = StringTable->insert("");

	// Set Default Overlay Colour.
	mOverlayColor.set(1, 1, 1, 0.5f);

	mLastCameraQuery.cameraMatrix.identity();
	mLastCameraQuery.fov = 45.0f;
	mLastCameraQuery.object = NULL;
	mLastCameraQuery.farPlane = 10.0f;
	mLastCameraQuery.nearPlane = 0.01f;
	mLastCameraQuery.frustumOffset = Point4F::Zero; // start jc
	mLastCameraQuery.ortho = false;
};

//------------------------------------------------------------------------------

void fxGuiSnooper::initPersistFields()
{
	// Initialise parents' persistent fields.
	Parent::initPersistFields();

	// Add out own persistent fields.
	addField( "ViewRotation", TypePoint3F, Offset( mRotateView, fxGuiSnooper ) );
	addField( "ViewOffset", TypePoint3F, Offset( mOffsetView, fxGuiSnooper ) );
	addField( "FOV", TypeF32, Offset( mFov, fxGuiSnooper ) );
	addField( "SweepAmplitude", TypePoint3F, Offset( mSweepAmplitude, fxGuiSnooper ) );
	addField( "SweepTime", TypePoint3F, Offset( mSweepTime, fxGuiSnooper ) );
	addField( "AttachedObject", TypeString, Offset( mObjectName, fxGuiSnooper ) );
	addField( "OverlayBitmap", TypeBool, Offset( mUseOverlayBitmap, fxGuiSnooper ) );
	addField( "OverlayTile", TypeBool, Offset( mOverlayTile, fxGuiSnooper ) );
	addField( "OverlayColour", TypeBool, Offset( mUseOverlayColour, fxGuiSnooper ) );
	addField( "BitmapOverlay", TypeFilename, Offset(mOverlayBitmapName, fxGuiSnooper));
	addField( "ColorOverlay", TypeColorF, Offset( mOverlayColor, fxGuiSnooper ) );
	addField( "RedMask", TypeBool, Offset( mOverlayRedMask, fxGuiSnooper ) );
	addField( "GreenMask", TypeBool, Offset( mOverlayGreenMask, fxGuiSnooper ) );
	addField( "BlueMask", TypeBool, Offset( mOverlayBlueMask, fxGuiSnooper ) );
}

//------------------------------------------------------------------------------

bool fxGuiSnooper::onWake()
{
	// Wake-up Parent.
	if (!Parent::onWake()) return false;

	// Set Active.
	setActive(true);

	// Have we an Attached Object Name?
	if (mObjectName)
	{
		// Yes, so attach to it.
		setViewObject(mObjectName);
	}

	// Set Overlay Bitmap.
	setOverlayBitmap(mOverlayBitmapName);

	// Return OK.
	return true;
}

//------------------------------------------------------------------------------

void fxGuiSnooper::onSleep()
{
	// Reset Overlay Texture Handle.
	mOverlayTextureHandle = NULL;
	// Call Parent.
	Parent::onSleep();
}

//------------------------------------------------------------------------------

void fxGuiSnooper::setViewRotation(Point3F Rotation)
{
	// Set the Rotation internally.
	mRotateView = Rotation;
}

//------------------------------------------------------------------------------

void fxGuiSnooper::setViewObject(const char* ObjectName)
{
	// Get Root Group.
	SimGroup* SG = Sim::getRootGroup();

	// Interate Sim Group.
	for (SimSetIterator itr(SG); *itr; ++itr)
	{
		// Yes, so cast our Object.
		SceneObject* SceneObj = static_cast<SceneObject*>(*itr);
		// Check that it's a Server Object.
		if (SceneObj->isServerObject())
		{
			const char* getName;

			getName = SceneObj->getName();

			// Yes, so is this our Object?
			if (getName && dStrcmp(getName, ObjectName) == 0)
			{
				// Store Scene Object.
				mAttachedObject = SceneObj;

				// Return OK.
				return;
			}
		}
	}

	// Reset Object.
	mAttachedObject = NULL;
}

//------------------------------------------------------------------------------

void fxGuiSnooper::setOverlayBitmap(const char *name)
{
	// Set Overlay Bitmap Name.
	mOverlayBitmapName = StringTable->insert(name);

	if (*mOverlayBitmapName)
		// Yes, so get Texture Handle.
	    mOverlayTextureHandle.set( mOverlayBitmapName, &GFXDefaultGUIProfile, "" );
	else
		// No, so reset Texture Handle.
		mOverlayTextureHandle = NULL;

	// Update.
	setUpdate();
}   

//------------------------------------------------------------------------------

void fxGuiSnooper::setOverlayColor(ColorF OverlayColor)
{
	// Set the Overlay Colour internally.
	mOverlayColor = OverlayColor;
}

//------------------------------------------------------------------------------

void fxGuiSnooper::setOverlayMask(bool RedMask, bool GreenMask, bool BlueMask)
{
	// Set the Overlay Masks internally.
	mOverlayRedMask		= RedMask;
	mOverlayGreenMask	= GreenMask;
	mOverlayBlueMask	= BlueMask;
}

//------------------------------------------------------------------------------

ConsoleMethod(fxGuiSnooper,setViewRotation,void,5,5,"Sets View Rotation.")
{
	Point3F		Rotation;

	// Fetch the fxGuiSnooper object.
	fxGuiSnooper *Viewport = static_cast<fxGuiSnooper*>(object);

	// Fetch Rotation.
	Rotation.set(	mDegToRad(dAtof(argv[2])),
					mDegToRad(dAtof(argv[3])),
					mDegToRad(dAtof(argv[4])));

	// Set Rotation.
	Viewport->setViewRotation(Rotation);
}

//------------------------------------------------------------------------------

ConsoleMethod(fxGuiSnooper,setViewObject,void,3,3,"Sets View to Object.")
{
	F32		Rotation;

	// Fetch the fxGuiSnooper object.
	fxGuiSnooper *Viewport = static_cast<fxGuiSnooper*>(object);

	// Set the GuiFilter Filter Colour.
	Viewport->setViewObject(argv[2]);
}

//------------------------------------------------------------------------------

ConsoleMethod(fxGuiSnooper,setOverlayBitmap,void,3,3,"Sets Overlay Bitmap.")
{
	// Fetch the fxGuiSnooper object.
	fxGuiSnooper *Viewport = static_cast<fxGuiSnooper*>(object);

	// Set Overlay Bitmap.
	Viewport->setOverlayBitmap(argv[2]);
}

//------------------------------------------------------------------------------

ConsoleMethod(fxGuiSnooper,setOverlayColor,void,5,6,"Sets Overlay Color.")
{
	F32		r,g,b,a;
	ColorF	TempColor;

	// Fetch the fxGuiSnooper object.
	fxGuiSnooper *Viewport = static_cast<fxGuiSnooper*>(object);

	// Convert RGB Ascii parms to float.
	r = dAtof(argv[2]);
	g = dAtof(argv[3]);
	b = dAtof(argv[4]);

	// Did we get an alpha param?
	if (argc == 6)
		// Yep, so convert it.
		a = dAtof(argv[5]);
	else
		// Nope, so default to 1.
		a = 1;

	// Setup our temporary colour vector.
	TempColor.set(r,g,b,a);

	// Set the fxGuiSnooper Overlay Colour.
	Viewport->setOverlayColor(TempColor);
}

//------------------------------------------------------------------------------

ConsoleMethod(fxGuiSnooper,setOverlayMask,void,5,5,"Sets Overlay Masks.")
{
	F32		r,g,b;

	// Fetch the fxGuiSnooper object.
	fxGuiSnooper *Viewport = static_cast<fxGuiSnooper*>(object);

	// Convert RGB Ascii parms to float.
	r = dAtof(argv[2]);
	g = dAtof(argv[3]);
	b = dAtof(argv[4]);

	// Set the fxGuiSnooper Masks.
	Viewport->setOverlayMask(r,g,b);
}

//------------------------------------------------------------------------------

static void cfxGuiBitmapSetOverlayBitmap(SimObject *obj, S32, const char **argv)
{
	// Fetch HUD Control.
	fxGuiSnooper *ctrl = static_cast<fxGuiSnooper*>(obj);

	// Set Overlay Bitmap.
	ctrl->setOverlayBitmap(argv[2]);
}

//------------------------------------------------------------------------------

void fxGuiSnooper::consoleInit()
{
   Con::addCommand("fxGuiSnooper", "setOverlayBitmap",  cfxGuiBitmapSetOverlayBitmap, "fxGuiSnooper.setOverlayBitmap(Bitmap)", 3, 3);
}

//------------------------------------------------------------------------------

void fxGuiSnooper::renderWorld(const RectI & updateRect)
{
	GFX->clear(GFXClearZBuffer, ColorI(0,0,0), 1.0f, 0);

	GFXStateBlockDesc snooperClear;
	snooperClear.zEnable = true;
	snooperClear.zWriteEnable = true;
	snooperClear.zFunc = GFXCmpLessEqual;
	snooperClear.cullMode = GFXCullNone;
		
	mSnooperClearSB = GFX->createStateBlock(snooperClear);
	
	GFXStateBlockDesc dzEnable;
	dzEnable.zEnable = false;
	mDzEnableSB = GFX->createStateBlock(dzEnable);
	GFX->setStateBlock(mSnooperClearSB);

	// Render Client Scene Graph.
	gClientSceneGraph->renderScene(SPT_Diffuse);
	
    GFX->setStateBlock(mDzEnableSB);
}

//------------------------------------------------------------------------------

void fxGuiSnooper::onRender(Point2I offset, const RectI &updateRect)
{	
	// Call Parent Render.
	Parent::onRender(offset, updateRect);

	// Set Clipping Rectangle to GUI Bounds.
	GFX->setClipRect(getBounds());

	// Do we have an attached Object?
	if (!mAttachedObject)
	{
		// No, so signal to user this problem ...
		ColorF ErrorColor(1,0,0);
		GFX->getDrawUtil()->drawRectFill(updateRect, ErrorColor);
		ErrorColor.set(1,1,1);
		char buf[256];
		dSprintf(buf, sizeof(buf), "*** Object not selected ***");
		GFX->getDrawUtil()->setBitmapModulation(ErrorColor);
		GFX->getDrawUtil()->drawText(mProfile->mFont, offset, buf);
		GFX->getDrawUtil()->clearBitmapModulation();

		// Return Error.
		return;
	}

	// Are we using the Overlay Bitmap?
	if (mUseOverlayBitmap)
	{
		// Yes, so do we have a texture Handle?
		if (mOverlayTextureHandle)
		{
			// Yes, so clear Bitmap Modulation.
			GFX->getDrawUtil()->clearBitmapModulation();

			// Are we tiling the Overlay Bitmap?
			if(mOverlayTile)
			{
				RectI SrcRegion;
				RectI DstRegion;

				// Yes, so fetch texture object.
				GFXTextureObject* TextureObj = mOverlayTextureHandle;

				// Calculate Tesselation Count.
				float XTess = ((float)getWidth()/(float)TextureObj->mBitmapSize.x)+1;
				float YTess = ((float)getHeight()/(float)TextureObj->mBitmapSize.y)+1;

				for(int y = 0; y < YTess; ++y)
				{
					for(int x = 0; x < XTess; ++x)
					{
						// Calculate Source Region.
						SrcRegion.set(0,0,TextureObj->mBitmapSize.x, TextureObj->mBitmapSize.y);

						// Calculate Destination Region.
						DstRegion.set(((TextureObj->mBitmapSize.x*x)+offset.x),
									  ((TextureObj->mBitmapSize.y*y)+offset.y),
									  TextureObj->mBitmapSize.x,	
									  TextureObj->mBitmapSize.y);

						// Draw Tiled Bitmap.
						GFX->getDrawUtil()->drawBitmapStretchSR(TextureObj, DstRegion, SrcRegion);
					}
				}
			}
			else
			{
				// No, so draw stretched Bitmap.
				GFX->getDrawUtil()->drawBitmapStretch(mOverlayTextureHandle, getBounds());
			}
		}
	}

	// Are we using the Overlay Colour?
	if (mUseOverlayColour)
	{
		// Set Colour Mask.
		
		// Declare StateBlocks
		GFXStateBlockDesc ecw;
		ecw.colorWriteRed   |= ( mOverlayRedMask ? GFXCOLORWRITEENABLE_RED		: 0 ); 
		ecw.colorWriteGreen |= ( mOverlayGreenMask ? GFXCOLORWRITEENABLE_GREEN	: 0 );
		ecw.colorWriteBlue  |= ( mOverlayBlueMask ? GFXCOLORWRITEENABLE_BLUE	: 0 );
		ecw.colorWriteAlpha = GFXCOLORWRITEENABLE_ALPHA;
		mEcwSB = GFX->createStateBlock(ecw);
		
		GFXStateBlockDesc ecwReset;
		ecwReset.colorWriteRed   = GFXCOLORWRITEENABLE_RED; 
		ecwReset.colorWriteGreen = GFXCOLORWRITEENABLE_GREEN;
		ecwReset.colorWriteBlue  = GFXCOLORWRITEENABLE_BLUE;
		ecwReset.colorWriteAlpha = GFXCOLORWRITEENABLE_ALPHA;
		mEcwResetSB = GFX->createStateBlock(ecwReset);

		GFX->setStateBlock(mEcwSB);

		// Draw our filled rectangle with the Filter Colour.
		GFX->getDrawUtil()->drawRectFill(updateRect, mOverlayColor);

		// Reset the Colour Mask.
		GFX->setStateBlock(mEcwResetSB);
	}

	REFLECTMGR->update( 1, getExtent(), mLastCameraQuery );
}

//------------------------------------------------------------------------------

bool fxGuiSnooper::processCameraQuery(CameraQuery * query)
{
	Point3F		CameraRotation;							// Rotated View.
	float		VisibleDistance = 1100.0f;				// Visible Distance.


	// Get Game Connection.
	GameConnection* pConnection = GameConnection::getConnectionToServer();

	// Did we get the connection?
	if (pConnection)
	{
		// Have we got an Attached Object?
		if (mAttachedObject)
		{
			// Current Sweep.
			EulerF mCurrentSweep;

			// Create Camera Matrix.
			MatrixF	Camera(true);

			// Craete Rotation Quaternion.
			QuatF	QRotation;

			// Get Time Elapsed.
			U32 CurrentTime = Platform::getRealMilliseconds();
			U32 TimeElapsed = CurrentTime - mLastTimeStamp;
			mLastTimeStamp = CurrentTime;

			// Calculate new Sweep.
			Point3F NewSweep(	(U32)(360.0f / mSweepTime[0] * TimeElapsed) % 360,
								(U32)(360.0f / mSweepTime[1] * TimeElapsed) % 360,
								(U32)(360.0f / mSweepTime[2] * TimeElapsed) % 360);

			// Add to Current Sweep.
			mCurrentSweepMagnitude += NewSweep;

			// Calculate Current Sweep Angle.
			mCurrentSweep.set(	mDegToRad((mSweepAmplitude[0] * mCos(mDegToRad(mCurrentSweepMagnitude[0])))/2 + mRotateView[0]),
								mDegToRad((mSweepAmplitude[1] * mCos(mDegToRad(mCurrentSweepMagnitude[1])))/2 + mRotateView[1]),
								mDegToRad((mSweepAmplitude[2] * mCos(mDegToRad(mCurrentSweepMagnitude[2])))/2 + mRotateView[2]));

			// Set-up Quaternion Rotation.
			QRotation.set(mCurrentSweep);

			// Set Camera Matrix to new Rotation.
			QRotation.setMatrix(&Camera);

			// Set Position @ Attached Object.
			Camera.setColumn(3, mAttachedObject->getBoxCenter() + mOffsetView);

			// Set Camera Matrix.
			query->cameraMatrix = Camera;

			// Set Near/Far Planes.
			query->nearPlane = 0.1;
			query->farPlane = getMax(VisibleDistance, 50.f);

			// Set FOV.
			query->fov = mDegToRad(mFov);

			// Set FOV.
			query->fov = mDegToRad(mFov);

			mLastCameraQuery.cameraMatrix = query->cameraMatrix; // <- Update mLastCameraQuery

			// Return OK.
			return(true);
		}
	}

	// Return Error.
	return(false);
}