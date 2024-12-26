# Makefile

# 目标文件
TARGET = cloud

# 源文件
SRCS = main.cc

# 编译选项和链接库
CXXFLAGS = -std=c++17 -I../include/ -lpthread -lstdc++fs -ljsoncpp -L../libs/ -lbundle

# 生成目标
$(TARGET):
	g++ -o $(TARGET) $(SRCS) $(CXXFLAGS)

# 清理目标
clean:
	rm -f $(TARGET)
