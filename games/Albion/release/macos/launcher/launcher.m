/**
 *
 *  Copyright (C) 2026 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@property (strong) NSWindow *window;
@property (strong) NSTextField *gamePathTextField;
@end

@implementation AppDelegate

- (void) applicationDidFinishLaunching:(NSNotification *) notification {
    NSBundle *main = [NSBundle mainBundle];
    NSString *lastPath = [main pathForResource:@"gamepath" ofType:@"cfg"];

    NSError *error = nil;
    NSString *gamePath = [NSString stringWithContentsOfFile:lastPath encoding:NSUTF8StringEncoding error:&error];
    if (error) {
        gamePath = @"";
    }

    self.window = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 690, 75)
        styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable)
        backing:NSBackingStoreBuffered
        defer:YES
    ];

    [self.window center];
    [self.window setTitle:@"SR-Albion launcher"];
    [self.window setDelegate:self];

    NSView *contentView = [self.window contentView];
    contentView.wantsLayer = YES;

    NSTextField *gamePathLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(10, 40, 70, 20)];
    [gamePathLabel setStringValue:@"Game path:"];
    [gamePathLabel setBezeled:NO];
    [gamePathLabel setDrawsBackground:NO];
    [gamePathLabel setEditable:NO];
    [contentView addSubview:gamePathLabel];

    self.gamePathTextField = [[NSTextField alloc] initWithFrame:NSMakeRect(80, 40, 590, 24)];
    [self.gamePathTextField setStringValue:gamePath];
    [self.gamePathTextField setEditable:NO];
    [self.gamePathTextField setBezeled:YES];
    [contentView addSubview:self.gamePathTextField];

    NSButton *selectButton = [[NSButton alloc] initWithFrame:NSMakeRect(75, 5, 150, 32)];
    [selectButton setTitle:@"Select game path"];
    [selectButton setBezelStyle:NSBezelStyleRounded];
    [selectButton setTarget:self];
    [selectButton setAction:@selector(selectGamePath:)];
    [contentView addSubview:selectButton];

    NSButton *openButton = [[NSButton alloc] initWithFrame:NSMakeRect(225, 5, 150, 32)];
    [openButton setTitle:@"Open readme file"];
    [openButton setBezelStyle:NSBezelStyleRounded];
    [openButton setTarget:self];
    [openButton setAction:@selector(openReadmeFile:)];
    [contentView addSubview:openButton];

    NSButton *editButton = [[NSButton alloc] initWithFrame:NSMakeRect(375, 5, 150, 32)];
    [editButton setTitle:@"Edit configuration file"];
    [editButton setBezelStyle:NSBezelStyleRounded];
    [editButton setTarget:self];
    [editButton setAction:@selector(editConfigurationFile:)];
    [contentView addSubview:editButton];

    NSButton *launchButton = [[NSButton alloc] initWithFrame:NSMakeRect(525, 5, 150, 32)];
    [launchButton setTitle:@"Launch game"];
    [launchButton setBezelStyle:NSBezelStyleRounded];
    [launchButton setTarget:self];
    [launchButton setAction:@selector(launchGame:)];
    [contentView addSubview:launchButton];

    [self.window makeKeyAndOrderFront:nil];

    [NSApp activateIgnoringOtherApps:YES];
    [self.window makeMainWindow];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *) sender {
    return YES;
}

- (void) selectGamePath:(id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseDirectories:YES];
    [panel setCanChooseFiles:NO];
    [panel setAllowsMultipleSelection:NO];
    [panel setTreatsFilePackagesAsDirectories:YES];
    [panel setMessage:@"Select game path"];
    [panel setDirectoryURL:[NSURL fileURLWithPath:[self.gamePathTextField stringValue]]];

    if ([panel runModal] == NSModalResponseOK) {
        NSURL *url = [[panel URLs] objectAtIndex:0];
        [self.gamePathTextField setStringValue:[url path]];
    }
}

- (void) openReadmeFile:(id) sender {
    NSBundle *main = [NSBundle mainBundle];
    NSString *readmePath = [main pathForResource:@"readme-MacOS" ofType:@"txt"];

    NSDictionary *attributes = @{NSFilePosixPermissions: @0444};
    NSError *error = nil;
    NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager setAttributes:attributes ofItemAtPath:readmePath error:&error];

    [[NSWorkspace sharedWorkspace] openFile:readmePath withApplication:@"TextEdit"];
}

- (void) editConfigurationFile:(id) sender {
    NSBundle *main = [NSBundle mainBundle];
    NSString *configPath = [main pathForResource:@"Albion" ofType:@"cfg"];
    [[NSWorkspace sharedWorkspace] openFile:configPath withApplication:@"TextEdit"];
}

- (void) launchGame:(id) sender {
    NSString *gamePath = [self.gamePathTextField stringValue];
    if ([gamePath length] == 0) {
        [self showAlertWithTitle:@"Error" message:@"Game path is empty"];
        return;
    }

    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:gamePath]) {
        [self showAlertWithTitle:@"Error" message:@"Game path doesn't exist"];
        return;
    }

    BOOL foundAlbionExe = NO;
    BOOL foundSetupIni = NO;
    NSString *setupIniPath = nil;

    NSError *error = nil;
    NSArray *gameContents = [fileManager contentsOfDirectoryAtPath:gamePath error:&error];
    if (!error) {
        for (NSString *filename in gameContents) {
            if ([filename caseInsensitiveCompare:@"ALBION.EXE"] == NSOrderedSame) {
                BOOL isDirectory = YES;
                [fileManager fileExistsAtPath:[gamePath stringByAppendingPathComponent:filename] isDirectory:&isDirectory];
                if (!isDirectory) {
                    foundAlbionExe = YES;
                }
            } else if ([filename caseInsensitiveCompare:@"SETUP.INI"] == NSOrderedSame) {
                BOOL isDirectory = YES;
                [fileManager fileExistsAtPath:[gamePath stringByAppendingPathComponent:filename] isDirectory:&isDirectory];
                if (!isDirectory) {
                    foundSetupIni = YES;
                    setupIniPath = [gamePath stringByAppendingPathComponent:filename];
                }
            }
        }
    }

    if (!foundAlbionExe || !foundSetupIni) {
        [self showAlertWithTitle:@"Error" message:@"Game path doesn't contain file ALBION.EXE and/or file SETUP.INI"];
        return;
    }

    NSString *sourcePath = nil;
    error = nil;
    NSString *setupIniContents = [NSString stringWithContentsOfFile:setupIniPath encoding:NSUTF8StringEncoding error:&error];
    if (!error) {
        NSArray *lines = [setupIniContents componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"\r\n"]];

        for (NSString *line in lines) {
            if ([line hasPrefix:@"SOURCE_PATH="]) {
                sourcePath = [line substringFromIndex:12];
                break;
            }
        }
    }

    if (sourcePath) {
        sourcePath = [sourcePath stringByReplacingOccurrencesOfString:@"\\" withString:@"/"];
        while ([sourcePath hasSuffix:@"/"]) {
            sourcePath = [sourcePath substringToIndex:[sourcePath length] - 1];
        }

        BOOL isDirectory = NO;
        [fileManager fileExistsAtPath:[gamePath stringByAppendingPathComponent:sourcePath] isDirectory:&isDirectory];
        if (!isDirectory) {
            NSString *currentPath = gamePath;

            NSArray *sourceDirectories = [sourcePath componentsSeparatedByString:@"/"];
            for (NSString *sourceDirectory in sourceDirectories) {
                NSString *nextDirectory = nil;
                error = nil;
                NSArray *sourceFiles = [fileManager contentsOfDirectoryAtPath:currentPath error:&error];
                if (!error) {
                    for (NSString *sourceFile in sourceFiles) {
                        if ([sourceFile caseInsensitiveCompare:sourceDirectory] == NSOrderedSame) {
                            BOOL isDirectory = NO;
                            [fileManager fileExistsAtPath:[currentPath stringByAppendingPathComponent:sourceFile] isDirectory:&isDirectory];
                            if (isDirectory) {
                                nextDirectory = sourceFile;
                                break;
                            }
                        }
                    }
                }

                if (nextDirectory == nil) {
                    currentPath = nil;
                    break;
                }

                currentPath = [currentPath stringByAppendingPathComponent:nextDirectory];
            }

            if (currentPath != nil) {
                sourcePath = currentPath;
            } else {
                sourcePath = nil;
            }
        }
    }

    if (!sourcePath) {
        [self showAlertWithTitle:@"Error" message:@"Source path in file SETUP.INI doesn't exist"];
        return;
    }

    NSBundle *main = [NSBundle mainBundle];
    NSString *lastPath = [main pathForResource:@"gamepath" ofType:@"cfg"];

    error = nil;
    [gamePath writeToFile:lastPath atomically:YES encoding:NSUTF8StringEncoding error:&error];

    NSString *executablePath = [main pathForResource:@"SR-Main" ofType:nil];
    NSString *executableDirectory = [executablePath stringByDeletingLastPathComponent];

    NSTask *task = [[NSTask alloc] init];
    [task setExecutableURL:[NSURL fileURLWithPath:executablePath]];
    [task setCurrentDirectoryURL:[NSURL fileURLWithPath:executableDirectory]];
    [task setArguments:@[[NSString stringWithFormat:@"--game-dir=%@", gamePath]]];

    NSMutableDictionary *environment = [[NSProcessInfo processInfo].environment mutableCopy];
    [environment setObject:executableDirectory forKey:@"DYLD_LIBRARY_PATH"];
    [task setEnvironment:environment];

    [self.window orderOut:nil];

    error = nil;
    if (![task launchAndReturnError:&error]) {
        [self.window makeKeyAndOrderFront:nil];
        [self showAlertWithTitle:@"Error" message:@"Failed to run the game"];
    } else {
        [NSApp terminate:self];
    }
}

- (void) showAlertWithTitle:(NSString *) title message:(NSString *) message {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:title];
    [alert setInformativeText:message];
    [alert setAlertStyle:NSAlertStyleWarning];
    [alert addButtonWithTitle:@"OK"];
    [alert runModal];
}

@end

int main(int argc, const char *argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        [app run];
    }
    return 0;
}

