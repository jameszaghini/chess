#import "common.h"
#import <Cocoa/Cocoa.h>
#include <libkern/OSAtomic.h>

const Common_Button BTN_IDS[] = {BTN_ACTION1, BTN_ACTION2, BTN_ACTION3, BTN_ACTION4, BTN_LEFT, BTN_RIGHT, BTN_UP, BTN_DOWN, BTN_MORE};
const unsigned int BTN_COUNT = sizeof(BTN_IDS) / sizeof(BTN_IDS[0]);

NSMutableString *gOutputBuffer;
NSTextField *gOutputWindow;
uint32_t gButtons;
uint32_t gButtonsRead;

void Common_Init(void **extraDriverData)
{
    gOutputBuffer = [NSMutableString stringWithCapacity:(NUM_COLUMNS * NUM_ROWS)];
}

void Common_Close()
{

}

void Common_Update()
{
    [gOutputWindow setStringValue:gOutputBuffer];
    [gOutputBuffer setString:@""];

    do
    {
        gButtonsRead = gButtons;
    } while (!OSAtomicCompareAndSwap32(gButtonsRead, 0, (int32_t*)&gButtons));
}

void Common_Sleep(unsigned int ms)
{
    [NSThread sleepForTimeInterval:(ms / 1000.0f)];
}

void Common_Exit(int returnCode)
{
    exit(-1);
}

void Common_DrawText(const char *text)
{   
    [gOutputBuffer appendFormat:@"%s\n", text];
}

bool Common_BtnPress(Common_Button btn)
{
    return ((gButtonsRead & (1 << btn)) != 0);
}

bool Common_BtnDown(Common_Button btn)
{
    return Common_BtnPress(btn);
}

const char *Common_BtnStr(Common_Button btn)
{
    switch (btn)
    {
        case BTN_ACTION1: return "1";
        case BTN_ACTION2: return "2";
        case BTN_ACTION3: return "3";
        case BTN_ACTION4: return "4";
        case BTN_UP:      return "Up";
        case BTN_DOWN:    return "Down";
        case BTN_LEFT:    return "Left";
        case BTN_RIGHT:   return "Right";
        case BTN_MORE:    return "More";
        case BTN_QUIT:    return "X";
    }
}

const char *Common_MediaPath(const char *fileName)
{
    return [[NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], fileName] UTF8String];
}

void Common_LoadFileMemory(const char *name, void **buff, int *length)
{
    FILE *file = fopen(name, "rb");

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *mem = malloc(len);
    fread(mem, 1, len, file);

    fclose(file);

    *buff = mem;
    *length = len;
}

void Common_UnloadFileMemory(void *buff)
{
    free(buff);
}