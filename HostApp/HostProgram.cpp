#include <stdio.h>
#include <windows.h>

HINSTANCE handle; // Handle to the DLL

// SOME TERMINOLOGY
// Host Applciation : this is the applcaition that wil load an use the plugins
// Plugin: This is a dynamically loaded DLL that the host application can use
// A symbol starting with T is a type

// These two structs much be declared in both teh Host applciation and any plugin

typedef struct
{
	int (*getVersion)();
	const char* (*getID)();
	const char* (*getDisplayName)();
	const char* (*getAuthor)();
	const char* (*getDescription)();
	int (*add)(int a, int b);
} TPluginInterface;


typedef struct {
	int (*getHostVersion)();
	int (*multPtr)(int a, int b);
	void (*printPtr) (int x);
} THostInterface;



typedef void* (*pluginInterfacePtr) ();
typedef void* (*hostInterfacePtr) (THostInterface*);
typedef void* (*makePluginTalkToHostPtr) ();

pluginInterfacePtr getPluginInterface;
hostInterfacePtr   setHostInterface;

makePluginTalkToHostPtr makePluginTalkToHost;

// These are the three methods that the host will export to a plugin

int getHostVersion () {
	return 4321;
}

int mult (int a, int b) {
	return a * b;
}


// We can't call printf inside the dll so when
// ever the dll needs to print something it calls this. 
void hostPrint (int x) {
	printf ("Host Print Function: %d\n", x);
}




void main () {

	// Two pointers to the host and plugin interfaces
	TPluginInterface* pluginInterface;
	THostInterface*   hostInterface;

	// Fill the interface method pointers with the actual methods
	hostInterface = (THostInterface*) malloc (sizeof (THostInterface));
	hostInterface->getHostVersion = getHostVersion;
	hostInterface->multPtr = mult;
	hostInterface->printPtr = hostPrint;

	printf ("Starting main program\n");
	// Load the DLL
	handle = LoadLibraryA ("myplugin.dll");

	// If loaded ok then retrieve the exported methods
	if (handle != NULL) {
		getPluginInterface = (pluginInterfacePtr)GetProcAddress (handle, "getPluginInterface");
		setHostInterface = (hostInterfacePtr) GetProcAddress (handle, "setHostInterface");

		makePluginTalkToHost = (makePluginTalkToHostPtr) GetProcAddress (handle, "makePluginTalkToHost");

		if (getPluginInterface != NULL) {
			// Get the plugin interface
			pluginInterface = (TPluginInterface*) getPluginInterface ();
			
			printf ("Call dll function: %d\n", pluginInterface->add (3, 4));
			printf ("Version = %d\n", pluginInterface->getVersion ());
			printf ("Author = %\s\n", pluginInterface->getAuthor ());
		} else
		   printf ("Unable to get plugin interface");

		// Next sent a pointer to the host interface to the plugin
		if (setHostInterface != NULL) {
			setHostInterface (hostInterface);
			// Call the dll test method that will test the call back to the host
			makePluginTalkToHost ();
		} else
			printf ("Unable to set host interface");

		FreeLibrary (handle);
	} else
		printf ("Error: Unable to open plugin dll\n");

	getchar ();
}