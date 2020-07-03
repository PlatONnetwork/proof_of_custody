#!/bin/bash

# netstat -anlp|grep 200

ps -ef | grep -E "main.x|mainfork|runfork|src/server" | grep -v grep | awk '{print $2}' | xargs kill -9 >/dev/nul 2>&1
