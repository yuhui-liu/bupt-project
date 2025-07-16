-- 查询数据库重要配置参数，包括内存配置、连接数、检查点设置等关键性能参数
SELECT 
    name,
    setting,
    unit,
    context,
    short_desc
FROM pg_settings 
WHERE name IN (
    'shared_buffers',
    'effective_cache_size',
    'work_mem',
    'maintenance_work_mem',
    'max_connections',
    'random_page_cost',
    'seq_page_cost',
    'checkpoint_completion_target',
    'wal_buffers',
    'default_statistics_target'
)
ORDER BY name