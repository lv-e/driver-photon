#!/bin/bash
set -e
echo "hi! this is particle's photon driver mantenance script for lv-engine"

## available commands

installEngine(){
    #todo: checkout on tag?
    git submodule add --force --depth=1 git@github.com:lv-e/engine.git
    cd engine
    git reset --hard 
    git fetch origin
    git pull --ff-only
}

flashSandbox(){
    #todo: ask device name
    /Users/$(eval whoami)/bin/particle flash lvndr sandbox/game.ino sandbox/scenes sandbox/lvk.h src/driver engine/lv-engine
}

compileSandbox(){
    #todo: ask device name
    /Users/$(eval whoami)/bin/particle compile photon sandbox/game.ino sandbox/scenes sandbox/lvk.h src/driver engine/lv-engine --saveTo firmware.bin
}

## what should we do?
while true; do

        echo "available options are:"
        echo "1) update game engine"
        echo "2) compile and flash sandbox game"
        echo "3) just compile and generate firmware"
        read -p "choose one: " opt; echo "--"

        case $opt in
            1) installEngine; break;;
            2) flashSandbox; break;;
            3) compileSandbox; break;;
            * ) echo "ok! bye."; exit;;
        esac
done

