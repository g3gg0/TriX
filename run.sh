export PATH=`pwd`/tools/arm:$PATH
cd projects/fsig_upload
rm *.o >/dev/null 2>&1
make >err.log 2>&1
