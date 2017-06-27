#include "cocoa_gl_init.h"
#include <limits.h>
#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CGBase.h>

#include <OpenGL/gl.h> // For testing

unsigned long long NSAnyEventMask = ULONG_MAX;
extern id NSApp;
extern id const NSDefaultRunLoopMode;

typedef struct _NSPoint {
    CGFloat x;
    CGFloat y;
} NSPoint;

typedef struct _NSSize {
    CGFloat width;              /* should never be negative */
    CGFloat height;             /* should never be negative */
} NSSize;

typedef struct _NSRect {
    NSPoint origin;
    NSSize size;
} NSRect;

NSRect __NSMakeRect(CGFloat x, CGFloat y, CGFloat w, CGFloat h)
{
    NSRect r;
    r.origin.x = x;
    r.origin.y = y;
    r.size.width = w;
    r.size.height = h;
    return r;
}

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
    NSWindowCollectionBehaviorFullScreenPrimary NS_ENUM_AVAILABLE_MAC(10_7) = 1 << 7, 
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
    NSApplicationDefined = 15,
};

/* types of window backing store */
enum /* typedef NS_ENUM(NSUInteger, NSBackingStoreType) */ {
    NSBackingStoreRetained       = 0,
    NSBackingStoreNonretained    = 1,
    NSBackingStoreBuffered       = 2
};

struct AppDel
{
    Class isa;
};

Class AppDelClass;

void _glfwPlatformPostEmptyEvent(void)
{
//    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    id pool = objc_msgSend(objc_msgSend((id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc")), sel_registerName("init"));
/*
    NSEvent* event = [NSEvent otherEventWithType:NSApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
                                           */
    id event = objc_msgSend((id)objc_getClass("NSEvent"), sel_getUid("otherEventWithType:location:modifierFlags:timestamp:windowNumber:context:subtype:data1:data2:"),
        NSApplicationDefined,
        (NSPoint){0, 0},
        0,
        0,
        0,
        nil,
        0,
        0,
        0);
//    [NSApp postEvent:event atStart:YES];
    objc_msgSend(NSApp, sel_getUid("postEvent:atStart:"), event, YES);
//    [pool drain];
    objc_msgSend(pool, sel_getUid("drain"));
}

BOOL AppDel_didFinishLaunching(struct AppDel *self, SEL _cmd, id notification)
{
    _glfwPlatformPostEmptyEvent();
//    [NSApp stop:nil];
    objc_msgSend(NSApp, sel_getUid("stop:"), nil);
    return YES;
}

void initAppDel()
{
    AppDelClass = objc_allocateClassPair(objc_getClass("NSObject"), "AppDelegate", 0);
    class_addMethod(AppDelClass, sel_getUid("applicationDidFinishLaunching:"), (IMP) AppDel_didFinishLaunching, "i@:@");
    objc_registerClassPair(AppDelClass);
}

struct WindowDel
{
    Class isa;

    _GLFWwindow* window;
};

Class WindowDelClass;

BOOL WindowDel_windowShouldClose(struct WindowDel *self, SEL _cmd, id sender)
{
    self->window->shouldClose = true;
    return NO;    
}

void WindowDel_setWindow(struct WindowDel *self, SEL _cmd, id window)
{
    self->window = (_GLFWwindow*) window;
}

void WindowDel_windowDidResize(struct WindowDel *self, SEL _cmd, id notification)
{
    objc_msgSend(self->window->contextObject, sel_getUid("update"));
/*
    const NSRect contentRect = [window->ns.view frame];
    const NSRect fbRect = [window->ns.view convertRectToBacking:contentRect];

    _glfwInputFramebufferSize(window, fbRect.size.width, fbRect.size.height);
    _glfwInputWindowSize(window, contentRect.size.width, contentRect.size.height);
*/
}

void WindowDel_windowDidMove(struct WindowDel *self, SEL _cmd, id notification)
{
    objc_msgSend(self->window->contextObject, sel_getUid("update"));
/*
    int x, y;
    _glfwPlatformGetWindowPos(window, &x, &y);
    _glfwInputWindowPos(window, x, y);
*/
}

void initWindowDel()
{
    WindowDelClass = objc_allocateClassPair(objc_getClass("NSObject"), "WindowDelegate", 0);
//    class_addMethod(AppDelClass, sel_getUid("applicationDidFinishLaunching:"), (IMP) AppDel_didFinishLaunching, "i@:@");
//    class_addMethod(WindowDelClass, sel_getUid("initWithGlfwWindow:", (IMP) WindowDel_initWithGlfwWindow, ""))

    class_addMethod(WindowDelClass, sel_getUid("windowShouldClose:"), (IMP)WindowDel_windowShouldClose, "c@:@");
    class_addMethod(WindowDelClass, sel_getUid("setWindow:"), (IMP)WindowDel_setWindow, "v@:@");
    class_addMethod(WindowDelClass, sel_getUid("windowDidResize:"), (IMP)WindowDel_windowDidResize, "v@:@");
    // Try to spec window as void *
    class_addIvar(WindowDelClass, "window", sizeof(_GLFWwindow*), log2(sizeof(_GLFWwindow*)), "^v");
    objc_registerClassPair(WindowDelClass);
}

//typedef uint32_t NSOpenGLPixelFormatAttribute;

bool initializeAppKit(_AppContext* context)
{
    if (NSApp) return true;

    initAppDel();
    initWindowDel();

//    context->autoreleasePool = [[NSAutoreleasePool alloc] init];
    context->autoreleasePool = objc_msgSend(objc_msgSend((id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc")), sel_registerName("init"));

//    [NSApplication sharedApplication];
    objc_msgSend((id)objc_getClass("NSApplication"), sel_getUid("sharedApplication"));
//    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    objc_msgSend(NSApp, sel_getUid("setActivationPolicy:"), NSApplicationActivationPolicyRegular);
//    [NSApp activateIgnoringOtherApps:YES];
    objc_msgSend(NSApp, sel_getUid("activateIgnoringOtherApps:"), YES);
//    context->delegate = [[GLFWApplicationDelegate alloc] init];
    context->delegate = objc_msgSend(objc_msgSend((id)objc_getClass("AppDelegate"), sel_getUid("alloc")), sel_getUid("init"));
    if (context->delegate == nil)
    {
        printf("Failed to create application delegate\n");
        return false;
    }
//    [NSApp setDelegate:context->delegate];
    objc_msgSend(NSApp, sel_getUid("setDelegate:"), context->delegate);
//    [NSApp run];
    objc_msgSend(NSApp, sel_getUid("run"));
    return true;
}

bool createGLContext(_GLFWwindow* window)
{
    unsigned int attributeCount = 0;
//    NSOpenGLPixelFormatAttribute attributes[40];
    uint32_t attributes[40];

    attributes[attributeCount++] = NSOpenGLPFAAccelerated;
    attributes[attributeCount++] = NSOpenGLPFAClosestPolicy;
    attributes[attributeCount++] = NSOpenGLPFAOpenGLProfile;
    attributes[attributeCount++] = NSOpenGLProfileVersion3_2Core;
    attributes[attributeCount++] = NSOpenGLPFADoubleBuffer;
    attributes[attributeCount++] = 0;

//    window->pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    window->pixelFormat = objc_msgSend(objc_msgSend((id)objc_getClass("NSOpenGLPixelFormat"), sel_getUid("alloc")), 
        sel_getUid("initWithAttributes:"), attributes);
    if (window->pixelFormat == nil)
    {
        printf("Failed to find a suitable pixel format\n");
        return false;
    }
//    window->contextObject = [[NSOpenGLContext alloc] initWithFormat:window->pixelFormat
//                                    shareContext:nil];
    window->contextObject = objc_msgSend(objc_msgSend((id)objc_getClass("NSOpenGLContext"), sel_getUid("alloc")),
        sel_getUid("initWithFormat:shareContext:"), window->pixelFormat, nil);
    if (window->contextObject == nil)
    {
        printf("Failed to create OpenGL context\n");
        return false;
    }
//    [window->contextObject setView:window->view];
//    [window->view setPixelFormat:window->pixelFormat];
    objc_msgSend(window->view, sel_getUid("setPixelFormat:"), window->pixelFormat);
//    [window->view setOpenGLContext:window->contextObject];
    objc_msgSend(window->view, sel_getUid("setOpenGLContext:"), window->contextObject);

    return true;
}

void destroyGLContext(_GLFWwindow* window)
{
//    [window->pixelFormat release];
    objc_msgSend(window->pixelFormat, sel_getUid("release"));
    window->pixelFormat = nil;

//    [window->contextObject release];
    objc_msgSend(window->contextObject, sel_getUid("release"));
    window->contextObject = nil;
}

bool createWindow(_GLFWwindow* window)
{
    window->delegate = objc_msgSend(objc_msgSend((id)objc_getClass("WindowDelegate"), sel_getUid("alloc")), sel_getUid("init"));
    if (window->delegate == nil)
    {
        printf("Failed to create window delegate\n");
        return false;
    }
    objc_msgSend(window->delegate, sel_getUid("setWindow:"), window);
    unsigned int styleMask = NSTitledWindowMask | 
                              NSClosableWindowMask | 
                              NSMiniaturizableWindowMask |
                              NSResizableWindowMask;
    NSRect contentRect = __NSMakeRect(0, 0, 640, 480);
/*
    window->object = [[NSWindow alloc]
        initWithContentRect:contentRect
        styleMask:styleMask
        backing:NSBackingStoreBuffered
        defer:NO];
*/
    window->object = objc_msgSend((id)objc_getClass("NSWindow"), sel_getUid("alloc"));
    window->object = objc_msgSend(window->object, sel_getUid("initWithContentRect:styleMask:backing:defer:"),
        contentRect,
        styleMask,
        NSBackingStoreBuffered,
        NO);

    if (window->object == nil)
    {
        printf("Failed to create window\n");
        return false;
    }
//    [window->object setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    objc_msgSend(window->object, sel_getUid("setCollectionBehavior:"), NSWindowCollectionBehaviorFullScreenPrimary);
//    [window->object center];
    objc_msgSend(window->object, sel_getUid("center"));
//    [window->object makeKeyAndOrderFront:nil];
    objc_msgSend(window->object, sel_getUid("makeKeyAndOrderFront:"), nil);
//    window->view = [[NSOpenGLView alloc] init];
    window->view = objc_msgSend(objc_msgSend((id)objc_getClass("NSOpenGLView"), sel_getUid("alloc")), sel_getUid("init"));
//    [window->object setTitle:[NSString stringWithUTF8String:"Hello"]];
    objc_msgSend(window->object, sel_getUid("setTitle:"), 
        objc_msgSend((id)objc_getClass("NSString"), sel_getUid("stringWithUTF8String:"), "Hello"));

//    [window->object setDelegate:window->delegate];
    objc_msgSend(window->object, sel_getUid("setDelegate:"), window->delegate);
//    [window->object setContentView:window->view];
    objc_msgSend(window->object, sel_getUid("setContentView:"), window->view);
//    [window->object setRestorable:NO];
    objc_msgSend(window->object, sel_getUid("setRestorable:"), NO);
//    [window->object orderFront:nil];
    objc_msgSend(window->object, sel_getUid("orderFront:"), nil);
    window->shouldClose = false;
    return true;
}

void destroyWindow(_GLFWwindow* window)
{
//    [window->object orderOut:nil];
    objc_msgSend(window->object, sel_getUid("orderOut:"), nil);

    destroyGLContext(window);

//    [window->object setDelegate:nil];
    objc_msgSend(window->object, sel_getUid("setDelegate:"), nil);
//    [window->delegate release];
    objc_msgSend(window->delegate, sel_getUid("release"));
    window->delegate = nil;

//    [window->view release];
    objc_msgSend(window->view, sel_getUid("release"));
    window->view = nil;

//    [window->object release];
    objc_msgSend(window->object, sel_getUid("release"));
    window->object = nil;
}

void appTerminate(_AppContext* context)
{
    if (context->delegate)
    {
//        [NSApp setDelegate:nil];
        objc_msgSend(NSApp, sel_getUid("setDelegate:"), nil);
//        [context->delegate release];
        objc_msgSend(context->delegate, sel_getUid("release"));
        context->delegate = nil;
    }

//    [context->autoreleasePool release];
    objc_msgSend(context->autoreleasePool, sel_getUid("release"));
    context->autoreleasePool = nil;
}

void pollEvents(_AppContext *context)
{
    for (;;)
    {
 /*       
        NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                            untilDate:[NSDate distantPast]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
*/
//        struct NSEvent* event = (struct NSEvent*)
        id event =
        objc_msgSend(NSApp, sel_getUid("nextEventMatchingMask:untilDate:inMode:dequeue:"), 
            NSAnyEventMask, 
            objc_msgSend((id)objc_getClass("NSDate"), sel_getUid("distantPast")),
            NSDefaultRunLoopMode,
            YES
            );

        if (event == nil)
            break;

//        [NSApp sendEvent:event];
        objc_msgSend(NSApp, sel_getUid("sendEvent:"), event);
    }

//    [context->autoreleasePool drain];
    objc_msgSend(context->autoreleasePool, sel_registerName("drain"));
//    context->autoreleasePool = [[NSAutoreleasePool alloc] init];
    context->autoreleasePool = objc_msgSend(objc_msgSend((id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc")), sel_registerName("init"));
}

void makeContextCurrent(_GLFWwindow* window)
{
    if (window)
    {
//        [window->contextObject makeCurrentContext];
        objc_msgSend(window->contextObject, sel_getUid("makeCurrentContext"));
    } else 
    {
//      [NSOpenGLContext clearCurrentContext];
        objc_msgSend((id)objc_getClass("NSOpenGLContext"), sel_getUid("clearCurrentContext"));
    }
}

void swapBuffers(_GLFWwindow* window)
{
//    [window->contextObject flushBuffer];
    objc_msgSend(window->contextObject, sel_getUid("flushBuffer"));
}

int main(int argc, char **argv)
{
    _AppContext app;
    _GLFWwindow window;

    if (!initializeAppKit(&app)) return -1;
    if (!createWindow(&window)) return -1;
    if (!createGLContext(&window)) return -1;
    makeContextCurrent(&window);

    float r = 1;
    while (!window.shouldClose)
    {
        pollEvents(&app);
        glClearColor(r, 0, 0, 0);
        r += 0.01;
        if (r > 1) r = 0;
        glClear(GL_COLOR_BUFFER_BIT);
        swapBuffers(&window);
    }
    destroyWindow(&window);
    appTerminate(&app);
    return 0;
}
