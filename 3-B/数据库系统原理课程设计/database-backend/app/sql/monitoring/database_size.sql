-- 查询所有非模板数据库的大小，显示数据库名称、可读格式大小和字节大小，按大小排序
SELECT 
    datname as database_name,
    pg_size_pretty(pg_database_size(datname)) as size,
    pg_database_size(datname) as size_bytes
FROM pg_database 
WHERE datistemplate = false
ORDER BY pg_database_size(datname) DESC