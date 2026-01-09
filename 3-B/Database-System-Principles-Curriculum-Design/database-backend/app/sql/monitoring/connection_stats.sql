-- 查询数据库连接状态统计，按连接状态分组显示连接数量和百分比
SELECT 
    state,
    count(*) as count,
    count(*) * 100.0 / sum(count(*)) OVER() as percentage
FROM pg_stat_activity 
GROUP BY state
ORDER BY count DESC