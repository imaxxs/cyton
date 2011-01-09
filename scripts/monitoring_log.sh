#!/bin/bash

nohup top -b -d1 -p $1 >> monitoring.log &
