-- 查询长时间运行的查询（超过1分钟），显示进程ID、用户、应用名称、客户端地址、运行时长和查询语句
SELECT 
    pid,
    usename,
    application_name,
    client_addr,
    now() - query_start as duration,
    state,
    left(query, 200) as query
FROM pg_stat_activity 
WHERE state = 'active' 
    AND now() - query_start > interval '1 minute'
ORDER BY duration DESC
LIMIT 10