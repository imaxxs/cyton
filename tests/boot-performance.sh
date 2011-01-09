#!/bin/bash

BOOT=$1

nohup top -b -d1 -p $BOOT > $HOST-cyton-boot-test.log &

ps -ef| grep top
