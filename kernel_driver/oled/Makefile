
subdir-ccflags-y += \
	-DUSE_GPIO \
	-I$(srctree)/drivers/spi/mediatek/$(MTK_PLATFORM) \
	-I$(srctree)/drivers/misc/mediatek/video/$(MTK_PLATFORM)/dispsys \
	-I$(srctree)/drivers/misc/mediatek/mmp/\
	-I$(srctree)/drivers/misc/mediatek/video/include/ \
	-I$(srctree)/drivers/misc/mediatek/lcm/inc/ \
	-I$(srctree)/drivers/misc/mediatek/cmdq/v2/ \
	-I$(srctree)/drivers/misc/mediatek/cmdq/v2/$(MTK_PLATFORM)/ \

subdir-ccflags-y += -DMICROPANEL_BPP=8

obj-y	+= OledMono.o
obj-y	+= OledDriver_intf.o
obj-y	+= OledDriver_2832TSWUG01.o


