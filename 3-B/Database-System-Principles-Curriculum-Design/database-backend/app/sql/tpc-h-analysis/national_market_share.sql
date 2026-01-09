-- 给定国家，地区，周期和零件 key，
-- 查找周期内的每年该国在该地区收入所占的份额，
-- 结果返回年份和周期比，按照年份升序。
SELECT
    o_year,
    SUM(CASE
            WHEN nation = 'BRAZIL' THEN volume
            ELSE 0
        END) / SUM(volume) AS mkt_share
FROM (
    SELECT
        EXTRACT(YEAR FROM o_orderdate) AS o_year,
        l_extendedprice * (1 - l_discount) AS volume,
        n2.n_name AS nation
    FROM
        part,
        supplier,
        lineitem,
        orders,
        customer,
        nation n1,
        nation n2,
        region
    WHERE
        p_partkey = l_partkey
        AND s_suppkey = l_suppkey
        AND l_orderkey = o_orderkey
        AND o_custkey = c_custkey
        AND c_nationkey = n1.n_nationkey
        AND n1.n_regionkey = r_regionkey
        AND r_name = 'AMERICA'
        AND s_nationkey = n2.n_nationkey
        AND o_orderdate BETWEEN DATE '1970-01-01' AND DATE '2020-01-01'
        AND p_type = 'PART_TYPE'
) AS all_nations
GROUP BY
    o_year
ORDER BY
    o_year;
