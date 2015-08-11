
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
