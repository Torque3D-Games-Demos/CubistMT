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

#include <Awesomium/STLHelpers.h>


IMPLEMENT_CO_NETOBJECT_V1(GuiWebViewCtrl);

ConsoleDocClass( GuiWebViewCtrl,
   "@brief A gui control that is used to display a web browser.\n\n"
   
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
IMPLEMENT_CALLBACK( GuiWebViewCtrl, onGetScrollData, void, (U32 contentWidth, U32 contentHeight, U32 preferredWidth, S32 scrollX, S32 scrollY), (contentWidth, contentHeight, preferredWidth, scrollX, scrollY),
	"" 
);
IMPLEMENT_CALLBACK( GuiWebViewCtrl, onFinishLoading, void, (), (), "" );



GuiWebViewCtrl::GuiWebViewCtrl()
 : mURL(),
   mStartPoint( 0, 0 ),
   mWrap( false ),
   mLockAspect(false),
   mForceNativeExtent(false),
   mWebViewTexture(NULL)
{
}
GuiWebViewCtrl::~GuiWebViewCtrl()
{
   if(mWebViewTexture)
   {
      mWebViewTexture->destroy();
      delete mWebViewTexture;
      mWebViewTexture = NULL;
   }
}

void GuiWebViewCtrl::OnShowCreatedWebView(Awesomium::WebView* caller,
                                 Awesomium::WebView* new_view,
                                 const Awesomium::WebURL& opener_url,
                                 const Awesomium::WebURL& target_url,
                                 const Awesomium::Rect& initial_pos,
                                 bool is_popup)
{
 //  new_view->Resize(mResolution.x, mResolution.y);
   if(mForceNativeExtent)
      setExtent(mWebViewTexture->getWidthHeight());
   else if(mLockAspect)
      setExtent(getExtent());

   //WebTile* new_tile = new WebTile(new_view, mResolution.x, mResolution.y);
   mWebViewTexture->setWebView(*new_view);
   new_view->set_view_listener(this);
   new_view->set_load_listener(this);
   new_view->set_process_listener(this);
   mWebViewTexture->refresh();
}

void GuiWebViewCtrl::OnFinishLoadingFrame(Awesomium::WebView* caller,
                                 int64 frame_id,
                                 bool is_main_frame,
                                 const Awesomium::WebURL& url)
{
   //Con::printf("WebViewData::OnFinishLoadingFrame - %s", Awesomium::ToString(url.path()).c_str());
   if(is_main_frame)
   {
      //Con::printf("WebViewData::OnFinishLoadingFrame - main frame");
      if( mWebViewTexture ){
         if(mWebViewTexture->getWebView())
            mWebViewTexture->getWebView()->SetTransparent(true);
      }
   }
}



void GuiWebViewCtrl::OnMethodCall(Awesomium::WebView* caller,
                         unsigned int remote_object_id, 
                         const Awesomium::WebString& method_name,
                         const Awesomium::JSArray& args)
{
      // call bound script function
   // todo: make this efficient
 
   String key = String::ToString(remote_object_id) + '.' + String(Awesomium::ToString(method_name).c_str());
   CompoundKey<String,String> &entry = mScriptCallbackCommands[key];
   String tsFunctionName = entry.key1;
   String defualtArgs = entry.key2;

   if(tsFunctionName.isNotEmpty())
   {
      U32 argSize = args.size();
      tsFunctionName += '(';

      if(defualtArgs.isNotEmpty())
         tsFunctionName += defualtArgs + ','; 

      for(U32 i = 0; i < argSize; i++)
      {
         const Awesomium::JSValue& arg = args[i];
            tsFunctionName += String(Awesomium::ToString(arg.ToString()).c_str());

         if((i+1) != argSize)
            tsFunctionName += ',';
      }
      tsFunctionName += ')';
      tsFunctionName += ';';

      Con::evaluate(tsFunctionName);
   }

}
Awesomium::JSValue GuiWebViewCtrl::OnMethodCallWithReturnValue(Awesomium::WebView* caller,
                                           unsigned int remote_object_id,
                                           const Awesomium::WebString& method_name,
                                           const Awesomium::JSArray& args)
{
      // call bound script function
   // todo: make this efficient
  // String tsFunctionName = mScriptCallbackCommands[String::ToString(remote_object_id) + '.' + String(Awesomium::ToString(method_name).c_str())];
   String key = String::ToString(remote_object_id) + '.' + String(Awesomium::ToString(method_name).c_str());
   CompoundKey<String,String> &entry = mScriptCallbackCommands[key];
   String tsFunctionName = entry.key1;
   String defualtArgs = entry.key2;

   if(tsFunctionName.isNotEmpty())
   {
      U32 argSize = args.size();
      tsFunctionName += '(';

      if(defualtArgs.isNotEmpty())
         tsFunctionName += defualtArgs + ','; 

      for(U32 i = 0; i < argSize; i++)
      {
         const Awesomium::JSValue& arg = args[i];
            tsFunctionName += String(Awesomium::ToString(arg.ToString()).c_str());

         if((i+1) != argSize)
            tsFunctionName += ',';
      }
      tsFunctionName += ')';
      tsFunctionName += ';';

      return Awesomium::JSValue(Awesomium::WSLit(Con::evaluate(tsFunctionName)));
   }
   return Awesomium::JSValue();
}


/*
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


      mWebViewTexture->setSize(width, height);
      if(mForceNativeExtent)
         setExtent(mWebViewTexture->getWidthHeight());
      else if(mLockAspect)
         setExtent(getExtent());
   }
}
*/

bool GuiWebViewCtrl::setURLString( void *object, const char *index, const char *data )
{
   GuiWebViewCtrl *obj = static_cast<GuiWebViewCtrl *>( object );

   obj->setURL( data );
   return false;
}

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
    
   setActive(true);

   setURL(mURL);

   getRoot()->getPlatformWindow()->nativeKeyEvent.notify(this, &GuiWebViewCtrl::handleKeyboard);

   return true;
}

void GuiWebViewCtrl::onSleep()
{
   Parent::onSleep();
   getRoot()->getPlatformWindow()->nativeKeyEvent.remove(this, &GuiWebViewCtrl::handleKeyboard);
}

//-------------------------------------
void GuiWebViewCtrl::inspectPostApply()
{
   // if the extent is set to (0,0) in the gui editor and appy hit, this control will
   // set it's extent to be exactly the size of the bitmap (if present)
   Parent::inspectPostApply();

   if(!mWebViewTexture)
      return;

   if(!mWrap && (getExtent().x == 0) && (getExtent().y == 0) && mWebViewTexture->getTexture())
   {
      setExtent( mWebViewTexture->getWidth(), mWebViewTexture->getHeight());
   }
   else if(mLockAspect && mWebViewTexture->isValid())
   {
      F32 nativeWidth = mWebViewTexture->getWidth();
      F32 nativeHeight = mWebViewTexture->getHeight();
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
   if(mLockAspect && mWebViewTexture && mWebViewTexture->isValid())
   {
      F32 nativeWidth = mWebViewTexture->getWidth();
      F32 nativeHeight = mWebViewTexture->getHeight();
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

void GuiWebViewCtrl::updateSizing()
{
   if(!getParent())
      return;
   // updates our bounds according to our horizSizing and verSizing rules
   RectI fakeBounds( getPosition(), getParent()->getExtent());
   parentResized( fakeBounds, fakeBounds);
}

void GuiWebViewCtrl::setObjectCallbackScript(String objectName, String callbackName, String tsFunctionName, String args) 
{ 
   
   Awesomium::WebView *view = mWebViewTexture->getWebView();
   if(view)
   {
    //  view->setObjectCallback(objectName.utf16(), callbackName.utf16());
      view->set_js_method_handler(this);
      
      Awesomium::JSValue value = view->ExecuteJavascriptWithResult(Awesomium::WSLit(objectName.utf8()), Awesomium::WSLit(""));
      if(value.IsObject())
      {
         Awesomium::JSObject& jsObject = value.ToObject();
         String key = String::ToString(jsObject.remote_id()) + '.' + callbackName;
         // todo: use pairs
         CompoundKey<String,String> &entry = mScriptCallbackCommands[key];
         entry.key1 = tsFunctionName;
         entry.key2 = args;

         jsObject.SetCustomMethod(Awesomium::WSLit(callbackName.utf8()),false);
      }
   }
   
}

void GuiWebViewCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if(!mWebViewTexture)
      return;

   mWebViewTexture->refresh();

   if (mWebViewTexture->getTexture())
   {
      GFX->getDrawUtil()->clearBitmapModulation();
		if(mWrap)
		{
         // We manually draw each repeat because non power of two textures will 
         // not tile correctly when rendered with GFX->drawBitmapTile(). The non POT
         // bitmap will be padded by the hardware, and we'll see lots of slack
         // in the texture. So... lets do what we must: draw each repeat by itself:
 			GFXTextureObject* texture = mWebViewTexture->getTexture();
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
         GFX->getDrawUtil()->drawBitmapStretch(mWebViewTexture->getTexture(), rect, GFXBitmapFlip_None, GFXTextureFilterLinear, false);
      }
   }

   if (mProfile->mBorder || !mWebViewTexture->getTexture())
   {
      RectI rect(offset.x, offset.y, getExtent().x, getExtent().y);
      GFX->getDrawUtil()->drawRect(rect, mProfile->mBorderColor);
   }

   renderChildControls(offset, updateRect);
}

void GuiWebViewCtrl::setValue(S32 x, S32 y)
{
   if(!mWebViewTexture)
      return;

   if(mWebViewTexture->getTexture())
   {
		x += mWebViewTexture->getWidth() / 2;
		y += mWebViewTexture->getHeight() / 2;
  	}
  	while(x < 0)
  		x += 256;
  	mStartPoint.x = x % 256;

  	while(y < 0)
  		y += 256;
  	mStartPoint.y = y % 256;
}

//--------------------------------------------------------------------------
// Mousing events...
void GuiWebViewCtrl::onMouseDown(const GuiEvent& event)
{
   if(!mActive || !mWebViewTexture)
      return;

   if(mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseDown(Awesomium::kMouseButton_Left);

}

//--------------------------------------------------------------------------
void GuiWebViewCtrl::onMouseMove(const GuiEvent& event)
{
   if (!mActive)// || (getRoot()->getMouseLockedControl() != this))
      return;

   Point2I point = globalToLocalCoord(event.mousePoint);
   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseMove(point.x, point.y);
}

//--------------------------------------------------------------------------
void GuiWebViewCtrl::onMouseUp(const GuiEvent& event)
{
   if (!mActive)// || (getRoot()->getMouseLockedControl() != this))
      return;

   Parent::onMouseUp(event);
////   mouseUnlock();


   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseUp(Awesomium::kMouseButton_Left);

}

void GuiWebViewCtrl::onMouseDragged(const GuiEvent &event)
{
   if(!mActive)
      return;

   Parent::onMouseDragged(event);

   Point2I point = globalToLocalCoord(event.mousePoint);
   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseMove(point.x, point.y);
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

   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseDown(Awesomium::kMouseButton_Right);
}
void GuiWebViewCtrl::onRightMouseUp(const GuiEvent &event)
{
   if (!mActive)
      return;

   Parent::onRightMouseUp(event);

   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseUp(Awesomium::kMouseButton_Right);
}
void GuiWebViewCtrl::onRightMouseDragged(const GuiEvent &event)
{
   if(!mActive)
      return;

   Parent::onRightMouseDragged(event);

   Point2I point = globalToLocalCoord(event.mousePoint);
   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseMove(point.x, point.y);
}

void GuiWebViewCtrl::onMiddleMouseDown(const GuiEvent &event)
{
   if (!mActive)
      return;

   Parent::onMiddleMouseDown(event);

   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseDown(Awesomium::kMouseButton_Middle);
}
void GuiWebViewCtrl::onMiddleMouseUp(const GuiEvent &event)
{
   if (!mActive)
      return;

   Parent::onMiddleMouseUp(event);

   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseUp(Awesomium::kMouseButton_Middle);
}
void GuiWebViewCtrl::onMiddleMouseDragged(const GuiEvent &event)
{
   if(!mActive)
      return;

   Parent::onMiddleMouseDragged(event);

   Point2I point = globalToLocalCoord(event.mousePoint);
   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectMouseMove(point.x, point.y);
}


bool GuiWebViewCtrl::onInputEvent(const InputEventInfo &event)
{
   return Parent::onInputEvent(event);
}

void GuiWebViewCtrl::handleKeyboard(WindowId did, UINT message, WPARAM wParam, WPARAM lParam)
{
  // const Awesomium::WebKeyboardEvent* key = new ;
   if(mWebViewTexture && mWebViewTexture->getWebView())
      mWebViewTexture->getWebView()->InjectKeyboardEvent(Awesomium::WebKeyboardEvent(message, wParam, lParam));
}

Point2I GuiWebViewCtrl::getNativeExtent() const
{
   return (mWebViewTexture && mWebViewTexture->isValid() ? mWebViewTexture->getWidthHeight() : getExtent());
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
DefineEngineMethod( GuiWebViewCtrl, setURL, void, (String url),,
   "( String filename | String filename ) Assign an image to the control.\n\n"
   "@hide" )
{
   object->setURL(url);
}

DefineEngineMethod( GuiWebViewCtrl, getNativeExtent, Point2I, (),,
   "( , bool resize ) Assign an image to the control.\n\n")
{
   return object->getNativeExtent();
}

DefineEngineMethod( GuiWebViewCtrl, setObjectCallback, void, (String objectName, String callbackName, String tsFunctionName, String args),,
   "set a javascript object property.\n"
   "@param \n\n" )
{ 
   object->setObjectCallbackScript(objectName, callbackName, tsFunctionName, args);
}		