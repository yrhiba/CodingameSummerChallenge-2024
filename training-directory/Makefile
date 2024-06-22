
SRCS = $(wildcard bots/*.cpp)
OBJS= $(SRCS:.cpp=.exe)

# rm -rf bots/*.exe
all: $(OBJS)
	rm -rf psyleague.db psyleage.games psyleague.msg
	cp temp.msg psyleague.msg
	psyleague run

%.exe: %.cpp
	g++ -std=c++17 $< -o $@

