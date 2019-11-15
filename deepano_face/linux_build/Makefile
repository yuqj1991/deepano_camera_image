#Author         :yuqj
#Email          :yuqj@deepano.com
#Last_update    :2018.07.11

CC              := g++
SRC_INCLUDE     := -I ../include \
                   -I  /usr/local/include \
                   -I /usr/local/include/libusb-1.0
SRC_DIR         := ../src
LIB_DIR         := ../so 
#RAWDATAOBJECTFILES :=VALID_PEOPLE.o
OBJS            := ${SRC_DIR}/fp16.o \
                   ${SRC_DIR}/share_utils.o\
                   ${SRC_DIR}/post_image.o \
                   ${SRC_DIR}/video_result_callback.o \
                   ${SRC_DIR}/Fp16Convert.o   \
                   ${SRC_DIR}/model_sample.o  \
                   ${SRC_DIR}/test.o	
TARGET          := ../deepano_face
SRC_DIR         := ${SRC_DIR}/fp16.c ${SRC_DIR}/share_utils.c ${SRC_DIR}/post_image.cpp ${SRC_DIR}/video_result_callback.c ${SRC_DIR}/Fp16Convert.c ${SRC_DIR}/model_sample.c ${SRC_DIR}/test.cpp
DEPS := ${SRC_DIR:.c=.d}
DEPS += ${SRC_DIR:.cpp=.d}

CFLAGS = -g -Wall -std=c++11 -fPIC
LFLAGS = -L ${LIB_DIR} -L /usr/local/lib/ `pkg-config opencv --libs` -lpthread -lusb-1.0 -ldp_api

.PHONY:all clean
all: ${TARGET}

${TARGET}:${OBJS}
	        ${CC} ${CFLAGS} ${SRC_INCLUDE} -o ${TARGET} ${OBJS} ${LFLAGS} 

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
	        
%.o:%.c
	${CC} ${CFLAGS} ${SRC_INCLUDE} -c $< -o $@

%.o:%.cpp
	${CC} ${CFLAGS} ${SRC_INCLUDE} -c $< -o $@
#VALID_PEOPLE.o: ../yuqj
#	@mkdir -p $(dir $@)
#	$(OBJS) -I binary --rename-section .data=.ddr.data \
#		--redefine-sym  _binary_$(subst /,_,$(subst .,_,$<))_start=VALID_PEOPLE \
#		-O elf32-sparc -B sparc $< $@

clean:
	rm -rf *.o *.d ${TARGET} *~ ../src/*.o  
