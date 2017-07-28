#!/bin/bash

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: INCLUDE
# ########## ########## ########## ########## ########## ########## ########## ##########
GRIB_ENV_FILE=grib_env.sh

if [ -f "./${GRIB_ENV_FILE}" ] ; then
	source ./${GRIB_ENV_FILE}
else
	echo -e
	echo "NOT FOUND ${GRIB_ENV_FILE} FILE!!!"
	exit
fi

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: COMMON UTIL
# ########## ########## ########## ########## ########## ########## ########## ##########
if [ "make" == "$1" ] ; then

	if [ "all" == "$2" ] ; then
		make clean
		make all
	else
		make ble_hub_clean
		make ble_hub
	fi

	if [ -f "${GRIB_BLE_HUB_NAME}" ] ; then
		echo -e
		echo -e "# ${COLOR_GREEN_BOLD}"${GRIB_BLE_HUB_NAME} MAKE DONE ..."${COLOR_END}"
		echo -e
		ldd ./${GRIB_BLE_HUB_NAME}
		echo -e
	else
		echo -e
		echo -e "# ${COLOR_RED_BOLD}"${GRIB_BLE_HUB_NAME} MAKE FAIL !!!"${COLOR_END}"
		echo -e
	fi

	echo -e
	exit
fi

if [ "kill" == "$1" ] ; then
	kill_grib_porc
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
	elif [ "adv" == "$2" ] ; then
		clear
		"sudo" ./${GRIB_HCI_NAME} "$2" "$3"
		echo -e
		exit
	elif [ "linno" == "$2" ] ; then
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
# shbaek: ONLY REBOOT
# ########## ########## ########## ########## ########## ########## ########## ##########
if [ "reboot" == "$1" ] ; then
	./${GRIB_REBOOT_NAME} "$2" "$3" "$4"
	echo -e
	exit
fi

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: ONLY BLE HUB
# ########## ########## ########## ########## ########## ########## ########## ##########

if [ "hub" == "$1" ] ; then
	if [ -f "./${GRIB_BLE_HUB_NAME}" ] ; then
		del_sys_log_file
		del_certi_file
		echo -e
		echo -e "# RUN PROGRAM :" "${COLOR_GREEN_BOLD}" "BLE HUB ..." "${COLOR_END}"
		echo -e
		./${GRIB_REBOOT_NAME} "config" &
		./${GRIB_BLE_HUB_NAME} "hub" &
	else
		echo -e
		echo -e "# ${COLOR_RED_BOLD}" "NOT FOUND ${GRIB_BLE_HUB_NAME} PROGRAM !!!" "${COLOR_END}"
		echo -e
	fi

	echo -e
	exit
fi

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: HUB ANOTHER
# ########## ########## ########## ########## ########## ########## ########## ##########
if [ -f "./${GRIB_BLE_HUB_NAME}" ] ; then
	echo -e
else
	echo -e
	echo -e "# ${COLOR_RED_BOLD}"NOT FOUND ${GRIB_BLE_HUB_NAME} PROGRAM !!!"${COLOR_END}"
	echo -e
	exit
fi

./${GRIB_BLE_HUB_NAME} "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9"
echo -e
exit

