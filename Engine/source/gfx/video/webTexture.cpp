
#include <Awesomium/WebCore.h>


#include "platform/platform.h"
#include "console/engineAPI.h"
#include "core/stream/bitStream.h"

#include "gfx/gfxDevice.h"
#include "gfx/gfxTextureManager.h"

#include "materials/materialDefinition.h"
#include "T3D/gameBase/gameConnection.h"
#include "T3D/gameBase/gameProcess.h"

#include "gfx/video/webTexture.h"

//#define TORQUE_AWESOMIUM_VERSION = 1.72;
#if TORQUE_AWESOMIUM_VERSION >= 17
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>

// Various macro definitions
#define WIDTH	512
#define HEIGHT	512
//#define URL	"http://www.google.com"
#define UPDATE_DELAY_MS	25

#define   CLEAR_ON_NEW_URL

/// Profile for the video texture.
GFX_ImplementTextureProfile(  GFXWebViewTextureProfile,
                              GFXTextureProfile::DiffuseMap,
                              GFXTextureProfile::NoMipmap | GFXTextureProfile::Dynamic | GFXTextureProfile::PreserveSize,
                              GFXTextureProfile::None );


U32 WebViewTexture::webViewCount = 0;
Awesomium::WebSession* WebViewTexture::webSession = 0;
Awesomium::WebCore* WebViewTexture::webCore = 0;

WebViewTexture::WebViewTexture() :
            mWidth(512), mHeight(512),
            mWebView(NULL),
            mTexture(GFXTexHandle::ZERO)
{	
   WebViewTexture(mWidth, mHeight);
}
WebViewTexture::WebViewTexture(U32 width, U32 height) :
            mWidth(width), mHeight(height),
            mWebView(NULL),
            mTexture(GFXTexHandle::ZERO)
{	
}

//-----------------------------------------------------------------------------

WebViewTexture::~WebViewTexture()
{
   destroy();
   destroyWebCore();
}

//-----------------------------------------------------------------------------

void WebViewTexture::_onTextureEvent( GFXTexCallbackCode code )
{
   switch( code )
   {
      case GFXZombify:
         mWebView->PauseRendering();
         break;
         
      case GFXResurrect:
         mWebView->ResumeRendering();
         break;
   }
}

//-----------------------------------------------------------------------------


Awesomium::WebSession* WebViewTexture::getWebSession()
{
   return webSession;
}
Awesomium::WebView* WebViewTexture::getWebView()
{ 
   if(createWebCore()) 
      if(!mWebView)
      {

         mWebView = createWebCore()->CreateWebView(mWidth, mHeight, getWebSession());
         if(mWebView)
         {
            mWebView->ExecuteJavascript(Awesomium::WSLit("document.body.style.backgroundColor = 'transparent'"), Awesomium::WSLit(""));
            mWebView->SetTransparent(true);
            webViewCount++;
            return mWebView;
         }
         else
         {
            Con::warnf("WebViewTexture::setURL - failed to create new web page");
            return NULL;
         }
      }
      else
         return mWebView;
      
   return NULL;
}


bool WebViewTexture::setSize(U32 width, U32 height)
{
   mWidth = width;
   mHeight = height;

   if(mWidth == 0 || mHeight == 0)
      return false;

   if(mWebView)
      mWebView->Resize(width, height);

   return true;
}

//-----------------------------------------------------------------------------


bool WebViewTexture::setURL( const FileName& url, 
							 const String& frameName, 
							 const String& username, 
							 const String& password )
{
   if( url.isEmpty() )
      return false;
   
   clear();

   if(!getWebView())
      return false;

   mWebView->LoadURL(Awesomium::WebURL(Awesomium::WSLit(url.utf8())));

	mWebView->Focus();

   return true;
}

//-----------------------------------------------------------------------------

void WebViewTexture::destroy()
{
   if(mWebView)
   {
	   mWebView->Destroy();
      mWebView = 0;
      webViewCount--;
   }
      
   if(!mTexture.isNull())
      mTexture.free();
}


//-----------------------------------------------------------------------------

void WebViewTexture::refresh()
{
   if(!createWebCore())
   {
      return;
   }

	webCore->Update();

   if(!getWebView() || mWebView->IsLoading())
      return;

  const Awesomium::BitmapSurface* renderBuffer = static_cast<const Awesomium::BitmapSurface*>(mWebView->surface());
   if(!renderBuffer)
      return;
      
   //---
   if(!renderBuffer->is_dirty())
      return;

   if(mTexture.isNull() || mTexture == GFXTexHandle::ZERO)
      mTexture.set(renderBuffer->width(), renderBuffer->height(), GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));

   if(mTexture.isValid())
   {
      GFXLockedRect* lock = mTexture->lock();
      if(lock)
      {
         renderBuffer->CopyTo(lock->bits, lock->pitch, 4, false, false);
         mTexture->unlock();
      }
//      else
//         Con::printf("WebViewTexture::refresh - cancelled as no texture lock(2)");
   }
}



Awesomium::WebCore* WebViewTexture::createWebCore() 
{
   // Create our WebCore singleton with the default options
   
   if(!webCore)
   {
      Awesomium::WebConfig config;
#ifdef TORQUE_DEBUG
      config.log_level = Awesomium::kLogLevel_Verbose;
#else
      config.log_level = Awesomium::kLogLevel_None;
#endif

      config.remote_debugging_port = 1337;
//      config.plugin_path.Assign(Awesomium::WSLit(Platform::getCurrentDirectory()).Append(Awesomium::WSLit("\\plugins")));
//      config.user_agent.Assign(Awesomium::WSLit("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.1 (KHTML, like Gecko) Chrome/22.0.1207.1 Safari/537.1"));
//      config.user_agent = Awesomium::WSLit("Mozilla/5.0 (iPhone; CPU iPhone OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9B176 Safari/7534.48.3");
//      config.user_agent.Assign(Awesomium::WSLit("Mozilla/5.0 (Linux; U; Android 4.0.3; ko-kr; LG-L160L Build/IML74K) AppleWebkit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30"));
   //   config.setEnablePlugins(true);
	//	 config.setSaveCacheAndCookies(true);
		// config.setMaxCacheSize(256);
	//	 config.setMaxCacheSize(102400);
	//	 config.setDisableSameOriginPolicy(true);
	//	 config.setDisableWinMessagePump(true);
   //   config.package_path.Assign(Awesomium::ToWebString(Platform::getCurrentDirectory()));
   //   config.additional_options.Assign(Awesomium::WSLit("--no-sandbox --enable-video-track"));

//      config.additional_options.Push(Awesomium::WSLit("--enable-touch-events=true"));


      webCore = Awesomium::WebCore::Initialize(config);
      
      if(webCore)
      {
         Awesomium::WebPreferences prefs;
         prefs.allow_file_access_from_file_url = true;
         prefs.allow_universal_access_from_file_url = true;
         prefs.shrink_standalone_images_to_fit = false;
         prefs.allow_running_insecure_content = true;
    //     prefs.enable_app_cache = true;
    //     prefs.enable_dart = true;
    //     prefs.enable_databases = true;
    //     prefs.enable_javascript = true;
    //     prefs.enable_local_storage = true;
    //     prefs.enable_plugins = true;
    //     prefs.enable_remote_fonts = true;
    //     prefs.enable_web_audio = true;
    //     prefs.enable_web_audio = false;
         prefs.enable_web_gl = true;
    //     prefs.enable_web_gl = false;
         prefs.enable_web_security = false;
         webSession = webCore->CreateWebSession(Awesomium::WSLit(Platform::getCurrentDirectory()),prefs);
         
      }
   }

   return webCore;
}
void WebViewTexture::destroyWebCore() 
{
   // Create our WebCore singleton with the default options
   if(webViewCount == 0 && webCore)
   {
      if(webSession)
         webSession->Release();

// todo: destroy from script
 //     webCore->Shutdown();
 //     webCore = NULL;
   }

}
void WebViewTexture::updateWebCore()
{
//   if(webCore)
//      webCore->update();
}


void WebViewTexture::clear()
{
   if(mWebView)
      mWebView->PauseRendering();

   if(mTexture.isValid())
   {
      GFXLockedRect* lock = mTexture->lock();
      if(lock){
         for(int iy=0; iy<mTexture.getHeight(); iy++)
         {
            U8 *to = (U8 *)lock->bits;
            to += (iy*lock->pitch);
            memset(to, 0, mTexture.getWidth()*4);
         }
         mTexture->unlock();
      }
   }
}

// todo: use this class to update clients faster
class WebViewDataURLEvent : public NetEvent
{
	String mFrame;
	FileName mURL;
	SimObjectPtr<WebViewData> mWebViewData;

public:
   typedef NetEvent Parent;
   WebViewDataURLEvent() {}
   WebViewDataURLEvent(WebViewData* webViewData, const FileName& url, const String& frame = "") :
			mWebViewData(webViewData), mURL(url), mFrame(frame)
   {

   }

   ~WebViewDataURLEvent()
   {
	//   dFree(msg);
   }

   virtual void pack(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mWebViewData) ? con->getGhostIndex(mWebViewData): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

	   bstream->write(mURL);   
	   bstream->write(mFrame);
   }
   virtual void write(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mWebViewData) ? con->getGhostIndex(mWebViewData): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

	   bstream->write(mURL);   
	   bstream->write(mFrame);
   }
   virtual void unpack(NetConnection* con, BitStream *bstream)
   {
       S32 gIndex = bstream->readInt(NetConnection::GhostIdBitSize);
       mWebViewData = static_cast<WebViewData*>(con->resolveObjectFromGhostIndex(gIndex));

	   bstream->read(&mURL);   
	   bstream->read(&mFrame);
   }
   virtual void process(NetConnection *con)
   {
	//   Con::printf("WebViewDataURLEvent: %s, %s, %s;", mWebViewData->getName(), mURL, mFrame);
	   if(mWebViewData.isValid())
		   mWebViewData->setURL(mURL, mFrame);
   }

   DECLARE_CONOBJECT(WebViewDataURLEvent);
};

IMPLEMENT_CO_NETEVENT_V1(WebViewDataURLEvent);

ConsoleDocClass( WebViewDataURLEvent,
	"@brief A.\n\n");
/*
DefineEngineStaticMethod( WebViewDataURLEvent, msg, void, (NetConnection* con, const char* message),,
   "@brief Send a WebViewDataURLEvent message to the specified connection.\n\n")
{
	//NetConnection *con = (NetConnection *) Sim::findObject(argv[1]);

	if(con)
		con->postNetEvent(new SimpleMessageEvent(message));
}
*/



class WebViewDataScrollEvent : public NetEvent
{
	String mFrame;
	Point2I mScroll;
	SimObjectPtr<WebViewData> mWebViewData;

public:
   typedef NetEvent Parent;
   WebViewDataScrollEvent() {}
   WebViewDataScrollEvent(WebViewData* webViewData, const Point2I& scroll, const String& frame = "") :
			mWebViewData(webViewData), mScroll(scroll), mFrame(frame)
   {
	//   if(message)
	//      msg = dStrdup(message);
	//   else
	//      msg = NULL;

   }

   ~WebViewDataScrollEvent()
   {
	//   dFree(msg);
   }

   virtual void pack(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mWebViewData) ? con->getGhostIndex(mWebViewData): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

	   bstream->write(mScroll.x);   
	   bstream->write(mScroll.y);   
	   bstream->write(mFrame);
   }
   virtual void write(NetConnection* con, BitStream *bstream)
   {
	   U32 gIndex = bool(mWebViewData) ? con->getGhostIndex(mWebViewData): -1;
	   bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);

	   bstream->write(mScroll.x);   
	   bstream->write(mScroll.y);   
	   bstream->write(mFrame);
   }
   virtual void unpack(NetConnection* con, BitStream *bstream)
   {
       S32 gIndex = bstream->readInt(NetConnection::GhostIdBitSize);
       mWebViewData = static_cast<WebViewData*>(con->resolveObjectFromGhostIndex(gIndex));

	   bstream->read(&(mScroll.x));   
	   bstream->read(&(mScroll.y));   
	   bstream->read(&mFrame);
   }
   virtual void process(NetConnection *con)
   {
	   if(mWebViewData.isValid())
		   mWebViewData->setScroll(mScroll, mFrame);
   }

   DECLARE_CONOBJECT(WebViewDataScrollEvent);
};

IMPLEMENT_CO_NETEVENT_V1(WebViewDataScrollEvent);

ConsoleDocClass( WebViewDataScrollEvent,
	"@brief A.\n\n");



IMPLEMENT_CO_NETOBJECT_V1(WebViewData);

ConsoleDocClass( WebViewData,
   "@brief Stores properties for an individual debris type.\n\n"

   "Critical references include a DTS that represents the mesh of the debris and a particle emitter that ejects the debris. "
   "The rest of the datablock properties determine physics and reaction to other game objects, such as water and terrain\n"

   "@tsexample\n"
   "datablock DebrisData(GrenadeDebris)\n"
   "{\n"
   "   shapeFile = \"art/shapes/weapons/ramrifle/debris.dts\";\n"
   "};\n"
   "@endtsexample\n\n"
   "@see Debris\n\n"
   "@ingroup FX\n"
);

IMPLEMENT_CALLBACK( WebViewData, onGetScrollData, void, (U32 contentWidth, U32 contentHeight, U32 preferredWidth, S32 scrollX, S32 scrollY), (contentWidth, contentHeight, preferredWidth, scrollX, scrollY),
	"" 
);
IMPLEMENT_CALLBACK( WebViewData, onFinishLoading, void, (), (), "" );


WebViewData::WebViewData() :
#ifdef   CLEAN_PAGE
      mPageLoaded(false),
#endif
      mResolution(0,0),
      mURL(), //mNamedTarget(),
      mResizeToNative(true),
      mDisableHScrollBar(false),
      mDisableVScrollBar(false),
      mScriptCallbackCommands(),
      mIsClientOnly(false),
      mWebViewTexture(NULL)
{
   // todo: scope this object properly from shape that uses it
   //ghostfix
//#ifdef   WEB_GHOSTING
   mNetFlags.set(Ghostable);
//#else
//  mNetFlags.set(Ghostable | ScopeAlways);
//#endif
   
}
WebViewData::~WebViewData()
{
   if(mWebViewTexture)
   {
      delete mWebViewTexture;
      mWebViewTexture = NULL;
   }
}

//------------------------------------------------------------------------------

bool WebViewData::onAdd()
{
   if(!Parent::onAdd())
      return false;

   return true;
}

void WebViewData::onRemove()
{  
   if(mWebViewTexture)
   {
      mWebViewTexture->destroy();
      mWebViewTexture = NULL;
   }

   Parent::onRemove();
}

void WebViewData::inspectPostApply()
{
   setMaskBits( 0xFFFFFFFF );
   
   Parent::inspectPostApply();
}

//------------------------------------------------------------------------------

void WebViewData::initPersistFields()
{
   addGroup("WebView");
   addField("url",              TypeStringFilename,     Offset(mURL,         WebViewData), "URL to load.");
   addField("resolution",        TypePoint2I,   Offset(mResolution,  WebViewData), "Resolution of texture.");
   addField("disableHScrollBar",        TypeBool,   Offset(mDisableHScrollBar,  WebViewData), "Resolution of texture.");
   addField("disableVScrollBar",        TypeBool,   Offset(mDisableVScrollBar,  WebViewData), "Resolution of texture.");
   addField("clientOnly",        TypeBool,   Offset(mIsClientOnly,  WebViewData), "Set clientOnly to true to stop url syncing.");
   
   endGroup("WebView");

   Parent::initPersistFields();
}

U32 WebViewData::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate( conn, mask, stream );

   if (stream->writeFlag((mask & ResolutionMask) || (mask & InitialUpdateMask)))
   {
      if (stream->writeFlag(mask & InitialUpdateMask))
      {
         stream->writeString(mSuperClassName);
         stream->writeString(mClassName);
      }
	   stream->write( mResolution.x );
	   stream->write( mResolution.y );
   }

   if (stream->writeFlag((mask & TargetURLMask) || (mask & InitialUpdateMask)))
   {
	   if(stream->writeFlag(mURL.isNotEmpty()))
      {
        stream->writeFlag(mIsClientOnly);
		  stream->write( mURL );
      }
   }

   if (stream->writeFlag(mask & FrameURLMask))
   {
      stream->write(mFrameUrl.size());
	  HashTable<String, FileName>::Iterator i = mFrameUrl.begin();
	  for(; i != mFrameUrl.end(); i++)
	  {
         stream->write( i->key );
         stream->write( i->value );
	  }
   }

   if (stream->writeFlag(mask & ScrollMask))
   {
	   stream->write(mScroll.x);   
	   stream->write(mScroll.y);   

	   /*
      stream->write(mFrameScroll.size());
	  HashTable<String, Point2F>::Iterator i = mFrameScroll.begin();
	  for(; i != mFrameScroll.end(); i++)
	  {
         stream->write( i->key );
         stream->write( i->value );
	  }
	  */
   }

   return retMask;
}

void WebViewData::unpackUpdate(NetConnection* conn, BitStream* stream)
{
   Parent::unpackUpdate(conn, stream);

   //mURL = stream->readString();
   if(stream->readFlag())
   {
      if(stream->readFlag())
      {
         setSuperClassNamespace(stream->readSTString());
         setClassNamespace(stream->readSTString());
      }
      S32 x, y;
	   stream->read(&(x));
	   stream->read(&(y));
      setResolution(x, y);
   }
   if(stream->readFlag())
   {
      String url;
	   if(stream->readFlag())
      {
         mIsClientOnly = stream->readFlag();

		   stream->read(&(url));
         //Con::printf("WebViewData::unpackUpdate - getWebViewTexture().setURL(%s)",url.c_str());
         mURL = url;

         if(!mWebViewTexture)
            mWebViewTexture = new WebViewTexture();
         if(mWebViewTexture)
         {

            mScriptCallbackCommands.clear();
#ifdef   CLEAN_PAGE
         mPageLoaded = false;
         mWebViewTexture->setPageLoaded(mPageLoaded);
#endif
            mWebViewTexture->setSize(mResolution.x,mResolution.y);
            mWebViewTexture->setURL(mURL);
           // ClientProcessList::get()->preTickSignal().notify( this, &WebViewData::refresh );
            ClientProcessList::get()->postTickSignal().notify( this, &WebViewData::refreshPageLoad, 1001.0f );
            if(mWebViewTexture->getWebView())
            {
               mWebViewTexture->getWebView()->set_view_listener(this);
               mWebViewTexture->getWebView()->set_load_listener(this);
               mWebViewTexture->getWebView()->set_process_listener(this);
               mWebViewTexture->refresh();
            }
            else
            {
               Con::printf("no web view!!");
            }
         }
         
      }

   }
   if(stream->readFlag())
   {
	   U32 count=0;
	   stream->read(&count);
	   for(U32 i=0; i<count; i++)
	   {
		   FileName *url = NULL;
		   String *frame = NULL;
		   
		   stream->read(frame);
		   stream->read(url);

		   if(frame && url && frame->isNotEmpty() && url->isNotEmpty())
		   {
            //Con::printf("WebViewData::unpackUpdate - getWebViewTexture().setURL(%s)",mURL.c_str());
		      mWebViewTexture->setURL(*url, *frame);
         //   mWebViewTexture.getWebView()->requestScrollData();
            refresh();
		   }
	   }
   }

   if(stream->readFlag())
   {
	   stream->read(&(mScroll.x));   
	   stream->read(&(mScroll.y));   

	   /*
	   U32 count=0;
	   stream->read(&count);
	   for(U32 i=0; i<count; i++)
	   {
		   Point2F *scroll = NULL;
		   String *frame = NULL;
		   
		   stream->read(frame);
		   stream->read(url);

		   if(frame && scroll && frame->isNotEmpty() && scroll->isNotEmpty())
		   {
			   mWebViewTexture.setScroll(*scroll, *frame);
		   }
	   }
	   */
   }
}

/*
void WebViewData::onGetScrollData(Awesomium::WebView* caller,
									 int contentWidth,
									 int contentHeight,
									 int preferredWidth,
									 int scrollX,
									 int scrollY)
{
	Con::printf("onGetScrollData");

   this->onGetScrollData_callback(contentWidth, contentHeight,
									   preferredWidth, scrollX, scrollY);

//	setScroll(Point2I(scrollX,scrollY));

}
*/
void WebViewData::setURL( const FileName& url, const String& frame )
{

   Con::printf("WebViewData::setURL %s",url.c_str());

   if(url.isEmpty())
      return;

	// todo: make sure we know whats really going on,
	// try and remove local mURL

	// only update this if 
	if(frame.isEmpty())
	{
      mScriptCallbackCommands.clear();

		if(isClientObject() && getWebViewTexture() && getWebViewTexture()->getWebView())
		{
			// update client
         Con::printf("WebViewData::setURL - getWebViewTexture().setURL(%s)",url.c_str());

         if(mIsClientOnly)
         {
   			getWebViewTexture()->setURL(url);

          //  ClientProcessList::get()->preTickSignal().notify( this, &WebViewData::refresh );
            ClientProcessList::get()->postTickSignal().notify( this, &WebViewData::refreshPageLoad, 1001.0f );
            
		   //	getWebViewTexture().getWebView()->requestScrollData();
         }
         else
         {
			   // sync client url changes back to server
	         GameConnection *con = GameConnection::getConnectionToServer();
            if(con)
			      con->postNetEvent(new WebViewDataURLEvent(this, url));
         }
		}
		else
      {
			setMaskBits(TargetURLMask);

		mURL = url;
      }
     
	}
	else
	{
		if(isClientObject())
		{
         clear();
			HashTable<String, FileName>::Iterator myUpdate = mFrameUrl.find(frame);
			if(myUpdate->value.compare(url) == 0)
			{
				mFrameUrl.erase(myUpdate);
				return;
			}

         if(mIsClientOnly)
         {
   			getWebViewTexture()->setURL(url, frame);
            refresh();
         }
         else
         {
			   // sync client url changes back to server
	         GameConnection *con = GameConnection::getConnectionToServer();
            if(con)
			      con->postNetEvent(new WebViewDataURLEvent(this, url, frame));
         }
		}
		else
		{
			mFrameUrl.insertUnique(frame, url);
			setMaskBits(FrameURLMask);
		}
	}
}

void WebViewData::refresh()                  
{
   if(mWebViewTexture)
   {

      mWebViewTexture->refresh(); 
      if(!mWebViewTexture->getTexture().isNull() && mWebViewTexture->getTexture() != GFXTexHandle::ZERO)
        // ClientProcessList::get()->preTickSignal().remove( this, &WebViewData::refresh );
         ClientProcessList::get()->postTickSignal().remove( this, &WebViewData::refreshPageLoad );
   }
}

bool  WebViewData::isSeeThruPixel(Point2F point)
{
   WebViewTexture* webViewTexture = getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)
      return true;

   GFXTextureObject* texture = webViewTexture->getTexture();
   if(!texture)
      return false;

   GFXLockedRect *lockedRect = texture->lock();
   int   alpha = 0;
   if(lockedRect){
      GFXFormat fmt = texture->getFormat();
      if(fmt==GFXFormatR8G8B8A8){
         F32 width = webViewTexture->getWidth();
         F32 height = webViewTexture->getHeight();
         int   x = (width * point.x) + (width/2.0f);
         int   y = (height * (-point.y)) + (height/2.0f);

         x = mClamp(x,0,width-1);          
         y = mClamp(y,0,height-1);          

         alpha = lockedRect->bits[ x*4 + y*lockedRect->pitch ];
      }
      texture->unlock();
   }
   return (alpha==0);
}

void WebViewData::setScroll( const Point2I& scroll, const String& frame )
{
	GameConnection *con = GameConnection::getConnectionToServer();

   if(mScroll.x == scroll.x && mScroll.y == scroll.y)
      return;

	if(frame.isEmpty())
	{
		if(isClientObject() && !(getWebViewTexture()->getWebView()))
			return;

		if(mScroll == scroll)
			return;

		if(isClientObject() && con)
		{
			// update client
			getWebViewTexture()->getWebView()->InjectMouseWheel(mScroll.x-scroll.x, mScroll.y-scroll.y);
		//	getWebViewTexture().getWebView()->requestScrollData();

			// sync client url changes back to server
			con->postNetEvent(new WebViewDataScrollEvent(this, scroll));
		}
		else
			setMaskBits(ScrollMask);

		mScroll = scroll;
	}
}
bool WebViewData::setResolution(U32 width, U32 height)  
{
   mResolution.x = width;
   mResolution.y = height;

   // server only
   if(isServerObject())
      setMaskBits(ResolutionMask);
   return true;
}


void WebViewData::setObjectCallbackScript(String objectName, String callbackName, String tsFunctionName, String args) 
{ 
   
   Awesomium::WebView *view = getWebViewTexture()->getWebView();
   if(view)
   {
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


void WebViewData::OnChangeTitle(Awesomium::WebView* caller,
                          const Awesomium::WebString& title)
{
}

void WebViewData::OnChangeAddressBar(Awesomium::WebView* caller,
                               const Awesomium::WebURL& url)
{
}

void WebViewData::OnChangeTooltip(Awesomium::WebView* caller,
                            const Awesomium::WebString& tooltip)
{
}

void WebViewData::OnChangeTargetURL(Awesomium::WebView* caller,
                              const Awesomium::WebURL& url)
{
}

void WebViewData::OnChangeCursor(Awesomium::WebView* caller,
                           Awesomium::Cursor cursor)
{
}

void WebViewData::OnChangeFocus(Awesomium::WebView* caller,
                             Awesomium::FocusedElementType focus_type)
{
}

void WebViewData::OnShowCreatedWebView(Awesomium::WebView* caller,
                                 Awesomium::WebView* new_view,
                                 const Awesomium::WebURL& opener_url,
                                 const Awesomium::WebURL& target_url,
                                 const Awesomium::Rect& initial_pos,
                                 bool is_popup)
{
   new_view->Resize(mResolution.x, mResolution.y);
   mWebViewTexture->setWebView(*new_view);
   new_view->set_view_listener(this);
   new_view->set_load_listener(this);
   new_view->set_process_listener(this);
   mWebViewTexture->refresh();
}

void WebViewData::OnBeginLoadingFrame(Awesomium::WebView* caller,
                                int64 frame_id,
                                bool is_main_frame,
                                const Awesomium::WebURL& url,
                                bool is_error_page)
{
   if(is_error_page)
      Con::printf("WebViewData::OnBeginLoadingFrame - error page for %s", Awesomium::ToString(url.path()).c_str());
}

void WebViewData::OnFailLoadingFrame(Awesomium::WebView* caller,
                               int64 frame_id,
                               bool is_main_frame,
                               const Awesomium::WebURL& url,
                               int error_code,
                               const Awesomium::WebString& error_description)
{
   Con::printf("WebViewData::OnFailLoadingFrame - failed to load %i : %s", error_code, Awesomium::ToString(error_description).c_str());
}

void WebViewData::OnFinishLoadingFrame(Awesomium::WebView* caller,
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
      refresh();

      onFinishLoading_callback();
   }
}

void WebViewData::OnDocumentReady(Awesomium::WebView* caller,
                                 const Awesomium::WebURL& url)
{
//Con::printf("WebViewData::OnDocumentReady - %s", Awesomium::ToString(url.path()).c_str());
}

void WebViewData::OnUnresponsive(Awesomium::WebView* caller)
{
//Con::printf("WebViewData::OnUnresponsive");
}

void WebViewData::OnResponsive(Awesomium::WebView* caller)
{
//Con::printf("WebViewData::OnResponsive");
}

void WebViewData::OnCrashed(Awesomium::WebView* caller,
                      Awesomium::TerminationStatus status)
{
//Con::printf("WebViewData::OnCrashed - %i", status);

}


void WebViewData::OnMethodCall(Awesomium::WebView* caller,
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
Awesomium::JSValue WebViewData::OnMethodCallWithReturnValue(Awesomium::WebView* caller,
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


DefineEngineMethod( WebViewData, injectMouseDown, void, (S32 button),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)
      return;

   webView->InjectMouseDown(static_cast<Awesomium::MouseButton>(button + Awesomium::kMouseButton_Left));
   object->refresh();
}
DefineEngineMethod( WebViewData, injectMouseUp, void, (S32 button),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)
      return;

   webView->InjectMouseUp(static_cast<Awesomium::MouseButton>(button + Awesomium::kMouseButton_Left));
   object->refresh();
}
DefineEngineMethod( WebViewData, injectMouseMove, void, (Point2F point),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)
      return;
   object->mMouseMove = point;

   F32 width = webViewTexture->getWidth();
   F32 height = webViewTexture->getHeight();
   webView->InjectMouseMove((width * point.x) + (width/2.0f), (height * (-point.y)) + (height/2.0f));
   object->refresh();
}
DefineEngineMethod( WebViewData, injectMouseWheel, void, (Point2F scroll),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)
      return;

   webView->InjectMouseWheel((int)((F32)webViewTexture->getWidth()*scroll.x), (int)((F32)webViewTexture->getHeight()*scroll.y));
   object->refresh();
}


DefineEngineMethod( WebViewData, injectTouchDown, void, (U32 touchId, Point2F point),,
   "Inject a touch event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   if(!webViewTexture || touchId >= 8)
      return;
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;

   F32 width = webViewTexture->getWidth();
   F32 height = webViewTexture->getHeight();
   U32 x = U32((width * point.x) + (width/2.0f));
   U32 y = U32((height * (-point.y)) + (height/2.0f));

   Awesomium::WebTouchEvent& touchEvents = webViewTexture->getTouchEvents();

   touchEvents.type = Awesomium::kWebTouchEventType_Start;

   // update target touch list
   touchEvents.target_touches_length++;
   Awesomium::WebTouchPoint& touch = touchEvents.target_touches[touchId];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Pressed;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;
      
   // update full touch list
   touchEvents.touches_length = touchEvents.target_touches_length;
   touch = touchEvents.touches[touchId];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Pressed;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;
      
   // update changed touch list
   touchEvents.changed_touches_length = 1;
   touch = touchEvents.changed_touches[0];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Pressed;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;

   webView->InjectTouchEvent(touchEvents);
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   object->refresh();
}
DefineEngineMethod( WebViewData, injectTouchMove, void, (U32 touchId, Point2F point),,
   "Inject a touch event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   if(!webViewTexture || touchId >= 8)
      return;
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;

   F32 width = webViewTexture->getWidth();
   F32 height = webViewTexture->getHeight();
   U32 x = U32((width * point.x) + (width/2.0f));
   U32 y = U32((height * (-point.y)) + (height/2.0f));

   Awesomium::WebTouchEvent& touchEvents = webViewTexture->getTouchEvents();

   touchEvents.type = Awesomium::kWebTouchEventType_Move;

   // update target touch list
//   touchEvents.target_touches_length++;
   Awesomium::WebTouchPoint& touch = touchEvents.target_touches[touchId];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Moved;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;
      
   // update full touch list
//   touchEvents.touches_length = touchEvents.target_touches_length;
   touch = touchEvents.touches[touchId];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Moved;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;
      
   // update changed touch list
   touchEvents.changed_touches_length = 1;
   touch = touchEvents.changed_touches[0];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Moved;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;

   webView->InjectTouchEvent(touchEvents);
   object->refresh();
}
DefineEngineMethod( WebViewData, injectTouchUp, void, (U32 touchId, Point2F point),,
   "Inject a touch event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   if(!webViewTexture || touchId >= 8)
      return;
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)
      return;

   F32 width = webViewTexture->getWidth();
   F32 height = webViewTexture->getHeight();
   U32 x = U32((width * point.x) + (width/2.0f));
   U32 y = U32((height * (-point.y)) + (height/2.0f));

   Awesomium::WebTouchEvent& touchEvents = webViewTexture->getTouchEvents();

   touchEvents.type = Awesomium::kWebTouchEventType_End;

   // update target touch list
   touchEvents.target_touches_length--;
   Awesomium::WebTouchPoint& touch = touchEvents.target_touches[touchId];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Released;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;
      
   // update full touch list
   touchEvents.touches_length = touchEvents.target_touches_length;
   touch = touchEvents.touches[touchId];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Released;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;
      
   // update changed touch list
   touchEvents.changed_touches_length = 1;
   touch = touchEvents.changed_touches[0];
   touch.id = touchId;
   touch.state = Awesomium::kWebTouchPointState_Pressed;
   touch.screen_position_x = x;
   touch.screen_position_y = y;
   touch.position_x = x;
   touch.position_y = y;
   touch.radius_x = 1;
   touch.radius_y = 1;
   touch.rotation_angle = 0;
   touch.force = 0;

   webView->InjectTouchEvent(touchEvents);
   object->refresh();
}


DefineEngineMethod( WebViewData, injectKeyboardEvent, void, (String text),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)
      return;

   Awesomium::WebKeyboardEvent event;
   event.type = Awesomium::WebKeyboardEvent::kTypeChar;
   event.text[0] = text[0];

   webView->InjectKeyboardEvent(event);
   object->refresh();
}

DefineEngineMethod( WebViewData, setURL, void, (String text),,
   "set url for webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
	object->setURL(text);
}
DefineEngineMethod( WebViewData, getURL, const char*, (),,
   "get current url.\n" )
{ 
	return object->getURL()->c_str();
}
DefineEngineMethod( WebViewData, setResolution, void, (Point2I res),,
   "set resolution of webpage.\n"
   "@param \n\n" )
{ 
	object->setResolution(res.x, res.y);
}

DefineEngineMethod( WebViewData, getResolution, Point2I, (),,
   "get resolution of webpage.\n"
   "@param \n\n" )
{ 
	return object->getResolution();
}

DefineEngineMethod( WebViewData, createObject, void, (String objectName),,
   "create a javascript object.\n"
   "@param \n\n" )
{ 
   Awesomium::WebView *view = object->getWebViewTexture()->getWebView();
   if(view)
      view->CreateGlobalJavascriptObject(Awesomium::WSLit(objectName.utf8()));
}

DefineEngineMethod( WebViewData, clear, void, (),,
   "clear the texture for the webview.\n"
   "@param \n\n" )
{ 
   object->clear();
}
	
DefineEngineMethod( WebViewData, setObjectCallback, void, (String objectName, String callbackName, String tsFunctionName, String args),,
   "set a javascript object property.\n"
   "@param \n\n" )
{ 
   object->setObjectCallbackScript(objectName, callbackName, tsFunctionName, args);
}		

DefineEngineFunction( preloadWebCore, void, (),,
   "preload webcore to avoid pause on first use.\n"
   "@param \n\n" )
{ 
   WebViewTexture::createWebCore();
}

#endif