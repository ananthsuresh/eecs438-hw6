#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=2
#SBATCH --output=hw6.txt 

cp forkedheat threadedheat serialheat $PFSDIR/.
cd $PFSDIR
echo Serial Heat on Array Size of 1000
./serialheat 1000 1000
echo Serial Heat on Array Size of 1500
./serialheat 1500 1500
echo Serial Heat on Array Size of 2000
./serialheat 2000 2000
echo Serial Heat on Array Size of 2500
./serialheat 2500 2500
echo Serial Heat on Array Size of 3000
./serialheat 3000 3000

echo Threaded Heat on Array Size of 1000
./threadedheat 1000 1000
echo Threaded Heat on Array Size of 1500
./threadedheat 1500 1500
echo Threaded Heat on Array Size of 2000
./threadedheat 2000 2000
echo Threaded Heat on Array Size of 2500
./threadedheat 2500 2500
echo Threaded Heat on Array Size of 3000
./threadedheat 3000 3000

echo Forked Heat on Array Size of 1000
./forkedheat 1000 1000
echo Forked Heat on Array Size of 1500
./forkedheat 1500 1500
echo Forked Heat on Array Size of 2000
./forkedheat 2000 2000
echo Forked Heat on Array Size of 2500
./forkedheat 2500 2500
echo Forked Heat on Array Size of 3000
./forkedheat 3000 3000

