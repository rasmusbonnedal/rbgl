#import <Cocoa/Cocoa.h>
#include <stdio.h>
#include "cocoa_gl_init.h"
#include <objc/runtime.h>
#include <objc/message.h>

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
#if 0
bool createWindow(_GLFWwindow* window)
{
//    window->delegate = [[GLFWWindowDelegate alloc] initWithGlfwWindow:window];
    window->delegate = objc_msgSend(objc_msgSend(objc_getClass("WindowDelegate"), sel_getUid("alloc")), sel_getUid("init"));
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

#endif


