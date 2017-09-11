#!/bin/sh

./luac -s -o d_skill.luo d_skill.lua
./luabt -c -o d_skill.dat d_skill.lua
./ltest d_skill.dat d_skill.luo
