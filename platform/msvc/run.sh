export PATH=`pwd`/tools/arm:$PATH
cd projects/nand_dump
rm *.o >/dev/null 2>&1
make >err.log 2>&1
