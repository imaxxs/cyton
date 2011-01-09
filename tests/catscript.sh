#!/bin/bash

cat samoa1-cyton-coord-nodeagent-test-1-5sec-110.log | grep nodeagent | cut -d" " -f13,15,18,20
