# Makefile for ecos_usbserial Linux kernel module

ifneq ($(KERNELRELEASE),)
	obj-m := ecos_usbserial.o
else
	KDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
endif

