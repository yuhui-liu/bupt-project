-- 查询当前数据库的总体统计信息，包括数据库大小、总连接数、活跃连接数和后端进程数
SELECT 
    pg_size_pretty(pg_database_size(current_database())) as database_size,
    (SELECT count(*) FROM pg_stat_activity) as total_connections,
    (SELECT count(*) FROM pg_stat_activity WHERE state = 'active') as active_connections,
    (SELECT sum(numbackends) FROM pg_stat_database) as backend_processes