# ########## ########## ########## ########## ########## ##########
# shbaek: MAKE FILE
# ########## ########## ########## ########## ########## ##########

CC=gcc
CXX=g++
RM=-rm
RMFLAG=-rf
LDFLAGS=
CFLAGS=-Wall -c
#CXXFLAGS=-Wall -g
CXXFLAGS=-w -g

# ########## ########## ########## ########## ########## ##########
# shbaek: COMMON VALUE
# ########## ########## ########## ########## ########## ##########
COM_FEATURE	=	-DFEATURE_GRIB

COM_INC		= 	-Iinclude						\

COM_LIB		=	-L/usr/lib/arm-linux-gnueabihf	\
				-lz -lm -ldl

# ########## ########## ########## ########## ########## ##########
# shbaek: FOR HCI
# ########## ########## ########## ########## ########## ##########
HCI_TARGET	=	grib_hci

HCI_INC		= 	$(COM_INC)

HCI_LIB		=	$(COM_LIB)						\
				-lbluetooth

HCI_OBJ		=	grib_hci.o						\
				grib_util.o

HCI_SRC 	= 	$(HCI_OBJ:.o=.cpp)

# ########## ########## ########## ########## ########## ##########
# shbaek: FOR CAS
# ########## ########## ########## ########## ########## ##########
CAS_FEATURE	=	-DFEATURE_CAS
#CAS_FEATURE	=	-DFEATURE_CAS_NOT_USED

CAS_PATH 	= 	${CURDIR}/libs/cas_package

CAS_INC		= 	-I${CAS_PATH}

CAS_LIB		=	-L${CAS_PATH}					\
				-lTrustNETCASClient				\
				-lTrustNETCASCstk

CAS_OBJ		=	grib_cas.o

# ########## ########## ########## ########## ########## ##########
# shbaek: FOR GRIB HUB
# ########## ########## ########## ########## ########## ##########
HUB_FEATURE	=	$(COM_FEATURE)					\
				$(CAS_FEATURE)

HUB_TARGET	= 	grib_hub

HUB_INC		= 	$(COM_INC)						\
			  	-I/usr/include/mysql			\
			  	$(CAS_INC)

HUB_LIB		=	$(COM_LIB)						\
				-lpthread 						\
				-lbluetooth 					\
			 	-lmysqlclient 					\
				$(CAS_LIB)

HUB_OBJ 	=	grib_hub.o 						\
				grib_util.o 					\
				grib_db.o						\
				grib_ble.o 						\
				grib_thread.o					\
				grib_http.o 					\
				grib_onem2m.o 					\
				grib_regi.o 					\
				grib_sda.o 						\
				grib_auth.o 					\
				$(CAS_OBJ)

HUB_SRC 	= 	$(HUB_OBJ:.o=.cpp)

# ########## ########## ########## ########## ########## ##########
# shbaek: FOR GRIB EMULATOR
# ########## ########## ########## ########## ########## ##########
EMUL_FEATURE=	$(COM_FEATURE)					\
				$(CAS_FEATURE)

EMUL_TARGET	= 	grib_emulator

EMUL_INC	= 	$(COM_INC)						\
			  	-I/usr/include/mysql			\
			  	$(CAS_INC)

EMUL_LIB	=	$(COM_LIB)						\
				-lpthread 						\
			 	-lmysqlclient 					\
				$(CAS_LIB)

EMUL_OBJ 	=	grib_emulator.o 				\
				grib_util.o 					\
				grib_http.o 					\
				grib_onem2m.o 					\
				grib_sda.o 						\
				grib_auth.o 					\
				$(CAS_OBJ)

EMUL_SRC 	= 	$(EMUL_OBJ:.o=.cpp)


# ########## ########## ########## ########## ########## ##########
# shbaek: COMMAND
# ########## ########## ########## ########## ########## ##########
all: clean hub hci

hci:
	$(CXX) $(CXXFLAGS) -o $(HCI_TARGET) $(HCI_SRC) $(HCI_INC) $(HCI_LIB)
	$(RM) $(RMFLAG) $(HCI_OBJ)
hci_clean:
	$(RM) $(RMFLAG) $(HCI_OBJ) $(HCI_TARGET)

hub:
	$(CXX) $(CXXFLAGS) -o $(HUB_TARGET) $(HUB_SRC) $(HUB_INC) $(HUB_LIB) $(HUB_FEATURE)
	$(RM) $(RMFLAG) $(HUB_OBJ)
	$(info FEATURE LIST: ${HUB_FEATURE})
	$(info )
hub_clean:
	$(RM) $(RMFLAG) $(HUB_OBJ) $(HUB_TARGET)

emulator:
	$(CXX) $(CXXFLAGS) -o $(EMUL_TARGET) $(EMUL_SRC) $(EMUL_INC) $(EMUL_LIB) $(EMUL_FEATURE)
emulator_clean:
	$(RM) $(RMFLAG) $(EMUL_OBJ) $(EMUL_TARGET)

clean: hci_clean hub_clean

