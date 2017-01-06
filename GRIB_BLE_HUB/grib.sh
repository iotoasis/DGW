#!/bin/bash

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: PROGRAM CONFIG
# ########## ########## ########## ########## ########## ########## ########## ##########
GRIB_HUB_PATH=/home/pi/20_GRIB_HUB
GRIB_INC_FILE=00_GribColor.sh
GRIB_HUB_NAME=grib_hub
GRIB_HCI_NAME=grib_hci
GRIB_EMUL_NAME=grib_emulator

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: DEPENDENCY
# ########## ########## ########## ########## ########## ########## ########## ##########
CAS_LIB_PATH=${GRIB_HUB_PATH}/libs/cas_package
export LD_LIBRARY_PATH=/usr/lib:$CAS_LIB_PATH:$LD_LIBRARY_PATH
#echo ${LD_LIBRARY_PATH}

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: INCLUDE
# ########## ########## ########## ########## ########## ########## ########## ##########
if [ -f "${GRIB_INC_FILE}" ] ; then
	source ${GRIB_HUB_PATH}/${GRIB_INC_FILE}
else
	echo -e
	echo "NOT FOUND " ${GRIB_INC_FILE} "!!!"
fi

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: COMMON UTIL
# ########## ########## ########## ########## ########## ########## ########## ##########
if [ "make" == "$1" ] ; then

	if [ "all" == "$2" ] ; then
		make clean
		make all
	else
		make hub_clean
		make hub
	fi

	if [ -f "${GRIB_HUB_NAME}" ] ; then
		echo -e
		echo -e "# ${COLOR_GREEN_BOLD}"${GRIB_HUB_NAME} MAKE DONE ..."${COLOR_END}"
		echo -e
		ldd ./${GRIB_HUB_NAME}
		echo -e
	else
		echo -e
		echo -e "# ${COLOR_RED_BOLD}"${GRIB_HUB_NAME} MAKE FAIL !!!"${COLOR_END}"
		echo -e
	fi

	exit
fi

if [ "kill" == "$1" ] ; then
	echo -e "# ${COLOR_RED_BOLD}"${GRIB_HUB_NAME} PROCESS KILL !!!"${COLOR_END}"
	ps -aux | grep ${GRIB_HUB_NAME}
	sudo "$1" -9 `pidof ${GRIB_HUB_NAME}`
	echo -e
	exit
fi

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: ONLY HCI
# ########## ########## ########## ########## ########## ########## ########## ##########
if [ "ble" == "$1" ] ; then

	if [ "init" == "$2" ] ; then
		clear
		"sudo" ./${GRIB_HCI_NAME} "$2"
		echo -e
		exit
	elif [ "scan" == "$2" ] ; then
		clear
		"sudo" ./${GRIB_HCI_NAME} "$2" "$3"
		echo -e
		exit
	fi
fi

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: ONLY EMULATOR
# ########## ########## ########## ########## ########## ########## ########## ##########
if [ "emulator" == "$1" ] ; then
	./${GRIB_EMUL_NAME} "$2" "$3" "$4"
	echo -e
	exit
fi

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: ONLY HUB
# ########## ########## ########## ########## ########## ########## ########## ##########
if [ -f "${GRIB_HUB_PATH}/${GRIB_HUB_NAME}" ] ; then
	echo -e
else
	echo -e
	echo -e "# ${COLOR_RED_BOLD}"NOT FOUND ${GRIB_HUB_NAME} PROGRAM !!!"${COLOR_END}"
	echo -e
	exit
fi

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: HUB ANOTHER
# ########## ########## ########## ########## ########## ########## ########## ##########
./${GRIB_HUB_NAME} "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9"
echo -e
exit
