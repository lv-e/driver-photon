#!/bin/bash
set -e
echo "hi! this is particle's photon driver mantenance script for lv-engine"

## available commands

installEngine(){
    git submodule add --force --depth=1 git@github.com:lv-e/engine.git
    cd engine
    git reset --hard 
    git fetch origin
    git pull --ff-only
}

updateEngine(){

    local tmp=$(mktemp -d)
    local current=$(pwd)

    echo "will now download engines latest version at $tmp"
    cd $tmp
    git clone --no-checkout -q git@github.com:lv-e/engine.git
    cd engine

    git config core.sparseCheckout true
    echo "lv-engine/*"> .git/info/sparse-checkout
    git checkout master

    echo "done! time to move things around. back to $current"
    cd $current/src
    rm -rf lv-engine
    mv $tmp/engine/lv-engine lv-engine

    rm -rf $tmp
}

## what should we do?
while true; do

        echo "available options are:"
        echo "1) update game engine"
        read -p "choose one: " opt; echo "--"

        case $opt in
            1) installEngine; break;;
            2) test; break;;
            * ) echo "ok! bye."; exit;;
        esac
done

