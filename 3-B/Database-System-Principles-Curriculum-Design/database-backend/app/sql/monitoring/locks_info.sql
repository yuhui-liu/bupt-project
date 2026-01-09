-- 查询数据库锁信息，显示未授权的锁类型、模式、用户、查询开始时间和查询片段
SELECT 
    pl.locktype,
    pl.mode,
    pl.granted,
    pa.usename,
    pa.query_start,
    pa.state,
    left(pa.query, 100) as query_snippet
FROM pg_locks pl
LEFT JOIN pg_stat_activity pa ON pl.pid = pa.pid
WHERE NOT pl.granted
ORDER BY pa.query_start
LIMIT 10
