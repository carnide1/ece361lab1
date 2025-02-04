CC=gcc
BASE_DIR=/homes/c/carnide1/ece361/Labs/Lab_1
SERVER_DIR=$(BASE_DIR)/serverFolder
DELIVER_DIR=$(BASE_DIR)/deliverFolder

all: $(SERVER_DIR)/server $(DELIVER_DIR)/deliver

$(SERVER_DIR)/server: $(SERVER_DIR)/server.o
	$(CC) -o $@ $(SERVER_DIR)/server.o

$(DELIVER_DIR)/deliver: $(DELIVER_DIR)/deliver.o
	$(CC) -o $@ $(DELIVER_DIR)/deliver.o

$(SERVER_DIR)/server.o: $(SERVER_DIR)/server.c
	$(CC) -c -o $@ $(SERVER_DIR)/server.c

$(DELIVER_DIR)/deliver.o: $(DELIVER_DIR)/deliver.c
	$(CC) -c -o $@ $(DELIVER_DIR)/deliver.c

clean:
	rm -f $(SERVER_DIR)/*.o $(SERVER_DIR)/server $(DELIVER_DIR)/*.o $(DELIVER_DIR)/deliver
