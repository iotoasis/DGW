#!/bin/bash

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: Constant
# ########## ########## ########## ########## ########## ########## ########## ##########

export MY_DONE=0
export MY_SUCCESS=0
export MY_OFF=0

export MY_ON=1
export MY_FAIL=1
export MY_ERROR=1

export MY_RESULT=0

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: Name & Path
# ########## ########## ########## ########## ########## ########## ########## ##########

export GRIB_HOME_PATH=/home/pi
export GRIB_BLE_HUB_DIR=31_GRIB_BLE_HUB
export GRIB_BLE_HUB_PATH=${GRIB_HOME_PATH}/${GRIB_BLE_HUB_DIR}

export GRIB_BLE_HUB_NAME=grib_ble_hub
export GRIB_BLE_TOOL_NAME=gatttool
export GRIB_HCI_NAME=grib_hci
export GRIB_EMUL_NAME=grib_emulator
export GRIB_REBOOT_NAME=grib_reboot

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: DEPENDENCY
# ########## ########## ########## ########## ########## ########## ########## ##########

LIB_PATH_BLE_HUB_CAS=${GRIB_BLE_HUB_PATH}/libs/cas_package
export LD_LIBRARY_PATH=/usr/lib:${LIB_PATH_BLE_HUB_CAS}:$LD_LIBRARY_PATH

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: Color
# ########## ########## ########## ########## ########## ########## ########## ##########

export COLOR_BLACK="\e[30m"

export COLOR_RED="\e[31m"
export COLOR_RED_BOLD="\e[1;31m"
export COLOR_RED_UNDER="\e[4;31m"
export COLOR_RED_BLINK="\e[5;31m"

export COLOR_GREEN="\e[32m"
export COLOR_GREEN_BOLD="\e[1;32m"
export COLOR_GREEN_UNDER="\e[4;32m"
export COLOR_GREEN_BLINK="\e[5;32m"

export COLOR_YELLOW="\e[33m"
export COLOR_YELLOW_BOLD="\e[1;33m"
export COLOR_YELLOW_UNDER="\e[4;33m"
export COLOR_YELLOW_BLINK="\e[5;33m"

export COLOR_BLUE="\e[34m"
export COLOR_BLUE_BOLD="\e[1;34m"
export COLOR_BLUE_UNDER="\e[4;34m"
export COLOR_BLUE_BLINK="\e[5;34m"

export COLOR_WHITE="\e[37m"
export COLOR_WHITE_BOLD="\e[1;37m"
export COLOR_WHITE_UNDER="\e[4;37m"
export COLOR_WHITE_BLINK="\e[5;37m"

export COLOR_END="\e[m"

# ########## ########## ########## ########## ########## ########## ########## ##########
# shbaek: FUNCTION
# ########## ########## ########## ########## ########## ########## ########## ##########

function print_ld_path()
{

	echo -e "# ########## ########## ########## ########## ########## ########## ########## ##########"
	echo -e "# LD_LIBRARY_PATH: "
	echo -e "# ${COLOR_YELLOW_BOLD}" ${LD_LIBRARY_PATH} "${COLOR_END}"
	echo -e "# ########## ########## ########## ########## ########## ########## ########## ##########"
	echo -e
}

function del_sys_log_file()
{
	local LOG_PATH="/var/log"
	local LOG_MAX_SIZE_MB=10

	myList=`sudo find ${LOG_PATH} -size +${LOG_MAX_SIZE_MB}M -type f -print`

	echo -e
	for file in ${myList[@]}
	do
		echo -e "# OVER SIZE LOG FILE: ${COLOR_RED_BOLD}" "${file}" "${COLOR_END}"
		sudo rm -rf "${file}"
		sleep 1
	done

	echo -e
}

function del_certi_file()
{
	local CERTI_FILE_LIST=(	"/home/pi/UNETsystem"
							"${GRIB_BLE_HUB_PATH}/(null)"
						  )

	echo -e
	for file in ${CERTI_FILE_LIST[@]}
	do
		echo -e "# DELETE FILE: ${COLOR_RED_BOLD}" "${file}" "${COLOR_END}"
		sudo rm -rf "${file}"
		sleep 1
	done

	echo -e
}

function kill_grib_porc()
{
	local PROC_LIST=(	${GRIB_BLE_HUB_NAME}
						${GRIB_BLE_TOOL_NAME}
						${GRIB_HCI_NAME}
						${GRIB_EMUL_NAME}
						${GRIB_REBOOT_NAME}
					)

	for proc in ${PROC_LIST[@]}
	do
		echo -e
		echo -e "# PROCESS KILL :" "${COLOR_RED_BOLD}${proc}" "${COLOR_END}"
		ps -aux | grep ${proc}
		sudo kill -9 `pidof ${proc}`
		sleep 1
	done

	echo -e
}
