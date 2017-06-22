#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "baseutils.h"

static JavaVM *savedVm;

void setVM(JavaVM *vm) {
	savedVm = vm;
}

JavaVM *getVM() {
	return savedVm;
}

JNIEnv *getEnv() {
    JNIEnv *env = NULL;
    if (savedVm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
    	env = NULL;
    }
    return env;
}

int getApiLevel(JNIEnv* env) {
    static int sApiLevel = 0;

    while (NULL != env && 0 == sApiLevel) {
        sApiLevel = -1;

        jclass versionClass = env->FindClass("android/os/Build$VERSION");
        if (NULL == versionClass) {
            LOGD("Can't find Build.VERSION");
            break;
        }

        jfieldID sdkIntFieldID = env->GetStaticFieldID(versionClass, "SDK_INT", "I");
        if (NULL == sdkIntFieldID) {
            LOGD("Can't find Build.VERSION.SDK_INT");
            break;
        }

        sApiLevel = env->GetStaticIntField(versionClass, sdkIntFieldID);
        LOGD("SDK_INT = %d.", sApiLevel);
        break;
    }

    return sApiLevel;
}

int checkBadReadPtr(void* ptr, size_t size) {
    int canRead = 0;
    int nullfd = open("/dev/random", O_WRONLY);
    if (-1 != nullfd) {
        if (write(nullfd, ptr, size) < 0) {
            /* Not OK */
            canRead = -1;
            LOGD("Address %p can't be read", ptr);
        } else {
            canRead = 1;
            LOGD("Address %p can be read", ptr);
        }
        close(nullfd);
    } else{
        LOGW("Can't open /dev/random");
    }
    return canRead;
}
