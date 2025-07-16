-- 给定时间和供应商所在的国家和客户所在的国家，
-- 查找出在该段时间内两个国家出货的总收入，
-- 返回供应国家名，客户国家名，年份，和出货的收入，
-- 结果按照供应国家名，客户国家名，年份升序。
SELECT
    supp_nation,
    cust_nation,
    l_year,
    SUM(volume) AS revenue
FROM (
    SELECT
        n1.n_name AS supp_nation,
        n2.n_name AS cust_nation,
        EXTRACT(YEAR FROM l_shipdate) AS l_year,
        l_extendedprice * (1 - l_discount) AS volume
    FROM
        supplier
        JOIN lineitem ON s_suppkey = l_suppkey
        JOIN orders ON o_orderkey = l_orderkey
        JOIN customer ON c_custkey = o_custkey
        JOIN nation n1 ON s_nationkey = n1.n_nationkey
        JOIN nation n2 ON c_nationkey = n2.n_nationkey
    WHERE
        n1.n_name = 'COUNTRYA' AND n2.n_name = 'COUNTRYB'
        AND l_shipdate BETWEEN DATE '1970-01-01' AND DATE '2020-01-01'
) AS shipping
GROUP BY
    supp_nation,
    cust_nation,
    l_year
ORDER BY
    supp_nation,
    cust_nation,
    l_year;
