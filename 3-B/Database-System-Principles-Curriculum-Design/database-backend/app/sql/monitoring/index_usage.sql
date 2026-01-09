-- 查询 public schema 下索引的使用情况及大小，按读取次数降序排列，显示前 20 条
SELECT 
    schemaname,
    relname,
    indexrelname,
    idx_tup_read,
    idx_tup_fetch,
    pg_size_pretty(pg_relation_size(indexrelid)) as index_size,
    CASE 
        WHEN idx_tup_read = 0 THEN 'Unused'
        ELSE 'Used'
END as usage_status
FROM pg_stat_user_indexes
WHERE schemaname = 'public'
ORDER BY idx_tup_read DESC
LIMIT 20