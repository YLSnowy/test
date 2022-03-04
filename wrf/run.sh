if [ $1 = 'test' ]
then
    cp data/test/input/namelist.input .
    echo "wrf test"
    time SGX=1 ./pal_loader ./curl
elif [ $1 = 'train' ]
then
    cp data/train/input/namelist.input .
    echo "wrf train"
    time SGX=1 ./pal_loader ./curl
elif [ $1 = 'ref' ]
then
    cp data/refspeed/input/namelist.input .
    echo "wrf ref"
    time SGX=1 ./pal_loader ./curl
else
    echo 'error'
fi

