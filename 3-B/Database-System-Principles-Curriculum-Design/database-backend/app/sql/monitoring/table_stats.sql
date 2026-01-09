-- 查询表统计信息，显示表大小、索引大小、插入更新删除次数、活跃和死亡元组数量
SELECT 
    t.schemaname,  -- 显式限定 schemaname 来源于 pg_tables
    t.tablename,
    pg_size_pretty(pg_total_relation_size(t.schemaname || '.' || t.tablename)) AS total_size,
    pg_size_pretty(pg_relation_size(t.schemaname || '.' || t.tablename)) AS table_size,
    pg_size_pretty(pg_total_relation_size(t.schemaname || '.' || t.tablename) - 
                  pg_relation_size(t.schemaname || '.' || t.tablename)) AS index_size,
    s.n_tup_ins AS inserts,
    s.n_tup_upd AS updates,
    s.n_tup_del AS deletes,
    s.n_live_tup AS live_tuples,
    s.n_dead_tup AS dead_tuples
FROM pg_tables t
LEFT JOIN pg_stat_user_tables s ON t.tablename = s.relname
WHERE t.schemaname = 'public'  -- 显式限定 schemaname 来源于 pg_tables
ORDER BY pg_total_relation_size(t.schemaname || '.' || t.tablename) DESC
LIMIT 15;
