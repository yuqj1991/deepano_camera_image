#Author         : yuqj
#Email          : yuqj@deepano.com
#Last_Date      : 2018.07.11

CC          := g++
INCLUDE_DIR := -I ./include \
               -I /usr/local/include \
               -I /usr/local/include/libusb-1.0
LIB_DIR     := ./so
SRC_DIR     := ./src/
BIN_DIR     := ./bin
OBJ_DIR     := ./obj

SOURCES     := $(wildcard $(SRC_DIR)*.c)
SOURCES     += $(wildcard $(SRC_DIR)*.cpp)
ALL_OBJS    := $(patsubst $(SRC_DIR), $(OBJ_DIR)/%.o, $(SOURCES))

DEPS        := ${SOURCES:.c=.d} #.d文件是包含文件的依赖关系不必手动书写若干目标文件的依赖关系，由编译器自动生成，不管是源文件还是头文件有更新，目标文件都会重新编译
DEPS        += ${SOURCES:.cpp=.d}
TARGET      := $(BIN_DIR)/deepano_image



CFLAGS = -g -Wall -std=c++11 
LFLAGS = -L ${LIB_DIR} `pkg-config opencv --libs` -lpthread -lusb-1.0 -ldp_api_mdk

.PHONY:all clean
all: ${TARGET}

${TARGET}:${ALL_OBJS}
	if [ ! -d $(BIN_DIR) ]; then mkdir -p $(BIN_DIR); fi;
	${CC} ${CFLAGS} ${INCLUDE_DIR} -o ${TARGET} ${ALL_OBJS} ${LFLAGS}
	
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJ_DIR)/%.o:$(SOURCES)
	if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi;
	${CC} ${CFLAGS} ${INCLUDE_DIR} -c $< -o $@

clean:
	rm -rf *.o *.d ${BIN_DIR} *~ ${OBJ_DIR} *~
