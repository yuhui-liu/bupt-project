-- 订单优先级查询
-- 用以查询指定日期范围内，实际送达日期晚于预计到达日期的各个优先级订单的订单数量，
-- 并根据订单优先级升序排序。
SELECT
    o_orderpriority,
    COUNT(*) AS order_count
FROM
    orders
WHERE
    o_orderdate >= DATE '1970-01-01'
    AND o_orderdate < DATE '2020-01-01'
    AND EXISTS (
        SELECT
            1
        FROM
            lineitem
        WHERE
            l_orderkey = o_orderkey
            AND l_commitdate < l_receiptdate
    )
GROUP BY
    o_orderpriority
ORDER BY
    o_orderpriority;