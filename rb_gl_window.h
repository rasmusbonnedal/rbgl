#ifndef RB_GL_WINDOW_H
#define RB_GL_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

enum create_window_flags
{
    RBGL_CWF_NONE = 0,
    RBGL_CWF_CORE_32 = 1,
    RBGL_CWF_CORE_41 = 2
};

int rbgl_create_window(int width, int height, const char* title, enum create_window_flags flags);
void rbgl_destroy_window();
void rbgl_get_window_size(int* width, int* height);
void rbgl_app_terminate();
void rbgl_poll_events();
void rbgl_swap_buffers();
int rbgl_window_should_close();
void rbgl_make_context_current();

#ifdef __cplusplus
}
#endif

#endif /* RB_GL_WINDOW_H */

#ifdef RB_GL_WINDOW_IMPLEMENTATION

#include <limits.h>
#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CGBase.h>
#include <CoreGraphics/CGGeometry.h>
#include <objc/objc-runtime.h>

typedef struct
{
    int width;
    int height;
    id delegate;
    id object;
    id view;
    bool should_close;
    /* GL Context */
    id pixel_format;
    id gl_context;
} rb_gl_window;

static rb_gl_window the_gl_window;

static struct
{
    id delegate;
    id autoreleasePool; 
} the_app_context;

extern id NSApp;
extern id const NSDefaultRunLoopMode;

typedef CGPoint NSPoint;
typedef CGSize NSSize;
typedef CGRect NSRect;

enum {
   NSOpenGLPFAAllRenderers       =   1,
   NSOpenGLPFATripleBuffer       =   3,
   NSOpenGLPFADoubleBuffer       =   5,
   NSOpenGLPFAStereo             =   6,
   NSOpenGLPFAAuxBuffers         =   7,
   NSOpenGLPFAColorSize          =   8,
   NSOpenGLPFAAlphaSize          =  11,
   NSOpenGLPFADepthSize          =  12,
   NSOpenGLPFAStencilSize        =  13,
   NSOpenGLPFAAccumSize          =  14,
   NSOpenGLPFAMinimumPolicy      =  51,
   NSOpenGLPFAMaximumPolicy      =  52,
   NSOpenGLPFAOffScreen          =  53,
   NSOpenGLPFAFullScreen         =  54,
   NSOpenGLPFASampleBuffers      =  55,
   NSOpenGLPFASamples            =  56,
   NSOpenGLPFAAuxDepthStencil    =  57,
   NSOpenGLPFAColorFloat         =  58,
   NSOpenGLPFAMultisample        =  59,
   NSOpenGLPFASupersample        =  60,
   NSOpenGLPFASampleAlpha        =  61,
   NSOpenGLPFARendererID         =  70,
   NSOpenGLPFASingleRenderer     =  71,
   NSOpenGLPFANoRecovery         =  72,
   NSOpenGLPFAAccelerated        =  73,
   NSOpenGLPFAClosestPolicy      =  74,
   NSOpenGLPFARobust             =  75,
   NSOpenGLPFABackingStore       =  76,
   NSOpenGLPFAMPSafe             =  78,
   NSOpenGLPFAWindow             =  80,
   NSOpenGLPFAMultiScreen        =  81,
   NSOpenGLPFACompliant          =  83,
   NSOpenGLPFAScreenMask         =  84,
   NSOpenGLPFAPixelBuffer        =  90,
   NSOpenGLPFARemotePixelBuffer  =  91,
   NSOpenGLPFAAllowOfflineRenderers = 96,
   NSOpenGLPFAAcceleratedCompute =  97,
   NSOpenGLPFAOpenGLProfile      =  99,
   NSOpenGLPFAVirtualScreenCount = 128
};

#define NS_ENUM_AVAILABLE_MAC(x)
/* NSOpenGLPFAOpenGLProfile values */
enum {
    NSOpenGLProfileVersionLegacy    NS_ENUM_AVAILABLE_MAC(10_7)     = 0x1000,   /* choose a
 Legacy/Pre-OpenGL 3.0 Implementation */
    NSOpenGLProfileVersion3_2Core   NS_ENUM_AVAILABLE_MAC(10_7)     = 0x3200,   /* choose an OpenGL 3.2 Core Implementation      */
    NSOpenGLProfileVersion4_1Core   NS_ENUM_AVAILABLE_MAC(10_10)    = 0x4100    /* choose an OpenGL 4.1 Core Implementation      */
};


enum {
    NSBorderlessWindowMask = 0,
    NSTitledWindowMask = 1 << 0,
    NSClosableWindowMask = 1 << 1,
    NSMiniaturizableWindowMask = 1 << 2,
    NSResizableWindowMask       = 1 << 3,
    
/* Specifies a window with textured background. Textured windows generally don't draw a top border line under the titlebar/toolbar. To get that line, use the NSUnifiedTitleAndToolbarWindowMask mask.
 */
    NSTexturedBackgroundWindowMask = 1 << 8,
    
/* Specifies a window whose titlebar and toolbar have a unified look - that is, a continuous background. Under the titlebar and toolbar a horizontal separator line will appear.
 */
    NSUnifiedTitleAndToolbarWindowMask = 1 << 12,
    
/* When set, the window will appear full screen. This mask is automatically toggled when toggleFullScreen: is called.
 */
    NSFullScreenWindowMask NS_ENUM_AVAILABLE_MAC(10_7) = 1 << 14,
    
/* If set, the contentView will consume the full size of the window; it can be combined with other window style masks, but is only respected for windows with a titlebar.
    Utilizing this mask opts-in to layer-backing. Utilize the contentLayoutRect or auto-layout contentLayoutGuide to layout views underneath the titlebar/toolbar area.
 */
    NSFullSizeContentViewWindowMask NS_ENUM_AVAILABLE_MAC(10_10) = 1 << 15
};

enum
{
    NSWindowCollectionBehaviorFullScreenPrimary NS_ENUM_AVAILABLE_MAC(10_7) = 1 << 7
};

#undef NS_ENUM_AVAILABLE_MAC

/* The following activation policies control whether and how an application may be activated.  They are determined by the Info.plist. */
enum /* typedef NS_ENUM(NSInteger, NSApplicationActivationPolicy) */ {
    /* The application is an ordinary app that appears in the Dock and may have a user interface.  This is the default for bundled apps, unless overridden in the Info.plist. */
    NSApplicationActivationPolicyRegular, 
    
    /* The application does not appear in the Dock and does not have a menu bar, but it may be activated programmatically or by clicking on one of its windows.  This corresponds to LSUIElement=1 in the Info.plist. */
    NSApplicationActivationPolicyAccessory,
    
    /* The application does not appear in the Dock and may not create windows or be activated.  This corresponds to LSBackgroundOnly=1 in the Info.plist.  This is also the default for unbundled executables that do not have Info.plists. */
    NSApplicationActivationPolicyProhibited
};

enum {
    NSApplicationDefined = 15
};

/* types of window backing store */
enum /* typedef NS_ENUM(NSUInteger, NSBackingStoreType) */ {
    NSBackingStoreRetained       = 0,
    NSBackingStoreNonretained    = 1,
    NSBackingStoreBuffered       = 2
};

id objc_allocInit(const char* classname)
{
    return objc_msgSend(objc_msgSend((id)objc_getClass(classname), sel_getUid("alloc")), sel_getUid("init"));
}

id make_nsstring(const char* text)
{
    return objc_msgSend((id)objc_getClass("NSString"), sel_getUid("stringWithUTF8String:"), text);
}

struct AppDel
{
    Class isa;
};

Class AppDelClass;

/*
static void post_empty_event(void)
{
    NSPoint point = {0, 0};
    id pool = objc_allocInit("NSAutoreleasePool");

    id event = objc_msgSend((id)objc_getClass("NSEvent"), sel_getUid("otherEventWithType:location:modifierFlags:timestamp:windowNumber:context:subtype:data1:data2:"),
        NSApplicationDefined,
        point,
        0,
        0,
        0,
        nil,
        0,
        0,
        0);
    objc_msgSend(NSApp, sel_getUid("postEvent:atStart:"), event, YES);
    objc_msgSend(pool, sel_getUid("drain"));
}

static BOOL AppDel_didFinishLaunching(struct AppDel *self, SEL _cmd, id notification)
{
    post_empty_event();
    objc_msgSend(NSApp, sel_getUid("stop:"), nil);
    return YES;
}
*/

static unsigned AppDel_shouldTerminate(struct AppDel *self, SEL _cmd, id notification)
{
    the_gl_window.should_close = true;
    return NO;
}

static void init_app_del()
{
    AppDelClass = objc_allocateClassPair(objc_getClass("NSObject"), "AppDelegate", 0);
/*    class_addMethod(AppDelClass, sel_getUid("applicationDidFinishLaunching:"), (IMP) AppDel_didFinishLaunching, "i@:@"); */
    class_addMethod(AppDelClass, sel_getUid("applicationShouldTerminate:"), (IMP) AppDel_shouldTerminate, "i@:@");
    objc_registerClassPair(AppDelClass);
}

struct WindowDel
{
    Class isa;

    rb_gl_window* window;
};

static Class WindowDelClass;

static BOOL WindowDel_windowShouldClose(struct WindowDel *self, SEL _cmd, id sender)
{
    self->window->should_close = true;
    return NO;    
}

static void WindowDel_setWindow(struct WindowDel *self, SEL _cmd, void *window)
{
    self->window = (rb_gl_window*) window;
}

static void WindowDel_windowDidResize(struct WindowDel *self, SEL _cmd, id notification)
{
    NSRect content_rect, fb_rect;

    objc_msgSend(self->window->gl_context, sel_getUid("update"));

    content_rect = ((NSRect(*)(id, SEL))objc_msgSend_stret)(self->window->view, sel_getUid("frame"));
    fb_rect = ((NSRect(*)(id, SEL, NSRect))objc_msgSend_stret)(self->window->view, sel_getUid("convertRectToBacking:"), content_rect);
    self->window->width = (int)fb_rect.size.width;
    self->window->height = (int)fb_rect.size.height;
}

static void WindowDel_windowDidMove(struct WindowDel *self, SEL _cmd, id notification)
{
    objc_msgSend(self->window->gl_context, sel_getUid("update"));
/*
    int x, y;
    _glfwPlatformGetWindowPos(window, &x, &y);
    _glfwInputWindowPos(window, x, y);
*/
}

static void init_window_del()
{
    WindowDelClass = objc_allocateClassPair(objc_getClass("NSObject"), "WindowDelegate", 0);

    class_addMethod(WindowDelClass, sel_getUid("windowShouldClose:"), (IMP)WindowDel_windowShouldClose, "c@:@");
    class_addMethod(WindowDelClass, sel_getUid("setWindow:"), (IMP)WindowDel_setWindow, "v@:^v");
    class_addMethod(WindowDelClass, sel_getUid("windowDidResize:"), (IMP)WindowDel_windowDidResize, "v@:@");
    class_addMethod(WindowDelClass, sel_getUid("windowDidMove:"), (IMP)WindowDel_windowDidMove, "v@:@");
    class_addIvar(WindowDelClass, "window", sizeof(rb_gl_window*), log2(sizeof(rb_gl_window*)), "^v");
    objc_registerClassPair(WindowDelClass);
}

static BOOL init_app_kit()
{
    id menubar;
    id app_menu_item;
    id app_menu;
    id window_menu_item;
    id window_menu;

    if (NSApp) return true;

    init_app_del();
    init_window_del();

    the_app_context.autoreleasePool = objc_allocInit("NSAutoreleasePool");

    objc_msgSend((id)objc_getClass("NSApplication"), sel_getUid("sharedApplication"));
    objc_msgSend(NSApp, sel_getUid("setActivationPolicy:"), NSApplicationActivationPolicyRegular);

    menubar = objc_allocInit("NSMenu");
    objc_msgSend(NSApp, sel_getUid("setMainMenu:"), menubar);
    app_menu_item = objc_allocInit("NSMenuItem");

    objc_msgSend(menubar, sel_getUid("addItem:"), app_menu_item);
    app_menu = objc_allocInit("NSMenu");

    objc_msgSend(app_menu_item, sel_getUid("setSubmenu:"), app_menu);
    objc_msgSend(app_menu, sel_getUid("addItemWithTitle:action:keyEquivalent:"),
        make_nsstring("Quit"),
        sel_getUid("terminate:"),
        make_nsstring("q"));

    window_menu_item = objc_msgSend(menubar, sel_getUid("addItemWithTitle:action:keyEquivalent:"), make_nsstring(""), NULL, make_nsstring(""));
    window_menu = objc_msgSend(objc_msgSend((id)objc_getClass("NSMenu"), sel_getUid("alloc")), sel_getUid("initWithTitle:"), make_nsstring("Window"));
    objc_msgSend(NSApp, sel_getUid("setWindowsMenu:"), window_menu);
    objc_msgSend(window_menu_item, sel_getUid("setSubmenu:"), window_menu);

    objc_msgSend(NSApp, sel_getUid("setAppleMenu:"), app_menu);

    the_app_context.delegate = objc_allocInit("AppDelegate");
    if (the_app_context.delegate == nil)
    {
        printf("Failed to create application delegate\n");
        return false;
    }

    objc_msgSend(NSApp, sel_getUid("setDelegate:"), the_app_context.delegate);
/*    objc_msgSend(NSApp, sel_getUid("run")); */
    objc_msgSend(NSApp, sel_getUid("finishLaunching"));

    return true;
}

static BOOL create_gl_context(rb_gl_window* window, enum create_window_flags flags)
{
    unsigned int attributeCount = 0;
    uint32_t attributes[40];

    attributes[attributeCount++] = NSOpenGLPFAAccelerated;
    attributes[attributeCount++] = NSOpenGLPFAClosestPolicy;
    if (flags & RBGL_CWF_CORE_32)
    {
        attributes[attributeCount++] = NSOpenGLPFAOpenGLProfile;
        attributes[attributeCount++] = NSOpenGLProfileVersion3_2Core;
    } else if (flags & RBGL_CWF_CORE_41)
    {
        attributes[attributeCount++] = NSOpenGLPFAOpenGLProfile;
        attributes[attributeCount++] = NSOpenGLProfileVersion4_1Core;
    } else
    {
        attributes[attributeCount++] = NSOpenGLPFAOpenGLProfile;
        attributes[attributeCount++] = NSOpenGLProfileVersionLegacy;        
    }
    attributes[attributeCount++] = NSOpenGLPFADoubleBuffer;
    attributes[attributeCount++] = NSOpenGLPFADepthSize;
    attributes[attributeCount++] = 16;
    attributes[attributeCount++] = 0;

    window->pixel_format = objc_msgSend(objc_msgSend((id)objc_getClass("NSOpenGLPixelFormat"), sel_getUid("alloc")), 
        sel_getUid("initWithAttributes:"), attributes);
    if (window->pixel_format == nil)
    {
        printf("Failed to find a suitable pixel format\n");
        return false;
    }
    window->gl_context = objc_msgSend(objc_msgSend((id)objc_getClass("NSOpenGLContext"), sel_getUid("alloc")),
        sel_getUid("initWithFormat:shareContext:"), window->pixel_format, nil);
    if (window->gl_context == nil)
    {
        printf("Failed to create OpenGL context\n");
        return false;
    }
/* Needed if using NSOpenGLView
    objc_msgSend(window->view, sel_getUid("setPixelFormat:"), window->pixel_format);
    objc_msgSend(window->view, sel_getUid("setOpenGLContext:"), window->gl_context);
*/

    objc_msgSend(window->gl_context, sel_registerName("setView:"), window->view);
    objc_msgSend(window->object, sel_registerName("setAcceptsMouseMovedEvents:"), YES);

    return true;
}

void rbgl_make_context_current()
{
    objc_msgSend(the_gl_window.gl_context, sel_getUid("makeCurrentContext"));
}

static void rbgl_clear_current_context()
{
    objc_msgSend((id)objc_getClass("NSOpenGLContext"), sel_getUid("clearCurrentContext"));
}

static void destroy_gl_context(rb_gl_window* window)
{
    rbgl_clear_current_context();
    objc_msgSend(window->pixel_format, sel_getUid("release"));
    window->pixel_format = nil;
    objc_msgSend(window->gl_context, sel_getUid("release"));
    window->gl_context = nil;
}

int rbgl_create_window(int width, int height, const char* title, enum create_window_flags flags)
{
    rb_gl_window* window = &the_gl_window;
    unsigned int styleMask;
    NSRect content_rect = {{0, 0}, {0, 0}};

    init_app_kit();

    window->width = width;
    window->height = height;
    window->delegate = objc_allocInit("WindowDelegate");
    if (window->delegate == nil)
    {
        printf("Failed to create window delegate\n");
        return false;
    }
    objc_msgSend(window->delegate, sel_getUid("setWindow:"), window);
    styleMask = NSTitledWindowMask | 
                NSClosableWindowMask |
                NSMiniaturizableWindowMask |
                NSResizableWindowMask;
    content_rect.size.width = window->width;
    content_rect.size.height = window->height;
    window->object = objc_msgSend((id)objc_getClass("NSWindow"), sel_getUid("alloc"));
    window->object = objc_msgSend(window->object, sel_getUid("initWithContentRect:styleMask:backing:defer:"),
        content_rect,
        styleMask,
        NSBackingStoreBuffered,
        NO);

    if (window->object == nil)
    {
        printf("Failed to create window\n");
        return false;
    }
    objc_msgSend(window->object, sel_getUid("setCollectionBehavior:"), NSWindowCollectionBehaviorFullScreenPrimary);
/*    
    window->view = objc_allocInit("NSOpenGLView"); 
    objc_msgSend(window->object, sel_getUid("setContentView:"), window->view);
*/
    window->view = objc_msgSend(window->object, sel_getUid("contentView"));
    objc_msgSend(window->object, sel_getUid("setTitle:"), make_nsstring(title));

    objc_msgSend(window->object, sel_getUid("setDelegate:"), window->delegate);
    objc_msgSend(window->object, sel_getUid("setRestorable:"), NO);
    window->should_close = false;
    if (!create_gl_context(window, flags)) return false;
    rbgl_make_context_current();

    rbgl_poll_events();
    objc_msgSend(window->object, sel_getUid("center"));
    objc_msgSend(NSApp, sel_getUid("activateIgnoringOtherApps:"), YES);
    objc_msgSend(window->object, sel_getUid("makeKeyAndOrderFront:"), window->object);

    return true;
}

void rbgl_destroy_window()
{
    rb_gl_window* window = &the_gl_window;
    if (!window->object) return;
    objc_msgSend(window->object, sel_getUid("orderOut:"), nil);

    destroy_gl_context(window);

    objc_msgSend(window->object, sel_getUid("setDelegate:"), nil);
    objc_msgSend(window->delegate, sel_getUid("release"));
    window->delegate = nil;
    objc_msgSend(window->object, sel_getUid("release"));
    window->object = nil;
}

void rbgl_get_window_size(int* width, int* height)
{
    *width = the_gl_window.width;
    *height = the_gl_window.height;
}

void rbgl_app_terminate()
{
    rbgl_destroy_window();
    if (the_app_context.delegate)
    {
        objc_msgSend(NSApp, sel_getUid("setDelegate:"), nil);
        objc_msgSend(the_app_context.delegate, sel_getUid("release"));
        the_app_context.delegate = nil;
    }

    objc_msgSend(the_app_context.autoreleasePool, sel_getUid("release"));
    the_app_context.autoreleasePool = nil;
}

void rbgl_poll_events()
{
    for (;;)
    {
        id event = objc_msgSend(NSApp, sel_getUid("nextEventMatchingMask:untilDate:inMode:dequeue:"), 
            ULONG_MAX /* NSAnyEventMask */ , 
            objc_msgSend((id)objc_getClass("NSDate"), sel_getUid("distantPast")),
            NSDefaultRunLoopMode,
            YES);

        if (event == nil)
            break;

        objc_msgSend(NSApp, sel_getUid("sendEvent:"), event);
    }
    objc_msgSend(NSApp, sel_getUid("updateWindows"));

/*
//    objc_msgSend(the_app_context.autoreleasePool, sel_registerName("drain"));
//    the_app_context.autoreleasePool = objc_msgSend(objc_msgSend((id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc")), sel_registerName("init"));
*/
}

int rbgl_window_should_close()
{
    return the_gl_window.should_close;
}

void rbgl_swap_buffers()
{
    objc_msgSend(the_gl_window.gl_context, sel_getUid("flushBuffer"));
}

#endif /* RB_GL_WINDOW_IMPLEMENTATION */

