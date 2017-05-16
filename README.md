Change ia32 compiler prefix on makedefs.

make veryclean
make APPLICATION=producer_consumer run
make APPLICATION=philosophers_dinner run

git update-index --assume-unchanged lib/README.md
git update-index --assume-unchanged makedefs
