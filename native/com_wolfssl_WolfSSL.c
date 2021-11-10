/* com_wolfssl_WolfSSL.c
 *
 * Copyright (C) 2006-2021 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdio.h>
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/random.h>
#ifdef HAVE_FIPS
    #include <wolfssl/wolfcrypt/fips_test.h>
#endif

#include "com_wolfssl_globals.h"
#include "com_wolfssl_WolfSSL.h"

/* global JavaVM reference for JNIEnv lookup */
JavaVM*  g_vm;

/* global object refs for logging callbacks */
static jobject g_loggingCbIfaceObj;

#ifdef HAVE_FIPS
/* global object ref for FIPS error callback */
static jobject g_fipsCbIfaceObj;
#endif

/* custom native fn prototypes */
void NativeLoggingCallback(const int logLevel, const char *const logMessage);

/* called when native library is loaded */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    /* store JavaVM */
    g_vm = vm;
    return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_init
  (JNIEnv* jenv, jobject jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef WC_RNG_SEED_CB
    wc_SetSeed_Cb(wc_GenerateSeed);
#endif
    return (jint)wolfSSL_Init();
}

/* used in unit tests */
JNIEXPORT void JNICALL Java_com_wolfssl_WolfSSL_nativeFree
  (JNIEnv* jenv, jobject jcl, jlong ptr)
{
    (void)jenv;
    (void)jcl;

    if((void*)(intptr_t)ptr)
        XFREE((void*)(intptr_t)ptr, NULL, DYNAMIC_TYPE_TMP_BUFFER);
}

/* functions to return BulkCipherAlgorithm enum values from ./wolfssl/ssl.h  */
JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumNULL
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_cipher_null;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumRC4
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_rc4;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumRC2
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_rc2;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumDES
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_des;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnum3DES
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_triple_des;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumDES40
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_des40;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumIDEA
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef HAVE_IDEA
    return wolfssl_idea;
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumAES
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_aes;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumAESGCM
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_aes_gcm;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumAESCCM
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_aes_ccm;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumCHACHA
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_chacha;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumCAMELLIA
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_camellia;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumHC128
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_hc128;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getBulkCipherAlgorithmEnumRABBIT
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfssl_rabbit;
}

JNIEXPORT jboolean JNICALL Java_com_wolfssl_WolfSSL_TLSv1Enabled
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if !defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_TLSV10)
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}

JNIEXPORT jboolean JNICALL Java_com_wolfssl_WolfSSL_TLSv11Enabled
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if !defined(NO_OLD_TLS)
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}

JNIEXPORT jboolean JNICALL Java_com_wolfssl_WolfSSL_TLSv12Enabled
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if !defined(WOLFSSL_NO_TLS12)
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}

JNIEXPORT jboolean JNICALL Java_com_wolfssl_WolfSSL_TLSv13Enabled
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef WOLFSSL_TLS13
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}

JNIEXPORT jboolean JNICALL Java_com_wolfssl_WolfSSL_EccEnabled
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef HAVE_ECC
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}

JNIEXPORT jboolean JNICALL Java_com_wolfssl_WolfSSL_RsaEnabled
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifndef NO_RSA
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_SSLv3_1ServerMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
    return (jlong)(intptr_t)wolfSSLv3_server_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_SSLv3_1ClientMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
    return (jlong)(intptr_t)wolfSSLv3_client_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_1Method
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if !defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_TLSV10)
    return (jlong)(intptr_t)wolfTLSv1_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_1ServerMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if !defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_TLSV10)
    return (jlong)(intptr_t)wolfTLSv1_server_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_1ClientMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if !defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_TLSV10)
    return (jlong)(intptr_t)wolfTLSv1_client_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_11_1Method
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifndef NO_OLD_TLS
    return (jlong)(intptr_t)wolfTLSv1_1_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_11_1ServerMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifndef NO_OLD_TLS
    return (jlong)(intptr_t)wolfTLSv1_1_server_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_11_1ClientMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifndef NO_OLD_TLS
    return (jlong)(intptr_t)wolfTLSv1_1_client_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_12_1Method
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return (jlong)(intptr_t)wolfTLSv1_2_method();
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_12_1ServerMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return (jlong)(intptr_t)wolfTLSv1_2_server_method();
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_12_1ClientMethod(
    JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return (jlong)(intptr_t)wolfTLSv1_2_client_method();
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_TLSv1_13_1Method
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef WOLFSSL_TLS13
    return (jlong)(intptr_t)wolfTLSv1_3_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_DTLSv1_1Method
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if defined(WOLFSSL_DTLS) && !defined(NO_OLD_TLS)
    return (jlong)(intptr_t)wolfDTLSv1_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_DTLSv1_1ClientMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if defined(WOLFSSL_DTLS) && !defined(NO_OLD_TLS)
    return (jlong)(intptr_t)wolfDTLSv1_client_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_DTLSv1_1ServerMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if defined(WOLFSSL_DTLS) && !defined(NO_OLD_TLS)
    return (jlong)(intptr_t)wolfDTLSv1_server_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_DTLSv1_12_1Method
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if defined(WOLFSSL_DTLS) && !defined(WOLFSSL_NO_TLS12)
    return (jlong)(intptr_t)wolfDTLSv1_2_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_DTLSv1_12_1ClientMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if defined(WOLFSSL_DTLS) && !defined(WOLFSSL_NO_TLS12)
    return (jlong)(intptr_t)wolfDTLSv1_2_client_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_DTLSv1_12_1ServerMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#if defined(WOLFSSL_DTLS) && !defined(WOLFSSL_NO_TLS12)
    return (jlong)(intptr_t)wolfDTLSv1_2_server_method();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_SSLv23_1Method
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return (jlong)(intptr_t)wolfSSLv23_method();
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_SSLv23_1ServerMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return (jlong)(intptr_t)wolfSSLv23_server_method();
}

JNIEXPORT jlong JNICALL Java_com_wolfssl_WolfSSL_SSLv23_1ClientMethod
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return (jlong)(intptr_t)wolfSSLv23_client_method();
}

JNIEXPORT jstring JNICALL Java_com_wolfssl_WolfSSL_getErrorString
  (JNIEnv* jenv, jclass jcl, jlong errNumber)
{
    char buffer[WOLFSSL_MAX_ERROR_SZ];
    jstring retString;

    (void)jcl;

    wolfSSL_ERR_error_string(errNumber, buffer);
    retString = (*jenv)->NewStringUTF(jenv, buffer);

    return retString;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_cleanup
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    /* release global logging callback object if registered */
    if (g_loggingCbIfaceObj != NULL) {
        (*jenv)->DeleteGlobalRef(jenv, g_loggingCbIfaceObj);
        g_loggingCbIfaceObj = NULL;
    }

#ifdef HAVE_FIPS
    /* release existing FIPS callback object if set */
    if (g_fipsCbIfaceObj != NULL) {
        (*jenv)->DeleteGlobalRef(jenv, g_fipsCbIfaceObj);
        g_fipsCbIfaceObj = NULL;
    }
#endif

    return wolfSSL_Cleanup();
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_debuggingON
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfSSL_Debugging_ON();
}

JNIEXPORT void JNICALL Java_com_wolfssl_WolfSSL_debuggingOFF
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return wolfSSL_Debugging_OFF();
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_setLoggingCb
  (JNIEnv* jenv, jclass jcl, jobject callback)
{
    int ret = 0;

    (void)jcl;

    if (jenv == NULL) {
        return BAD_FUNC_ARG;
    }

    /* release existing logging callback object if registered */
    if (g_loggingCbIfaceObj != NULL) {
        (*jenv)->DeleteGlobalRef(jenv, g_loggingCbIfaceObj);
        g_loggingCbIfaceObj = NULL;
    }

    if (callback != NULL) {
        /* store Java logging callback Interface object */
        g_loggingCbIfaceObj = (*jenv)->NewGlobalRef(jenv, callback);
        if (g_loggingCbIfaceObj == NULL) {
            printf("error storing global logging callback interface\n");
            return SSL_FAILURE;
        }

        ret = wolfSSL_SetLoggingCb(NativeLoggingCallback);
    }

    return ret;
}

void NativeLoggingCallback(const int logLevel, const char *const logMessage)
{
    JNIEnv*   jenv;
    jint      vmret  = 0;
    jclass    excClass;
    jmethodID logMethod;
    jobjectRefType refcheck;

    /* get JNIEnv from JavaVM */
    vmret = (int)((*g_vm)->GetEnv(g_vm, (void**) &jenv, JNI_VERSION_1_6));
    if (vmret == JNI_EDETACHED) {
#ifdef __ANDROID__
        vmret = (*g_vm)->AttachCurrentThread(g_vm, &jenv, NULL);
#else
        vmret = (*g_vm)->AttachCurrentThread(g_vm, (void**) &jenv, NULL);
#endif
        if (vmret) {
            printf("Failed to attach JNIEnv to thread\n");
        }
    } else if (vmret != JNI_OK) {
        printf("Unable to get JNIEnv from JavaVM\n");
    }

    /* find exception class */
    excClass = (*jenv)->FindClass(jenv, "java/lang/Exception");
    if ((*jenv)->ExceptionOccurred(jenv)) {
        (*jenv)->ExceptionDescribe(jenv);
        (*jenv)->ExceptionClear(jenv);
        return;
    }

    /* check if our stored object reference is valid */
    refcheck = (*jenv)->GetObjectRefType(jenv, g_loggingCbIfaceObj);
    if (refcheck == 2) {

        /* lookup WolfSSLLoggingCallback class from global object ref */
        jclass logClass = (*jenv)->GetObjectClass(jenv, g_loggingCbIfaceObj);
        if (!logClass) {
            if ((*jenv)->ExceptionOccurred(jenv)) {
                (*jenv)->ExceptionDescribe(jenv);
                (*jenv)->ExceptionClear(jenv);
            }

            (*jenv)->ThrowNew(jenv, excClass,
                "Can't get native WolfSSLLoggingCallback class reference");
            return;
        }

        logMethod = (*jenv)->GetMethodID(jenv, logClass,
                                            "loggingCallback",
                                            "(ILjava/lang/String;)V");
        if (logMethod == 0) {
            if ((*jenv)->ExceptionOccurred(jenv)) {
                (*jenv)->ExceptionDescribe(jenv);
                (*jenv)->ExceptionClear(jenv);
            }
            (*jenv)->ThrowNew(jenv, excClass,
                "Error getting loggingCallback method from JNI");
            return;
        }

        /* create jstring from char* */
        jstring logMsg = (*jenv)->NewStringUTF(jenv, logMessage);

        (*jenv)->CallVoidMethod(jenv, g_loggingCbIfaceObj, logMethod,
                logLevel, logMsg);

        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);

            (*jenv)->ThrowNew(jenv, excClass,
                    "Error calling logging callback from JNI");
            return;
        }

    } else {
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
        }

        (*jenv)->ThrowNew(jenv, excClass,
                "Object reference invalid in NativeLoggingCallback");
    }
}

void NativeFIPSErrorCallback(const int ok, const int err,
                             const char* const hash)
{
#ifdef HAVE_FIPS
    JNIEnv*   jenv;
    jint      vmret  = 0;
    jclass    excClass;
    jmethodID errorMethod;
    jobjectRefType refcheck;

    /* get JNIEnv from JavaVM */
    vmret = (int)((*g_vm)->GetEnv(g_vm, (void**) &jenv, JNI_VERSION_1_6));
    if (vmret == JNI_EDETACHED) {
#ifdef __ANDROID__
        vmret = (*g_vm)->AttachCurrentThread(g_vm, &jenv, NULL);
#else
        vmret = (*g_vm)->AttachCurrentThread(g_vm, (void**) &jenv, NULL);
#endif
        if (vmret) {
            printf("Failed to attach JNIEnv to thread\n");
        }
    } else if (vmret != JNI_OK) {
        printf("Unable to get JNIEnv from JavaVM\n");
    }

    /* find exception class */
    excClass = (*jenv)->FindClass(jenv, "java/lang/Exception");
    if ((*jenv)->ExceptionOccurred(jenv)) {
        (*jenv)->ExceptionDescribe(jenv);
        (*jenv)->ExceptionClear(jenv);
        return;
    }

    /* check if our stored object reference is valid */
    refcheck = (*jenv)->GetObjectRefType(jenv, g_fipsCbIfaceObj);
    if (refcheck == JNIGlobalRefType) {

        /* lookup WolfSSLLoggingCallback class from global object ref */
        jclass fipsCbClass = (*jenv)->GetObjectClass(jenv, g_fipsCbIfaceObj);
        if (!fipsCbClass) {
            if ((*jenv)->ExceptionOccurred(jenv)) {
                (*jenv)->ExceptionDescribe(jenv);
                (*jenv)->ExceptionClear(jenv);
            }

            (*jenv)->ThrowNew(jenv, excClass,
                "Can't get native WolfSSLFIPSErrorCallback class reference");
            return;
        }

        errorMethod = (*jenv)->GetMethodID(jenv, fipsCbClass,
                                            "errorCallback",
                                            "(IILjava/lang/String;)V");
        if (errorMethod == 0) {
            if ((*jenv)->ExceptionOccurred(jenv)) {
                (*jenv)->ExceptionDescribe(jenv);
                (*jenv)->ExceptionClear(jenv);
            }
            (*jenv)->ThrowNew(jenv, excClass,
                "Error getting errorCallback method from JNI");
            return;
        }

        /* create jstring from char* */
        jstring hashString = (*jenv)->NewStringUTF(jenv, hash);

        (*jenv)->CallVoidMethod(jenv, g_fipsCbIfaceObj, errorMethod,
                ok, err, hashString);

        /* release local reference to jstring, since returning to native */
        (*jenv)->DeleteLocalRef(jenv, hashString);

        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);

            (*jenv)->ThrowNew(jenv, excClass,
                    "Error calling FIPS error callback from JNI");
            return;
        }

    } else {
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
        }

        (*jenv)->ThrowNew(jenv, excClass,
                "Object reference invalid in NativeFIPSErrorCallback");
    }
#else
    (void)ok;
    (void)err;
    (void)hash;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_setFIPSCb
  (JNIEnv* jenv, jclass jcl, jobject callback)
{
    int ret = NOT_COMPILED_IN;
    (void)jcl;

#ifdef HAVE_FIPS
    if (jenv == NULL) {
        return BAD_FUNC_ARG;
    }

    /* release existing FIPS callback object if set */
    if (g_fipsCbIfaceObj != NULL) {
        (*jenv)->DeleteGlobalRef(jenv, g_fipsCbIfaceObj);
        g_fipsCbIfaceObj = NULL;
    }

    if (callback != NULL) {
        /* store Java FIPS callback Interface object */
        g_fipsCbIfaceObj = (*jenv)->NewGlobalRef(jenv, callback);
        if (g_fipsCbIfaceObj == NULL) {
            printf("error storing global wolfCrypt FIPS callback interface\n");
            return SSL_FAILURE;
        }

        /* register NativeFIPSErrorCallback, wraps Java callback */
        ret = wolfCrypt_SetCb_fips(NativeFIPSErrorCallback);
        if (ret == 0) {
            ret = SSL_SUCCESS;
        }
    }
#else
    (void)jenv;
    (void)callback;
    printf("Unable to set FIPS callback without wolfCrypt FIPS code\n");
#endif

    return ret;
}

JNIEXPORT jstring JNICALL Java_com_wolfssl_WolfSSL_getWolfCryptFIPSCoreHash
  (JNIEnv* jenv, jclass jcl)
{
#ifdef HAVE_FIPS
    return (*jenv)->NewStringUTF(jenv, wolfCrypt_GetCoreHash_fips());
#else
    (void)jenv;
    (void)jcl;
    return NULL;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_memsaveSessionCache
  (JNIEnv* jenv, jclass jcl, jbyteArray mem, jint sz)
{
#ifdef PERSIST_SESSION_CACHE
    int ret;
    int cacheSz;
    char memBuf[sz];

    (void)jcl;

    if (!jenv || !mem || (sz <= 0))
        return BAD_FUNC_ARG;

    ret = wolfSSL_memsave_session_cache(memBuf, sz);

    /* how much data do we need to write? */
    cacheSz = wolfSSL_get_session_cache_memsize();

    /* set jbyteArray for return */
    if (cacheSz >= 0) {
        (*jenv)->SetByteArrayRegion(jenv, mem, 0, cacheSz, (jbyte*)memBuf);
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
            return SSL_FAILURE;
        }
    }

    return ret;
#else
    (void)jenv;
    (void)jcl;
    (void)mem;
    (void)sz;
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_memrestoreSessionCache
  (JNIEnv* jenv, jclass jcl, jbyteArray mem, jint sz)
{
#ifdef PERSIST_SESSION_CACHE
    int ret;
    char memBuf[sz];

    (void)jcl;

    if (!jenv || !mem || (sz <= 0))
        return BAD_FUNC_ARG;

    (*jenv)->GetByteArrayRegion(jenv, mem, 0, sz, (jbyte*)memBuf);
    if ((*jenv)->ExceptionOccurred(jenv)) {
        (*jenv)->ExceptionDescribe(jenv);
        (*jenv)->ExceptionClear(jenv);
        return SSL_FAILURE;
    }

    ret = wolfSSL_memrestore_session_cache(memBuf, sz);
    return ret;
#else
    (void)jenv;
    (void)jcl;
    (void)mem;
    (void)sz;
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getSessionCacheMemsize
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;
#ifdef PERSIST_SESSION_CACHE
    return wolfSSL_get_session_cache_memsize();
#else
    return NOT_COMPILED_IN;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getPkcs8TraditionalOffset
  (JNIEnv* jenv, jclass jcl, jbyteArray in, jlong idx, jlong sz)
{
    int ret;
    word32 inOutIdx;
    unsigned char inBuf[sz];

    (void)jcl;

    if (!jenv || !in || (sz <= 0))
        return BAD_FUNC_ARG;

    (*jenv)->GetByteArrayRegion(jenv, in, 0, sz, (jbyte*)inBuf);
    if ((*jenv)->ExceptionOccurred(jenv)) {
        (*jenv)->ExceptionDescribe(jenv);
        (*jenv)->ExceptionClear(jenv);
        return SSL_FAILURE;
    }

    inOutIdx = (word32)idx;
    ret = wc_GetPkcs8TraditionalOffset(inBuf, &inOutIdx, (word32)sz);

    if (ret < 0)
        return ret;

    return (int)inOutIdx;
}

JNIEXPORT jbyteArray JNICALL Java_com_wolfssl_WolfSSL_x509_1getDer
  (JNIEnv* jenv, jclass jcl, jlong x509)
{
#if defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)
    int* outSz = NULL;
    const unsigned char* derCert;
    jbyteArray out = NULL;

    (void)jcl;

    if (!jenv || !x509)
        return NULL;

    derCert = wolfSSL_X509_get_der((WOLFSSL_X509*)(intptr_t)x509, outSz);

    if (*outSz >= 0) {

        (*jenv)->SetByteArrayRegion(jenv, out, 0, *outSz, (jbyte*)derCert);
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
            return NULL;
        }
        return out;

    } else {
        return NULL;
    }
#else
    (void)jenv;
    (void)jcl;
    (void)x509;
    return NULL;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_getHmacMaxSize
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

    return WC_MAX_DIGEST_SIZE;
}

JNIEXPORT jstring JNICALL Java_com_wolfssl_WolfSSL_getEnabledCipherSuites
  (JNIEnv* jenv, jclass jcl)
{
    int ret;
    char ciphers[4096];
    jstring retString;

    (void)jcl;

    ret = wolfSSL_get_ciphers(ciphers, sizeof(ciphers));
    if (ret != WOLFSSL_SUCCESS) {
        return NULL;
    }

    retString = (*jenv)->NewStringUTF(jenv, ciphers);

    return retString;
}

JNIEXPORT jstring JNICALL Java_com_wolfssl_WolfSSL_getEnabledCipherSuitesIana
  (JNIEnv *jenv, jclass jcl)
{
    int ret;
    char ciphers[4096];
    jstring retString;

    (void)jcl;

    ret = wolfSSL_get_ciphers_iana(ciphers, sizeof(ciphers));
    if (ret != WOLFSSL_SUCCESS) {
        return NULL;
    }

    retString = (*jenv)->NewStringUTF(jenv, ciphers);

    return retString;
}

/* Returns list of available cipher suites in IANA format. Uses
 * wolfSSL_get_ciphers_compat() in order to get a prioritized list. Normal
 * wolfSSL_get_ciphers() returns list of compiled-in cipher suites, but not
 * in same priority order that would be set during a normal connection.
 *
 * @param protocolVersion protocol version that matches the Enum in
 *        src/java/com/wolfssl/WolfSSL.java:
 *
 *        public static enum TLS_VERSION {
 *            INVALID, (0)
 *            TLSv1,   (1)
 *            TLSv1_1, (2)
 *            TLSv1_2, (3)
 *            TLSv1_3, (4)
 *            SSLv23   (5)
 *        }
 * @returns colon-separated cipher suite string.
 */
JNIEXPORT jstring JNICALL Java_com_wolfssl_WolfSSL_getAvailableCipherSuitesIana
  (JNIEnv* jenv, jclass jcl, jint protocolVersion)
{
    char cipherList[4096];
    int i = 0;
    int numCiphers = 0;
#if defined(WOLFSSL_CIPHER_INTERNALNAME) || defined(NO_ERROR_STRINGS) || \
    defined(WOLFSSL_QT)
    int ret = 0;
    int flags;
    byte cipherSuite0;
    byte cipherSuite;
#endif
    const char* cipherName = NULL;
    const char* ianaName = NULL;
    WOLFSSL_METHOD* method = NULL;

    WOLFSSL* ssl = NULL;
    WOLFSSL_CTX* ctx = NULL;

    STACK_OF(SSL_CIPHER) *supportedCiphers = NULL;
    const SSL_CIPHER* cipher = NULL;

    jstring retString;
    (void)jcl;

    if (jenv == NULL) {
        return NULL;
    }

    if (protocolVersion < 0 || protocolVersion > 5) {
        printf("Input protocol version invalid: %d\n", protocolVersion);
        return NULL;
    }

    XMEMSET(cipherList, 0, sizeof(cipherList));

    switch (protocolVersion) {
#ifndef NO_OLD_TLS
    #ifdef WOLFSSL_ALLOW_TLSV10
        case 1:
            method = wolfTLSv1_client_method();
            break;
    #endif
        case 2:
            method = wolfTLSv1_1_client_method();
            break;
#endif /* NO_OLD_TLS */
#ifndef WOLFSSL_NO_TLS12
        case 3:
            method = wolfTLSv1_2_client_method();
            break;
#endif
#ifdef WOLFSSL_TLS13
        case 4:
            method = wolfTLSv1_3_client_method();
            break;
#endif
        case 5:
            method = wolfSSLv23_client_method();
            break;
        default:
            printf("Input protocol version invalid: %d\n", protocolVersion);
            return NULL;
    }

    /* create temporary WOLFSSL_CTX and WOLFSSL structs to get expected
     * available cipher list */
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        return NULL;
    }

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        wolfSSL_CTX_free(ctx);
        return NULL;
    }

    supportedCiphers = wolfSSL_get_ciphers_compat(ssl);
    if (supportedCiphers == NULL) {
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);
        return NULL;
    }

    numCiphers = sk_num(supportedCiphers);

    for (i = 0; i < numCiphers; i++) {
        cipher = (const WOLFSSL_CIPHER*)sk_value(supportedCiphers, i);
        if (cipher != NULL) {
            cipherName =  wolfSSL_CIPHER_get_name(cipher);

        #if defined(WOLFSSL_CIPHER_INTERNALNAME) || \
            defined(NO_ERROR_STRINGS) || defined(WOLFSSL_QT)
            /* CIPHER_get_name() returns internal cipher format in this case,
             * need to convert to IANA format next */
            ret = wolfSSL_get_cipher_suite_from_name(cipherName,
                        &cipherSuite0, &cipherSuite, &flags);
            if (ret == 0) {
                ianaName = wolfSSL_get_cipher_name_iana_from_suite(
                                cipherSuite0, cipherSuite);
            }
        #else
            /* cipherName already in IANA format */
            ianaName = cipherName;
        #endif
            if (ianaName != NULL) {
                /* colon separated list */
                if (i != 0 && (XSTRLEN(cipherList) + 1) < sizeof(cipherList)) {
                    XSTRNCAT(cipherList, ":", 1);
                }
                if ((XSTRLEN(ianaName) + XSTRLEN(cipherList) + 1) <
                        sizeof(cipherList)) {
                    XSTRNCAT(cipherList, ianaName, XSTRLEN(ianaName));
                }
            }
        }
        /* reset ianaName to NULL for next loop */
        ianaName = NULL;
    }

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);

    /* build and return Java String from cipherList array */
    retString = (*jenv)->NewStringUTF(jenv, cipherList);

    return retString;
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_isEnabledCRL
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef HAVE_CRL
    return 1;
#else
    return 0;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_isEnabledCRLMonitor
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef HAVE_CRL_MONITOR
    return 1;
#else
    return 0;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_isEnabledOCSP
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef HAVE_OCSP
    return 1;
#else
    return 0;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_isEnabledPSK
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifndef NO_PSK
    return 1;
#else
    return 0;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_isEnabledDTLS
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef WOLFSSL_DTLS
    return 1;
#else
    return 0;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_isEnabledAtomicUser
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef ATOMIC_USER
    return 1;
#else
    return 0;
#endif
}

JNIEXPORT jint JNICALL Java_com_wolfssl_WolfSSL_isEnabledPKCallbacks
  (JNIEnv* jenv, jclass jcl)
{
    (void)jenv;
    (void)jcl;

#ifdef HAVE_PK_CALLBACKS
    return 1;
#else
    return 0;
#endif
}

JNIEXPORT jobjectArray JNICALL Java_com_wolfssl_WolfSSL_getProtocols
  (JNIEnv* jenv, jclass jcl)
{
    return Java_com_wolfssl_WolfSSL_getProtocolsMask(jenv, jcl, 0);
}

JNIEXPORT jobjectArray JNICALL Java_com_wolfssl_WolfSSL_getProtocolsMask
  (JNIEnv* jenv, jclass jcl, jlong mask)
{
    jobjectArray ret;
    int numProtocols = 0, idx = 0;

    (void)jcl;

    /* get the number of protocols enabled */
#ifdef WOLFSSL_TLS13
    if(!(mask & SSL_OP_NO_TLSv1_3))
        numProtocols += 1;
#endif
#ifndef WOLFSSL_NO_TLS12
    if(!(mask & SSL_OP_NO_TLSv1_2))
        numProtocols += 1;
#endif
#ifndef NO_OLD_TLS
    if(!(mask & SSL_OP_NO_TLSv1_1))
        numProtocols += 1;
#ifdef WOLFSSL_ALLOW_TLSV10
    if(!(mask & SSL_OP_NO_TLSv1))
        numProtocols += 1;
#endif /* WOLFSSL_ALLOW_TLSv10 */
#endif /* !NO_OLD_TLS */
#ifdef WOLFSSL_ALLOW_SSLv3
    if(!(mask & SSL_OP_NO_SSLv3))
        numProtocols += 1;
#endif

    ret = (*jenv)->NewObjectArray(jenv, numProtocols,
            (*jenv)->FindClass(jenv, "java/lang/String"), NULL);
    if (ret == NULL) {
        return NULL;
    }

#ifdef WOLFSSL_TLS13
    if(!(mask & SSL_OP_NO_TLSv1_3)) {
        (*jenv)->SetObjectArrayElement(jenv, ret, idx++,
                (*jenv)->NewStringUTF(jenv, "TLSv1.3"));
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
            (*jenv)->ThrowNew(jenv, jcl, "Error setting TLSv1.3 string");
            return NULL;
        }
    }
#endif

#ifndef WOLFSSL_NO_TLS12
    if(!(mask & SSL_OP_NO_TLSv1_2)) {
        (*jenv)->SetObjectArrayElement(jenv, ret, idx++,
                (*jenv)->NewStringUTF(jenv, "TLSv1.2"));
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
            (*jenv)->ThrowNew(jenv, jcl, "Error setting TLSv1.2 string");
            return NULL;
        }
    }
#endif

#ifndef NO_OLD_TLS
    if(!(mask & SSL_OP_NO_TLSv1_1)) {
        (*jenv)->SetObjectArrayElement(jenv, ret, idx++,
                (*jenv)->NewStringUTF(jenv, "TLSv1.1"));
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
            (*jenv)->ThrowNew(jenv, jcl, "Error setting TLSv1.1 string");
            return NULL;
        }
    }
#ifdef WOLFSSL_ALLOW_TLSV10
    if(!(mask & SSL_OP_NO_TLSv1)) {
        (*jenv)->SetObjectArrayElement(jenv, ret, idx++,
                (*jenv)->NewStringUTF(jenv, "TLSv1"));
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
            (*jenv)->ThrowNew(jenv, jcl, "Error setting TLSv1 string");
            return NULL;
        }
    }
#endif /* WOLFSSL_ALLOW_TLSv10 */
#endif /* !NO_OLD_TLS */

#ifdef WOLFSSL_ALLOW_SSLv3
    if(!(mask & SSL_OP_NO_SSLv3)) {
        (*jenv)->SetObjectArrayElement(jenv, ret, idx++,
                (*jenv)->NewStringUTF(jenv, "SSLv3"));
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionDescribe(jenv);
            (*jenv)->ExceptionClear(jenv);
            (*jenv)->ThrowNew(jenv, jcl, "Error setting SSLv3 string");
            return NULL;
        }
    }
#endif
    return ret;
}
