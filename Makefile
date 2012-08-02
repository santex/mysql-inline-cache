#
#
gcc -fPIC -DDBUG_OFF -shared -o santex_inline_cache.so mysql-inline-cache.c -g -Wall -O2 -lmemcache `mysql_config --cflags`

sudo cp ./santex_inline_cache.so /usr/local/lib/santex_inline_cache.so
