-- 查询数据库缓存命中率统计，包括表和索引的缓存命中率百分比
SELECT 
    'Tables' as type,
    round(100.0 * sum(heap_blks_hit) / (sum(heap_blks_hit) + sum(heap_blks_read)), 2) as hit_ratio
FROM pg_statio_user_tables
UNION ALL
SELECT 
    'Indexes' as type,
    round(100.0 * sum(idx_blks_hit) / (sum(idx_blks_hit) + sum(idx_blks_read)), 2) as hit_ratio
FROM pg_statio_user_indexes