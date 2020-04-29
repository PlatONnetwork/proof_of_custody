#!/bin/bash

# netstat -anlp|grep 200

ps -ef | grep main.x | grep -v grep | awk '{print $2}' | xargs kill -9
ps -ef | grep mainfork | grep -v grep | awk '{print $2}' | xargs kill -9
ps -ef | grep runfork | grep -v grep | awk '{print $2}' | xargs kill -9
