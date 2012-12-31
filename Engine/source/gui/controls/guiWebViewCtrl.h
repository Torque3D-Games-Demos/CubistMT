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

/// Renders a web page.
class GuiWebViewCtrl : public GuiMouseEventCtrl, 
   public Awesomium::WebViewListener::View,
   public Awesomium::WebViewListener::Load,
   public Awesomium::WebViewListener::Process,
   public Awesomium::JSMethodHandler
{
   public:
   
      typedef GuiMouseEventCtrl Parent;

   protected:
   
      /// Name of the bitmap file.  If this is 'texhandle' the bitmap is not loaded
      /// from a file but rather set explicitly on the control.
      String mURL;
      
      /// Loaded texture.
      //GFXTexHandle mTextureObject;
      WebViewTexture* mWebViewTexture;

      Point2I mStartPoint;
      
      /// If true, bitmap tiles inside control.  Otherwise stretches.
      bool mWrap;
      bool mLockAspect;
      bool mForceNativeExtent;

      Map<String, CompoundKey<String,String>> mScriptCallbackCommands;

      static bool setURLString( void *object, const char *index, const char *data );
      static const char *setURLString( void *obj, const char *data );

   public:
      
      GuiWebViewCtrl();
      ~GuiWebViewCtrl();

      static void initPersistFields();

      // GuiControl.
      bool onWake();
      void onSleep();
      void inspectPostApply();

      void updateSizing();

      void onRender(Point2I offset, const RectI &updateRect);
      void setValue(S32 x, S32 y);
      bool setURL( const String& url )
      {
         if(!mWebViewTexture)
         {
            Point2I extent = getBounds().extent;
            mWebViewTexture = new WebViewTexture(extent.x,extent.y);
         }
         if(mWebViewTexture && mWebViewTexture->setURL(url))
         {
            Awesomium::WebView* view = mWebViewTexture->getWebView();
            if(view)
            {
               view->set_view_listener(this);
               view->set_load_listener(this);
               view->set_process_listener(this);
               return true;
            }
         }

         return false;
      }

      bool setExtent( const Point2I& newExtent);
      inline  void setExtent( const S32 width, const S32 height) { setExtent(Point2I(width, height)); }

      Point2I getNativeExtent() const;
      void setObjectCallbackScript(String objectName, String callbackName, String tsFunctionName, String args);

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
                                       bool is_popup);

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
                                       const Awesomium::WebURL& url);

     virtual void OnDocumentReady(Awesomium::WebView* caller,
                                       const Awesomium::WebURL& url) {};

     virtual void OnUnresponsive(Awesomium::WebView* caller) {};

     virtual void OnResponsive(Awesomium::WebView* caller) {};

     virtual void OnCrashed(Awesomium::WebView* caller,
                            Awesomium::TerminationStatus status) {};
   		
     virtual void OnMethodCall(Awesomium::WebView* caller,
                               unsigned int remote_object_id, 
                               const Awesomium::WebString& method_name,
                               const Awesomium::JSArray& args);

     virtual Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView* caller,
                                                 unsigned int remote_object_id,
                                                 const Awesomium::WebString& method_name,
                                                 const Awesomium::JSArray& args);


      DECLARE_CONOBJECT( GuiWebViewCtrl );
      DECLARE_CATEGORY( "Gui Web Browser" );
      DECLARE_DESCRIPTION( "A control that displays a web browser." );

      DECLARE_CALLBACK(void, onGetScrollData, (U32 contentWidth, U32 contentHeight, U32 preferredWidth, S32 scrollX, S32 scrollY) );
      DECLARE_CALLBACK(void, onFinishLoading, () );
};

#else
#endif

#endif
