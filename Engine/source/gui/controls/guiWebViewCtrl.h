//-----------------------------------------------------------------------------
// Torque 3D
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIWEBVIEWCTRL_H_
#define _GUIWEBVIEWCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _WEBVIEWTEXTURE_H_
#include "gfx/video/webTexture.h"
#endif

#ifndef _GUIINPUTCTRL_H_
   #include "gui/utility/guiInputCtrl.h"
#endif

#if TORQUE_AWESOMIUM_VERSION >= 17

/// Renders a bitmap.
//class GuiWebViewCtrl : public GuiMouseEventCtrl, public Awesomium::WebViewListener
class GuiWebViewCtrl : public GuiMouseEventCtrl, 
   public Awesomium::WebViewListener::View,
   public Awesomium::WebViewListener::Load,
   public Awesomium::WebViewListener::Process
{
   public:
   
      typedef GuiMouseEventCtrl Parent;

   protected:
   
      /// Name of the bitmap file.  If this is 'texhandle' the bitmap is not loaded
      /// from a file but rather set explicitly on the control.
      String mURL;
      
      /// Loaded texture.
      //GFXTexHandle mTextureObject;
      WebViewTexture mWebViewTexture;

      Point2I mStartPoint;
      
      /// If true, bitmap tiles inside control.  Otherwise stretches.
      bool mWrap;
      bool mLockAspect;
      bool mForceNativeExtent;

      static bool setURLString( void *object, const char *index, const char *data );
      static const char *setURLString( void *obj, const char *data );

   public:
      
      GuiWebViewCtrl();
      ~GuiWebViewCtrl();

      static void initPersistFields();

    //  void setBitmap(const char *name,bool resize = false);
    //  void setBitmapHandle(GFXTexHandle handle, bool resize = false);

      // GuiControl.
      bool onWake();
      void onSleep();
      void inspectPostApply();

      void updateSizing();

      void onRender(Point2I offset, const RectI &updateRect);
      void setValue(S32 x, S32 y);
      bool setURL( const String& url )
      {
         if(mWebViewTexture.setURL(url))
         {
         //   mWebViewTexture.getWebView()->setListener(this);
            mWebViewTexture.getWebView()->set_view_listener(this);
            mWebViewTexture.getWebView()->set_load_listener(this);
            mWebViewTexture.getWebView()->set_process_listener(this);

            return true;
         }

         return false;
      }

      bool setExtent( const Point2I& newExtent);
      inline  void setExtent( const S32 width, const S32 height) { setExtent(Point2I(width, height)); }

      Point2I getNativeExtent() const;

      /// General input handler.
      void handleKeyboard(WindowId did, UINT message, WPARAM wParam, WPARAM lParam);
      bool onInputEvent(const InputEventInfo &event);

      void onMouseUp(const GuiEvent &event);
      void onMouseDown(const GuiEvent &event);
      void onMouseMove(const GuiEvent &event);
      void onMouseDragged(const GuiEvent &event);
      void onMouseEnter(const GuiEvent &event);
      void onMouseLeave(const GuiEvent &event);
      
      bool onMouseWheelUp(const GuiEvent &event);
      bool onMouseWheelDown(const GuiEvent &event);
      
      void onRightMouseDown(const GuiEvent &event);
      void onRightMouseUp(const GuiEvent &event);
      void onRightMouseDragged(const GuiEvent &event);
      
      void onMiddleMouseDown(const GuiEvent &event);
      void onMiddleMouseUp(const GuiEvent &event);
      void onMiddleMouseDragged(const GuiEvent &event);
/*
      void onGetScrollData(Awesomium::WebView* caller,
									       int contentWidth,
									       int contentHeight,
									       int preferredWidth,
									       int scrollX,
									       int scrollY);

		void onBeginNavigation(Awesomium::WebView* caller, 
									   const std::string& url, 
									   const std::wstring& frameName) {}
		void onBeginLoading(Awesomium::WebView* caller, 
									const std::string& url, 
									const std::wstring& frameName, 
									int statusCode, 
									const std::wstring& mimeType) {}
      void onFinishLoading(Awesomium::WebView* caller);
		void onCallback(Awesomium::WebView* caller, 
								const std::wstring& objectName, 
								const std::wstring& callbackName, 
								const Awesomium::JSArguments& args) {}
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
									   const std::string& url) {}
		void onOpenExternalLink(Awesomium::WebView* caller, 
										const std::string& url, 
										const std::wstring& source) {}
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
                             const Awesomium::WebString& title) {};

  virtual void OnChangeAddressBar(Awesomium::WebView* caller,
                                  const Awesomium::WebURL& url) {};

  virtual void OnChangeTooltip(Awesomium::WebView* caller,
                               const Awesomium::WebString& tooltip) {};

  virtual void OnChangeTargetURL(Awesomium::WebView* caller,
                                 const Awesomium::WebURL& url) {};

  virtual void OnChangeCursor(Awesomium::WebView* caller,
                              Awesomium::Cursor cursor) {};

  virtual void OnChangeFocus(Awesomium::WebView* caller,
                                Awesomium::FocusedElementType focus_type) {};

  virtual void OnShowCreatedWebView(Awesomium::WebView* caller,
                                    Awesomium::WebView* new_view,
                                    const Awesomium::WebURL& opener_url,
                                    const Awesomium::WebURL& target_url,
                                    const Awesomium::Rect& initial_pos,
                                    bool is_popup) {};

  virtual void OnBeginLoadingFrame(Awesomium::WebView* caller,
                                   int64 frame_id,
                                   bool is_main_frame,
                                   const Awesomium::WebURL& url,
                                   bool is_error_page) {};

  virtual void OnFailLoadingFrame(Awesomium::WebView* caller,
                                  int64 frame_id,
                                  bool is_main_frame,
                                  const Awesomium::WebURL& url,
                                  int error_code,
                                  const Awesomium::WebString& error_description) {};

  virtual void OnFinishLoadingFrame(Awesomium::WebView* caller,
                                    int64 frame_id,
                                    bool is_main_frame,
                                    const Awesomium::WebURL& url) {};

  virtual void OnDocumentReady(Awesomium::WebView* caller,
                                    const Awesomium::WebURL& url) {};

  virtual void OnUnresponsive(Awesomium::WebView* caller) {};

  virtual void OnResponsive(Awesomium::WebView* caller) {};

  virtual void OnCrashed(Awesomium::WebView* caller,
                         Awesomium::TerminationStatus status) {};
		
      DECLARE_CONOBJECT( GuiWebViewCtrl );
      DECLARE_CATEGORY( "Gui Images" );
      DECLARE_DESCRIPTION( "A control that displays a single, static image from a file.\n"
                           "The bitmap can either be tiled or stretched inside the control." );

	//   DECLARE_CALLBACK( void, onInputEvent, ( const char* device, const char* action, bool state ));
};

#else
#endif

#endif
