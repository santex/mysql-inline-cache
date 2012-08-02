SET @db = "`udf_cache`.";
SET @table = "";

DROP FUNCTION IF EXISTS memcache_get;

DROP FUNCTION IF EXISTS memcache_set;

DROP FUNCTION IF EXISTS memcache_delete;

DROP FUNCTION IF EXISTS memcache_exists;

CREATE FUNCTION memcache_get RETURNS STRING SONAME 'santex_inline_cache.so';

CREATE FUNCTION memcache_set RETURNS INTEGER SONAME 'santex_inline_cache.so';

CREATE FUNCTION memcache_delete RETURNS INTEGER SONAME 'santex_inline_cache.so';

CREATE FUNCTION memcache_exists RETURNS INTEGER SONAME 'santex_inline_cache.so';

SELECT memcache_get("","1");
SELECT memcache_set("","2");
SELECT memcache_delete("","3");
//
