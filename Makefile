CC = gcc
CFLAGS = -g -Wall
CPPFLAGS = -Iinclude
LDLIBS = -pthread

SRC_DIR = src
BUILD_DIR = build

COMMON_OBJ = $(BUILD_DIR)/comptine_utils.o

.PHONY: all clean

all: wcp_clt wcp_srv

wcp_clt: $(BUILD_DIR)/wcp_clt.o $(COMMON_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

wcp_srv: $(BUILD_DIR)/wcp_srv.o $(COMMON_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c include/comptine_utils.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) wcp_clt wcp_srv
