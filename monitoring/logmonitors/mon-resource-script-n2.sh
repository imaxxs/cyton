#!/bin/bash
set +C
~/./cyton/monitoring/dstat -tcm --output ~/cyton/monitoring/dstat-n2.log  1> /dev/null 2> /dev/null &
