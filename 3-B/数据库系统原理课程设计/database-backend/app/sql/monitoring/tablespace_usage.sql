-- 查询数据库表空间使用情况，包括表空间名称、位置和大小
SELECT 
    spcname as tablespace_name,
    pg_tablespace_location(oid) as location,
    pg_size_pretty(pg_tablespace_size(spcname)) as size
FROM pg_tablespace