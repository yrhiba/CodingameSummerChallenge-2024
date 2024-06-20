# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yrhiba <yrhiba@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/21 16:59:00 by yrhiba            #+#    #+#              #
#    Updated: 2024/06/20 20:49:17 by yrhiba           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SCRIPT = makeCodingameFile

FILES = build.cpp

buildClean:
	make run

$(SCRIPT) : $(FILES)
	@make build

build :
	g++ -std=c++17 $(FILES) -o $(SCRIPT)

run : $(SCRIPT)
	@mkdir -p prod
	./$(SCRIPT)
	cat ./prod/code.cpp | xsel --clipboard --input

clean:
	@rm -rf .vscode .DS_Store
	@rm -rf prod $(SCRIPT)

re : clean build

all : clean build run

.PHONY : run build clean re
