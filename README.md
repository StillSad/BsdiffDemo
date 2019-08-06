#资料准备
1.下载bsdiff
http://www.daemonology.net/bsdiff/
2.下载bzip2
https://sourceforge.net/projects/bzip2/
3.创建NDK项目，并在项目cpp目录下创建bspath和bzip2文件夹
#复制文件
1.复制bsdiff中bspatch.c到bspath目录
2.复制 blocksort.c,bzlib.c,bzlib.h,bzlib_private.h,compress.c,crctable.c,decompress.c,huffman.c,randtable.c 到bzip2文件夹
#配置CMakeLists
```cmake
cmake_minimum_required(VERSION 3.4.1)

include_directories(${CMAKE_SOURCE_DIR}/bzip2)

aux_source_directory(
        ${CMAKE_SOURCE_DIR}/bzip2/ bzip2_src
)

aux_source_directory(
        ${CMAKE_SOURCE_DIR}/bspath/ bspath_src
)

add_library(
        native-lib
        SHARED
        native-lib.cpp
        ${bspath_src}
        ${bzip2_src}
)


find_library(
        log-lib
        log)


target_link_libraries(
        native-lib
        ${log-lib})
```
#修改bspatch.c
bspatch.c中main函数名称为bspatch_main
```c
int bspatch_main(int argc,char * argv[])
```
#JNI
在java中添加native方法
```java
    private native void doPatchNative(String oldApk, String newApk, String pathch);
```
在对应生成jni方法中合成新apk
```c++
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
```
bspatch_main(4,argv)会报错需要声明bspatch_main是外部函数
```
extern "C"{
    extern int bspatch_main(int argc,char * argv[]);
}
```
#合并apk参数，安装新apk
```java
 new AsyncTask<Void, Void, File>() {

            @Override
            protected File doInBackground(Void... voids) {
                //差分包
                String patch = new File(Environment.getExternalStorageDirectory(), "patch.diff").getAbsolutePath();
                //合并后apk路径
                File newApk = new File(Environment.getExternalStorageDirectory(), "new.apk");

                if (!newApk.exists()) {
                    try {
                        newApk.createNewFile();
                    } catch (IOException e){
                        e.printStackTrace();
                    }
                }
                //已安装app的apk文件路径
                String oldApk = getApplicationInfo().sourceDir;
                doPatchNative(oldApk,newApk.getAbsolutePath(),patch);
                return newApk;
            }

            @Override
            protected void onPostExecute(File apkFile) {
                if (!apkFile.exists()) {
                    return;
                }

                Intent intent = new Intent(Intent.ACTION_VIEW);
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                if (Build.VERSION.SDK_INT >= 24) { //Android 7.0及以上
                    // 参数2 清单文件中provider节点里面的authorities ; 参数3  共享的文件,即apk包的file类
                    Uri apkUri = FileProvider.getUriForFile(MainActivity.this,
                            getApplicationInfo().packageName + ".provider", apkFile);
                    //对目标应用临时授权该Uri所代表的文件
                    intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
                    intent.setDataAndType(apkUri, "application/vnd.android.package-archive");
                } else {
                    intent.setDataAndType(Uri.fromFile(apkFile),
                            "application/vnd.android.package-archive");
                }
                startActivity(intent);
            }
        }.execute();
```