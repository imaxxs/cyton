#!/bin/bash

NAGENT=$1

nohup top -b -d1 -p $NAGENT > $HOST-cyton-coord-nodeagent-test.log &

ps -ef| grep top
