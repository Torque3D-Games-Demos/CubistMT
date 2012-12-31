
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
                          //    GFXTextureProfile::NoMipmap | GFXTextureProfile::Dynamic | GFXTextureProfile::PreserveSize | GFXTextureProfile::KeepBitmap /*| GFXTextureProfile::NoPadding*/,
                          //    GFXTextureProfile::NoMipmap | GFXTextureProfile::NoPadding | GFXTextureProfile::PreserveSize | GFXTextureProfile::SystemMemory,
                             //   GFXTextureProfile::NoMipmap | GFXTextureProfile::Dynamic | GFXTextureProfile::PreserveSize | GFXTextureProfile::Pooled | GFXTextureProfile::NoPadding/* | GFXTextureProfile::KeepBitmap*/,
                              //  GFXTextureProfile::NoMipmap | GFXTextureProfile::Dynamic | GFXTextureProfile::PreserveSize/* | GFXTextureProfile::NoPadding | GFXTextureProfile::KeepBitmap*/,
                                GFXTextureProfile::NoMipmap | GFXTextureProfile::Dynamic | GFXTextureProfile::PreserveSize/* | GFXTextureProfile::KeepBitmap*/,
                                GFXTextureProfile::None );

/*
//=============================================================================
//    WebViewData implementation.
//=============================================================================
IMPLEMENT_CO_DATABLOCK_V1(WebViewData);

//-----------------------------------------------------------------------------

WebViewData::WebViewData() : mWebView(NULL), mWidth(512), mHeight(512)
{
}

//-----------------------------------------------------------------------------

WebViewData::~WebViewData()
{
   GFXTextureManager::removeEventDelegate( this, &WebViewData::_onTextureEvent );

   // Destroy our WebView instance
   if(mWebView)
   {
	   mWebView->destroy();
      mWebView = NULL;
   }

	
	// Destroy our WebCore instance
	//delete webCore;
}

void WebViewData::initPersistFields()
{
 //  addField("explosionShape",  TypeFilename,                Offset(dtsFileName,     ExplosionData), "DTS shape to place at the center of the explosion. The 'ambient' animation of this model will automatically be played at the start of the explosion.");
   Parent::initPersistFields();
}
bool WebViewData::onAdd()
{
      if (Parent::onAdd() == false)
      return false;

   setURL("http://www.google.com");

   return true;
}
void WebViewData::packData(BitStream* stream)
{
   Parent::packData(stream);

}
void WebViewData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
}
bool WebViewData::preload(bool server, String &errorStr)
{
   if (Parent::preload(server, errorStr) == false)
      return false;
      
   if( !server )
   {


   }
   return true;
}
//-----------------------------------------------------------------------------

void WebViewData::_onTextureEvent( GFXTexCallbackCode code )
{
   switch( code )
   {
      case GFXZombify:
         
       //  mCurrentFrame = NULL;
       //  if( mAsyncState )
       //  {
            // Blast out work items and then release all texture locks.
            
       //     ThreadPool::GLOBAL().flushWorkItems();
       //     mAsyncState->getFrameStream()->releaseTextureLocks();
            
            // The Theora decoder does not implement seeking at the moment,
            // so we absolutely want to make sure we don't fall behind too far or
            // we may end up having the decoder go crazy trying to skip through
            // Ogg packets (even just reading these undecoded packets takes a
            // lot of time).  So, for the time being, just pause playback when
            // we go zombie.
            
         //   if( mSFXSource )
         //      mSFXSource->pause();
         //   else
         //      mPlaybackTimer.pause();
         //}
         
         break;
         
      case GFXResurrect:
         
       //  if( mAsyncState )
       //  {
            // Reacquire texture locks.
            
       //     mAsyncState->getFrameStream()->acquireTextureLocks();
            
            // Resume playback if we have paused it.
            
        //    if( !mIsPaused )
        //    {
        //       if( mSFXSource )
        //          mSFXSource->play();
        //       else
        //          mPlaybackTimer.start();
        //    }
        // }
         
         break;
   }
}

//-----------------------------------------------------------------------------

U32 WebViewData::getWidth() const
{
   return mWidth;
}

//-----------------------------------------------------------------------------

U32 WebViewData::getHeight() const
{
   return mHeight;
}

//-----------------------------------------------------------------------------

bool WebViewData::setURL( const String& url )
{
   if( url.isEmpty() )
      return true;
   
	// Create our WebCore singleton with the default options
	//webCore = new Awesomium::WebCore();
	
	// Create a new WebView instance with a certain width and height, using the 
	// WebCore we just created
	mWebView = webCore->createWebView(mWidth, mHeight);
	
	// Load a certain URL into our WebView instance
	mWebView->loadURL(url.c_str());

	mWebView->focus();

   GFXTextureManager::addEventDelegate( this, &WebViewData::_onTextureEvent );

   // Create an RGB conditioner
//   mConditioner = new AdvancedLightBufferConditioner( GFXFormatR8G8B8A8, AdvancedLightBufferConditioner::RGB );
//   mNamedTarget.setConditioner( mConditioner ); 
   mNamedTarget.registerWithName( this->getClassName() );

   return true;
}

//-----------------------------------------------------------------------------

void WebViewData::refresh()
{
	webCore->update();

	// Call our display func when the WebView needs rendering
	if(!mWebView->isDirty())
      return;

	const Awesomium::RenderBuffer* renderBuffer = mWebView->render();

  // mTexture.set(renderBuffer->width, renderBuffer->height, static_cast<U32>(32), static_cast<void*>(renderBuffer->buffer), GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, "");
   mTexture.set(new GBitmap(renderBuffer->width, renderBuffer->height, renderBuffer->buffer), &GFXWebViewTextureProfile, true, "");
   mNamedTarget.setTexture(mTexture);
}
*/
//-----------------------------------------------------------------------------
//=============================================================================
//    WebViewData implementation.
//=============================================================================
//IMPLEMENT_CO_DATABLOCK_V1(WebViewTexture);

//-----------------------------------------------------------------------------
//static Awesomium::WebView* mWebView = 0;
//static Awesomium::WebCore* webCore = 0;

//static U32 gWebViewCount = 0;
U32 WebViewTexture::webViewCount = 0;
Awesomium::WebSession* WebViewTexture::webSession = 0;
Awesomium::WebCore* WebViewTexture::webCore = 0;

WebViewTexture::WebViewTexture() :
            mWidth(512), mHeight(512),
            mWebView(NULL),
            mTexture(GFXTexHandle::ZERO)
         //   mBitmap(NULL)
{	
	// Create a new WebView instance with a certain width and height, using the 
	// WebCore we just created
//	mWebView = *webCore->createWebView(mWidth, mHeight);
   WebViewTexture(mWidth, mHeight);
//   webViewCount++;
#ifdef   CLEAN_PAGE
   mPageLoaded = false;
#endif

//   ClientProcessList::get()->preTickSignal().notify( this, &WebViewTexture::refresh );
}
WebViewTexture::WebViewTexture(U32 width, U32 height) :
            mWidth(width), mHeight(height),
            mWebView(NULL),
            mTexture(GFXTexHandle::ZERO)
         //   mBitmap(NULL)
{	
	// Create a new WebView instance with a certain width and height, using the 
	// WebCore we just created
//	mWebView = *webCore->createWebView(mWidth, mHeight);
//   setSize();
#ifdef   CLEAN_PAGE
   mPageLoaded = false;
#endif
}

//-----------------------------------------------------------------------------

WebViewTexture::~WebViewTexture()
{
   
 //     if(mWebView)
 //     {
 //  	   mWebView->Destroy();
      //   webViewCount--;
 //     }
      
  //    ClientProcessList::get()->preTickSignal().remove( this, &WebViewTexture::refresh );

      destroy();
   //   if(mTexture)
   //      mTexture->destroySelf();

   //   mWebView = NULL;
  // }

      destroyWebCore();
}

//-----------------------------------------------------------------------------

void WebViewTexture::_onTextureEvent( GFXTexCallbackCode code )
{
   switch( code )
   {
      case GFXZombify:
         
       //  mCurrentFrame = NULL;
       //  if( mAsyncState )
       //  {
            // Blast out work items and then release all texture locks.
            
       //     ThreadPool::GLOBAL().flushWorkItems();
       //     mAsyncState->getFrameStream()->releaseTextureLocks();
            
            // The Theora decoder does not implement seeking at the moment,
            // so we absolutely want to make sure we don't fall behind too far or
            // we may end up having the decoder go crazy trying to skip through
            // Ogg packets (even just reading these undecoded packets takes a
            // lot of time).  So, for the time being, just pause playback when
            // we go zombie.
            
         //   if( mSFXSource )
         //      mSFXSource->pause();
         //   else
         //      mPlaybackTimer.pause();
         //}
         mWebView->PauseRendering();
         
         break;
         
      case GFXResurrect:
         mWebView->ResumeRendering();
      //   mTexture.refresh();
       //  refresh();
         
       //  if( mAsyncState )
       //  {
            // Reacquire texture locks.
            
       //     mAsyncState->getFrameStream()->acquireTextureLocks();
            
            // Resume playback if we have paused it.
            
        //    if( !mIsPaused )
        //    {
        //       if( mSFXSource )
        //          mSFXSource->play();
        //       else
        //          mPlaybackTimer.start();
        //    }
        // }
         
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
   {
   //   mWebView->Destroy();
   //   mTexture.free();
      return false;
   }

   if(mWebView)
      mWebView->Resize(width, height);
//   if(!mWebView)
//      return false;

//   mWebView->Resize(width, height);
//   mTexture.set(width, height, GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));

//   if(mTexture.isValid())
//      mTexture.free();
 //  mTexture = GFXTexHandle::ZERO;

   
//   mTexture.set(width, height, GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));
//   GFXLockedRect* lock = mTexture->lock();
//   if(lock){
//      dMemset(lock->bits, 0, sizeof(width*height*4));
//      mTexture->unlock();
//   }else{
//         Con::printf("WebViewTexture::setSize - failed to lock texture");
//   } 
   
   
//   updateWebCore();

//   refresh();

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
   
 
//	updateWebCore();
   clear();

   if(!getWebView())
      return false;
/*
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
*/

//   if(mWebView->IsLoading())
//      mWebView->Stop();

   // clear texture for old page
   // todo: we should be able to clear this
 //  if(mTexture.isValid())
 //  {
 //     mTexture.free();
 //     mTexture = GFXTexHandle::ZERO;
 //  }

	// Load a certain URL into our WebView instance
  // if(mWebView->getURL().compare(url) == 0)
//   if(mWebView->url GetURL().compare(url) == 0)
//      return true;



   /*
   // this is just silly
   std::string str = url.utf8();
   if(frameName.isEmpty())
	mWebView->loadURL(str);
   else
	mWebView->loadURL(str,frameName.utf16(),username.utf8(),password.utf8());
*/
//	mWebView->loadURL(url.utf16(),frameName.utf16(),username.utf8(),password.utf8());

 //  mWebView->Resize(mResolution.x, mResolution.y);
   mWebView->LoadURL(Awesomium::WebURL(Awesomium::WSLit(url.utf8())));

	mWebView->Focus();

//  updateWebCore();

//   GFXTextureManager::addEventDelegate( this, &WebViewTexture::_onTextureEvent );

   // Create an RGB conditioner
//   mConditioner = new AdvancedLightBufferConditioner( GFXFormatR8G8B8A8, AdvancedLightBufferConditioner::RGB );
//   mNamedTarget.setConditioner( mConditioner ); 
//   mNamedTarget.registerWithName( this->getClassName() );
 //  refresh();
   return true;
}
/*
bool WebViewTexture::setLocalFile( const FileName& url, 
							 const String& frameName )
{
   if( url.isEmpty() )
      return true;
   
  
	// Load a certain URL into our WebView instance

   
	//mWebView->LoadFile(url.utf8(),frameName.utf16());
	mWebView->LoadURL(Awesomium::WebURL(Awesomium::ToWebString(url.utf8())));

//	mWebView->focus();

//   GFXTextureManager::addEventDelegate( this, &WebViewTexture::_onTextureEvent );

   // Create an RGB conditioner
//   mConditioner = new AdvancedLightBufferConditioner( GFXFormatR8G8B8A8, AdvancedLightBufferConditioner::RGB );
//   mNamedTarget.setConditioner( mConditioner ); 
//   mNamedTarget.registerWithName( this->getClassName() );

   refresh();
   return true;
}
*/

//-----------------------------------------------------------------------------

void WebViewTexture::destroy()
{
   if(mWebView)
   {
	//   mWebView->destroy();
	   mWebView->Destroy();
      mWebView = 0;
      webViewCount--;
   }
      
   if(!mTexture.isNull())
      mTexture.free();

   //   if(mTexture)
   //   {
   //      mTexture->destroySelf();
   //      mTexture->is
   //   }
   /*
//   GFXTextureManager::removeEventDelegate( this, &WebViewTexture::_onTextureEvent );

   // Destroy our WebView instance
  // if(mWebView)
  // {
      if(mWebView)
      {
   	   mWebView->destroy();
         webViewCount--;
      }
      if(mTexture)
         mTexture->destroySelf();

   //   mWebView = NULL;
  // }

      destroyWebCore();
	// Destroy our WebCore instance
//   if(gWebViewCount == 0 && webCore)
//    delete webCore;
//
*/
}


//-----------------------------------------------------------------------------

/*
void WebViewTexture::refresh()
{
   if(!webCore)
      return;

	webCore->update();

	// Call our display func when the WebView needs rendering
	if(!mWebView.isDirty())
      return;

	const Awesomium::RenderBuffer* renderBuffer = mWebView.render();

  // mTexture.set(renderBuffer->width, renderBuffer->height, static_cast<U32>(32), static_cast<void*>(renderBuffer->buffer), GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, "");
  // GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height, renderBuffer->buffer);
  // bitmap->setFormat(GFXFormatB8G8R8A8);
   GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height, GFXFormatB8G8R8A8, renderBuffer->buffer, true);
//   bitmap->createPaddedBitmap
   mTexture.set(bitmap, &GFXWebViewTextureProfile, true, "");
//   mNamedTarget.setTexture(mTexture);
}
*/
#ifdef   CLEAN_PAGE
void  WebViewTexture::setPageLoaded(bool _flag)
{
   if(_flag==mPageLoaded)return;

   mPageLoaded = _flag;

#ifdef   CLEAR_ON_NEW_URL
   if(!mPageLoaded  && mTexture.isValid() && mTexture != GFXTexHandle::ZERO)
   {
      GFXLockedRect* lock = mTexture->lock();
      if(lock){
         for(int iy=0; iy<mTexture.getHeight(); iy++){
            U8 *to = (U8 *)lock->bits;
            to += (iy*lock->pitch);
            memset(to, 0, mTexture.getWidth()*4);
         }
         mTexture->unlock();
      }
   }
#endif
}
#endif
void WebViewTexture::refresh()
{
//	updateWebCore();

   if(!createWebCore())
   {
   //   Con::printf("WebViewTexture::refresh - no webcore or webview");
      return;
   }
  // else


//   if(getWebView())
//      mWebView->Focus();

	webCore->Update();

   if(!getWebView() || mWebView->IsLoading())
  // if(!getWebView())
      return;


 //  mWebView->ResumeRendering();

//	if(!mWebView || mWebView->IsLoadingPage() || !mWebView->isDirty())
//   {
////      Con::printf("WebViewTexture::refresh cancelled - isLoading = %d: isDirty = %d",
////         (int)mWebView->isLoadingPage(),
////         (int)mWebView->isDirty()
////         );
//      return;
//   }

   /*
	const Awesomium::RenderBuffer* renderBuffer = mWebView->render();
   if(!renderBuffer){
//      Con::printf("WebViewTexture::refresh - cancelled as no render buffer");
      return;
   }
   */

  // Get the WebView's rendering Surface. The default Surface is of
  // type 'BitmapSurface', we must cast it before we can use it.

        //    mWebView->ExecuteJavascript(Awesomium::WSLit("document.body.style.backgroundColor = 'transparent'"), Awesomium::WSLit(""));
         //   mWebView->SetTransparent(true);
   
  const Awesomium::BitmapSurface* renderBuffer = static_cast<const Awesomium::BitmapSurface*>(mWebView->surface());
   if(!renderBuffer)
      return;
      
   //---
   if(!renderBuffer->is_dirty())
      return;


/*
   if(!mBitmap || mWidth != renderBuffer->width || mHeight != renderBuffer->height)
   {
      mBitmap = new GBitmap(renderBuffer->width, renderBuffer->height, false, GFXFormatR8G8B8A8);
   //   mBitmap->createPaddedBitmap();
   //   mBitmap->allocateBitmap(mBitmap->getWidth(),mBitmap->getHeight(),false, GFXFormatR8G8B8A8);

      renderBuffer->copyTo(mBitmap->getWritableBits(), mBitmap->getWidth(), 4, true);
      mTexture.set(mBitmap, &GFXWebViewTextureProfile, true, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));


      mTexture.set(renderBuffer->width, renderBuffer->height, GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));
      renderBuffer->copyTo(mTexture.getBitmap()->getWritableBits(), mTexture.getBitmap()->getWidth(), 4, true);
   }
   else
   {
      if(mTexture.getBitmap())
         renderBuffer->copyTo(mTexture.getBitmap()->getWritableBits(), mTexture.getBitmap()->getWidth(), 4, true);
   }
   */
//	webCore->update();
   /*
   if(!mTexture.isValid() || mTexture->getWidth() != renderBuffer->width || mTexture->getHeight() != renderBuffer->height)
   {
   //   mTexture.free();
      mTexture.set(renderBuffer->width, renderBuffer->height, GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));
      //   mTexture.refresh();
         if(mTexture.isValid())
         {
            GFXLockedRect* lock = mTexture->lock();
            if(lock)
            {
               renderBuffer->copyTo(lock->bits, lock->pitch, 4, false, false);
            mTexture->unlock();
         //   mTexture.refresh();
            }
            else
            {
      //         Con::printf("WebViewTexture::refresh - cancelled as no texture lock(1)");
            }

         }
      }
   else // if(mTexture.isValid())
   {
      */

   if(mTexture.isNull() || mTexture == GFXTexHandle::ZERO)
      mTexture.set(renderBuffer->width(), renderBuffer->height(), GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));

   if(mTexture.isValid())
   {
      GFXLockedRect* lock = mTexture->lock();
      if(lock){
#ifdef   CLEAN_PAGE
         if(mPageLoaded){
#else
     //    if(true){
#endif
//            Con::printf("+++ ");
            renderBuffer->CopyTo(lock->bits, lock->pitch, 4, false, false);
            /*
         }else{
//            Con::printf("blank ");
#ifdef   CLEAR_ON_NEW_URL
            for(int iy=0; iy<mTexture.getHeight(); iy++){
               U8 *to = (U8 *)lock->bits;
               to += (iy*lock->pitch);
               memset(to, 0, mTexture.getWidth()*4);
            }
#endif
         }
         */
         mTexture->unlock();
      //   mTexture.refresh();
      }else{
//         Con::printf("WebViewTexture::refresh - cancelled as no texture lock(2)");
      }
   }

   //mTexture.set(renderBuffer->width, renderBuffer->height, 1, renderBuffer->buffer, GFXFormatB8G8R8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));






  // mTexture.set(renderBuffer->width, renderBuffer->height, static_cast<U32>(32), static_cast<void*>(renderBuffer->buffer), GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, "");
  // GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height, renderBuffer->buffer);
  // bitmap->setFormat(GFXFormatB8G8R8A8);

//  // GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height, GFXFormatB8G8R8A8, renderBuffer->buffer, true);
//   GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height);
////   bitmap->createPaddedBitmap
//   mTexture.set(bitmap, &GFXWebViewTextureProfile, true, "");
////   mNamedTarget.setTexture(mTexture);

//	webCore->update();

}



Awesomium::WebCore* WebViewTexture::createWebCore() 
{
   // Create our WebCore singleton with the default options
   
   if(!webCore)
   {
//      Awesomium::WebCoreConfig config = Awesomium::WebCoreConfig();
      /*
       *     enablePlugins           = false
		 *     enableJavascript        = true
		 *     enableDatabases         = false
		 *     packagePath             = (empty)
		 *     localePath              = (empty)
		 *     userDataPath            = (empty)
		 *     pluginPath              = (empty)
		 *     logPath                 = (empty)
		 *     logLevel                = LOG_NORMAL
		 *     forceSingleProcess      = false
		 *     childProcessPath        = (empty)
		 *     enableAutoDetectEncoding = true
		 *     acceptLanguageOverride  = (empty)
		 *     defaultCharsetOverride  = (empty)
		 *     userAgentOverride       = (empty)
		 *     proxyServer             = "auto"
		 *     proxyConfigScript       = (empty)
		 *     authServerWhitelist     = (empty)
		 *     saveCacheAndCookies     = false
		 *     maxCacheSize            = 0
		 *     disableSameOriginPolicy = false
		 *     disableWinMessagePump   = false
		 *     customCSS               = (empty)
      */

//      webCore = new Awesomium::WebCore(config);


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
   //      webCore->setBaseDirectory(Platform::getCurrentDirectory());
   //      Con::printf("WebViewTexture::createWebCore - WebCore created.");
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
         prefs.enable_web_audio = false;
         prefs.enable_web_gl = true;
    //     prefs.enable_web_gl = false;
         prefs.enable_web_security = false;
         webSession = webCore->CreateWebSession(Awesomium::WSLit(Platform::getCurrentDirectory()),prefs);
         
      }
   //   else
   //      return NULL;
   }
	//webCore->update();

   return webCore;
}
void WebViewTexture::destroyWebCore() 
{
   // Create our WebCore singleton with the default options
   if(webViewCount == 0 && webCore)
   {
//      delete webCore;
//      webCore = NULL;
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

 //  mTexture.free();
 //  mTexture = GFXTexHandle::ZERO;
//   if(mWebView)

   if(mWebView)
   {
      mWebView->PauseRendering();
   //   mWebView->Destroy();
   //   mWebView = NULL;
   }

//   mTexture = GFXTexHandle::ZERO;

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

//-----------------------------------------------------------------------------


//=============================================================================
//    WebCoreData implementation.
//=============================================================================

/*
IMPLEMENT_CONOBJECT( WebCoreData );

ConsoleDocClass(  WebCoreData, 
                  "@brief A state block description for rendering.\n\n"
                  "This object is used with ShaderData in CustomMaterial and PostEffect to define the "
                  "render state.\n"
                  "@tsexample\n"
                  "singleton GFXStateBlockData( PFX_DOFDownSampleStateBlock )\n"
                  "{\n"
                  "   zDefined = true;\n"
                  "   zEnable = false;\n"
                  "   zWriteEnable = false;\n"
                  "\n"
                  "   samplersDefined = true;\n"
                  "   samplerStates[0] = SamplerClampLinear;\n"
                  "   samplerStates[1] = SamplerClampPoint;\n"
                  "\n"
                  "   // Copy the clamped linear sampler, but change\n"
                  "   // the u coord to wrap for this special case.\n"
                  "   samplerStates[2] = new GFXSamplerStateData( : SamplerClampLinear )\n"
                  "   {\n"
                  "      addressModeU = GFXAddressWrap;\n"
                  "   };\n"                  
                  "};\n"
                  "@endtsexample\n"
                  "@note The 'xxxxDefined' fields are used to know what groups of fields are modified "
                  "when combining multiple state blocks in material processing.  You should take care to "
                  "enable the right ones when setting values.\n"
                  "@ingroup GFX\n" );

WebCoreData::WebCoreData() :
		      mEnablePlugins(false),
		      mEnableJavascript(true),
		      mUserDataPath(Null),
		      mPluginPath(Null),
		      mLogPath(Null),
		      mLogLevel(LOG_NORMAL),
		      mUserAgentOverride(Null),
		      mProxyServer(Null),
		      mProxyConfigScript(Null),
		      mSaveCacheAndCookies(true),
		      mMaxCacheSize(0),
		      mDisableSameOriginPolicy(false),
		      mCustomCSS(Null)
{
}

WebCoreData::~WebCoreData() 
{
	// Destroy our WebCore instance
   if(webCore)
	   delete webCore;
}

void WebCoreData::initPersistFields()
{
   addGroup( "config" );

      addField( "enablePlugins",             TypeBool,   Offset(mEnablePlugins, WebCoreData), "Enable or disable support for plugins such as flash." );
      addField( "enableJavascript",          TypeBool,   Offset(mEnableJavascript, WebCoreData), "Enable or disable support for Javascript execution in web pages." );
      addField( "userDataPath",              TypeString, Offset(mUserDataPath, WebCoreData), "" );
      addField( "pluginPath",                TypeString, Offset(mPluginPath, WebCoreData), "" );
      addField( "logPath",                   TypeString, Offset(mLogPath, WebCoreData), "" );
      addField( "logLevel",                  TypeS32,    Offset(mLogLevel, WebCoreData), "" );
      addField( "userAgentOverride",         TypeString, Offset(mUserAgentOverride, WebCoreData), "" );
      addField( "proxyServer",               TypeString, Offset(mProxyServer, WebCoreData), "" );
      addField( "proxyConfigScript",         TypeString, Offset(mProxyConfigScript, WebCoreData), "" );
      addField( "saveCacheAndCookies",       TypeBool,   Offset(mSaveCacheAndCookies, WebCoreData), "" );
      addField( "maxCacheSize",              TypeS32,    Offset(mMaxCacheSize, WebCoreData), "" );
      addField( "mDisableSameOriginPolicy",  TypeBool,   Offset(mDisableSameOriginPolicy, WebCoreData), "" );
      addField( "mCustomCSS",                TypeString, Offset(mCustomCSS, WebCoreData), "" );

   endGroup( "config" );

   Parent::initPersistFields();
}

bool WebCoreData::onAdd()
{
   if (!Parent::onAdd())
      return false;

   if(webCore)
	   delete webCore;

   Awesomium::WebCoreConfig config;
   config.setEnablePlugins(mEnablePlugins);
   config.setEnableJavascript(mEnableJavascript);
   config.setUserDataPath(mUserDataPath);
   config.setPluginPath(mPluginPath);
   config.setLogPath(mLogPath);
   config.setLogLevel(mLogLevel);
   config.setUserAgentOverride(mUserAgentOverride);
   config.setProxyServer(mProxyServer);
   config.setProxyConfigScript(mProxyConfigScript);
   config.setSaveCacheAndCookies(mSaveCacheAndCookies);
   config.setMaxCacheSize(mMaxCacheSize);
   config.setDisableSameOriginPolicy(mDisableSameOriginPolicy);
   config.setCustomCSS(mCustomCSS);

	// Create our WebCore singleton with the default options
   webCore = new Awesomium::WebCore(config);

   return webCore;
}
*/

/*
DefineEngineFunction( createWebCore, void, (),,"Create the webCore singleton object used by WebViewData.\n" )
{
   		
		 // Creates a default WebCoreConfig with the following default settings:
		 // 
		 // <pre>
		 //     enablePlugins           = false
		 //     enableJavascript        = true
		 //     userDataPath            = (empty)
		 //     pluginPath              = (empty)
		 //     logPath                 = (empty)
		 //     logLevel                = LOG_NORMAL
		 //     userAgentOverride       = (empty)
		 //     proxyServer             = "none"
		 //     proxyConfigScript       = (empty)
		 //     saveCacheAndCookies     = true
		 //     maxCacheSize            = 0
		 //     disableSameOriginPolicy = false
		 //     customCSS               = (empty)
		 // </pre>
		 

	// Destroy our WebCore instance
   if(webCore)
	   delete webCore;

   Awesomium::WebCoreConfig config;
   if(Con::getVariable( "$pref::WebCore::EnablePlugins" ))
      config.setEnablePlugins(Con::getBoolVariable( "$pref::WebCore::EnablePlugins" ));
   else Con::setBoolVariable( "$pref::WebCore::EnablePlugins", false );

   if(Con::getVariable( "$pref::WebCore::EnableJavascript" ))
      config.setEnableJavascript(Con::getBoolVariable( "$pref::WebCore::EnableJavascript" ));
   else Con::setBoolVariable( "$pref::WebCore::EnableJavascript", true );

   if(Con::getVariable( "$pref::WebCore::UserDataPath" ))
      config.setUserDataPath(Con::getVariable( "$pref::WebCore::UserDataPath"));
   else Con::setVariable( "$pref::WebCore::UserDataPath", NULL );

   if(Con::getVariable( "$pref::WebCore::PluginPath" ))
      config.setPluginPath(Con::getVariable( "$pref::WebCore::PluginPath"));
   else Con::setVariable( "$pref::WebCore::PluginPath", NULL );

   if(Con::getVariable( "$pref::WebCore::LogPath" ))
      config.setLogPath(Con::getVariable( "$pref::WebCore::LogPath"));
   else Con::setVariable( "$pref::WebCore::LogPath", NULL );

   if(Con::getVariable( "$pref::WebCore::LogLevel" ))
      config.setLogLevel(static_cast<Awesomium::LogLevel>(Con::getIntVariable( "$pref::WebCore::LogLevel")));
   else Con::setIntVariable( "$pref::WebCore::LogLevel", Awesomium::LOG_NORMAL );

   if(Con::getVariable( "$pref::WebCore::UserAgentOverride" ))
      config.setUserAgentOverride(Con::getVariable( "$pref::WebCore::UserAgentOverride" ));
   else Con::setVariable( "$pref::WebCore::UserAgentOverride", NULL );

   if(Con::getVariable( "$pref::WebCore::ProxyServer" ))
      config.setProxyServer(Con::getVariable( "$pref::WebCore::ProxyServer"));
   else Con::setVariable( "$pref::WebCore::ProxyServer", "none" );

   if(Con::getVariable( "$pref::WebCore::ProxyConfigScript" ))
      config.setProxyConfigScript(Con::getVariable( "$pref::WebCore::ProxyConfigScript"));
   else Con::setVariable( "$pref::WebCore::ProxyConfigScript", NULL );

   if(Con::getVariable( "$pref::WebCore::SaveCacheAndCookies" ))
      config.setSaveCacheAndCookies(Con::getBoolVariable( "$pref::WebCore::SaveCacheAndCookies" ));
   else Con::setBoolVariable( "$pref::WebCore::SaveCacheAndCookies", true );

   if(Con::getVariable( "$pref::WebCore::MaxCacheSize" ))
      config.setMaxCacheSize(Con::getIntVariable("$pref::WebCore::MaxCacheSize"));
   else Con::setIntVariable( "$pref::WebCore::MaxCacheSize", 0 );

   if(Con::getVariable( "$pref::WebCore::DisableSameOriginPolicy" ))
      config.setDisableSameOriginPolicy(Con::getBoolVariable( "$pref::WebCore::DisableSameOriginPolicy" ));
   else Con::setBoolVariable( "$pref::WebCore::DisableSameOriginPolicy", false );

   if(Con::getVariable( "$pref::WebCore::CustomCSS" ))
      config.setCustomCSS(Con::getVariable( "$pref::WebCore::CustomCSS"));
   else Con::setVariable( "$pref::WebCore::CustomCSS", NULL );

	// Create our WebCore singleton with the default options
   webCore = new Awesomium::WebCore(config);
}

*/

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
	//   if(message)
	//      msg = dStrdup(message);
	//   else
	//      msg = NULL;

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
    //  mScriptCallbackArgs(),
      mIsClientOnly(false),
      mWebViewTexture(NULL)
{
   // todo: scope this object properly
   //ghostfix
//#ifdef   WEB_GHOSTING
   mNetFlags.set(Ghostable);
//#else
//  mNetFlags.set(Ghostable | ScopeAlways);
//#endif

   // From ProcessObject   
//   mProcessTag = 0;
//   mIsGameBase = false;
   
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
/*
const WebViewData * WebViewData::getServerObject()
{
   SimSet * scopeAlwaysSet = Sim::getGhostAlwaysSet();
   for(SimSet::iterator itr = scopeAlwaysSet->begin(); itr != scopeAlwaysSet->end(); itr++)
   {
      WebViewData * wv = dynamic_cast<WebViewData*>(*itr);
      if(wv)
      {
         AssertFatal(wv->isServerObject(), "WebViewData::getServerObject: found client object in ghost always set!");
         return(wv);
      }
   }
   return(0);
}
*/
//------------------------------------------------------------------------------


bool WebViewData::onAdd()
{
//   if(isServerObject() && WebViewData::getServerObject())
//   {
//      Con::errorf(ConsoleLogEntry::General, "WebViewData::onAdd - WebViewData already instantiated!");
//      return(false);
//   }


//   setGlobalBounds();
//   resetWorldBox();

//   addToScene();

 //  if(isGhost() && isProperlyAdded())
 //  {
   //   if(!mWebViewTexture)
   //      mWebViewTexture = new WebViewTexture(mResolution.x, mResolution.y);
   //   else mWebViewTexture->setSize(mResolution.x, mResolution.y);
/*
      if(!mWebViewTexture)
         mWebViewTexture = new WebViewTexture();
      mWebViewTexture->setSize(mResolution.x, mResolution.y);
 //     Con::printf("res = %d x %d",mResolution.x,mResolution.y);

      if(mURL.isNotEmpty())
      {
         Con::printf("WebViewData::onAdd - getWebViewTexture().setURL(%s)",mURL.c_str());
         mWebViewTexture->setURL(mURL);

	     // this is ugly
	     if(mWebViewTexture->getWebView())
        {
		    // mWebViewTexture.getWebView()->setListener(this);
            mWebViewTexture->getWebView()->set_view_listener(this);
            mWebViewTexture->getWebView()->set_load_listener(this);
            mWebViewTexture->getWebView()->set_process_listener(this);
            mWebViewTexture->refresh();


		   //  mWebViewTexture.getWebView()->requestScrollData();
        }else{
   //         Con::printf("WebViewData::onAdd - failed to get view");
        }
   //   }else{
//         Con::printf("WebViewData::onAdd - no url");
   //   }


   //   mWebViewTexture.refresh();
        

   }
*/
   if(!Parent::onAdd())
      return false;

   
 //  mWebViewTexture.setURL(mURL);
//   String errorStr;
//   preload(isServerObject(), errorStr);

   return true;
}

void WebViewData::onRemove()
{

 //  mWebViewTexture.setURL(mURL);
//   if(mWebViewTexture.isValid())
//   {
//      mWebViewTexture.destroy();
//      delete mWebViewTexture;
//   }
//   if(mMaterial)
//   {
//      mMaterial->destroySelf();
//      mMaterial = NULL;
//   }
 //  if(mWebViewTexture)
   
   if(mWebViewTexture)
   {
      mWebViewTexture->destroy();
      mWebViewTexture = NULL;
   }

   Parent::onRemove();
}

void WebViewData::inspectPostApply()
{
  // _updateSceneGraph();
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
	//  mFrameUrl.clear();
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
    //  mWebViewTexture->setSize(mResolution.x,mResolution.y);

    //  if(mWebViewTexture && mWebViewTexture->getWebView())
    //  {
      //   mWebViewTexture->getWebView()->set_view_listener(this);
      //   mWebViewTexture->getWebView()->set_load_listener(this);
      //   mWebViewTexture->getWebView()->set_process_listener(this);
      //   mWebViewTexture->refresh();
   //   }
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

/*
   if(mURL)
   {
      if(!mWebViewTexture)
         mWebViewTexture = new WebViewTexture(mResolution.x, mResolution.y);
      else mWebViewTexture->setSize(mResolution.x, mResolution.y);

      mWebViewTexture->setURL(mURL);
      mWebViewTexture->refresh();
   }
   */
}

/*
void WebViewData::onFinishLoading(Awesomium::WebView* caller)
{
	Con::printf("onFinishLoading");
    //mWebViewTexture.getWebView().requestScrollData();

   onFinishLoading_callback();
}
void WebViewData::onGetPageContents(Awesomium::WebView* caller, 
									   const std::string& url, 
									   const std::wstring& contents)
{
//   if(mWebViewTexture.getWebView())
//      mWebViewTexture.getWebView()->requestScrollData();
	Con::printf("onGetPageContents");
   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->requestScrollData();

}

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
#ifdef   CLEAN_PAGE
   mPageLoaded = false;

   if( mWebViewTexture ){
      Con::printf("setting pageloaded = false");
      mWebViewTexture->setPageLoaded(false);
      refresh();
   }
#endif

   if(url.isEmpty())
      return;


	// make sure we know whats really going on
	// todo: try and remove local mURL

	// only update this if 
	if(frame.isEmpty())
	{
		//if(mURL.length() == url.length() && mURL.compare(url, url.length()) == 0)
		//	return;
      mScriptCallbackCommands.clear();

		if(isClientObject() && getWebViewTexture() && getWebViewTexture()->getWebView())
		{
			// update client
         //clear();
         Con::printf("WebViewData::setURL - getWebViewTexture().setURL(%s)",url.c_str());

// todo: this should work and be better?
//			getWebViewTexture().setURL(url);

		//	getWebViewTexture().getWebView()->requestScrollData();
         if(mIsClientOnly)
         {
   			getWebViewTexture()->setURL(url);
            //refresh();
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

			// update client
		//	getWebViewTexture().setURL(url, frame);
		//	getWebViewTexture().getWebView()->requestScrollData(frame);

         if(mIsClientOnly)
         {
   			getWebViewTexture()->setURL(url, frame);
            refresh();
		   //	getWebViewTexture().getWebView()->requestScrollData();
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
//   refresh();
}

void WebViewData::refresh()                  
{
   /*
   if(!isTicking())
   {
      setProcessTick(true);
   }
   */
   if(mWebViewTexture)
   {
#ifdef   CLEAN_PAGE
      mWebViewTexture->setPageLoaded(mPageLoaded);
#endif
      mWebViewTexture->refresh(); 
//      mNamedTarget.setTexture(mWebViewTexture->getTexture());]
      if(!mWebViewTexture->getTexture().isNull() && mWebViewTexture->getTexture() != GFXTexHandle::ZERO)
        // ClientProcessList::get()->preTickSignal().remove( this, &WebViewData::refresh );
         ClientProcessList::get()->postTickSignal().remove( this, &WebViewData::refreshPageLoad );
   }


}

/*
void WebViewData::setLocalFile( const FileName& url, const String& frame )
{

   if(url.isEmpty())
      return;

	// make sure we know whats really going on
	// todo: try and remove local mURL

	// only update this if 
	if(frame.isEmpty())
	{
		if(mURL.length() == url.length() && mURL.compare(url, url.length()) == 0)
			return;

		if(isClientObject() && getWebViewTexture().getWebView())
		{
			// update client
         Con::printf("WebViewData::setLocalFile - getWebViewTexture().setURL(%s)",url.c_str());
			getWebViewTexture().setLocalFile(url);
		//	getWebViewTexture().getWebView()->requestScrollData();

			// sync client url changes back to server
	      GameConnection *con = GameConnection::getConnectionToServer();
         if(con)
			   con->postNetEvent(new WebViewDataURLEvent(this, url));
		}
		else
			setMaskBits(TargetURLMask);

		mURL = url;
	}
	else
	{
		if(isClientObject())
		{
			HashTable<String, FileName>::Iterator myUpdate = mFrameUrl.find(frame);
			if(myUpdate->value.compare(url) == 0)
			{
				mFrameUrl.erase(myUpdate);
				return;
			}

			// update client
		//	getWebViewTexture().setLocalFile(url, frame);
		//	getWebViewTexture().getWebView()->requestScrollData(frame);

			// sync client url changes back to server
	      GameConnection *con = GameConnection::getConnectionToServer();
         if(con)
			   con->postNetEvent(new WebViewDataURLEvent(this, url, frame));
		}
		else
		{
			mFrameUrl.insertUnique(frame, url);
			setMaskBits(FrameURLMask);
		}
	}
}
*/
#ifdef   CLEAN_PAGE
void        WebViewData::setPageLoaded(bool _pageLoaded)
{
   mPageLoaded = _pageLoaded;
   if(mWebViewTexture){
      mWebViewTexture->setPageLoaded(_pageLoaded);
   }
}
#endif
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
//   Con::printf("alpha=%d",alpha);
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
	/*
	else
	{
		if(isClientObject() && con)
		{
			HashTable<String, FileName>::Iterator myUpdate = mFrameUrl.find(frame);
			if(myUpdate->value.compare(url) == 0)
			{
				mFrameUrl.erase(myUpdate);
				return;
			}

			// update client
			getWebViewTexture().setURL(url, frame);

			// sync client url changes back to server
			con->postNetEvent(new WebViewDataURLEvent(this, url, frame));
		}
		else
		{
			mFrameUrl.insertUnique(frame, url);
			setMaskBits(ScrollMask);
		}
	}
	*/
}
bool WebViewData::setResolution(U32 width, U32 height)  
{
   mResolution.x = width;
   mResolution.y = height;
//   if(isClientObject())
//      if (/*mWebViewTexture.isValid() && */mWebViewTexture.setSize(width, height))
//         return true;
//      else
//         return false;
//   if(isGhost() && mWebViewTexture)
//      mWebViewTexture->setSize(width,height);

   // server only
   if(isServerObject())
      setMaskBits(ResolutionMask);
   return true;
}

/*
void WebViewData::onChangeTargetURL(Awesomium::WebView* caller, 
			   const std::string& url)
{
//	getWebViewTexture().setURL(url.c_str());
//	setURL(FileName(url.c_str()));
}
void WebViewData::onOpenExternalLink(Awesomium::WebView* caller, 
				const std::string& url, 
				const std::wstring& source)
{
//	getWebViewTexture().setURL(url.c_str());
	setURL(FileName(url.c_str()));
}

void WebViewData::onBeginNavigation(Awesomium::WebView* caller, 
							   const std::string& url, 
							   const std::wstring& frameName)
{
	// message to server

//	mFrameUrl.insertUnique(frameName.c_str(), FileName(url.c_str()));
//	setMaskBits(FrameURLMask);

 //  if(!url.empty())
//   {
//      Con::printf("WebViewData::onBeginNavigation - setURL(%s)",url.c_str());
	setURL(FileName(url.c_str()), frameName.c_str());
//   }

}
void WebViewData::onBeginLoading(Awesomium::WebView* caller, 
							const std::string& url, 
							const std::wstring& frameName, 
							int statusCode, 
							const std::wstring& mimeType)
{
}
*/


void WebViewData::setObjectCallbackScript(String objectName, String callbackName, String tsFunctionName, String args) 
{ 
   
   Awesomium::WebView *view = getWebViewTexture()->getWebView();
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
/*
void WebViewData::onCallback(Awesomium::WebView* caller, 
								const std::wstring& objectName, 
								const std::wstring& callbackName, 
								const Awesomium::JSArguments& args)
{
   
   // call bound script function
   // todo: make this efficient
   String tsFunctionName = mScriptCallbackCommands[String(objectName.c_str()) + '.' + String(callbackName.c_str())];
   if(tsFunctionName.isNotEmpty())
   {
      tsFunctionName += '(';
      Awesomium::JSArguments::const_iterator i = args.begin();
      for(; i != args.end(); i++)
      {
         if(i->isBoolean())
            tsFunctionName += String::ToString(i->toBoolean());
         else if(i->isDouble())
            tsFunctionName += String::ToString(i->toDouble());
         else if(i->isInteger())
            tsFunctionName += String::ToString(i->toInteger());
         if((i+1) != args.end())
            tsFunctionName += ',';
      }
      tsFunctionName += ')';
      tsFunctionName += ';';

      Con::evaluate(tsFunctionName);
   }
   
}
*/



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
  //WebTile* new_tile = new WebTile(new_view, mResolution.x, mResolution.y);
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
#ifdef   CLEAN_PAGE
      mPageLoaded = true;
#endif
      if( mWebViewTexture ){
#ifdef   CLEAN_PAGE
         mWebViewTexture->setPageLoaded(true);
#endif
         //    mWebView->ExecuteJavascript(Awesomium::WSLit("document.body.style.backgroundColor = 'transparent'"), Awesomium::WSLit(""));
         if(mWebViewTexture->getWebView())
            mWebViewTexture->getWebView()->SetTransparent(true);
      }


      refresh();


      onFinishLoading_callback();
   //   ClientProcessList::get()->preTickSignal().remove( this, &WebViewData::refresh );
   //   refresh();
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
      //   if(arg.IsBoolean())
            tsFunctionName += String(Awesomium::ToString(arg.ToString()).c_str());
      //   else if(arg.IsDouble())
      //      tsFunctionName += String::ToString(arg.ToDouble());
      //   else if(arg.IsInteger())
      //      tsFunctionName += String::ToString(arg.ToInteger());
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
      //   if(arg.IsBoolean())
            tsFunctionName += String(Awesomium::ToString(arg.ToString()).c_str());
      //   else if(arg.IsDouble())
      //      tsFunctionName += String::ToString(arg.ToDouble());
      //   else if(arg.IsInteger())
      //      tsFunctionName += String::ToString(arg.ToInteger());
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
   if(!webView)// || webView->IsLoadingPage())
      return;

   webView->InjectMouseDown(static_cast<Awesomium::MouseButton>(button + Awesomium::kMouseButton_Left));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   object->refresh();
}
DefineEngineMethod( WebViewData, injectMouseUp, void, (S32 button),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;

   webView->InjectMouseUp(static_cast<Awesomium::MouseButton>(button + Awesomium::kMouseButton_Left));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   object->refresh();
}
DefineEngineMethod( WebViewData, injectMouseMove, void, (Point2F point),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;
   object->mMouseMove = point;

   F32 width = webViewTexture->getWidth();
   F32 height = webViewTexture->getHeight();
   webView->InjectMouseMove((width * point.x) + (width/2.0f), (height * (-point.y)) + (height/2.0f));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   object->refresh();
}
DefineEngineMethod( WebViewData, injectMouseWheel, void, (Point2F scroll),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;

   webView->InjectMouseWheel((int)((F32)webViewTexture->getWidth()*scroll.x), (int)((F32)webViewTexture->getHeight()*scroll.y));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
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
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
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
   if(!webView)// || webView->IsLoadingPage())
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
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   object->refresh();
}


DefineEngineMethod( WebViewData, injectKeyboardEvent, void, (String text),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture* webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture->getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;

   Awesomium::WebKeyboardEvent event;
   event.type = Awesomium::WebKeyboardEvent::kTypeChar;
   event.text[0] = text[0];
//   event.unmodifiedText[0] = text[0];

   webView->InjectKeyboardEvent(event);
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   object->refresh();
}

DefineEngineMethod( WebViewData, setURL, void, (String text),,
   "set url for webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
//   WebViewTexture& webViewTexture = object->getWebViewTexture();

//   webViewTexture.setURL(text.utf8());
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
   //   view->CreateObject(objectName.utf16());
      view->CreateGlobalJavascriptObject(Awesomium::WSLit(objectName.utf8()));
}

DefineEngineMethod( WebViewData, clear, void, (),,
   "clear the texture for the webview.\n"
   "@param \n\n" )
{ 
   object->clear();
}



/*
DefineEngineMethod( WebViewData, destroyObject, void, (String objectName),,
   "destroy a javascript object.\n"
   "@param \n\n" )
{ 
   Awesomium::WebView *view = object->getWebViewTexture().getWebView();
   if(view)
      view->DestroyObject(objectName.utf16());
}		
DefineEngineMethod( WebViewData, setObjectPropertyBool, void, (String objectName, String propName, bool value),,
   "set a javascript object property.\n"
   "@param \n\n" )
{ 
   Awesomium::WebView *view = object->getWebViewTexture().getWebView();
   if(view)
      view->SetObjectProperty(objectName.utf16(), propName.utf16(), Awesomium::JSValue(value));
}		
	*/
	
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



//Awesomium::WebView::injectKeyboardEvent();

/*
DefineEngineMethod( WebViewData, injectTouchDown, void, (S32 button),,
   "Inject a touch event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture& webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;

   Awesomium::WebTouchEvent touch_event
   webView->InjectTouchEvent(static_cast<Awesomium::MouseButton>(button + Awesomium::kMouseButton_Left));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   webViewTexture.refresh();
}
DefineEngineMethod( WebViewData, injectTouchUp, void, (S32 button),,
   "Inject a touch event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture& webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;

   webView->InjectMouseUp(static_cast<Awesomium::MouseButton>(button + Awesomium::kMouseButton_Left));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   webViewTexture.refresh();
}
DefineEngineMethod( WebViewData, injectTouchMove, void, (Point2F point),,
   "Inject a touch event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture& webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView)// || webView->IsLoadingPage())
      return;
   object->mMouseMove = point;

   F32 width = webViewTexture.getWidth();
   F32 height = webViewTexture.getHeight();
   webView->InjectMouseMove((width * point.x) + (width/2.0f), (height * (-point.y)) + (height/2.0f));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   webViewTexture.refresh();
}
*/



#else



// Various macro definitions
#define WIDTH	512
#define HEIGHT	512
//#define URL	"http://www.google.com"
#define UPDATE_DELAY_MS	25


/// Profile for the video texture.
GFX_ImplementTextureProfile(  GFXWebViewTextureProfile,
                              GFXTextureProfile::DiffuseMap,
                          //    GFXTextureProfile::NoMipmap | GFXTextureProfile::Dynamic | GFXTextureProfile::PreserveSize | GFXTextureProfile::KeepBitmap /*| GFXTextureProfile::NoPadding*/,
                          //    GFXTextureProfile::NoMipmap | GFXTextureProfile::NoPadding | GFXTextureProfile::PreserveSize | GFXTextureProfile::SystemMemory,
                                GFXTextureProfile::NoMipmap | GFXTextureProfile::Dynamic | GFXTextureProfile::PreserveSize | GFXTextureProfile::Pooled | GFXTextureProfile::NoPadding/* | GFXTextureProfile::KeepBitmap*/,
                                GFXTextureProfile::None );

/*
//=============================================================================
//    WebViewData implementation.
//=============================================================================
IMPLEMENT_CO_DATABLOCK_V1(WebViewData);

//-----------------------------------------------------------------------------

WebViewData::WebViewData() : mWebView(NULL), mWidth(512), mHeight(512)
{
}

//-----------------------------------------------------------------------------

WebViewData::~WebViewData()
{
   GFXTextureManager::removeEventDelegate( this, &WebViewData::_onTextureEvent );

   // Destroy our WebView instance
   if(mWebView)
   {
	   mWebView->destroy();
      mWebView = NULL;
   }

	
	// Destroy our WebCore instance
	//delete webCore;
}

void WebViewData::initPersistFields()
{
 //  addField("explosionShape",  TypeFilename,                Offset(dtsFileName,     ExplosionData), "DTS shape to place at the center of the explosion. The 'ambient' animation of this model will automatically be played at the start of the explosion.");
   Parent::initPersistFields();
}
bool WebViewData::onAdd()
{
      if (Parent::onAdd() == false)
      return false;

   setURL("http://www.google.com");

   return true;
}
void WebViewData::packData(BitStream* stream)
{
   Parent::packData(stream);

}
void WebViewData::unpackData(BitStream* stream)
{
	Parent::unpackData(stream);
}
bool WebViewData::preload(bool server, String &errorStr)
{
   if (Parent::preload(server, errorStr) == false)
      return false;
      
   if( !server )
   {


   }
   return true;
}
//-----------------------------------------------------------------------------

void WebViewData::_onTextureEvent( GFXTexCallbackCode code )
{
   switch( code )
   {
      case GFXZombify:
         
       //  mCurrentFrame = NULL;
       //  if( mAsyncState )
       //  {
            // Blast out work items and then release all texture locks.
            
       //     ThreadPool::GLOBAL().flushWorkItems();
       //     mAsyncState->getFrameStream()->releaseTextureLocks();
            
            // The Theora decoder does not implement seeking at the moment,
            // so we absolutely want to make sure we don't fall behind too far or
            // we may end up having the decoder go crazy trying to skip through
            // Ogg packets (even just reading these undecoded packets takes a
            // lot of time).  So, for the time being, just pause playback when
            // we go zombie.
            
         //   if( mSFXSource )
         //      mSFXSource->pause();
         //   else
         //      mPlaybackTimer.pause();
         //}
         
         break;
         
      case GFXResurrect:
         
       //  if( mAsyncState )
       //  {
            // Reacquire texture locks.
            
       //     mAsyncState->getFrameStream()->acquireTextureLocks();
            
            // Resume playback if we have paused it.
            
        //    if( !mIsPaused )
        //    {
        //       if( mSFXSource )
        //          mSFXSource->play();
        //       else
        //          mPlaybackTimer.start();
        //    }
        // }
         
         break;
   }
}

//-----------------------------------------------------------------------------

U32 WebViewData::getWidth() const
{
   return mWidth;
}

//-----------------------------------------------------------------------------

U32 WebViewData::getHeight() const
{
   return mHeight;
}

//-----------------------------------------------------------------------------

bool WebViewData::setURL( const String& url )
{
   if( url.isEmpty() )
      return true;
   
	// Create our WebCore singleton with the default options
	//webCore = new Awesomium::WebCore();
	
	// Create a new WebView instance with a certain width and height, using the 
	// WebCore we just created
	mWebView = webCore->createWebView(mWidth, mHeight);
	
	// Load a certain URL into our WebView instance
	mWebView->loadURL(url.c_str());

	mWebView->focus();

   GFXTextureManager::addEventDelegate( this, &WebViewData::_onTextureEvent );

   // Create an RGB conditioner
//   mConditioner = new AdvancedLightBufferConditioner( GFXFormatR8G8B8A8, AdvancedLightBufferConditioner::RGB );
//   mNamedTarget.setConditioner( mConditioner ); 
   mNamedTarget.registerWithName( this->getClassName() );

   return true;
}

//-----------------------------------------------------------------------------

void WebViewData::refresh()
{
	webCore->update();

	// Call our display func when the WebView needs rendering
	if(!mWebView->isDirty())
      return;

	const Awesomium::RenderBuffer* renderBuffer = mWebView->render();

  // mTexture.set(renderBuffer->width, renderBuffer->height, static_cast<U32>(32), static_cast<void*>(renderBuffer->buffer), GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, "");
   mTexture.set(new GBitmap(renderBuffer->width, renderBuffer->height, renderBuffer->buffer), &GFXWebViewTextureProfile, true, "");
   mNamedTarget.setTexture(mTexture);
}
*/
//-----------------------------------------------------------------------------
//=============================================================================
//    WebViewData implementation.
//=============================================================================
//IMPLEMENT_CO_DATABLOCK_V1(WebViewTexture);

//-----------------------------------------------------------------------------
//static Awesomium::WebView* mWebView = 0;
//static Awesomium::WebCore* webCore = 0;

//static U32 gWebViewCount = 0;
U32 WebViewTexture::webViewCount = 0;
Awesomium::WebCore* WebViewTexture::webCore = 0;

WebViewTexture::WebViewTexture() :
            mWidth(512), mHeight(512),
            mWebView(NULL)
         //   mBitmap(NULL)
{	
	// Create a new WebView instance with a certain width and height, using the 
	// WebCore we just created
//	mWebView = *webCore->createWebView(mWidth, mHeight);

//   webViewCount++;
}
WebViewTexture::WebViewTexture(U32 width, U32 height) :
            mWidth(width), mHeight(height),
            mWebView(NULL)
         //   mBitmap(NULL)
{	
	// Create a new WebView instance with a certain width and height, using the 
	// WebCore we just created
//	mWebView = *webCore->createWebView(mWidth, mHeight);

}

//-----------------------------------------------------------------------------

WebViewTexture::~WebViewTexture()
{
   /*
      if(mWebView)
      {
   	   mWebView->destroy();
         webViewCount--;
      }
      */
      destroy();
   //   if(mTexture)
   //      mTexture->destroySelf();

   //   mWebView = NULL;
  // }

      destroyWebCore();
}

//-----------------------------------------------------------------------------

void WebViewTexture::_onTextureEvent( GFXTexCallbackCode code )
{
   switch( code )
   {
      case GFXZombify:
         
       //  mCurrentFrame = NULL;
       //  if( mAsyncState )
       //  {
            // Blast out work items and then release all texture locks.
            
       //     ThreadPool::GLOBAL().flushWorkItems();
       //     mAsyncState->getFrameStream()->releaseTextureLocks();
            
            // The Theora decoder does not implement seeking at the moment,
            // so we absolutely want to make sure we don't fall behind too far or
            // we may end up having the decoder go crazy trying to skip through
            // Ogg packets (even just reading these undecoded packets takes a
            // lot of time).  So, for the time being, just pause playback when
            // we go zombie.
            
         //   if( mSFXSource )
         //      mSFXSource->pause();
         //   else
         //      mPlaybackTimer.pause();
         //}
         mWebView->pauseRendering();
         
         break;
         
      case GFXResurrect:
         mWebView->resumeRendering();
      //   mTexture.refresh();
         refresh();
         
       //  if( mAsyncState )
       //  {
            // Reacquire texture locks.
            
       //     mAsyncState->getFrameStream()->acquireTextureLocks();
            
            // Resume playback if we have paused it.
            
        //    if( !mIsPaused )
        //    {
        //       if( mSFXSource )
        //          mSFXSource->play();
        //       else
        //          mPlaybackTimer.start();
        //    }
        // }
         
         break;
   }
}

//-----------------------------------------------------------------------------

bool WebViewTexture::setSize(U32 width, U32 height)
{
   if(mWebView && !mWebView->resize(width, height, true))
      return false;
//   mTexture.set(width, height, GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));

//   if(mTexture.isValid())
//      mTexture.free();
//   mTexture = GFXTexHandle::ZERO;

   
   mTexture.set(width, height, GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));
   GFXLockedRect* lock = mTexture->lock();
   if(lock){
      dMemset(lock->bits, 0, sizeof(width*height*4));
      mTexture->unlock();
   }else{
         Con::printf("WebViewTexture::setSize - failed to lock texture");
   } 
   

   mWidth = width;
   mHeight = height;
//   updateWebCore();

   refresh();

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
   
//	updateWebCore();

   if(!mWebView && createWebCore())
   {

      mWebView = createWebCore()->createWebView(mWidth, mHeight);
      if(mWebView)
      {
         mWebView->setTransparent(true);
         webViewCount++;
      }
      else
      {
         Con::warnf("WebViewTexture::setURL - failed to create new web page");
         return false;
      }
   }

   if(!mWebView)
      return false;

	// Load a certain URL into our WebView instance

   if(mWebView->getURL().compare(url) == 0)
      return true;


   /*
   // this is just silly
   std::string str = url.utf8();
   if(frameName.isEmpty())
	mWebView->loadURL(str);
   else
	mWebView->loadURL(str,frameName.utf16(),username.utf8(),password.utf8());
*/
	mWebView->loadURL(url.utf16(),frameName.utf16(),username.utf8(),password.utf8());

	mWebView->focus();
 //  updateWebCore();

//   GFXTextureManager::addEventDelegate( this, &WebViewTexture::_onTextureEvent );

   // Create an RGB conditioner
//   mConditioner = new AdvancedLightBufferConditioner( GFXFormatR8G8B8A8, AdvancedLightBufferConditioner::RGB );
//   mNamedTarget.setConditioner( mConditioner ); 
//   mNamedTarget.registerWithName( this->getClassName() );

   refresh();
   return true;
}

bool WebViewTexture::setLocalFile( const FileName& url, 
							 const String& frameName )
{
   if( url.isEmpty() )
      return true;
   
   if(!mWebView)
   {
      mWebView = createWebCore()->createWebView(mWidth, mHeight);
      mWebView->setTransparent(true);
      if(mWebView)
         webViewCount++;
   }
	// Load a certain URL into our WebView instance

   /*
   // this is just silly
   std::string str = url.utf8();
   if(frameName.isEmpty())
	mWebView->loadURL(str);
   else
	mWebView->loadURL(str,frameName.utf16(),username.utf8(),password.utf8());
*/
	mWebView->loadFile(url.utf8(),frameName.utf16());

	mWebView->focus();

//   GFXTextureManager::addEventDelegate( this, &WebViewTexture::_onTextureEvent );

   // Create an RGB conditioner
//   mConditioner = new AdvancedLightBufferConditioner( GFXFormatR8G8B8A8, AdvancedLightBufferConditioner::RGB );
//   mNamedTarget.setConditioner( mConditioner ); 
//   mNamedTarget.registerWithName( this->getClassName() );

   refresh();
   return true;
}


//-----------------------------------------------------------------------------

void WebViewTexture::destroy()
{
      if(mWebView)
      {
   	   mWebView->destroy();
         mWebView = 0;
         webViewCount--;
      }
   //   if(mTexture)
   //   {
   //      mTexture->destroySelf();
   //      mTexture->is
   //   }
   /*
//   GFXTextureManager::removeEventDelegate( this, &WebViewTexture::_onTextureEvent );

   // Destroy our WebView instance
  // if(mWebView)
  // {
      if(mWebView)
      {
   	   mWebView->destroy();
         webViewCount--;
      }
      if(mTexture)
         mTexture->destroySelf();

   //   mWebView = NULL;
  // }

      destroyWebCore();
	// Destroy our WebCore instance
//   if(gWebViewCount == 0 && webCore)
//    delete webCore;
//
*/
}


//-----------------------------------------------------------------------------

/*
void WebViewTexture::refresh()
{
   if(!webCore)
      return;

	webCore->update();

	// Call our display func when the WebView needs rendering
	if(!mWebView.isDirty())
      return;

	const Awesomium::RenderBuffer* renderBuffer = mWebView.render();

  // mTexture.set(renderBuffer->width, renderBuffer->height, static_cast<U32>(32), static_cast<void*>(renderBuffer->buffer), GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, "");
  // GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height, renderBuffer->buffer);
  // bitmap->setFormat(GFXFormatB8G8R8A8);
   GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height, GFXFormatB8G8R8A8, renderBuffer->buffer, true);
//   bitmap->createPaddedBitmap
   mTexture.set(bitmap, &GFXWebViewTextureProfile, true, "");
//   mNamedTarget.setTexture(mTexture);
}
*/
void WebViewTexture::clear()
{
/*
   if(mTexture.isValid()){
      if(GFXLockedRect* lock = mTexture->lock() ){
         U32   width = mTexture.getWidth();
         U32   height = mTexture.getHeight();
         U32   bytesPerPixel = GFXFormat_getByteSize( mTexture.getFormat() );
         U8  *to = lock->bits;
         for(int iy=0; iy<height; iy++){
           memset(to, 0, width*bytesPerPixel);
           to += lock->pitch;
         }
         mTexture->unlock();
      }
   }
   */
}
void WebViewTexture::refresh()
{
//	updateWebCore();

   if(!webCore)
   {
   //   if(webCore)
   //      webCore->update();
   //   Con::printf("WebViewTexture::refresh - no webcore or webview");
      return;
   }
   else
	   webCore->update();

	if(!mWebView || mWebView->isLoadingPage() || !mWebView->isDirty())
   {
//      Con::printf("WebViewTexture::refresh cancelled - isLoading = %d: isDirty = %d",
//         (int)mWebView->isLoadingPage(),
//         (int)mWebView->isDirty()
//         );
    //  webCore->update();
      return;
   }

//	webCore->update();

   // Call our display func when the WebView needs rendering
//	if(mWebView->isLoadingPage() && mTexture.isValid())
//      return;

//	if((!mWebView->isDirty()) && mTexture.isValid())
//      return;
 //  else mTexture.refresh();

	const Awesomium::RenderBuffer* renderBuffer = mWebView->render();
   if(!renderBuffer){
//      Con::printf("WebViewTexture::refresh - cancelled as no render buffer");
      return;
   }
/*
   if(!mBitmap || mWidth != renderBuffer->width || mHeight != renderBuffer->height)
   {
      mBitmap = new GBitmap(renderBuffer->width, renderBuffer->height, false, GFXFormatR8G8B8A8);
   //   mBitmap->createPaddedBitmap();
   //   mBitmap->allocateBitmap(mBitmap->getWidth(),mBitmap->getHeight(),false, GFXFormatR8G8B8A8);

      renderBuffer->copyTo(mBitmap->getWritableBits(), mBitmap->getWidth(), 4, true);
      mTexture.set(mBitmap, &GFXWebViewTextureProfile, true, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));


      mTexture.set(renderBuffer->width, renderBuffer->height, GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));
      renderBuffer->copyTo(mTexture.getBitmap()->getWritableBits(), mTexture.getBitmap()->getWidth(), 4, true);
   }
   else
   {
      if(mTexture.getBitmap())
         renderBuffer->copyTo(mTexture.getBitmap()->getWritableBits(), mTexture.getBitmap()->getWidth(), 4, true);
   }
   */
//	webCore->update();
   /*
   if(!mTexture.isValid() || mTexture->getWidth() != renderBuffer->width || mTexture->getHeight() != renderBuffer->height)
   {
   //   mTexture.free();
      mTexture.set(renderBuffer->width, renderBuffer->height, GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));
      //   mTexture.refresh();
         if(mTexture.isValid())
         {
            GFXLockedRect* lock = mTexture->lock();
            if(lock)
            {
               renderBuffer->copyTo(lock->bits, lock->pitch, 4, false, false);
            mTexture->unlock();
         //   mTexture.refresh();
            }
            else
            {
      //         Con::printf("WebViewTexture::refresh - cancelled as no texture lock(1)");
            }

         }
      }
   else // if(mTexture.isValid())
   {
      */
   if(mTexture.isValid())
   {
      GFXLockedRect* lock = mTexture->lock();
      if(lock){
         renderBuffer->copyTo(lock->bits, lock->pitch, 4, false, false);
         mTexture->unlock();
      //   mTexture.refresh();
      }else{
//         Con::printf("WebViewTexture::refresh - cancelled as no texture lock(2)");
      }
   }

   //mTexture.set(renderBuffer->width, renderBuffer->height, 1, renderBuffer->buffer, GFXFormatB8G8R8A8, &GFXWebViewTextureProfile, avar("%s() - refresh (line %d)", __FUNCTION__, __LINE__));






  // mTexture.set(renderBuffer->width, renderBuffer->height, static_cast<U32>(32), static_cast<void*>(renderBuffer->buffer), GFXFormatR8G8B8A8, &GFXWebViewTextureProfile, "");
  // GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height, renderBuffer->buffer);
  // bitmap->setFormat(GFXFormatB8G8R8A8);

//  // GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height, GFXFormatB8G8R8A8, renderBuffer->buffer, true);
//   GBitmap* bitmap = new GBitmap(renderBuffer->width, renderBuffer->height);
////   bitmap->createPaddedBitmap
//   mTexture.set(bitmap, &GFXWebViewTextureProfile, true, "");
////   mNamedTarget.setTexture(mTexture);

//	webCore->update();

}



Awesomium::WebCore* WebViewTexture::createWebCore() 
{
   // Create our WebCore singleton with the default options
   
   if(!webCore)
   {
      Awesomium::WebCoreConfig config = Awesomium::WebCoreConfig();
      /*
       *     enablePlugins           = false
		 *     enableJavascript        = true
		 *     enableDatabases         = false
		 *     packagePath             = (empty)
		 *     localePath              = (empty)
		 *     userDataPath            = (empty)
		 *     pluginPath              = (empty)
		 *     logPath                 = (empty)
		 *     logLevel                = LOG_NORMAL
		 *     forceSingleProcess      = false
		 *     childProcessPath        = (empty)
		 *     enableAutoDetectEncoding = true
		 *     acceptLanguageOverride  = (empty)
		 *     defaultCharsetOverride  = (empty)
		 *     userAgentOverride       = (empty)
		 *     proxyServer             = "auto"
		 *     proxyConfigScript       = (empty)
		 *     authServerWhitelist     = (empty)
		 *     saveCacheAndCookies     = false
		 *     maxCacheSize            = 0
		 *     disableSameOriginPolicy = false
		 *     disableWinMessagePump   = false
		 *     customCSS               = (empty)
      */
      config.setEnablePlugins(true);
		 config.setSaveCacheAndCookies(true);
		// config.setMaxCacheSize(256);
		 config.setMaxCacheSize(102400);
		 config.setDisableSameOriginPolicy(true);
		 config.setDisableWinMessagePump(true);

      webCore = new Awesomium::WebCore(config);
      webCore->setBaseDirectory(Platform::getCurrentDirectory());
      Con::printf("WebViewTexture::createWebCore - WebCore created.");
   }
	//webCore->update();

   return webCore;
}
void WebViewTexture::destroyWebCore() 
{
   // Create our WebCore singleton with the default options
   if(webViewCount == 0 && webCore)
   {
      delete webCore;
      webCore = NULL;
   }

}
void WebViewTexture::updateWebCore()
{
//   if(webCore)
//      webCore->update();
}


//-----------------------------------------------------------------------------


//=============================================================================
//    WebCoreData implementation.
//=============================================================================

/*
IMPLEMENT_CONOBJECT( WebCoreData );

ConsoleDocClass(  WebCoreData, 
                  "@brief A state block description for rendering.\n\n"
                  "This object is used with ShaderData in CustomMaterial and PostEffect to define the "
                  "render state.\n"
                  "@tsexample\n"
                  "singleton GFXStateBlockData( PFX_DOFDownSampleStateBlock )\n"
                  "{\n"
                  "   zDefined = true;\n"
                  "   zEnable = false;\n"
                  "   zWriteEnable = false;\n"
                  "\n"
                  "   samplersDefined = true;\n"
                  "   samplerStates[0] = SamplerClampLinear;\n"
                  "   samplerStates[1] = SamplerClampPoint;\n"
                  "\n"
                  "   // Copy the clamped linear sampler, but change\n"
                  "   // the u coord to wrap for this special case.\n"
                  "   samplerStates[2] = new GFXSamplerStateData( : SamplerClampLinear )\n"
                  "   {\n"
                  "      addressModeU = GFXAddressWrap;\n"
                  "   };\n"                  
                  "};\n"
                  "@endtsexample\n"
                  "@note The 'xxxxDefined' fields are used to know what groups of fields are modified "
                  "when combining multiple state blocks in material processing.  You should take care to "
                  "enable the right ones when setting values.\n"
                  "@ingroup GFX\n" );

WebCoreData::WebCoreData() :
		      mEnablePlugins(false),
		      mEnableJavascript(true),
		      mUserDataPath(Null),
		      mPluginPath(Null),
		      mLogPath(Null),
		      mLogLevel(LOG_NORMAL),
		      mUserAgentOverride(Null),
		      mProxyServer(Null),
		      mProxyConfigScript(Null),
		      mSaveCacheAndCookies(true),
		      mMaxCacheSize(0),
		      mDisableSameOriginPolicy(false),
		      mCustomCSS(Null)
{
}

WebCoreData::~WebCoreData() 
{
	// Destroy our WebCore instance
   if(webCore)
	   delete webCore;
}

void WebCoreData::initPersistFields()
{
   addGroup( "config" );

      addField( "enablePlugins",             TypeBool,   Offset(mEnablePlugins, WebCoreData), "Enable or disable support for plugins such as flash." );
      addField( "enableJavascript",          TypeBool,   Offset(mEnableJavascript, WebCoreData), "Enable or disable support for Javascript execution in web pages." );
      addField( "userDataPath",              TypeString, Offset(mUserDataPath, WebCoreData), "" );
      addField( "pluginPath",                TypeString, Offset(mPluginPath, WebCoreData), "" );
      addField( "logPath",                   TypeString, Offset(mLogPath, WebCoreData), "" );
      addField( "logLevel",                  TypeS32,    Offset(mLogLevel, WebCoreData), "" );
      addField( "userAgentOverride",         TypeString, Offset(mUserAgentOverride, WebCoreData), "" );
      addField( "proxyServer",               TypeString, Offset(mProxyServer, WebCoreData), "" );
      addField( "proxyConfigScript",         TypeString, Offset(mProxyConfigScript, WebCoreData), "" );
      addField( "saveCacheAndCookies",       TypeBool,   Offset(mSaveCacheAndCookies, WebCoreData), "" );
      addField( "maxCacheSize",              TypeS32,    Offset(mMaxCacheSize, WebCoreData), "" );
      addField( "mDisableSameOriginPolicy",  TypeBool,   Offset(mDisableSameOriginPolicy, WebCoreData), "" );
      addField( "mCustomCSS",                TypeString, Offset(mCustomCSS, WebCoreData), "" );

   endGroup( "config" );

   Parent::initPersistFields();
}

bool WebCoreData::onAdd()
{
   if (!Parent::onAdd())
      return false;

   if(webCore)
	   delete webCore;

   Awesomium::WebCoreConfig config;
   config.setEnablePlugins(mEnablePlugins);
   config.setEnableJavascript(mEnableJavascript);
   config.setUserDataPath(mUserDataPath);
   config.setPluginPath(mPluginPath);
   config.setLogPath(mLogPath);
   config.setLogLevel(mLogLevel);
   config.setUserAgentOverride(mUserAgentOverride);
   config.setProxyServer(mProxyServer);
   config.setProxyConfigScript(mProxyConfigScript);
   config.setSaveCacheAndCookies(mSaveCacheAndCookies);
   config.setMaxCacheSize(mMaxCacheSize);
   config.setDisableSameOriginPolicy(mDisableSameOriginPolicy);
   config.setCustomCSS(mCustomCSS);

	// Create our WebCore singleton with the default options
   webCore = new Awesomium::WebCore(config);

   return webCore;
}
*/

/*
DefineEngineFunction( createWebCore, void, (),,"Create the webCore singleton object used by WebViewData.\n" )
{
   		
		 // Creates a default WebCoreConfig with the following default settings:
		 // 
		 // <pre>
		 //     enablePlugins           = false
		 //     enableJavascript        = true
		 //     userDataPath            = (empty)
		 //     pluginPath              = (empty)
		 //     logPath                 = (empty)
		 //     logLevel                = LOG_NORMAL
		 //     userAgentOverride       = (empty)
		 //     proxyServer             = "none"
		 //     proxyConfigScript       = (empty)
		 //     saveCacheAndCookies     = true
		 //     maxCacheSize            = 0
		 //     disableSameOriginPolicy = false
		 //     customCSS               = (empty)
		 // </pre>
		 

	// Destroy our WebCore instance
   if(webCore)
	   delete webCore;

   Awesomium::WebCoreConfig config;
   if(Con::getVariable( "$pref::WebCore::EnablePlugins" ))
      config.setEnablePlugins(Con::getBoolVariable( "$pref::WebCore::EnablePlugins" ));
   else Con::setBoolVariable( "$pref::WebCore::EnablePlugins", false );

   if(Con::getVariable( "$pref::WebCore::EnableJavascript" ))
      config.setEnableJavascript(Con::getBoolVariable( "$pref::WebCore::EnableJavascript" ));
   else Con::setBoolVariable( "$pref::WebCore::EnableJavascript", true );

   if(Con::getVariable( "$pref::WebCore::UserDataPath" ))
      config.setUserDataPath(Con::getVariable( "$pref::WebCore::UserDataPath"));
   else Con::setVariable( "$pref::WebCore::UserDataPath", NULL );

   if(Con::getVariable( "$pref::WebCore::PluginPath" ))
      config.setPluginPath(Con::getVariable( "$pref::WebCore::PluginPath"));
   else Con::setVariable( "$pref::WebCore::PluginPath", NULL );

   if(Con::getVariable( "$pref::WebCore::LogPath" ))
      config.setLogPath(Con::getVariable( "$pref::WebCore::LogPath"));
   else Con::setVariable( "$pref::WebCore::LogPath", NULL );

   if(Con::getVariable( "$pref::WebCore::LogLevel" ))
      config.setLogLevel(static_cast<Awesomium::LogLevel>(Con::getIntVariable( "$pref::WebCore::LogLevel")));
   else Con::setIntVariable( "$pref::WebCore::LogLevel", Awesomium::LOG_NORMAL );

   if(Con::getVariable( "$pref::WebCore::UserAgentOverride" ))
      config.setUserAgentOverride(Con::getVariable( "$pref::WebCore::UserAgentOverride" ));
   else Con::setVariable( "$pref::WebCore::UserAgentOverride", NULL );

   if(Con::getVariable( "$pref::WebCore::ProxyServer" ))
      config.setProxyServer(Con::getVariable( "$pref::WebCore::ProxyServer"));
   else Con::setVariable( "$pref::WebCore::ProxyServer", "none" );

   if(Con::getVariable( "$pref::WebCore::ProxyConfigScript" ))
      config.setProxyConfigScript(Con::getVariable( "$pref::WebCore::ProxyConfigScript"));
   else Con::setVariable( "$pref::WebCore::ProxyConfigScript", NULL );

   if(Con::getVariable( "$pref::WebCore::SaveCacheAndCookies" ))
      config.setSaveCacheAndCookies(Con::getBoolVariable( "$pref::WebCore::SaveCacheAndCookies" ));
   else Con::setBoolVariable( "$pref::WebCore::SaveCacheAndCookies", true );

   if(Con::getVariable( "$pref::WebCore::MaxCacheSize" ))
      config.setMaxCacheSize(Con::getIntVariable("$pref::WebCore::MaxCacheSize"));
   else Con::setIntVariable( "$pref::WebCore::MaxCacheSize", 0 );

   if(Con::getVariable( "$pref::WebCore::DisableSameOriginPolicy" ))
      config.setDisableSameOriginPolicy(Con::getBoolVariable( "$pref::WebCore::DisableSameOriginPolicy" ));
   else Con::setBoolVariable( "$pref::WebCore::DisableSameOriginPolicy", false );

   if(Con::getVariable( "$pref::WebCore::CustomCSS" ))
      config.setCustomCSS(Con::getVariable( "$pref::WebCore::CustomCSS"));
   else Con::setVariable( "$pref::WebCore::CustomCSS", NULL );

	// Create our WebCore singleton with the default options
   webCore = new Awesomium::WebCore(config);
}

*/

void  WebViewData::clear()
{
   mWebViewTexture.clear(); 
}

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
	//   if(message)
	//      msg = dStrdup(message);
	//   else
	//      msg = NULL;

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
      mResolution(0,0),
      mURL(), //mNamedTarget(),
      mResizeToNative(true),
      mDisableHScrollBar(false),
      mDisableVScrollBar(false),
      mIsClientOnly(false),
      mScriptCallbackCommands(),
      mWebViewTexture()
{
   // todo: scope this object properly
   //ghostfix
#ifdef   WEB_GHOSTING
   mNetFlags.set(Ghostable);
#else
  mNetFlags.set(Ghostable | ScopeAlways);
#endif

}

//------------------------------------------------------------------------------
/*
const WebViewData * WebViewData::getServerObject()
{
   SimSet * scopeAlwaysSet = Sim::getGhostAlwaysSet();
   for(SimSet::iterator itr = scopeAlwaysSet->begin(); itr != scopeAlwaysSet->end(); itr++)
   {
      WebViewData * wv = dynamic_cast<WebViewData*>(*itr);
      if(wv)
      {
         AssertFatal(wv->isServerObject(), "WebViewData::getServerObject: found client object in ghost always set!");
         return(wv);
      }
   }
   return(0);
}
*/
//------------------------------------------------------------------------------


bool WebViewData::onAdd()
{
//   if(isServerObject() && WebViewData::getServerObject())
//   {
//      Con::errorf(ConsoleLogEntry::General, "WebViewData::onAdd - WebViewData already instantiated!");
//      return(false);
//   }

   if(!Parent::onAdd())
      return false;

//   setGlobalBounds();
//   resetWorldBox();

//   addToScene();

   if(isClientObject() && isProperlyAdded())
   {
   //   if(!mWebViewTexture)
   //      mWebViewTexture = new WebViewTexture(mResolution.x, mResolution.y);
   //   else mWebViewTexture->setSize(mResolution.x, mResolution.y);

      mWebViewTexture.setSize(mResolution.x, mResolution.y);
 //     Con::printf("res = %d x %d",mResolution.x,mResolution.y);

      if(mURL.isNotEmpty())
      {
         Con::printf("WebViewData::onAdd - getWebViewTexture().setURL(%s)",mURL.c_str());
         mWebViewTexture.setURL(mURL);
      }else{
//         Con::printf("WebViewData::onAdd - no url");
      }

	  // this is ugly
	  if(mWebViewTexture.getWebView())
     {
		  mWebViewTexture.getWebView()->setListener(this);

		//  mWebViewTexture.getWebView()->requestScrollData();
     }else{
//         Con::printf("WebViewData::onAdd - failed to get view");
     }

   //   mWebViewTexture.refresh();
   }

 //  mWebViewTexture.setURL(mURL);
//   String errorStr;
//   preload(isServerObject(), errorStr);

   return true;
}

void WebViewData::onRemove()
{

 //  mWebViewTexture.setURL(mURL);
//   if(mWebViewTexture.isValid())
//   {
//      mWebViewTexture.destroy();
//      delete mWebViewTexture;
//   }
//   if(mMaterial)
//   {
//      mMaterial->destroySelf();
//      mMaterial = NULL;
//   }
 //  if(mWebViewTexture)
      mWebViewTexture.destroy();

   Parent::onRemove();
}

void WebViewData::inspectPostApply()
{
  // _updateSceneGraph();
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

   if (stream->writeFlag(mask & (ResolutionMask | InitialUpdateMask)))
   {
      stream->writeString(mClassName);

	   stream->write( mResolution.x );
	   stream->write( mResolution.y );
   }

   if (stream->writeFlag(mask & (TargetURLMask | InitialUpdateMask)))
   {
	   if(stream->writeFlag(mURL.isNotEmpty()))
		  stream->write( mURL );
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
	//  mFrameUrl.clear();
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
      setClassNamespace(stream->readSTString());

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
		  stream->read(&(url));
         Con::printf("WebViewData::unpackUpdate - getWebViewTexture().setURL(%s)",mURL.c_str());
         mURL = url;
		   mWebViewTexture.setURL(mURL);
         if(mWebViewTexture.getWebView()){
            mWebViewTexture.getWebView()->setListener(this);
         }else{
            Con::printf("no web view!!");
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
         Con::printf("WebViewData::unpackUpdate - getWebViewTexture().setURL(%s)",mURL.c_str());
		      mWebViewTexture.setURL(*url, *frame);
         //   mWebViewTexture.getWebView()->requestScrollData();

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

/*
   if(mURL)
   {
      if(!mWebViewTexture)
         mWebViewTexture = new WebViewTexture(mResolution.x, mResolution.y);
      else mWebViewTexture->setSize(mResolution.x, mResolution.y);

      mWebViewTexture->setURL(mURL);
      mWebViewTexture->refresh();
   }
   */
}


void WebViewData::onFinishLoading(Awesomium::WebView* caller)
{
	Con::printf("onFinishLoading");
    //mWebViewTexture.getWebView().requestScrollData();

   onFinishLoading_callback();
}
void WebViewData::onGetPageContents(Awesomium::WebView* caller, 
									   const std::string& url, 
									   const std::wstring& contents)
{
//   if(mWebViewTexture.getWebView())
//      mWebViewTexture.getWebView()->requestScrollData();
	Con::printf("onGetPageContents");
   if(mWebViewTexture.getWebView())
      mWebViewTexture.getWebView()->requestScrollData();

}

void WebViewData::onGetScrollData(Awesomium::WebView* caller,
									 int contentWidth,
									 int contentHeight,
									 int preferredWidth,
									 int scrollX,
									 int scrollY)
{
	Con::printf("onGetScrollData");
/*
   if(contentWidth != mWebViewTexture.getWidth() || contentHeight != mWebViewTexture.getHeight())
   {
      U32 width = (contentWidth > preferredWidth ? contentWidth : preferredWidth);
      U32 height;
      if(getRoot())
         height = (contentHeight < getRoot()->getExtent().y ? contentHeight : getRoot()->getExtent().y);
      else height = (contentHeight > getExtent().y ? contentHeight : getExtent().y);


      mWebViewTexture.setSize(width, height);

   //   if(mForceNativeExtent)
   //      setExtent(mWebViewTexture.getWidthHeight());
   //   else if(mLockAspect)
   //      setExtent(getExtent());
   }
*/
   this->onGetScrollData_callback(contentWidth, contentHeight,
									   preferredWidth, scrollX, scrollY);

//	setScroll(Point2I(scrollX,scrollY));

}

void WebViewData::setURL( const FileName& url, const String& frame )
{

   if(url.isEmpty())
      return;


	// make sure we know whats really going on
	// todo: try and remove local mURL

	// only update this if 
	if(frame.isEmpty())
	{
		//if(mURL.length() == url.length() && mURL.compare(url, url.length()) == 0)
		//	return;

		if(isClientObject() && getWebViewTexture().getWebView())
		{
			// update client
         clear();
         Con::printf("WebViewData::setURL - getWebViewTexture().setURL(%s)",url.c_str());

// todo: this should work and be better?
//			getWebViewTexture().setURL(url);

		//	getWebViewTexture().getWebView()->requestScrollData();

			// sync client url changes back to server
	  //    GameConnection *con = GameConnection::getConnectionToServer();
     //    if(con)
		//	   con->postNetEvent(new WebViewDataURLEvent(this, url));

         if(mIsClientOnly)
         {
   			getWebViewTexture().setURL(url);
            refresh();
         //   ClientProcessList::get()->preTickSignal().notify( this, &WebViewData::refresh );

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

			// update client
		//	getWebViewTexture().setURL(url, frame);
		//	getWebViewTexture().getWebView()->requestScrollData(frame);

			// sync client url changes back to server
	  //    GameConnection *con = GameConnection::getConnectionToServer();
     //    if(con)
		//	   con->postNetEvent(new WebViewDataURLEvent(this, url, frame));

         if(mIsClientOnly)
         {
   			getWebViewTexture().setURL(url,frame);
            refresh();
         //   ClientProcessList::get()->preTickSignal().notify( this, &WebViewData::refresh );

		   //	getWebViewTexture().getWebView()->requestScrollData();
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
   refresh();
}
void WebViewData::setLocalFile( const FileName& url, const String& frame )
{

   if(url.isEmpty())
      return;

	// make sure we know whats really going on
	// todo: try and remove local mURL

	// only update this if 
	if(frame.isEmpty())
	{
		if(mURL.length() == url.length() && mURL.compare(url, url.length()) == 0)
			return;

		if(isClientObject() && getWebViewTexture().getWebView())
		{
			// update client
         Con::printf("WebViewData::setLocalFile - getWebViewTexture().setURL(%s)",url.c_str());
			getWebViewTexture().setLocalFile(url);
		//	getWebViewTexture().getWebView()->requestScrollData();

			// sync client url changes back to server
	      GameConnection *con = GameConnection::getConnectionToServer();
         if(con)
			   con->postNetEvent(new WebViewDataURLEvent(this, url));
		}
		else
			setMaskBits(TargetURLMask);

		mURL = url;
	}
	else
	{
		if(isClientObject())
		{
			HashTable<String, FileName>::Iterator myUpdate = mFrameUrl.find(frame);
			if(myUpdate->value.compare(url) == 0)
			{
				mFrameUrl.erase(myUpdate);
				return;
			}

			// update client
		//	getWebViewTexture().setLocalFile(url, frame);
		//	getWebViewTexture().getWebView()->requestScrollData(frame);

			// sync client url changes back to server
	      GameConnection *con = GameConnection::getConnectionToServer();
         if(con)
			   con->postNetEvent(new WebViewDataURLEvent(this, url, frame));
		}
		else
		{
			mFrameUrl.insertUnique(frame, url);
			setMaskBits(FrameURLMask);
		}
	}
}
bool  WebViewData::isSeeThruPixel(Point2F point)
{
   WebViewTexture& webViewTexture = getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView)
      return true;

   GFXTextureObject* texture = webViewTexture.getTexture();
   if(!texture)
      return false;

   GFXLockedRect *lockedRect = texture->lock();
   int   alpha = 0;
   if(lockedRect){
      GFXFormat fmt = texture->getFormat();
      if(fmt==GFXFormatR8G8B8A8){
         F32 width = webViewTexture.getWidth();
         F32 height = webViewTexture.getHeight();
         int   x = (width * point.x) + (width/2.0f);
         int   y = (height * (-point.y)) + (height/2.0f);

         x = mClamp(x,0,width-1);          
         y = mClamp(y,0,height-1);          

         alpha = lockedRect->bits[ x*4 + y*lockedRect->pitch ];
      }
      texture->unlock();
   }
//   Con::printf("alpha=%d",alpha);
   return (alpha==0);
}

void WebViewData::setScroll( const Point2I& scroll, const String& frame )
{
	GameConnection *con = GameConnection::getConnectionToServer();

   if(mScroll.x == scroll.x && mScroll.y == scroll.y)
      return;

	if(frame.isEmpty())
	{
		if(isClientObject() && !(getWebViewTexture().getWebView()))
			return;

		if(mScroll == scroll)
			return;

		if(isClientObject() && con)
		{
			// update client
			getWebViewTexture().getWebView()->injectMouseWheel(mScroll.x-scroll.x, mScroll.y-scroll.y);
		//	getWebViewTexture().getWebView()->requestScrollData();

			// sync client url changes back to server
			con->postNetEvent(new WebViewDataScrollEvent(this, scroll));
		}
		else
			setMaskBits(ScrollMask);

		mScroll = scroll;
	}
	/*
	else
	{
		if(isClientObject() && con)
		{
			HashTable<String, FileName>::Iterator myUpdate = mFrameUrl.find(frame);
			if(myUpdate->value.compare(url) == 0)
			{
				mFrameUrl.erase(myUpdate);
				return;
			}

			// update client
			getWebViewTexture().setURL(url, frame);

			// sync client url changes back to server
			con->postNetEvent(new WebViewDataURLEvent(this, url, frame));
		}
		else
		{
			mFrameUrl.insertUnique(frame, url);
			setMaskBits(ScrollMask);
		}
	}
	*/
}
bool WebViewData::setResolution(U32 width, U32 height)  
{
   mResolution.x = width;
   mResolution.y = height;
   if(isClientObject())
      if (/*mWebViewTexture.isValid() && */mWebViewTexture.setSize(width, height))
         return true;
      else
         return false;

   // server only
   setMaskBits(ResolutionMask);
   return true;
}


void WebViewData::onChangeTargetURL(Awesomium::WebView* caller, 
			   const std::string& url)
{
//	getWebViewTexture().setURL(url.c_str());
//	setURL(FileName(url.c_str()));
}
void WebViewData::onOpenExternalLink(Awesomium::WebView* caller, 
				const std::string& url, 
				const std::wstring& source)
{
//	getWebViewTexture().setURL(url.c_str());
	setURL(FileName(url.c_str()));
}

void WebViewData::onBeginNavigation(Awesomium::WebView* caller, 
							   const std::string& url, 
							   const std::wstring& frameName)
{
	// message to server

//	mFrameUrl.insertUnique(frameName.c_str(), FileName(url.c_str()));
//	setMaskBits(FrameURLMask);

 //  if(!url.empty())
//   {
//      Con::printf("WebViewData::onBeginNavigation - setURL(%s)",url.c_str());
	setURL(FileName(url.c_str()), frameName.c_str());
//   }

}
void WebViewData::onBeginLoading(Awesomium::WebView* caller, 
							const std::string& url, 
							const std::wstring& frameName, 
							int statusCode, 
							const std::wstring& mimeType)
{
}

void WebViewData::setObjectCallbackScript(String objectName, String callbackName, String tsFunctionName) 
{ 
   Awesomium::WebView *view = getWebViewTexture().getWebView();
   if(view)
   {
      view->setObjectCallback(objectName.utf16(), callbackName.utf16());
      mScriptCallbackCommands[objectName + '.' + callbackName] = tsFunctionName;
   }
}

void WebViewData::onCallback(Awesomium::WebView* caller, 
								const std::wstring& objectName, 
								const std::wstring& callbackName, 
								const Awesomium::JSArguments& args)
{
   
   // call bound script function
   // todo: make this efficient
   String tsFunctionName = mScriptCallbackCommands[String(objectName.c_str()) + '.' + String(callbackName.c_str())];
   if(tsFunctionName.isNotEmpty())
   {
      tsFunctionName += '(';
      Awesomium::JSArguments::const_iterator i = args.begin();
      for(; i != args.end(); i++)
      {
         /*if(i->isString())
            tsFunctionName += (unsigned char*)i->getObject();
         else */if(i->isBoolean())
            tsFunctionName += String::ToString(i->toBoolean());
         else if(i->isDouble())
            tsFunctionName += String::ToString(i->toDouble());
         else if(i->isInteger())
            tsFunctionName += String::ToString(i->toInteger());
         if((i+1) != args.end())
            tsFunctionName += ',';
      }
      tsFunctionName += ')';
      tsFunctionName += ';';

      Con::evaluate(tsFunctionName);
   }
   
}



DefineEngineMethod( WebViewData, injectMouseDown, void, (S32 button),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture& webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView || webView->isLoadingPage())
      return;

   webView->injectMouseDown(static_cast<Awesomium::MouseButton>(button + Awesomium::LEFT_MOUSE_BTN));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   webViewTexture.refresh();
}
DefineEngineMethod( WebViewData, injectMouseUp, void, (S32 button),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture& webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView || webView->isLoadingPage())
      return;

   webView->injectMouseUp(static_cast<Awesomium::MouseButton>(button + Awesomium::LEFT_MOUSE_BTN));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   webViewTexture.refresh();
}
DefineEngineMethod( WebViewData, injectMouseMove, void, (Point2F point),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture& webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView || webView->isLoadingPage())
      return;
   object->mMouseMove = point;

   F32 width = webViewTexture.getWidth();
   F32 height = webViewTexture.getHeight();
   webView->injectMouseMove((width * point.x) + (width/2.0f), (height * (-point.y)) + (height/2.0f));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   webViewTexture.refresh();
}
DefineEngineMethod( WebViewData, injectMouseWheel, void, (Point2F scroll),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture& webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView || webView->isLoadingPage())
      return;

   webView->injectMouseWheel((int)((F32)webViewTexture.getWidth()*scroll.x), (int)((F32)webViewTexture.getHeight()*scroll.y));
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   webViewTexture.refresh();
}

DefineEngineMethod( WebViewData, injectKeyboardEvent, void, (String text),,
   "Inject a mouse event into the webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
   WebViewTexture& webViewTexture = object->getWebViewTexture();
   Awesomium::WebView* webView = webViewTexture.getWebView();
   if(!webView || webView->isLoadingPage())
      return;

   Awesomium::WebKeyboardEvent event;
   event.type = Awesomium::WebKeyboardEvent::TYPE_CHAR;
   event.text[0] = text[0];
   event.unmodifiedText[0] = text[0];

   webView->injectKeyboardEvent(event);
   //webViewTexture.updateWebCore();   // fix for FATAL:child_process_security_policy.cc(155) crash
   webViewTexture.refresh();
}

DefineEngineMethod( WebViewData, setURL, void, (String text),,
   "set url for webpage.\n"
   "@param button mouse button to simulate\n\n" )
{ 
//   WebViewTexture& webViewTexture = object->getWebViewTexture();

//   webViewTexture.setURL(text.utf8());
	object->setURL(text);
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
   Awesomium::WebView *view = object->getWebViewTexture().getWebView();
   if(view)
      view->createObject(objectName.utf16());
}

DefineEngineMethod( WebViewData, destroyObject, void, (String objectName),,
   "destroy a javascript object.\n"
   "@param \n\n" )
{ 
   Awesomium::WebView *view = object->getWebViewTexture().getWebView();
   if(view)
      view->destroyObject(objectName.utf16());
}		
	
DefineEngineMethod( WebViewData, setObjectPropertyBool, void, (String objectName, String propName, bool value),,
   "set a javascript object property.\n"
   "@param \n\n" )
{ 
   Awesomium::WebView *view = object->getWebViewTexture().getWebView();
   if(view)
      view->setObjectProperty(objectName.utf16(), propName.utf16(), Awesomium::JSValue(value));
}		
	
DefineEngineMethod( WebViewData, setObjectCallback, void, (String objectName, String callbackName, String tsFunctionName),,
   "set a javascript object property.\n"
   "@param \n\n" )
{ 
   object->setObjectCallbackScript(objectName, callbackName, tsFunctionName);
}		
	


//Awesomium::WebView::injectKeyboardEvent();



#endif