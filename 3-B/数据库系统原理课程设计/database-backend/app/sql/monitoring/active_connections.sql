-- 查询当前活跃的数据库连接，显示连接详情、用户、应用名称、客户端地址、状态和执行时间
SELECT 
    pid,
    usename,
    application_name,
    client_addr,
    state,
    backend_start,
    query_start,
    state_change,
    CASE 
        WHEN state = 'active' THEN now() - query_start 
        ELSE now() - state_change 
    END as duration,
    left(query, 100) as query_snippet
FROM pg_stat_activity 
WHERE state != 'idle'
ORDER BY backend_start DESC
LIMIT 20