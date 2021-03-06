KBUILD_CFLAGS += -DZEPHYR_USED
KBUILD_CFLAGS += -DPERIPHERAL_CONFIG

PROJECT_BASE	= 	$(PROJ_DIR)
SOURCE_DIR 		= 	$(PROJ_DIR)
BOARD					?= 	nrf52840_pca10056
CONF_FILE 		= 	$(PROJECT_BASE)/Environment/Peripheral/Zephyr/zephyr.config

obj-y += Drivers/
obj-y += Debug/
obj-y += AL/Zephyr/
obj-y += Peripheral/

include ${ZEPHYR_BASE}/Makefile.inc
