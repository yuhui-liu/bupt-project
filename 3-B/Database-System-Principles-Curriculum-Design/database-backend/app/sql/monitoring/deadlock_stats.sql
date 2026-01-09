-- 查询当前数据库的死锁统计信息，包括死锁次数、临时文件数量和临时文件大小
SELECT 
    datname,
    deadlocks,
    temp_files,
    temp_bytes
FROM pg_stat_database 
WHERE datname = current_database()