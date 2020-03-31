#!/bin/bash

# netstat -anlp|grep 200

ps -ef | grep main.x | grep -v grep | awk '{print $2}' | xargs kill -9
