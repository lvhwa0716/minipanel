# minipanel
# 0. 需要使用 native中的java 库
	out/target/common/obj/JAVA_LIBRARIES/com.i029.minipanel.IMicroPanelService_intermediates/javalib.jar
# 1. APK 必须系统签名
	android {
    compileSdkVersion 26
    defaultConfig {
        applicationId "com.i029.minipanel.minipaneltest"
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
    package="com.i029.minipanel.minipaneltest"
    coreApp="true"
    android:sharedUserId="android.uid.system">

# 3. 功能说明
	minipanelservice : native service
		BoardConfig.mk
			PRODUCT_COPY_FILES += vendor/i029/display/minipanel/init.minipanel.rc:root/init.minipanel.rc

		init.${ro.hardware}.rc
			import init.minipanel.rc

	com.i029.minipanel.IMicroPanelService : java libraray
	libminipanel : core library , C Client 
	minipaneldump : dump oled framebuffer in console[DRIVER_SW_MODE must be DRIVER_MODE_APP in OledDriver_intfApp.c ]

# 4. sepolicy
	BoardConfig.mk
		BOARD_SEPOLICY_DIRS += vendor/i029/display/minipanel/sepolicy_minipanel

	Debug :
		a. ls -Z   查看文件属性(system , boot )
		b. getprop | grep init.svc  查看启动状态
		c. dmesg , logcat

# 5. 工具
	OledDumpServer.py :
		Http Put Server, 将硬件显示的Frame Buffer显示在PC上, 默认端口9234 (也可用于其他图形模拟显示, 速度较慢)
		Android设备 需要设置 adb reverse tcp:9234 tcp:9234
		
	DumpBmp.py :
		将图片转化为单色的C语言数组

# 6. config
	Android.mk : 
		must define 
			local_define := -DMICROPANEL_BPP=8
			or
			local_define := -DMICROPANEL_BPP=1
		
		must remove when release : local_define += -DDEBUG_LOG
	Makefile :
		must define same as Android.mk
			subdir-ccflags-y += -DMICROPANEL_BPP=8
			or
			subdir-ccflags-y += -DMICROPANEL_BPP=1
		 must define 
			USE_GPIO or USE_SPI following hardware
			
# 7. adb默认打开
		UsbDeviceManager.java : 默认开启 adb

			public void systemReady() {
				if (DEBUG) Slog.d(TAG, "systemReady");

				mNotificationManager = (NotificationManager)
				        mContext.getSystemService(Context.NOTIFICATION_SERVICE);

				// We do not show the USB notification if the primary volume supports mass storage.
				// The legacy mass storage UI will be used instead.
				boolean massStorageSupported = false;
				final StorageManager storageManager = StorageManager.from(mContext);
				final StorageVolume primary = storageManager.getPrimaryVolume();
				massStorageSupported = primary != null && primary.allowMassStorage();
				mUseUsbNotification = !massStorageSupported;

				// make sure the ADB_ENABLED setting value matches the current state
				try {
					Slog.d(TAG, "lvh systemReady: " + mAdbEnabled);
					mAdbEnabled = true;
				    Settings.Global.putInt(mContentResolver,
				            Settings.Global.ADB_ENABLED, mAdbEnabled ? 1 : 0);
			
				} catch (SecurityException e) {
				    // If UserManager.DISALLOW_DEBUGGING_FEATURES is on, that this setting can't be changed.
				    Slog.d(TAG, "ADB_ENABLED is restricted.");
				}
				mHandler.sendEmptyMessage(MSG_SYSTEM_READY);
			}

			private String getDefaultFunctions() {
			    String func = SystemProperties.get(USB_PERSISTENT_CONFIG_PROPERTY,
			            UsbManager.USB_FUNCTION_NONE);
			    if (UsbManager.USB_FUNCTION_NONE.equals(func)) {
			        func = UsbManager.USB_FUNCTION_MTP;
					// lvh@@@
					func = func + "," + UsbManager.USB_FUNCTION_ADB;
			    }
				Slog.d(TAG, "lvh@tcl getDefaultFunctions : " + func);
			    return func;
			}



		USB 调试授权

			UsbDebuggingActivity.java

		ADDITIONAL_DEFAULT_PROPERTIES += persist.sys.usb.config=adb

# 8. 权限自动允许
		厂商通常会自己实现PackageInstaller, 需要使用默认的包UI: GrantPermissionsActivity.java
		DevicePolicyManagerService.java
			public DevicePolicyData(int userHandle) {
		        mUserHandle = userHandle;
				//{{lvh@@@
				mPermissionPolicy = DevicePolicyManager.PERMISSION_POLICY_AUTO_GRANT; 
				//}}
		    }

			 private void clearUserPoliciesLocked(UserHandle userHandle) {
					int userId = userHandle.getIdentifier();
					// Reset some of the user-specific policies
					DevicePolicyData policy = getUserData(userId);
					//{{lvh@@@
					policy.mPermissionPolicy = DevicePolicyManager.PERMISSION_POLICY_AUTO_GRANT;// DevicePolicyManager.PERMISSION_POLICY_PROMPT;
					//}}
					policy.mDelegatedCertInstallerPackage = null;
					policy.mStatusBarDisabled = false;
					saveSettingsLocked(userId);

					final long ident = Binder.clearCallingIdentity();
					try {
						clearUserRestrictions(userHandle);
						AppGlobals.getPackageManager().updatePermissionFlagsForAllApps(
						        PackageManager.FLAG_PERMISSION_POLICY_FIXED,
						        0  /* flagValues */, userHandle.getIdentifier());
					} catch (RemoteException re) {
					} finally {
						Binder.restoreCallingIdentity(ident);
					}
				}
			
		@Override
		public int getPermissionPolicy(ComponentName admin) throws RemoteException {
		    int userId = UserHandle.getCallingUserId();
		    synchronized (this) {
		        DevicePolicyData userPolicy = getUserData(userId);
				//adb logcat -b system | grep lvh
				Slog.i(LOG_TAG, "lvh@@@ , userPolicy.mPermissionPolicy: " + userPolicy.mPermissionPolicy);
		        return userPolicy.mPermissionPolicy;
		    }
		}
	
#9 禁止锁屏
	frameworks/base/packages/SettingsProvider/src/com/android/providers/settings/DatabaseHelper.java
	缺省禁止锁屏
	Settings.System.LOCKSCREEN_DISABLED
	PRODUCT_PROPERTY_OVERRIDES += ro.lockscreen.disable.default=true
	可以在PhoneWindowManager.java中不创建 KeyguardServiceDelegate(mContext)
