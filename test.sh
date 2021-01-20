#!/bin/sh

result="$(./brainfuck hello.bf)"
[ "$result" = "Hello World!" ]
