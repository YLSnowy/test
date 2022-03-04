if [ $1 = 'test' ]
then
    echo "roms test"
    time SGX=1 ./pal_loader ./curl < data/test/input/ocean_benchmark0.in.x
elif [ $1 = 'train' ]
then
    echo "roms train"
    time SGX=1 ./pal_loader ./curl < data/train/input/ocean_benchmark1.in.x
elif [ $1 = 'ref' ]
then
    echo "roms ref"
    time SGX=1 ./pal_loader ./curl < data/refspeed/input/ocean_benchmark3.in.x
else
    echo 'error'
fi

