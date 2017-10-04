#include <jni.h>
#include "DeltaProtoDriver.h"
#include "DeltaProtoDriverImpl.h"
 
#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jlong JNICALL Java_DeltaProtoDriver_openPort (JNIEnv *env, jobject thisObj, jint portNum) {
	void * dpd= createDriver(portNum);
	return (jlong) dpd;
}

JNIEXPORT void JNICALL Java_DeltaProtoDriver_closePort (JNIEnv *env, jobject thisObj, jlong object) {
	freeDriver((void*)object);
}

JNIEXPORT void JNICALL Java_DeltaProtoDriver_moveTo (JNIEnv *env, jobject thisObj, jlong object, jint x, jint y) {
    moveTo((void*)object, x, y);
}

JNIEXPORT void JNICALL Java_DeltaProtoDriver_setVar(JNIEnv *env, jobject thisObj, jlong object, jint x) {
	setVar( (void*)object, x );
}

JNIEXPORT jint JNICALL Java_DeltaProtoDriver_getVar(JNIEnv *env, jobject thisObj, jlong object) {
	return getVar( (void*)object );
}

#ifdef __cplusplus
}
#endif