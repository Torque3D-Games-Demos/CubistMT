//-----------------------------------------------------------------------------
// Torque 3D
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "gui/controls/guiWebViewCtrl.h"

#include "console/console.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxDrawUtil.h"
#include "gui/core/guiCanvas.h"
#include "sim/actionMap.h"


IMPLEMENT_CONOBJECT(GuiWebViewCtrl);

ConsoleDocClass( GuiWebViewCtrl,
   "@brief A gui control that is used to display an image.\n\n"
   
   "The image is stretched to the constraints of the control by default. However, the control can also\n"
   "tile the image as well.\n\n"

   "The image itself is stored inside the GuiWebViewCtrl::bitmap field. The boolean value that decides\n"
   "whether the image is stretched or tiled is stored inside the GuiWebViewCtrl::wrap field.\n"
   
   "@tsexample\n"
   "// Create a tiling GuiWebViewCtrl that displays \"myImage.png\"\n"
   "%bitmapCtrl = new GuiWebViewCtrl()\n"
   "{\n"
   "   bitmap = \"myImage.png\";\n"
   "   wrap = \"true\";\n"
   "};\n"
   "@endtsexample\n\n"
   
   "@ingroup GuiControls"
);

/*
IMPLEMENT_CALLBACK( GuiWebViewCtrl, onInputEvent, void, (const char* device, const char* action, bool state ),
														  ( device, action, state),
	"@brief Callback that occurs when an input is triggered on this control\n\n"
	"@param device The device type triggering the input, such as keyboard, mouse, etc\n"
	"@param action The actual event occuring, such as a key or button\n"
	"@param state True if the action is being pressed, false if it is being release\n\n"
);
*/


GuiWebViewCtrl::GuiWebViewCtrl(void)
 : mURL(),
   mStartPoint( 0, 0 ),
   mWrap( false ),
   mLockAspect(false),
   mForceNativeExtent(false),
   mWebViewTexture(NULL)
{
//   mWebViewTexture.getWebView()->setListener(this);
}
GuiWebViewCtrl::~GuiWebViewCtrl(void)
{
   mWebViewTexture.destroy();
}

/*
//void GuiWebViewCtrl::onFinishLoading(Awesomium::WebView* caller)
void GuiWebViewCtrl::OnDocumentReady(Awesomium::WebView* caller,
                                    const Awesomium::WebURL& url)
{
//   mWebViewTexture.getWebView().requestScrollData();
}
//void GuiWebViewCtrl::onGetPageContents(Awesomium::WebView* caller, 
//									   const std::string& url, 
//									   const std::wstring& contents)
void GuiWebViewCtrl::OnShowCreatedWebView(Awesomium::WebView* caller,
                                    Awesomium::WebView* new_view,
                                    const Awesomium::WebURL& opener_url,
                                    const Awesomium::WebURL& target_url,
                                    const Awesomium::Rect& initial_pos,
                                    bool is_popup)
{
   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->requestScrollData();
}

void GuiWebViewCtrl::onGetScrollData(Awesomium::WebView* caller,
									 int contentWidth,
									 int contentHeight,
									 int preferredWidth,
									 int scrollX,
									 int scrollY)
{
   if(contentWidth != mWebViewTexture.getWidth() || contentHeight != mWebViewTexture.getHeight())
   {
      U32 width = (contentWidth > preferredWidth ? contentWidth : preferredWidth);
      U32 height;
      if(getRoot())
         height = (contentHeight < getRoot()->getExtent().y ? contentHeight : getRoot()->getExtent().y);
      else height = (contentHeight > getExtent().y ? contentHeight : getExtent().y);


      mWebViewTexture.setSize(width, height);
      if(mForceNativeExtent)
         setExtent(mWebViewTexture.getWidthHeight());
      else if(mLockAspect)
         setExtent(getExtent());
   }
}

bool GuiWebViewCtrl::setURLString( void *object, const char *index, const char *data )
{
   // Prior to this, you couldn't do bitmap.bitmap = "foo.jpg" and have it work.
   // With protected console types you can now call the setBitmap function and
   // make it load the image.
   GuiWebViewCtrl *obj = static_cast<GuiWebViewCtrl *>( object );

   obj->setURL( data );

//   obj->mWebViewTexture.getWebView().resetZoom();
//   obj->mWebViewTexture.getWebView().requestScrollData();

   // Return false because the setBitmap method will assign 'mURL' to the
   // argument we are specifying in the call.
   return false;
}
*/

void GuiWebViewCtrl::initPersistFields()
{
   addGroup( "WebView" );
   
      addProtectedField( "url", TypeString, Offset( mURL, GuiWebViewCtrl ),
         &setURLString, &defaultProtectedGetFn,
         "The bitmap file to display in the control." );
      addField( "wrap",   TypeBool,     Offset( mWrap, GuiWebViewCtrl ),
         "If true, the bitmap is tiled inside the control rather than stretched to fit." );
      addField( "lockAspect",   TypeBool,     Offset( mLockAspect, GuiWebViewCtrl ),
         "If true, maintain native aspect ratio at all times." );
      addField( "forceNativeExtent",   TypeBool,     Offset( mForceNativeExtent, GuiWebViewCtrl ),
         "If true, resize to native extents when size is reported by page." );
      
   endGroup( "WebView" );

   Parent::initPersistFields();
}

bool GuiWebViewCtrl::onWake()
{
   
   // Set the default profile on start-up:
   if( !mProfile )
   {
      GuiControlProfile* profile;
      Sim::findObject( "GuiInputCtrlProfile", profile);
      if( profile )
         setControlProfile( profile );
   }

   if ( !Parent::onWake() )
      return false;

//   if( !smDesignTime )
//      mouseLock();
    
   setActive(true);

   setURL(mURL);
//   setFirstResponder();
   getRoot()->getPlatformWindow()->nativeKeyEvent.notify(this, &GuiWebViewCtrl::handleKeyboard);

   return true;
}

void GuiWebViewCtrl::onSleep()
{
//   if ( !mURL.equal("texhandle", String::NoCase) )
//      mTextureObject = NULL;
   Parent::onSleep();
//   mouseUnlock();
//   clearFirstResponder();
   getRoot()->getPlatformWindow()->nativeKeyEvent.remove(this, &GuiWebViewCtrl::handleKeyboard);
   //mWebViewTexture.destroy();

}

//-------------------------------------
void GuiWebViewCtrl::inspectPostApply()
{
   // if the extent is set to (0,0) in the gui editor and appy hit, this control will
   // set it's extent to be exactly the size of the bitmap (if present)
   Parent::inspectPostApply();

   if (!mWrap && (getExtent().x == 0) && (getExtent().y == 0) && mWebViewTexture.getTexture())
   {
      setExtent( mWebViewTexture.getWidth(), mWebViewTexture.getHeight());
   }
   else if(mLockAspect && mWebViewTexture.isValid())
   {
      F32 nativeWidth = mWebViewTexture.getWidth();
      F32 nativeHeight = mWebViewTexture.getHeight();
      F32 aspect = nativeWidth/nativeHeight;
      F32 width = getExtent().x;
      F32 height = getExtent().y;

      // reuse aspect variable as scale factor
      if(aspect > width/height)
         // fit to hieght
         aspect = height / nativeHeight;     
      else
         // fit to width
         aspect = width / nativeWidth;
      
      width = nativeHeight * aspect;
      height = nativeWidth * aspect;
      setExtent(width, height);
   }
}

bool GuiWebViewCtrl::setExtent( const Point2I &newExtent)
{
   if(mLockAspect && mWebViewTexture.isValid())
   {
      F32 nativeWidth = mWebViewTexture.getWidth();
      F32 nativeHeight = mWebViewTexture.getHeight();
      F32 aspect = nativeWidth/nativeHeight;
      F32 width = newExtent.x;
      F32 height = newExtent.y;

      // reuse aspect variable as scale factor
      if(aspect < width/height)
         // fit to hieght
         aspect = height / nativeHeight;     
      else
         // fit to width
         aspect = width / nativeWidth;
      
      width = nativeWidth * aspect;
      height = nativeHeight * aspect;

      return Parent::setExtent(Point2I(width, height));
   }
   return Parent::setExtent(newExtent);
}

/*
void GuiWebViewCtrl::setURL( const char *name, bool resize )
{
   mURL = name;
   if ( !isAwake() )
      return;

   if ( mURL.isNotEmpty() )
	{
      if ( !mURL.equal("texhandle", String::NoCase) )
		   mTextureObject.set( mURL, &GFXDefaultGUIProfile, avar("%s() - mTextureObject (line %d)", __FUNCTION__, __LINE__) );

      // Resize the control to fit the bitmap
      if ( mTextureObject && resize )
      {
         setExtent( mTextureObject->getWidth(), mTextureObject->getHeight() );
         updateSizing();
      }
   }
   else
      mTextureObject = NULL;

   setUpdate();
}
*/

void GuiWebViewCtrl::updateSizing()
{
   if(!getParent())
      return;
   // updates our bounds according to our horizSizing and verSizing rules
   RectI fakeBounds( getPosition(), getParent()->getExtent());
   parentResized( fakeBounds, fakeBounds);
}

/*
void GuiWebViewCtrl::setBitmapHandle(GFXTexHandle handle, bool resize)
{
   mTextureObject = handle;

   mURL = String("texhandle");

   // Resize the control to fit the bitmap
   if (resize) 
   {
      setExtent(mTextureObject->getWidth(), mTextureObject->getHeight());
      updateSizing();
   }
}
*/

void GuiWebViewCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   mWebViewTexture.refresh();

   if (mWebViewTexture.getTexture())
   {
      GFX->getDrawUtil()->clearBitmapModulation();
		if(mWrap)
		{
         // We manually draw each repeat because non power of two textures will 
         // not tile correctly when rendered with GFX->drawBitmapTile(). The non POT
         // bitmap will be padded by the hardware, and we'll see lots of slack
         // in the texture. So... lets do what we must: draw each repeat by itself:
 			GFXTextureObject* texture = mWebViewTexture.getTexture();
			RectI srcRegion;
			RectI dstRegion;
			float xdone = ((float)getExtent().x/(float)texture->mBitmapSize.x)+1;
			float ydone = ((float)getExtent().y/(float)texture->mBitmapSize.y)+1;

			int xshift = mStartPoint.x%texture->mBitmapSize.x;
			int yshift = mStartPoint.y%texture->mBitmapSize.y;
			for(int y = 0; y < ydone; ++y)
				for(int x = 0; x < xdone; ++x)
				{
		 			srcRegion.set(0,0,texture->mBitmapSize.x,texture->mBitmapSize.y);
  					dstRegion.set( ((texture->mBitmapSize.x*x)+offset.x)-xshift,
								      ((texture->mBitmapSize.y*y)+offset.y)-yshift,
								      texture->mBitmapSize.x,
								      texture->mBitmapSize.y);
               GFX->getDrawUtil()->drawBitmapStretchSR(texture,dstRegion, srcRegion, GFXBitmapFlip_None, GFXTextureFilterLinear);
				}

		}
		else
      {
         //RectI rect(offset, Point2I(mWebViewTexture.getTexture().getWidth(), mWebViewTexture.getTexture().getHeight()));
         RectI rect(offset, getExtent());
         GFX->getDrawUtil()->drawBitmapStretch(mWebViewTexture.getTexture(), rect, GFXBitmapFlip_None, GFXTextureFilterLinear, false);
      }
   }

   if (mProfile->mBorder || !mWebViewTexture.getTexture())
   {
      RectI rect(offset.x, offset.y, getExtent().x, getExtent().y);
      GFX->getDrawUtil()->drawRect(rect, mProfile->mBorderColor);
   }

   renderChildControls(offset, updateRect);
}

void GuiWebViewCtrl::setValue(S32 x, S32 y)
{
   if (mWebViewTexture.getTexture())
   {
		x += mWebViewTexture.getWidth() / 2;
		y += mWebViewTexture.getHeight() / 2;
  	}
  	while (x < 0)
  		x += 256;
  	mStartPoint.x = x % 256;

  	while (y < 0)
  		y += 256;
  	mStartPoint.y = y % 256;
}


//--------------------------------------------------------------------------
// Keyboard events...
//bool GuiWebViewCtrl::onKeyDown(const GuiEvent& event)
//{
      /*
   //only cut/copy work with this control...
   if (event.modifier & SI_COPYPASTE)
   {
      switch(event.keyCode)
      {
         //copy
         case KEY_C:
         {
            //make sure we actually have something selected
            if (mSelectionActive)
            {
               copyToClipboard(mSelectionStart, mSelectionEnd);
               setUpdate();
            }
            return true;
         }
         
         default:
            break;
      }
   }
      */

//   mWebViewTexture.getWebView()->injectKeyboardEvent();


   // Otherwise, let the parent have the event...
 //  return Parent::onKeyDown(event);
//}

//--------------------------------------------------------------------------
// Mousing events...
void GuiWebViewCtrl::onMouseDown(const GuiEvent& event)
{
   if(!mActive)
      return;
/*
   Atom *hitAtom = findHitAtom(globalToLocalCoord(event.mousePoint));
   if(hitAtom && !mIsEditCtrl)
   {
      mouseLock();
      mHitURL = hitAtom->url;
      if (mHitURL)
         mHitURL->mouseDown = true;
   }
*/
//   setFirstResponder();
//   mouseLock();
/*
   mSelectionActive = false;
   mSelectionAnchor        = getTextPosition(globalToLocalCoord(event.mousePoint));
   mSelectionAnchorDropped = event.mousePoint;
   if (mSelectionAnchor < 0)
      mSelectionAnchor = 0;
   else if (mSelectionAnchor >= mTextBuffer.length())
      mSelectionAnchor = mTextBuffer.length();

   mVertMoveAnchorValid = false;

   setUpdate();
   */
   
   //   LEFT_MOUSE_BTN,
	//	MIDDLE_MOUSE_BTN,
	//	RIGHT_MOUSE_BTN

   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseDown(Awesomium::LEFT_MOUSE_BTN);

}

//--------------------------------------------------------------------------
void GuiWebViewCtrl::onMouseMove(const GuiEvent& event)
{
   if (!mActive)// || (getRoot()->getMouseLockedControl() != this))
      return;

   Point2I point = globalToLocalCoord(event.mousePoint);
   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseMove(point.x, point.y);

/*
   Atom *hitAtom = findHitAtom(globalToLocalCoord(event.mousePoint));
   bool down = false;

   //note mHitURL can't be set unless this is (!mIsEditCtrl)
   if(hitAtom && hitAtom->url == mHitURL)
      down = true;

   if(mHitURL && down != mHitURL->mouseDown)
      mHitURL->mouseDown = down;

   if (!mHitURL)
   {
      S32 newSelection = 0;
      newSelection = getTextPosition(globalToLocalCoord(event.mousePoint));
      if (newSelection < 0)
         newSelection = 0;
      else if (newSelection > mTextBuffer.length())
         newSelection = mTextBuffer.length();

      if (newSelection == mSelectionAnchor) 
      {
         mSelectionActive = false;
      }
      else if (newSelection > mSelectionAnchor) 
      {
         mSelectionActive = true;
         mSelectionStart  = mSelectionAnchor;
         mSelectionEnd    = newSelection - 1;
      }
      else 
      {
         mSelectionStart  = newSelection;
         mSelectionEnd    = mSelectionAnchor - 1;
         mSelectionActive = true;
      }
      setCursorPosition(newSelection);
      mDirty = true;
   }

   setUpdate();
   */
}

//--------------------------------------------------------------------------
void GuiWebViewCtrl::onMouseUp(const GuiEvent& event)
{
   if (!mActive)// || (getRoot()->getMouseLockedControl() != this))
      return;

   Parent::onMouseUp(event);
////   mouseUnlock();


   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseUp(Awesomium::LEFT_MOUSE_BTN);


/*
   //see if we've clicked on a URL
   Atom *hitAtom = findHitAtom(globalToLocalCoord(event.mousePoint));
   if (mHitURL && hitAtom && hitAtom->url == mHitURL && mHitURL->mouseDown)
   {
      mHitURL->mouseDown = false;

      // Convert URL from UTF16 to UTF8.
      UTF8* url = mTextBuffer.createSubstring8(mHitURL->textStart, mHitURL->len);
	  onURL_callback(url);

      delete[] url;
      mHitURL = NULL;

      setUpdate();
      return;
   }

   //else, update our selection
   else
   {
      if ((event.mousePoint - mSelectionAnchorDropped).len() < 3)
         mSelectionActive = false;

      setCursorPosition(getTextPosition(globalToLocalCoord(event.mousePoint)));
      mVertMoveAnchorValid = false;
      setUpdate();
   }
   */
}

void GuiWebViewCtrl::onMouseDragged(const GuiEvent &event)
{
   if(!mActive)
      return;

   Parent::onMouseDragged(event);

   Point2I point = globalToLocalCoord(event.mousePoint);
   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseMove(point.x, point.y);
}
void GuiWebViewCtrl::onMouseEnter(const GuiEvent &event)
{
   Parent::onMouseDragged(event);
}
void GuiWebViewCtrl::onMouseLeave(const GuiEvent &event)
{
   Parent::onMouseLeave(event);
}

bool GuiWebViewCtrl::onMouseWheelUp(const GuiEvent &event)
{
   return Parent::onMouseWheelUp(event);
}
bool GuiWebViewCtrl::onMouseWheelDown(const GuiEvent &event)
{
   return Parent::onMouseWheelDown(event);
}

void GuiWebViewCtrl::onRightMouseDown(const GuiEvent &event)
{
   if (!mActive)
      return;

   Parent::onRightMouseDown(event);

   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseDown(Awesomium::RIGHT_MOUSE_BTN);
}
void GuiWebViewCtrl::onRightMouseUp(const GuiEvent &event)
{
   if (!mActive)
      return;

   Parent::onRightMouseUp(event);

   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseUp(Awesomium::RIGHT_MOUSE_BTN);
}
void GuiWebViewCtrl::onRightMouseDragged(const GuiEvent &event)
{
   if(!mActive)
      return;

   Parent::onRightMouseDragged(event);

   Point2I point = globalToLocalCoord(event.mousePoint);
   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseMove(point.x, point.y);
}

void GuiWebViewCtrl::onMiddleMouseDown(const GuiEvent &event)
{
   if (!mActive)
      return;

   Parent::onMiddleMouseDown(event);

   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseDown(Awesomium::MIDDLE_MOUSE_BTN);
}
void GuiWebViewCtrl::onMiddleMouseUp(const GuiEvent &event)
{
   if (!mActive)
      return;

   Parent::onMiddleMouseUp(event);

   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseUp(Awesomium::MIDDLE_MOUSE_BTN);
}
void GuiWebViewCtrl::onMiddleMouseDragged(const GuiEvent &event)
{
   if(!mActive)
      return;

   Parent::onMiddleMouseDragged(event);

   Point2I point = globalToLocalCoord(event.mousePoint);
   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectMouseMove(point.x, point.y);
}


bool GuiWebViewCtrl::onInputEvent(const InputEventInfo &event)
{
   return Parent::onInputEvent(event);
   /*
   // TODO - add POV support...
   if ( event.action == SI_MAKE )
   {
      if ( event.objType == SI_BUTTON
        || event.objType == SI_POV
        || ( ( event.objType == SI_KEY ) && !isModifierKey( event.objInst ) ) )
      {
       //  char deviceString[32];
       //  if ( !ActionMap::getDeviceName( event.deviceType, event.deviceInst, deviceString ) )
       //     return( false );

       //  char actionString[4];
       //  ActionMap::getKeyString( event.action, actionString );

		 //Con::executef( this, "onInputEvent", deviceString, actionString, "1" );
		 //onInputEvent_callback(deviceString, actionString, 1);
   	
	   Awesomium::WebKeyboardEvent keyEvent;
	   //keyEvent.text[0] = actionString[0];
	   //keyEvent.text[1] = actionString[1];
	   //keyEvent.text[2] = actionString[2];
	   //keyEvent.text[3] = actionString[3];
	   //keyEvent.unmodifiedText[0] = actionString[0];
	   //keyEvent.unmodifiedText[1] = actionString[1];
	   //keyEvent.unmodifiedText[2] = actionString[2];
	   //keyEvent.unmodifiedText[3] = actionString[3];
      keyEvent.type = Awesomium::WebKeyboardEvent::TYPE_KEY_DOWN;
      keyEvent.virtualKeyCode = event.objInst;
	   keyEvent.nativeKeyCode = event.objInst;
	   mWebViewTexture.getWebView()->injectKeyboardEvent(keyEvent);

         return true;
      }
   }
   else if ( event.action == SI_BREAK )
   {
      if ( ( event.objType == SI_KEY ) && isModifierKey( event.objInst ) )
      {
      //   char actionString[4];
      //   ActionMap::getKeyString( event.action, actionString );

		 //Con::executef( this, "onInputEvent", deviceString, actionString, "1" );
		 //onInputEvent_callback(deviceString, actionString, 1);
   	
	   Awesomium::WebKeyboardEvent keyEvent;
	 //  keyEvent.text[0] = actionString[0];
	 //  keyEvent.text[1] = actionString[1];
	 //  keyEvent.text[2] = actionString[2];
	 //  keyEvent.text[3] = actionString[3];
	 //  keyEvent.unmodifiedText[0] = actionString[0];
	 //  keyEvent.unmodifiedText[1] = actionString[1];
	 //  keyEvent.unmodifiedText[2] = actionString[2];
	 //  keyEvent.unmodifiedText[3] = actionString[3];
      keyEvent.type = Awesomium::WebKeyboardEvent::TYPE_KEY_UP;
      keyEvent.virtualKeyCode = event.objInst;
	   keyEvent.nativeKeyCode = event.objInst;
	   mWebViewTexture.getWebView()->injectKeyboardEvent(keyEvent);


         return true;
      }
   }
   return false;
*/
   /*
   switch(event.deviceType)
   {
   case MouseDeviceType:
      switch(event.action)
      {
      case SI_MAKE:
         mWebViewTexture.getWebView()->injectMouseDown(static_cast<Awesomium::MouseButton>(event.objInst - KEY_BUTTON0));
         return true;
      case SI_MOVE:
         {
            Point2I point = globalToLocalCoord(getRoot()->getCanvasCursorPos());
            mWebViewTexture.getWebView()->injectMouseMove(point.x, point.y);
         }
         return true;
      case SI_BREAK:
         mWebViewTexture.getWebView()->injectMouseUp(static_cast<Awesomium::MouseButton>(event.objInst - KEY_BUTTON0));
         return true;
      }
      break;
   }

   return false;
   */
//   if ( event.action == SI_MAKE )
//   {

     // static_cast<win32Window>(getRoot()->getPlatformWindow())
/*
      if(key == 8 || key == 127) // Backspace or Delete key
	   {
		   injectKey(Awesomium::KeyCodes::AK_BACK);
		   return;
	   }
	   else if(key == 9) // Tab key
	   {
		   injectKey(Awesomium::KeyCodes::AK_TAB);
		   return;
	   }
	   else if(key == 27) // Escape key
	   {
		   exit(0);
	   }
      */
//      int key = event.ascii;
   	
//	   Awesomium::WebKeyboardEvent keyEvent;
//	   keyEvent.text[0] = key;
//	   keyEvent.unmodifiedText[0] = key;
//	   keyEvent.type = Awesomium::WebKeyboardEvent::TYPE_CHAR;
//	   keyEvent.virtualKeyCode = key;
//	   keyEvent.nativeKeyCode = key;
//	   mWebViewTexture.getWebView()->injectKeyboardEvent(keyEvent);
     // mWebViewTexture.getWebView()->injectKeyboardEvent(Awesomium::WebKeyboardEvent(event.objInst));
//      return true;


/*
      if ( event.objType == SI_BUTTON
        || event.objType == SI_POV
        || ( ( event.objType == SI_KEY ) && !isModifierKey( event.objInst ) ) )
      {
       //  char deviceString[32];
       //  if ( !ActionMap::getDeviceName( event.deviceType, event.deviceInst, deviceString ) )
       //     return( false );

       //  const char* actionString = ActionMap::buildActionString( &event );

		 //Con::executef( this, "onInputEvent", deviceString, actionString, "1" );
		// onInputEvent_callback(deviceString, actionString, 1);


         return( true );
      }
      */
//   }
//   else if ( event.action == SI_BREAK )
//   {
      /*
      if ( ( event.objType == SI_KEY ) && isModifierKey( event.objInst ) )
      {
         char keyString[32];
         if ( !ActionMap::getKeyString( event.objInst, keyString ) )
            return( false );

         //Con::executef( this, "onInputEvent", "keyboard", keyString, "0" );
		 onInputEvent_callback("keyboard", keyString, 0);

         return( true );
      }
      */
     // static_cast<win32Window>(getRoot()->getPlatformWindow())
     // mWebViewTexture.getWebView()->injectKeyboardEvent(Awesomium::WebKeyboardEvent(event.objInst));
 //     return true;
 //  }

//   return false;
}

void GuiWebViewCtrl::handleKeyboard(WindowId did, UINT message, WPARAM wParam, WPARAM lParam)
{
  // const Awesomium::WebKeyboardEvent* key = new ;
   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->injectKeyboardEvent(Awesomium::WebKeyboardEvent(message, wParam, lParam));
}

Point2I GuiWebViewCtrl::getNativeExtent() const
{
   return (mWebViewTexture.isValid() ? mWebViewTexture.getWidthHeight() : getExtent());
}



DefineEngineMethod( GuiWebViewCtrl, setValue, void, ( S32 x, S32 y ),,
   "Set the offset of the bitmap within the control.\n"
   "@param x The x-axis offset of the image.\n"
   "@param y The y-axis offset of the image.\n")
{
   object->setValue(x, y);
}


//

static ConsoleDocFragment _sGuiWebViewCtrlSetURL1(
   "@brief Assign an image to the control.\n\n"
   "Child controls with resize according to their layout settings.\n"
   "@param filename The filename of the image.",
   "GuiWebViewCtrl", // The class to place the method in; use NULL for functions.
   "void setBitmap( String filename, bool resize );" ); // The definition string.

static ConsoleDocFragment _sGuiWebViewCtrlSetURL2(
   "@brief Assign an image to the control.\n\n"
   "Child controls will resize according to their layout settings.\n"
   "@param filename The filename of the image.\n"
   "@param resize A boolean value that decides whether the ctrl refreshes or not.",
   "GuiWebViewCtrl", // The class to place the method in; use NULL for functions.
   "void setBitmap( String filename );" ); // The definition string.


//"Set the bitmap displayed in the control. Note that it is limited in size, to 256x256."
ConsoleMethod( GuiWebViewCtrl, setURL, void, 3, 3,
   "( String filename | String filename ) Assign an image to the control.\n\n"
   "@hide" )
{
//   char filename[1024];
   object->setURL(argv[2]);
}

DefineEngineMethod( GuiWebViewCtrl, getNativeExtent, Point2I, (),,
   "( , bool resize ) Assign an image to the control.\n\n")
{
   return object->getNativeExtent();
}