// dllmain.cpp : Defines the entry point for the DLL application.
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

BOOL APIENTRY DllMain (HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// SOME TERMINOLOGY
// Host Applciation : this is the application that will load and use the plugins
// Plugin: This is a dynamically loaded DLL that the host application can use
// A symbol starting with T is a type


// This is the interface that the plugin will expose 
// to the host application. The interface is simply
// a struct of method pointers

// These two structs much be declared in both the Host application and any plugin

typedef struct
{
    int (*getVersion)();
    const char* (*getID)();
    const char* (*getDisplayName)();
    const char* (*getAuthor)();
    const char* (*getDescription)();
    int (*add)(int a, int b);
} TPluginInterface;


// This is the interface that the host application expses to the plugin
// The interface is simply a struct of method pointers
typedef struct {
    int (*getHostVersion)();
    int (*multPtr)(int a, int b);
    void (*printPtr) (int x);
} THostInterface;


// Pointers to the two interfaces
TPluginInterface* pluginInterface;
THostInterface* hostInterface;


// These are the two methods in the interface that the plugin will expose to the host application via TPluginInterface
int add (int a, int b) {
    return a + b;
}


int getVersion () {
    return 1234;
}

const char* getID() {
    return "id23";
}

const char* getDisplayName () {
    return "MakeCoffee";
}

const char* getAuthor () {
    return "John Smith";
}

const char* getDescription () {
    return "This plugin makes coffee";
}



// There are three exportable methods from the DLL plugin.
// 1. A method that the host applciation can call to get a pointer to the plugin interface
// 2. A method that passes a pointer to the host interface to the plugin
// 3. A test method that the host application calls which in turn calls a
//    method in the host. 

extern "C" {

    // Create the plugin interface and return a pointer to it 
    __declspec(dllexport) TPluginInterface* getPluginInterface () {
        pluginInterface = (TPluginInterface*) malloc (sizeof (TPluginInterface));
        // Initialize the method pointers with the actual methods. 
        pluginInterface->add = add;
        pluginInterface->getVersion = getVersion;
        pluginInterface->getAuthor = getAuthor;
        pluginInterface->getDescription = getDescription;
        pluginInterface->getDisplayName = getDisplayName;
        pluginInterface->getID = getID;
        return pluginInterface;
    }

    // The host application can call this to pass over to the DLL a 
    // pointer to the host interface
    __declspec(dllexport) void setHostInterface (THostInterface* _hostInterface) {
        hostInterface = _hostInterface;
    }


    // A test method the host application can call which in turn calls
    // two other host methods. A DLL can't use printf to print messages to the
    // console, so we expose a print method in the host that the DLL can use instead. 
    __declspec(dllexport) void makePluginTalkToHost () {
        int hostversion;

        hostversion = hostInterface->getHostVersion ();
        hostInterface->printPtr (hostversion);
    }



}






