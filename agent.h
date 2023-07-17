#include <jvmti.h>

#ifndef JNI_H
#define JNI_H
#include <jni.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef _STD_LIB_H
#define _STD_LIB_H
#include <stdlib.h>
#endif

#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
// #include "list.h"
#include "heap_data.h"

void heap_usage_of_function(JNIEnv *env, jobject function, jobject object);