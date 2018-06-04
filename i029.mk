PRODUCT_PACKAGES += minipanelservice minipanelDriverTest

BOARD_SEPOLICY_DIRS += vendor/i029/display/minipanel/sepolicy_minipanel

PRODUCT_COPY_FILES += vendor/i029/display/minipanel/init.minipanel.rc:root/init.minipanel.rc

#must touch device/....../system.prop to rebuild 

ADDITIONAL_DEFAULT_PROPERTIES += persist.sys.usb.config=adb

PRODUCT_PROPERTY_OVERRIDES += ro.lockscreen.disable.default=true
PRODUCT_PROPERTY_OVERRIDES += persist.service.adb.enable=1
#ADDITIONAL_BUILD_PROPERTIES += ro.lockscreen.disable.default=true
#ADDITIONAL_BUILD_PROPERTIES += persist.service.adb.enable=1
