#ifndef _WEBVIEWTEXTURE_H_
#define _WEBVIEWTEXTURE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _GFXTEXTUREHANDLE_H_
   #include "gfx/gfxTextureHandle.h"
#endif

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

#ifndef _CONDITIONER_BASE_H_
#include "shaderGen/conditionerFeature.h"
#endif

#ifndef _MATTEXTURETARGET_H_
#include "materials/matTextureTarget.h"
#endif

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif

#ifndef _TDICTIONARY_H_
#include "core/util/tDictionary.h"
#endif

#ifndef __AWESOMIUM_WEB_CORE_H__
#include <Awesomium/WebCore.h>
#endif


#define TORQUE_AWESOMIUM_VERSION 17

#if TORQUE_AWESOMIUM_VERSION >= 17


class Material;

class WebViewTexture
{
private:
   String mURL;
   Awesomium::WebView* mWebView;
   GFXTexHandle mTexture;

   Awesomium::WebTouchEvent mTouchEvents;

   U32 mWidth;
   U32 mHeight;
#ifdef   CLEAN_PAGE
   bool  mPageLoaded;
#endif
   bool  mGhost;

   static U32 webViewCount;
   static Awesomium::WebSession* webSession;
   static Awesomium::WebCore* webCore;

   static void destroyWebCore();

   void _onTextureEvent( GFXTexCallbackCode code );

public:
   WebViewTexture();
   WebViewTexture(U32 width, U32 height);
   ~WebViewTexture();

   static void updateWebCore();

   void  setGhost(bool _flag){mGhost=_flag;}
   GFXTexHandle& getTexture()                      { return mTexture; }
   Awesomium::WebTouchEvent& getTouchEvents()       { return mTouchEvents; }   

   Awesomium::WebSession* getWebSession();
   Awesomium::WebView* getWebView();
   void setWebView(Awesomium::WebView& webView)    { mWebView = &webView; }

   bool isValid() const             { return mTexture.isValid(); }

   U32 getWidth() const             { return mWidth; }
   U32 getHeight() const            { return mHeight; }
   Point2I getWidthHeight() const   { return Point2I(mWidth, mHeight); }

   bool setSize(U32 width, U32 height);

   bool setURL( const FileName& url, 
				 const String& frameName = "", 
				 const String& username = "", 
				 const String& password = ""  );
   const String& getURL()                 { return mURL; }

   void destroy();

   void refresh();
   void clear();

   static Awesomium::WebCore* createWebCore();  
};


//class WebViewData : public NetObject, public Awesomium::WebViewListener
class WebViewData : public NetObject,
   public Awesomium::WebViewListener::View,
   public Awesomium::WebViewListener::Load,
   public Awesomium::WebViewListener::Process,
   public Awesomium::JSMethodHandler
{
private:

   typedef NetObject Parent;

   enum WebViewDataMasks
   { 
		InitialUpdateMask = BIT( 0 ),
		TargetURLMask     = BIT( 1 ),
		FrameURLMask      = BIT( 2 ),
		ResolutionMask		  = BIT( 3 ),
		ScrollMask		  = BIT( 4 ),
		NextFreeMask      = BIT( 5 ),
   };   

   Point2I mResolution;
   bool mResizeToNative;

   FileName mURL;
   Point2I mScroll;

   WebViewTexture* mWebViewTexture;
   HashTable<String, FileName> mFrameUrl;

   bool mDisableHScrollBar;
   bool mDisableVScrollBar;

   Map<String, CompoundKey<String,String>> mScriptCallbackCommands;
   bool mIsClientOnly;

public:

   WebViewData();
   ~WebViewData();
   Point2F mMouseMove;

   bool        onAdd();
   void        onRemove();

   static void initPersistFields();
   void inspectPostApply();
   U32         packUpdate(NetConnection* conn, U32 mask, BitStream* stream);
   void        unpackUpdate(NetConnection* conn, BitStream* stream);
   bool        isSeeThruPixel(Point2F _where);

   void        refresh();
   void        refreshPageLoad(U32 timeElapsed) { if(timeElapsed>=32)  refresh(); }
   
   void  clear()        {   if(mWebViewTexture) mWebViewTexture->clear(); }

   WebViewTexture* getWebViewTexture()
   {
      refresh();
      if(mWebViewTexture && mWebViewTexture->getWebView())
         mWebViewTexture->getWebView()->ResumeRendering();
      return mWebViewTexture;
   }
   void setURL( const FileName& url, const String& frame = "" );
   const FileName *getURL(){return &mURL;}

   void setScroll( const Point2I& scroll, const String& frame = "" );
   void setObjectCallbackScript(String objectName, String callbackName, String tsFunctionName, String args);

   U32 getWidth() const                   { return (mWebViewTexture && (mWebViewTexture->isValid()) ? mWebViewTexture->getWidth() : 0);  }
   U32 getHeight() const                  { return (mWebViewTexture && (mWebViewTexture->isValid()) ? mWebViewTexture->getHeight() : 0);  }
   Point2I getResolution() const          { return (mWebViewTexture && (mWebViewTexture->isValid()) ? Point2I(mWebViewTexture->getWidth(), mWebViewTexture->getHeight()) : Point2I(0, 0)); }
   bool setResolution(U32 width, U32 height);
   
  virtual void OnChangeTitle(Awesomium::WebView* caller,
                             const Awesomium::WebString& title);

  virtual void OnChangeAddressBar(Awesomium::WebView* caller,
                                  const Awesomium::WebURL& url);

  virtual void OnChangeTooltip(Awesomium::WebView* caller,
                               const Awesomium::WebString& tooltip);

  virtual void OnChangeTargetURL(Awesomium::WebView* caller,
                                 const Awesomium::WebURL& url);

  virtual void OnChangeCursor(Awesomium::WebView* caller,
                              Awesomium::Cursor cursor);

  virtual void OnChangeFocus(Awesomium::WebView* caller,
                                Awesomium::FocusedElementType focus_type);

  virtual void OnShowCreatedWebView(Awesomium::WebView* caller,
                                    Awesomium::WebView* new_view,
                                    const Awesomium::WebURL& opener_url,
                                    const Awesomium::WebURL& target_url,
                                    const Awesomium::Rect& initial_pos,
                                    bool is_popup);

  virtual void OnBeginLoadingFrame(Awesomium::WebView* caller,
                                   int64 frame_id,
                                   bool is_main_frame,
                                   const Awesomium::WebURL& url,
                                   bool is_error_page);

  virtual void OnFailLoadingFrame(Awesomium::WebView* caller,
                                  int64 frame_id,
                                  bool is_main_frame,
                                  const Awesomium::WebURL& url,
                                  int error_code,
                                  const Awesomium::WebString& error_description);

  virtual void OnFinishLoadingFrame(Awesomium::WebView* caller,
                                    int64 frame_id,
                                    bool is_main_frame,
                                    const Awesomium::WebURL& url);

  virtual void OnDocumentReady(Awesomium::WebView* caller,
                                    const Awesomium::WebURL& url);

  virtual void OnUnresponsive(Awesomium::WebView* caller);

  virtual void OnResponsive(Awesomium::WebView* caller);

  virtual void OnCrashed(Awesomium::WebView* caller,
                         Awesomium::TerminationStatus status);

  virtual void OnMethodCall(Awesomium::WebView* caller,
                            unsigned int remote_object_id, 
                            const Awesomium::WebString& method_name,
                            const Awesomium::JSArray& args);

  virtual Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView* caller,
                                              unsigned int remote_object_id,
                                              const Awesomium::WebString& method_name,
                                              const Awesomium::JSArray& args);


   DECLARE_CONOBJECT(WebViewData);
   DECLARE_CALLBACK(void, onGetScrollData, (U32 contentWidth, U32 contentHeight, U32 preferredWidth, S32 scrollX, S32 scrollY) );
   DECLARE_CALLBACK(void, onFinishLoading, () );

};


// Depth buffer texture
GFX_DeclareTextureProfile(GFXWebViewTextureProfile);


#endif
#endif


