#
# eCos support tools for EP7211
#

DL_FILES = dl_7211.o io.o
DOWNLOAD_FILES = download_file.o io.o

all: dl_7211 download

dl_7211: $(DL_FILES)
	$(CC) -o $@ $(DL_FILES)

download: $(DOWNLOAD_FILES)
	$(CC) -o $@ $(DOWNLOAD_FILES)
