# ########## ########## ########## ########## ########## ##########
# shbaek: MAKE FILE
# ########## ########## ########## ########## ########## ##########

CC=gcc
CXX=g++
RM=rm
RMFLAG=-rf
LDFLAGS=
CFLAGS=-Wall -c
CXXFLAGS=-Wall -g

# ########## ########## ########## ########## ########## ##########
# shbaek: FOR GRIB HUB
# ########## ########## ########## ########## ########## ##########
TARGET		= grib
INCLUDES	= -Iinclude	\
			  -I/usr/include/mysql

LIBS		= -L/usr/lib/arm-linux-gnueabihf \
			  -lbluetooth \
			  -lmysqlclient \
			  -lpthread \
			  -lz -lm -ldl

GRIB_OBJ 	= grib_db.o grib_util.o grib_thread.o grib_onem2m.o grib_regi.o grib_ble.o grib.o
OBJ			= ${GRIB_OBJ}

SRC 		= $(OBJ:.o=.cpp)

# ########## ########## ########## ########## ########## ##########
# shbaek: COMMAND
# ########## ########## ########## ########## ########## ##########
all: clean grib

grib:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(INCLUDES) $(LIBS)
	$(RM) $(RMFLAG) $(GRIB_OBJ)

clean:
	$(RM) $(RMFLAG) ${GRIB_OBJ} ${TARGET}

