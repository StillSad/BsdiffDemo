#include <jni.h>
#include <string>

extern "C"{
    extern int bspatch_main(int argc,char * argv[]);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_ice_bsdiffwangyi_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ice_bsdiffwangyi_MainActivity_doPatchNative(JNIEnv *env, jobject instance, jstring oldApk_,
                                                     jstring newApk_, jstring pathch_) {
    const char *oldApk = env->GetStringUTFChars(oldApk_, 0);
    const char *newApk = env->GetStringUTFChars(newApk_, 0);
    const char *pathch = env->GetStringUTFChars(pathch_, 0);

    char * argv[4] = {
            "bspatch",
            const_cast<char *>(oldApk),
            const_cast<char *>(newApk),
            const_cast<char *>(pathch)
    };
    bspatch_main(4,argv);

    env->ReleaseStringUTFChars(oldApk_, oldApk);
    env->ReleaseStringUTFChars(newApk_, newApk);
    env->ReleaseStringUTFChars(pathch_, pathch);
}