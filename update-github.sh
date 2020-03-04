#!/bin/bash

GITHUB_URL='https://github.com/PlatONnetwork/proof_of_custody'

# 删除gitlab服务器
git remote | grep github | xargs -I @ git remote remove @

# 更新master分支
git fetch --all
git checkout master
git branch --set-upstream-to=origin/master master
git pull

# 删除所有本地分支（保留master），拉取创建所有远程分支
git branch  | grep -v HEAD | grep -v master | xargs -I @ git branch -D @
git branch -r --list "origin/*"  | grep -v HEAD | grep -v master | xargs -I @ git checkout -t @

# 添加gitlab服务器
git remote add github $GITHUB_URL

git push -u github --all
git push -u github --tags

# 删除gitlab服务器
git remote remove github
