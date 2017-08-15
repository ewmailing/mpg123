#if defined(__i686__) || defined(__i386__)
	// Can't get this to work. See CMakeLists.txt for notes.
//	#include "config_android_x86.h"
	#include "config_android_x86_noasm.h"
#elif defined(__arm__)
	#include "config_android_arm.h"
#elif defined(__aarch64__) 
	#include "config_android_arm64.h"
#elif defined(__amd64__) || defined(__x86_64__)
	#include "config_android_amd64.h"
#else
	#error "Need an Android arch"
#endif

