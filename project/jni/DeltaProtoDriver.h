/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class nl_bytesoflife_DeltaProtoDriver */

#ifndef _Included_nl_bytesoflife_DeltaProtoDriver
#define _Included_nl_bytesoflife_DeltaProtoDriver
#ifdef __cplusplus
extern "C" {
#endif
#undef nl_bytesoflife_DeltaProtoDriver_TRUE
#define nl_bytesoflife_DeltaProtoDriver_TRUE 1L
#undef nl_bytesoflife_DeltaProtoDriver_FALSE
#define nl_bytesoflife_DeltaProtoDriver_FALSE 0L
/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    openPort
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_nl_bytesoflife_DeltaProtoDriver_openPort
  (JNIEnv *, jobject, jint);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    home
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_home
  (JNIEnv *, jobject, jlong);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    closePort
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_closePort
  (JNIEnv *, jobject, jlong);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    moveTo
 * Signature: (JIIIII)V
 */
JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_moveTo__JIIIII
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint, jint);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    moveTo
 * Signature: (JIIIIIII)V
 */
JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_moveTo__JIIIIIII
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint, jint, jint, jint);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    moveToX
 * Signature: (JIIII)V
 */
JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_moveToX
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    moveToY
 * Signature: (JIIII)V
 */
JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_moveToY
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    release
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_nl_bytesoflife_DeltaProtoDriver_release
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    getPositionX
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getPositionX
  (JNIEnv *, jobject, jlong);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    getPositionY
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getPositionY
  (JNIEnv *, jobject, jlong);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    getMaxAcc
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getMaxAcc
  (JNIEnv *, jobject, jlong);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    getMaxVel
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getMaxVel
  (JNIEnv *, jobject, jlong);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    getMinAcc
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getMinAcc
  (JNIEnv *, jobject, jlong);

/*
 * Class:     nl_bytesoflife_DeltaProtoDriver
 * Method:    getMinVel
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_nl_bytesoflife_DeltaProtoDriver_getMinVel
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
