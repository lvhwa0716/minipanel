# minipanel
# 0. 需要使用 native中的java 库
	out/target/common/obj/JAVA_LIBRARIES/com.tclxa.minipanel.IMicroPanelService_intermediates/javalib.jar
# 1. APK 必须系统签名
	android {
    compileSdkVersion 26
    defaultConfig {
        applicationId "com.tclxa.minipanel.minipaneltest"
        minSdkVersion 21
        targetSdkVersion 26
        versionCode 1
        versionName "1.0"
        testInstrumentationRunner "android.support.test.runner.AndroidJUnitRunner"
    }
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
        }
    }
    signingConfigs {
        release {
            storeFile file("key/demo.jks")
            storePassword '123456'
            keyAlias 'demo'
            keyPassword '123456'
        }

        debug {
            storeFile file("key/demo.jks")
            storePassword '123456'
            keyAlias 'demo'
            keyPassword '123456'
        }
    }
}

# 2. APK需要ShareUID system
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.tclxa.minipanel.minipaneltest"
    coreApp="true"
    android:sharedUserId="android.uid.system">

# 3. 功能说明
	minipanelservice : native service
	com.tclxa.minipanel.IMicroPanelService : java libraray
	libminipanel : core library , C Client 
	minipaneldump : dump oled framebuffer in console[DRIVER_SW_MODE must be DRIVER_MODE_APP in OledDriver_intfApp.c ]
