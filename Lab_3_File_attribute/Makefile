TARGET=fig4_22
test:$(TARGET)
	bash createFile.sh
	./$(TARGET) `cat /tmp/${USER}/esllab20140411.sp`

$(TARGET): fig4_22.c error.c path_alloc.c
	gcc $^ -o $@

clean:
	rm -f fig4_22

#-------My Try--------------------------------
# CXX = gcc
# TARGET = fig4_22
# SRC = $(wildcard *.c)
# OBJ = $(patsubst %.c, %.o, $(SRC))

# CXXFLAGS = -c -Wall

# test : $(TARGET)
# 	bash createFile.sh
# 	./$(TARGET) `cat /tmp/${USER}/esllab20140411.sp`

# $(TARGET) : $(OBJ)
# 	$(CXX) $^ -o $@

# %.o : %.c
# 	$(CXX) $(CXXFLAGS) $< -o $@

# .PHONY : clean
# clean:
# 	rm -f fig4_22

