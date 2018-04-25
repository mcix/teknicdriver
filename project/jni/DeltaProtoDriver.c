#include <jni.h>
#include "DeltaProtoDriver.h"
//#include "nl_bytesoflife_DeltaProtoDriver.h"
#include "DeltaProtoDriverImpl.h"
 
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_nl_bytesoflife_DeltaProtoDriver_openPort (JNIEnv *env, jobject thisObj, jint portNum) {
	void * dpd= createDriver(portNum);
	return (jlong) dpd;
}

JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_home (JNIEnv *env, jobject thisObj, jlong object) {
	home((void*)object);
}

JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_closePort (JNIEnv *env, jobject thisObj, jlong object) {
	freeDriver((void*)object);
}

JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_moveTo (JNIEnv *env, jobject thisObj, jlong object, jint x, jint y, jint acc, jint vel, jint release) {
    moveTo((void*)object, x, y, acc, vel, release);
}

JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_moveToX (JNIEnv *env, jobject thisObj, jlong object, jint x, jint acc, jint vel, jint release) {
    moveToX((void*)object, x, acc, vel, release);
}

JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_moveToY (JNIEnv *env, jobject thisObj, jlong object, jint y, jint acc, jint vel, jint release) {
    moveToY((void*)object, y, acc, vel, release);
}

JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_release (JNIEnv *env, jobject thisObj, jlong object, jint rel) {
    release((void*)object, rel);
}

JNIEXPORT jdouble JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getPositionX(JNIEnv *env, jobject thisObj, jlong object) {
	return getPositionX( (void*)object );
}

JNIEXPORT jdouble JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getPositionY(JNIEnv *env, jobject thisObj, jlong object) {
	return getPositionY( (void*)object );
}

JNIEXPORT jint JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getMaxVel(JNIEnv *env, jobject thisObj, jlong object) {
	return VEL_LIM_RPM_MAX;
}

JNIEXPORT jint JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getMaxAcc(JNIEnv *env, jobject thisObj, jlong object) {
	return ACC_LIM_RPM_PER_SEC_MAX;
}

JNIEXPORT jint JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getMinVel(JNIEnv *env, jobject thisObj, jlong object) {
	return VEL_LIM_RPM_MIN;
}

JNIEXPORT jint JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getMinAcc(JNIEnv *env, jobject thisObj, jlong object) {
	return ACC_LIM_RPM_PER_SEC_MIN;
}

#ifdef __cplusplus
}
#endif