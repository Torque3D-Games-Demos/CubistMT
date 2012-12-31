#ifndef _WEBVIEWTEXTURE_H_
#define _WEBVIEWTEXTURE_H_

//#define   CLEAN_PAGE


#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _GFXTEXTUREHANDLE_H_
   #include "gfx/gfxTextureHandle.h"
#endif

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif

//#ifndef _ADVANCEDLIGHTMANAGER_H_
//#include "lighting/advanced/advancedLightManager.h"
//#endif
//#ifndef _ADVANCED_LIGHTBUFFER_CONDITIONER_H_
//#include "lighting/advanced/advancedLightBufferConditioner.h"
//#endif

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
//#if defined(__WIN32__) || defined(_WIN32)
//#include "WebCore.h"
//#elif defined(__APPLE__)
#include <Awesomium/WebCore.h>
//#endif
#endif


#define TORQUE_AWESOMIUM_VERSION 17

#if TORQUE_AWESOMIUM_VERSION >= 17


/*
class webTexture
{
  
      /// The Theora video file.
      String mURL;

      
      /// The SFXDescription used for sound playback.  Synthesized if not provided.
   //   SimObjectPtr< SFXDescription > mSFXDescription;
                  
      /// If there's a Vorbis stream, this is the sound source used for playback.
      /// Playback will synchronize to this source then.
  //    SimObjectPtr< SFXSound > mSFXSource;
      

      ///
      void _onTextureEvent( GFXTexCallbackCode code );

}
*/
/*

class WebCoreData : public SimObject
{
   typedef SimObject Parent;
	bool mEnablePlugins;
	bool mEnableJavascript;
	String mUserDataPath;
	String mPluginPath;
	String mLogPath;
	LogLevel mLogLevel;
	String mUserAgentOverride;
	String mProxyServer;
	String mProxyConfigScript;
	bool mSaveCacheAndCookies;
	int mMaxCacheSize;
	bool mDisableSameOriginPolicy;
	String customCSS;
public:
   WebCoreData();
   ~WebCoreData();

   // SimObject
   virtual bool onAdd();
   static void initPersistFields();  

   Awesomium::WebView* getWebView();
   setWebView(Awesomium::WebView* webView);

   DECLARE_CONOBJECT(WebCoreData);   
};
class WebCoreManager
{
private:

protected:
   static Awesomium::WebCore* mWebCore;

public:
   WebCoreManager();
   ~WebCoreManager();

   Awesomium::WebCore* getWebCore()          { return mWebCore;      }
   //setWebView(Awesomium::WebCore* webCore)   { mWebCore = webCore;   }
};
*/
/*
class WebViewData: public GameBaseData
{
private:
   typedef GameBaseData Parent;

      /// The Theora video file.
      String mURL;

      
      /// The SFXDescription used for sound playback.  Synthesized if not provided.
   //   SimObjectPtr< SFXDescription > mSFXDescription;
                  
      /// If there's a Vorbis stream, this is the sound source used for playback.
      /// Playback will synchronize to this source then.
  //    SimObjectPtr< SFXSound > mSFXSource;
      


   Awesomium::WebView* mWebView;
   GFXTexHandle mTexture;
   NamedTexTarget mNamedTarget;
//   AdvancedLightBufferConditioner *mConditioner;
   U32 mWidth;
   U32 mHeight;

      ///
      void _onTextureEvent( GFXTexCallbackCode code );

public:
   WebViewData();
   ~WebViewData();

   static void initPersistFields();
   bool onAdd();
   void packData(BitStream* stream);
   void unpackData(BitStream* stream);
   bool preload(bool server, String &errorStr);

   //GFXTexHandle& getTexture()                      { return mTexture; }
   GFXTextureObject *getTexture()                      { return mNamedTarget.getTexture(); }

   Awesomium::WebView* getWebView()                { return mWebView; }
   void setWebView(Awesomium::WebView* webView)    { mWebView = webView; }

   U32 getWidth() const;
   U32 getHeight() const;

   bool setURL( const String& url );
   void refresh();

   DECLARE_CONOBJECT(WebViewData);   
};
*/

class Material;

class WebViewTexture // : public ProcessObject
{
private:
//   typedef GameBaseData Parent;

      /// The Theora video file.
      String mURL;

      
      /// The SFXDescription used for sound playback.  Synthesized if not provided.
   //   SimObjectPtr< SFXDescription > mSFXDescription;
                  
      /// If there's a Vorbis stream, this is the sound source used for playback.
      /// Playback will synchronize to this source then.
  //    SimObjectPtr< SFXSound > mSFXSource;
      


   Awesomium::WebView* mWebView;
   GFXTexHandle mTexture;
//   GBitmap* mBitmap;

   Awesomium::WebTouchEvent mTouchEvents;

//   AdvancedLightBufferConditioner *mConditioner;
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

      ///
      void _onTextureEvent( GFXTexCallbackCode code );

public:
   WebViewTexture();
   WebViewTexture(U32 width, U32 height);
   ~WebViewTexture();

   static void updateWebCore();
#ifdef   CLEAN_PAGE
   void  setPageLoaded(bool _flag);
#endif
   void  setGhost(bool _flag){mGhost=_flag;}
   GFXTexHandle& getTexture()                      { return mTexture; }
   Awesomium::WebTouchEvent& getTouchEvents()       { return mTouchEvents; }
   //GFXTextureObject *getTexture()                      { return mNamedTarget.getTexture(); }
   

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
 //  bool setLocalFile( const FileName& url, const String& frameName = "" );
   void destroy();

   void refresh();
   void clear();

   static Awesomium::WebCore* createWebCore();

//   DECLARE_CONOBJECT(WebViewTexture);   
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
//   String mURL;
   WebViewTexture* mWebViewTexture;
   HashTable<String, FileName> mFrameUrl;
//   HashTable<String, Point2F> mFrameScroll;
//   NamedTexTarget mNamedTarget;
//   Material* mMaterial;
   bool mDisableHScrollBar;
   bool mDisableVScrollBar;
//   Map<String, String> mScriptCallbackCommands;
//   Map<String, String> mScriptCallbackArgs;
   Map<String, CompoundKey<String,String>> mScriptCallbackCommands;
   bool mIsClientOnly;
#ifdef   CLEAN_PAGE
   bool  mPageLoaded;
#endif

//   bool refresh;

public:

   WebViewData();
   ~WebViewData();
   Point2F mMouseMove;

   bool        onAdd();
   void        onRemove();
//   bool        preload( bool server, String &errorStr );
   static void initPersistFields();
   void inspectPostApply();
   U32         packUpdate(NetConnection* conn, U32 mask, BitStream* stream);
   void        unpackUpdate(NetConnection* conn, BitStream* stream);
   bool        isSeeThruPixel(Point2F _where);
#ifdef   CLEAN_PAGE
   void        setPageLoaded(bool _pageLoaded);
#endif
//   const WebViewData * getServerObject();

//   void        refresh()                  { if(mWebViewTexture) mWebViewTexture->refresh(); }
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
 //  void setLocalFile( const FileName& url, const String& frame = ""  );


   void setScroll( const Point2I& scroll, const String& frame = "" );
   void setObjectCallbackScript(String objectName, String callbackName, String tsFunctionName, String args);

//   NamedTexTarget& getNamedTexTarget()    { return mNamedTarget; }
   

   U32 getWidth() const                   { return (mWebViewTexture && (mWebViewTexture->isValid()) ? mWebViewTexture->getWidth() : 0);  }
   U32 getHeight() const                  { return (mWebViewTexture && (mWebViewTexture->isValid()) ? mWebViewTexture->getHeight() : 0);  }
   Point2I getResolution() const          { return (mWebViewTexture && (mWebViewTexture->isValid()) ? Point2I(mWebViewTexture->getWidth(), mWebViewTexture->getHeight()) : Point2I(0, 0)); }
   bool setResolution(U32 width, U32 height);

   /*
      void onGetScrollData(Awesomium::WebView* caller,
									       int contentWidth,
									       int contentHeight,
									       int preferredWidth,
									       int scrollX,
									       int scrollY);

		void onBeginNavigation(Awesomium::WebView* caller, 
									   const std::string& url, 
									   const std::wstring& frameName);
		void onBeginLoading(Awesomium::WebView* caller, 
									const std::string& url, 
									const std::wstring& frameName, 
									int statusCode, 
									const std::wstring& mimeType);
      void onFinishLoading(Awesomium::WebView* caller);
		void onCallback(Awesomium::WebView* caller, 
								const std::wstring& objectName, 
								const std::wstring& callbackName, 
								const Awesomium::JSArguments& args);
		void onReceiveTitle(Awesomium::WebView* caller, 
									const std::wstring& title, 
									const std::wstring& frameName) {}
		void onChangeTooltip(Awesomium::WebView* caller, 
									 const std::wstring& tooltip) {}
		void onChangeCursor(Awesomium::WebView* caller, 
									Awesomium::CursorType cursor) {}
		void onChangeKeyboardFocus(Awesomium::WebView* caller, 
										   bool isFocused) {}
		void onChangeTargetURL(Awesomium::WebView* caller, 
									   const std::string& url);
		void onOpenExternalLink(Awesomium::WebView* caller, 
										const std::string& url, 
										const std::wstring& source);
		void onRequestDownload(Awesomium::WebView* caller,
										const std::string& url) {}
		void onWebViewCrashed(Awesomium::WebView* caller) {}
		void onPluginCrashed(Awesomium::WebView* caller, 
									 const std::wstring& pluginName) {}
		void onRequestMove(Awesomium::WebView* caller, 
								   int x, int y) {}
		void onGetPageContents(Awesomium::WebView* caller, 
									   const std::string& url, 
									   const std::wstring& contents);
		void onDOMReady(Awesomium::WebView* caller) {}
		void onRequestFileChooser(Awesomium::WebView* caller,
										  bool selectMultipleFiles,
										  const std::wstring& title,
                                const std::wstring& defaultPath) {}
		void onJavascriptConsoleMessage(Awesomium::WebView* caller,
												const std::wstring& message,
												int lineNumber,
												const std::wstring& source) {}
		void onGetFindResults(Awesomium::WebView* caller,
                                      int requestID,
                                      int numMatches,
                                      const Awesomium::Rect& selection,
                                      int curMatch,
									  bool finalUpdate) {}
		void onUpdateIME(Awesomium::WebView* caller,
                                 Awesomium::IMEState imeState,
								 const Awesomium::Rect& caretRect) {}
		void onShowContextMenu(Awesomium::WebView* caller,
                                       int mouseX,
									   int mouseY,
									   Awesomium::MediaType type,
									   int mediaState,
									   const std::string& linkURL,
									   const std::string& srcURL,
									   const std::string& pageURL,
									   const std::string& frameURL,
									   const std::wstring& selectionText,
									   bool isEditable,
									   int editFlags) {}

		void onRequestLogin(Awesomium::WebView* caller,
                                       int requestID,
									   const std::string& requestURL,
									   bool isProxy,
									   const std::wstring& hostAndPort,
									   const std::wstring& scheme,
									   const std::wstring& realm) {}
		void onChangeHistory(Awesomium::WebView* caller,
										int backCount,
										int forwardCount) {}
		void onFinishResize(Awesomium::WebView* caller,
										int width,
										int height) {}

		void onShowJavascriptDialog(Awesomium::WebView* caller,
												int requestID,
												int dialogFlags,
												const std::wstring& message,
												const std::wstring& defaultPrompt,
												const std::string& frameURL) {}
*/
   
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

/*
   void advanceTime( F32 delta ) { 
      if(mWebViewTexture)
         mWebViewTexture->refresh(); 
   }
*/

   DECLARE_CONOBJECT(WebViewData);
   DECLARE_CALLBACK(void, onGetScrollData, (U32 contentWidth, U32 contentHeight, U32 preferredWidth, S32 scrollX, S32 scrollY) );
   DECLARE_CALLBACK(void, onFinishLoading, () );

};


// Depth buffer texture
GFX_DeclareTextureProfile(GFXWebViewTextureProfile);




#else


class Material;

class WebViewTexture
{
private:
//   typedef GameBaseData Parent;

      /// The Theora video file.
      String mURL;

      
      /// The SFXDescription used for sound playback.  Synthesized if not provided.
   //   SimObjectPtr< SFXDescription > mSFXDescription;
                  
      /// If there's a Vorbis stream, this is the sound source used for playback.
      /// Playback will synchronize to this source then.
  //    SimObjectPtr< SFXSound > mSFXSource;
      


   Awesomium::WebView* mWebView;
   GFXTexHandle mTexture;
//   GBitmap* mBitmap;

//   AdvancedLightBufferConditioner *mConditioner;
   U32 mWidth;
   U32 mHeight;

   static U32 webViewCount;
   static Awesomium::WebCore* webCore;
   static Awesomium::WebCore* createWebCore();

   static void destroyWebCore();

      ///
      void _onTextureEvent( GFXTexCallbackCode code );

public:
   WebViewTexture();
   WebViewTexture(U32 width, U32 height);
   ~WebViewTexture();

   static void updateWebCore();

   GFXTexHandle& getTexture()                      { return mTexture; }
   //GFXTextureObject *getTexture()                      { return mNamedTarget.getTexture(); }

   Awesomium::WebView* getWebView()                { return mWebView; }
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
   bool setLocalFile( const FileName& url, const String& frameName = "" );
   void destroy();

   void refresh();
   void  clear();


//   DECLARE_CONOBJECT(WebViewTexture);   
};


class WebViewData : public NetObject, public Awesomium::WebViewListener
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
//   String mURL;
   WebViewTexture mWebViewTexture;
   HashTable<String, FileName> mFrameUrl;
//   HashTable<String, Point2F> mFrameScroll;
//   NamedTexTarget mNamedTarget;
//   Material* mMaterial;
   bool mDisableHScrollBar;
   bool mDisableVScrollBar;
   Map<String, String> mScriptCallbackCommands;
   bool mIsClientOnly;

public:

   WebViewData();
   Point2F mMouseMove;

   bool        onAdd();
   void        onRemove();
//   bool        preload( bool server, String &errorStr );
   static void initPersistFields();
   void inspectPostApply();
   U32         packUpdate(NetConnection* conn, U32 mask, BitStream* stream);
   void        unpackUpdate(NetConnection* conn, BitStream* stream);
   bool        isSeeThruPixel(Point2F _where);

//   const WebViewData * getServerObject();

//   void        refresh()                  { if(mWebViewTexture) mWebViewTexture->refresh(); }
   void        refresh()                  
   {
//      if(mWebViewTexture)
         mWebViewTexture.refresh(); 
//      mNamedTarget.setTexture(mWebViewTexture->getTexture());
   
   
   }
   void  clear();

   WebViewTexture& getWebViewTexture()    { return mWebViewTexture; }
   void setURL( const FileName& url, const String& frame = "" );
   void setLocalFile( const FileName& url, const String& frame = ""  );


   void setScroll( const Point2I& scroll, const String& frame = "" );
   void setObjectCallbackScript(String objectName, String callbackName, String tsFunctionName);

//   NamedTexTarget& getNamedTexTarget()    { return mNamedTarget; }
   

   U32 getWidth() const                   { return ((mWebViewTexture.isValid()) ? mWebViewTexture.getWidth() : 0);  }
   U32 getHeight() const                  { return ((mWebViewTexture.isValid()) ? mWebViewTexture.getHeight() : 0);  }
   Point2I getResolution() const          { return ((mWebViewTexture.isValid()) ? Point2I(mWebViewTexture.getWidth(), mWebViewTexture.getHeight()) : Point2I(0, 0)); }
   bool setResolution(U32 width, U32 height);

      void onGetScrollData(Awesomium::WebView* caller,
									       int contentWidth,
									       int contentHeight,
									       int preferredWidth,
									       int scrollX,
									       int scrollY);

		void onBeginNavigation(Awesomium::WebView* caller, 
									   const std::string& url, 
									   const std::wstring& frameName);
		void onBeginLoading(Awesomium::WebView* caller, 
									const std::string& url, 
									const std::wstring& frameName, 
									int statusCode, 
									const std::wstring& mimeType);
      void onFinishLoading(Awesomium::WebView* caller);
		void onCallback(Awesomium::WebView* caller, 
								const std::wstring& objectName, 
								const std::wstring& callbackName, 
								const Awesomium::JSArguments& args);
		void onReceiveTitle(Awesomium::WebView* caller, 
									const std::wstring& title, 
									const std::wstring& frameName) {}
		void onChangeTooltip(Awesomium::WebView* caller, 
									 const std::wstring& tooltip) {}
		void onChangeCursor(Awesomium::WebView* caller, 
									Awesomium::CursorType cursor) {}
		void onChangeKeyboardFocus(Awesomium::WebView* caller, 
										   bool isFocused) {}
		void onChangeTargetURL(Awesomium::WebView* caller, 
									   const std::string& url);
		void onOpenExternalLink(Awesomium::WebView* caller, 
										const std::string& url, 
										const std::wstring& source);
		void onRequestDownload(Awesomium::WebView* caller,
										const std::string& url) {}
		void onWebViewCrashed(Awesomium::WebView* caller) {}
		void onPluginCrashed(Awesomium::WebView* caller, 
									 const std::wstring& pluginName) {}
		void onRequestMove(Awesomium::WebView* caller, 
								   int x, int y) {}
		void onGetPageContents(Awesomium::WebView* caller, 
									   const std::string& url, 
									   const std::wstring& contents);
		void onDOMReady(Awesomium::WebView* caller) {}
		void onRequestFileChooser(Awesomium::WebView* caller,
										  bool selectMultipleFiles,
										  const std::wstring& title,
                                const std::wstring& defaultPath) {}
		void onJavascriptConsoleMessage(Awesomium::WebView* caller,
												const std::wstring& message,
												int lineNumber,
												const std::wstring& source) {}
		void onGetFindResults(Awesomium::WebView* caller,
                                      int requestID,
                                      int numMatches,
                                      const Awesomium::Rect& selection,
                                      int curMatch,
									  bool finalUpdate) {}
		void onUpdateIME(Awesomium::WebView* caller,
                                 Awesomium::IMEState imeState,
								 const Awesomium::Rect& caretRect) {}
		void onShowContextMenu(Awesomium::WebView* caller,
                                       int mouseX,
									   int mouseY,
									   Awesomium::MediaType type,
									   int mediaState,
									   const std::string& linkURL,
									   const std::string& srcURL,
									   const std::string& pageURL,
									   const std::string& frameURL,
									   const std::wstring& selectionText,
									   bool isEditable,
									   int editFlags) {}

		void onRequestLogin(Awesomium::WebView* caller,
                                       int requestID,
									   const std::string& requestURL,
									   bool isProxy,
									   const std::wstring& hostAndPort,
									   const std::wstring& scheme,
									   const std::wstring& realm) {}
		void onChangeHistory(Awesomium::WebView* caller,
										int backCount,
										int forwardCount) {}
		void onFinishResize(Awesomium::WebView* caller,
										int width,
										int height) {}

		void onShowJavascriptDialog(Awesomium::WebView* caller,
												int requestID,
												int dialogFlags,
												const std::wstring& message,
												const std::wstring& defaultPrompt,
												const std::string& frameURL) {}

   DECLARE_CONOBJECT(WebViewData);
   DECLARE_CALLBACK(void, onGetScrollData, (U32 contentWidth, U32 contentHeight, U32 preferredWidth, S32 scrollX, S32 scrollY) );
   DECLARE_CALLBACK(void, onFinishLoading, () );

};


// Depth buffer texture
GFX_DeclareTextureProfile(GFXWebViewTextureProfile);


#endif



#endif


