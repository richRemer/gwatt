app = gwatt
src = src
bin = bin
build = build
deps = gtk+-3.0

objects = $(patsubst $(src)/%.c, $(build)/%.o, $(shell find $(src) -type f -name "*".c))

cc = gcc
ld = gcc

cflags = $(shell pkg-config --cflags $(deps))
ldflags = $(shell pkg-config --libs $(deps))

default: build

build: $(bin)/$(app)

clean:
	rm -fr $(build)/* $(bin)/*

$(bin)/$(app): $(objects)
	$(ld) $^ -o $@ $(ldflags)

$(build)/%.o: $(src)/%.c
	$(cc) $(cflags) -c $< -o $@

.PHONY: default build clean
