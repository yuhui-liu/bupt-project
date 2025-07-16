-- 查询访问最频繁的表，显示表的扫描次数、修改次数、清理和分析时间等统计信息
SELECT 
    schemaname,
    relname,
    seq_scan + idx_scan as total_scans,
    seq_scan,
    idx_scan,
    n_tup_ins + n_tup_upd + n_tup_del as total_modifications,
    last_vacuum,
    last_autovacuum,
    last_analyze,
    last_autoanalyze
FROM pg_stat_user_tables
WHERE schemaname = 'public'
ORDER BY total_scans DESC
LIMIT 15