#!/bin/bash
set -e
echo "hi! this is particle's photon driver mantenance script for lv-engine"

## available commands

buildDockerImage(){
    docker login
    docker build -t lvedock/lve_particle_photon .
    docker push lvedock/lve_particle_photon
}

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
    /Users/$(eval whoami)/bin/particle flash lvndr-416 game/game.ino sandbox/ src/ engine/
}

compileSandbox(){
    #todo: ask device name
    /Users/$(eval whoami)/bin/particle compile photon sandbox/game.ino sandbox/lv-game sandbox/lvk.h src/driver engine/lv-engine --saveTo firmware.bin
}

## what should we do?
while true; do

        echo "available options are:"
        echo "1) update game engine"
        echo "2) compile and flash sandbox game"
        echo "3) just compile and generate firmware"
        echo "4) build and publish docker image"
        read -p "choose one: " opt; echo "--"

        case $opt in
            1) installEngine; break;;
            2) flashSandbox; break;;
            3) compileSandbox; break;;
            4) buildDockerImage; break;;
            * ) echo "ok! bye."; exit;;
        esac
done

