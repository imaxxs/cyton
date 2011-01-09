#!/bin/bash

valgrind --tool=memcheck ./$1 $2
