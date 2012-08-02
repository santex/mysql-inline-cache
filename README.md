mysql-inline-cache
==================

user defined function in c which enables direct access to memcache buckets from a sql query 


```
select faz from foo where fiz in (select memcache_get("","fun"));

```