#import <Cocoa/Cocoa.h>
#include <OpenGL/gl.h>

#include <stdio.h>


void _glfwPlatformPostEmptyEvent(void)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    NSEvent* event = [NSEvent otherEventWithType:NSApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];
    [pool drain];
}

typedef struct
{
    id delegate;
    id object;
    id view;
    bool shouldClose;
    // GL Context
    id pixelFormat;
    id contextObject;
} _GLFWwindow;

typedef struct
{
    id delegate;
    id autoreleasePool; 
} _AppContext;

@interface GLFWApplicationDelegate : NSObject
@end

@implementation GLFWApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    printf("App should terminate\n");
    return NSTerminateCancel;
}

- (void)applicationDidChangeScreenParameters:(NSNotification *) notification
{
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    _glfwPlatformPostEmptyEvent();
    [NSApp stop:nil];
}

- (void)applicationDidHide:(NSNotification *)notification
{
}

@end

//------------------------------------------------------------------------
// Delegate for window related notifications
//------------------------------------------------------------------------

@interface GLFWWindowDelegate : NSObject
{
    _GLFWwindow* window;
}

- (id)initWithGlfwWindow:(_GLFWwindow *)initWindow;

@end

@implementation GLFWWindowDelegate

- (id)initWithGlfwWindow:(_GLFWwindow *)initWindow
{
    self = [super init];
    if (self != nil)
        window = initWindow;

    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    window->shouldClose = true;
    return NO;
}

- (void)windowDidResize:(NSNotification *)notification
{
    [window->contextObject update];
/*

    const NSRect contentRect = [window->ns.view frame];
    const NSRect fbRect = [window->ns.view convertRectToBacking:contentRect];

    _glfwInputFramebufferSize(window, fbRect.size.width, fbRect.size.height);
    _glfwInputWindowSize(window, contentRect.size.width, contentRect.size.height);
*/
}

- (void)windowDidMove:(NSNotification *)notification
{
    [window->contextObject update];
/*
    int x, y;
    _glfwPlatformGetWindowPos(window, &x, &y);
    _glfwInputWindowPos(window, x, y);
*/
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
}

@end

#if 0

//------------------------------------------------------------------------
// Content view class for the GLFW window
//------------------------------------------------------------------------

@interface GLFWContentView : NSView
{
    _GLFWwindow* window;
}

- (id)initWithGlfwWindow:(_GLFWwindow *)initWindow;

@end

@implementation GLFWContentView

+ (void)initialize
{
}

- (id)initWithGlfwWindow:(_GLFWwindow *)initWindow
{
    self = [super init];
    if (self != nil)
    {
        window = initWindow;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)viewDidChangeBackingProperties
{
    const NSRect contentRect = [window->view frame];
    const NSRect fbRect = [window->view convertRectToBacking:contentRect];

//    _glfwInputFramebufferSize(window, fbRect.size.width, fbRect.size.height);
}

- (void)drawRect:(NSRect)rect
{
//    _glfwInputWindowDamage(window);
}

@end

#endif

bool initializeAppKit(_AppContext* context)
{
    if (NSApp) return true;

    context->autoreleasePool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
    context->delegate = [[GLFWApplicationDelegate alloc] init];
    if (context->delegate == nil)
    {
        printf("Failed to create application delegate\n");
        return false;
    }
    [NSApp setDelegate:context->delegate];
    [NSApp run];
    return true;
}

bool createGLContext(_GLFWwindow* window)
{
    unsigned int attributeCount = 0;
    NSOpenGLPixelFormatAttribute attributes[40];

    attributes[attributeCount++] = NSOpenGLPFAAccelerated;
    attributes[attributeCount++] = NSOpenGLPFAClosestPolicy;
    attributes[attributeCount++] = NSOpenGLPFAOpenGLProfile;
    attributes[attributeCount++] = NSOpenGLProfileVersion3_2Core;
    attributes[attributeCount++] = NSOpenGLPFADoubleBuffer;
    attributes[attributeCount++] = 0;

    window->pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if (window->pixelFormat == nil)
    {
        printf("Failed to find a suitable pixel format\n");
        return false;
    }
    window->contextObject = [[NSOpenGLContext alloc] initWithFormat:window->pixelFormat
                                    shareContext:nil];
    if (window->contextObject == nil)
    {
        printf("Failed to create OpenGL context\n");
        return false;
    }
//    [window->contextObject setView:window->view];
    [window->view setPixelFormat:window->pixelFormat];
    [window->view setOpenGLContext:window->contextObject];

    return true;
}

void destroyGLContext(_GLFWwindow* window)
{
    [window->pixelFormat release];
    window->pixelFormat = nil;

    [window->contextObject release];
    window->contextObject = nil;
}

bool createWindow(_GLFWwindow* window)
{
    window->delegate = [[GLFWWindowDelegate alloc] initWithGlfwWindow:window];
    if (window->delegate == nil)
    {
        printf("Failed to create window delegate\n");
        return false;
    }
    unsigned int styleMask = NSTitledWindowMask | 
                              NSClosableWindowMask | 
                              NSMiniaturizableWindowMask |
                              NSResizableWindowMask;
    NSRect contentRect = NSMakeRect(0, 0, 640, 480);
    window->object = [[NSWindow alloc]
        initWithContentRect:contentRect
        styleMask:styleMask
        backing:NSBackingStoreBuffered
        defer:NO];

    if (window->object == nil)
    {
        printf("Failed to create window\n");
        return false;
    }

    [window->object setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    [window->object center];
    [window->object makeKeyAndOrderFront:nil];
//    window->view = [[GLFWContentView alloc] initWithGlfwWindow:window];
    window->view = [[NSOpenGLView alloc] init];
//    window->view = [[NSButton alloc] init];
    [window->object setTitle:[NSString stringWithUTF8String:"Hello"]];
    [window->object setDelegate:window->delegate];
    [window->object setContentView:window->view];
    [window->object setRestorable:NO];
    window->shouldClose = false;
    [window->object orderFront:nil];

    return true;
}

void destroyWindow(_GLFWwindow* window)
{
    [window->object orderOut:nil];

    destroyGLContext(window);

    [window->object setDelegate:nil];
    [window->delegate release];
    window->delegate = nil;

    [window->view release];
    window->view = nil;

    [window->object release];
    window->object = nil;
}

void appTerminate(_AppContext* context)
{
    if (context->delegate)
    {
        [NSApp setDelegate:nil];
        [context->delegate release];
        context->delegate = nil;
    }

    [context->autoreleasePool release];
    context->autoreleasePool = nil;
}

void pollEvents(_AppContext *context)
{
    for (;;)
    {
        NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                            untilDate:[NSDate distantPast]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event == nil)
            break;

        [NSApp sendEvent:event];
    }

    [context->autoreleasePool drain];
    context->autoreleasePool = [[NSAutoreleasePool alloc] init];
}

void makeContextCurrent(_GLFWwindow* window)
{
    if (window)
    {
        [window->contextObject makeCurrentContext];
    }
    else [NSOpenGLContext clearCurrentContext];
}

void swapBuffers(_GLFWwindow* window)
{
    [window->contextObject flushBuffer];
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
