-- 查询表的清理统计信息，显示死亡元组数量、死亡元组比例、最后清理时间和清理次数
SELECT 
    schemaname,
    relname AS tablename,  -- 使用 relname 来表示表名
    n_dead_tup,
    n_live_tup,
    CASE 
        WHEN n_live_tup > 0 
        THEN round(100.0 * n_dead_tup / (n_live_tup + n_dead_tup), 2)
        ELSE 0 
    END AS dead_tuple_ratio,
    last_vacuum,
    last_autovacuum,
    vacuum_count,
    autovacuum_count
FROM pg_stat_user_tables
WHERE schemaname = 'public'
    AND (n_dead_tup > 1000 OR (n_live_tup > 0 AND n_dead_tup::float / n_live_tup > 0.1))
ORDER BY dead_tuple_ratio DESC
LIMIT 10;
