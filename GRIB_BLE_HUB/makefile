# ########## ########## ########## ########## ########## ##########
# shbaek: MAKE FILE
# ########## ########## ########## ########## ########## ##########

CC=gcc
CXX=g++
RM=-rm
RMFLAG=-rf
LDFLAGS=
CFLAGS=-Wall -c
CXXFLAGS=-w -g

TEMP_OS=$(shell sed -n '/^ID=/p' "/etc/os-release")
OS=$(subst ID=,,$(TEMP_OS))

# ########## ########## ########## ########## ########## ##########
# shbaek: COMMON VALUE
# ########## ########## ########## ########## ########## ##########
COM_FEATURE	=	-DFEATURE_GRIB

COM_INC		= 	-Iinclude						\
				-I/usr/include/mysql

ifeq ($(OS), "centos")
COM_LIB		=	-L/usr/lib/arm-linux-gnueabihf	\
				-L/usr/lib64/mysql				\
				-lmysqlclient					\
				-lz -lm -ldl
else
COM_LIB		=	-L/usr/lib/arm-linux-gnueabihf	\
				-lmysqlclient					\
				-lz -lm -ldl
endif

COM_OBJ		=	grib_util.o						\
				grib_log.o						\
				grib_db.o						\
				grib_config.o

# ########## ########## ########## ########## ########## ##########
# shbaek: FOR REBOOT
# ########## ########## ########## ########## ########## ##########
REBOOT_TARGET	=	grib_reboot

REBOOT_INC		= 	$(COM_INC)

REBOOT_LIB		=	$(COM_LIB)

REBOOT_OBJ		=	$(COM_OBJ)						\
					grib_reboot.o

REBOOT_SRC 		= 	$(REBOOT_OBJ:.o=.cpp)

# ########## ########## ########## ########## ########## ##########
# shbaek: FOR HCI
# ########## ########## ########## ########## ########## ##########
HCI_TARGET	=	grib_hci

HCI_INC		= 	$(COM_INC)

HCI_LIB		=	$(COM_LIB)						\
				-lbluetooth

HCI_OBJ		=	$(COM_OBJ)						\
				grib_hci.o

HCI_SRC 	= 	$(HCI_OBJ:.o=.cpp)

# ########## ########## ########## ########## ########## ##########
# shbaek: FOR CAS
# FEATURE: -DFEATURE_CAS or -DFEATURE_CAS_NOT_USED
# ########## ########## ########## ########## ########## ##########
CAS_FEATURE	=	-DFEATURE_CAS_NOT_USED

ifeq (${CAS_FEATURE}, -DFEATURE_CAS)
CAS_PATH 	= 	${CURDIR}/libs/cas_package
CAS_INC		= 	-I${CAS_PATH}
CAS_LIB		=	-L${CAS_PATH}					\
				-lTrustNETCASClient				\
				-lTrustNETCASCstk
CAS_OBJ		=	grib_cas.o
endif
# ########## ########## ########## ########## ########## ##########
# shbaek: FOR GRIB BLE HUB
# ########## ########## ########## ########## ########## ##########
BLE_HUB_FEATURE	=	$(COM_FEATURE) \
					$(CAS_FEATURE)

BLE_HUB_TARGET	= 	grib_ble_hub

BLE_HUB_INC		= 	$(COM_INC)						\
				  	$(CAS_INC)

BLE_HUB_LIB		=	$(COM_LIB)						\
					$(CAS_LIB)						\
					-lpthread 						\
					-lbluetooth

BLE_HUB_OBJ 	=	$(COM_OBJ)						\
					$(CAS_OBJ)						\
					grib_hub.o						\
					grib_ble.o 						\
					grib_thread.o					\
					grib_http.o 					\
					grib_onem2m.o 					\
					grib_regi.o 					\
					grib_smd.o 						\
					grib_auth.o

BLE_HUB_SRC 	= 	$(BLE_HUB_OBJ:.o=.cpp)


# ########## ########## ########## ########## ########## ##########
# shbaek: COMMAND
# ########## ########## ########## ########## ########## ##########
all: reboot hci ble_hub

reboot:
	@echo "GRIB REBOOT MAKE ..."
	@$(CXX) $(CXXFLAGS) -o $(REBOOT_TARGET) $(REBOOT_SRC) $(REBOOT_INC) $(REBOOT_LIB)
	@$(RM) $(RMFLAG) $(REBOOT_OBJ)
	@echo
reboot_clean:
	@echo "GRIB REBOOT CLEAN ..."
	@$(RM) $(RMFLAG) $(REBOOT_OBJ) $(REBOOT_TARGET)
	@echo

hci:
	@echo "GRIB HCI MAKE ..."
	@$(CXX) $(CXXFLAGS) -o $(HCI_TARGET) $(HCI_SRC) $(HCI_INC) $(HCI_LIB)
	@$(RM) $(RMFLAG) $(HCI_OBJ)
	@echo
hci_clean:
	@echo "GRIB HCI CLEAN ..."
	@$(RM) $(RMFLAG) $(HCI_OBJ) $(HCI_TARGET)
	@echo

ble_hub:
	@echo "GRIB BLE HUB MAKE ..."
	@$(CXX) $(CXXFLAGS) -o $(BLE_HUB_TARGET) $(BLE_HUB_SRC) $(BLE_HUB_INC) $(BLE_HUB_LIB) $(BLE_HUB_FEATURE)
	@$(RM) $(RMFLAG) $(BLE_HUB_OBJ)
	@echo "FEATURE LIST: ${BLE_HUB_FEATURE}"
	@echo
ble_hub_clean:
	@echo "GRIB BLE HUB CLEAN ..."
	@$(RM) $(RMFLAG) $(BLE_HUB_OBJ) $(BLE_HUB_TARGET)
	@echo

clean: hci_clean ble_hub_clean

