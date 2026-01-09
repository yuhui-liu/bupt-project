-- 创建通用的UPSERT触发器函数
CREATE OR REPLACE FUNCTION upsert_trigger_function()
RETURNS TRIGGER AS $$
BEGIN
    -- 对于不同的表，执行相应的UPSERT操作
    IF TG_TABLE_NAME = 'nation' THEN
        INSERT INTO nation (n_nationkey, n_name, n_regionkey, n_comment)
        VALUES (NEW.n_nationkey, NEW.n_name, NEW.n_regionkey, NEW.n_comment)
        ON CONFLICT (n_nationkey) 
        DO UPDATE SET 
            n_name = EXCLUDED.n_name,
            n_regionkey = EXCLUDED.n_regionkey,
            n_comment = EXCLUDED.n_comment;
        
    ELSIF TG_TABLE_NAME = 'region' THEN
        INSERT INTO region (r_regionkey, r_name, r_comment)
        VALUES (NEW.r_regionkey, NEW.r_name, NEW.r_comment)
        ON CONFLICT (r_regionkey)
        DO UPDATE SET
            r_name = EXCLUDED.r_name,
            r_comment = EXCLUDED.r_comment;
            
    ELSIF TG_TABLE_NAME = 'part' THEN
        INSERT INTO part (p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment)
        VALUES (NEW.p_partkey, NEW.p_name, NEW.p_mfgr, NEW.p_brand, NEW.p_type, NEW.p_size, NEW.p_container, NEW.p_retailprice, NEW.p_comment)
        ON CONFLICT (p_partkey)
        DO UPDATE SET
            p_name = EXCLUDED.p_name,
            p_mfgr = EXCLUDED.p_mfgr,
            p_brand = EXCLUDED.p_brand,
            p_type = EXCLUDED.p_type,
            p_size = EXCLUDED.p_size,
            p_container = EXCLUDED.p_container,
            p_retailprice = EXCLUDED.p_retailprice,
            p_comment = EXCLUDED.p_comment;
            
    ELSIF TG_TABLE_NAME = 'supplier' THEN
        INSERT INTO supplier (s_suppkey, s_name, s_address, s_nationkey, s_phone, s_acctbal, s_comment)
        VALUES (NEW.s_suppkey, NEW.s_name, NEW.s_address, NEW.s_nationkey, NEW.s_phone, NEW.s_acctbal, NEW.s_comment)
        ON CONFLICT (s_suppkey)
        DO UPDATE SET
            s_name = EXCLUDED.s_name,
            s_address = EXCLUDED.s_address,
            s_nationkey = EXCLUDED.s_nationkey,
            s_phone = EXCLUDED.s_phone,
            s_acctbal = EXCLUDED.s_acctbal,
            s_comment = EXCLUDED.s_comment;
            
    ELSIF TG_TABLE_NAME = 'partsupp' THEN
        INSERT INTO partsupp (ps_partkey, ps_suppkey, ps_availqty, ps_supplycost, ps_comment)
        VALUES (NEW.ps_partkey, NEW.ps_suppkey, NEW.ps_availqty, NEW.ps_supplycost, NEW.ps_comment)
        ON CONFLICT (ps_partkey, ps_suppkey)
        DO UPDATE SET
            ps_availqty = EXCLUDED.ps_availqty,
            ps_supplycost = EXCLUDED.ps_supplycost,
            ps_comment = EXCLUDED.ps_comment;
            
    ELSIF TG_TABLE_NAME = 'customer' THEN
        INSERT INTO customer (c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment)
        VALUES (NEW.c_custkey, NEW.c_name, NEW.c_address, NEW.c_nationkey, NEW.c_phone, NEW.c_acctbal, NEW.c_mktsegment, NEW.c_comment)
        ON CONFLICT (c_custkey)
        DO UPDATE SET
            c_name = EXCLUDED.c_name,
            c_address = EXCLUDED.c_address,
            c_nationkey = EXCLUDED.c_nationkey,
            c_phone = EXCLUDED.c_phone,
            c_acctbal = EXCLUDED.c_acctbal,
            c_mktsegment = EXCLUDED.c_mktsegment,
            c_comment = EXCLUDED.c_comment;
            
    ELSIF TG_TABLE_NAME = 'orders' THEN
        INSERT INTO orders (o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment)
        VALUES (NEW.o_orderkey, NEW.o_custkey, NEW.o_orderstatus, NEW.o_totalprice, NEW.o_orderdate, NEW.o_orderpriority, NEW.o_clerk, NEW.o_shippriority, NEW.o_comment)
        ON CONFLICT (o_orderkey)
        DO UPDATE SET
            o_custkey = EXCLUDED.o_custkey,
            o_orderstatus = EXCLUDED.o_orderstatus,
            o_totalprice = EXCLUDED.o_totalprice,
            o_orderdate = EXCLUDED.o_orderdate,
            o_orderpriority = EXCLUDED.o_orderpriority,
            o_clerk = EXCLUDED.o_clerk,
            o_shippriority = EXCLUDED.o_shippriority,
            o_comment = EXCLUDED.o_comment;
            
    ELSIF TG_TABLE_NAME = 'lineitem' THEN
        INSERT INTO lineitem (l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment)
        VALUES (NEW.l_orderkey, NEW.l_partkey, NEW.l_suppkey, NEW.l_linenumber, NEW.l_quantity, NEW.l_extendedprice, NEW.l_discount, NEW.l_tax, NEW.l_returnflag, NEW.l_linestatus, NEW.l_shipdate, NEW.l_commitdate, NEW.l_receiptdate, NEW.l_shipinstruct, NEW.l_shipmode, NEW.l_comment)
        ON CONFLICT (l_orderkey, l_linenumber)
        DO UPDATE SET
            l_partkey = EXCLUDED.l_partkey,
            l_suppkey = EXCLUDED.l_suppkey,
            l_quantity = EXCLUDED.l_quantity,
            l_extendedprice = EXCLUDED.l_extendedprice,
            l_discount = EXCLUDED.l_discount,
            l_tax = EXCLUDED.l_tax,
            l_returnflag = EXCLUDED.l_returnflag,
            l_linestatus = EXCLUDED.l_linestatus,
            l_shipdate = EXCLUDED.l_shipdate,
            l_commitdate = EXCLUDED.l_commitdate,
            l_receiptdate = EXCLUDED.l_receiptdate,
            l_shipinstruct = EXCLUDED.l_shipinstruct,
            l_shipmode = EXCLUDED.l_shipmode,
            l_comment = EXCLUDED.l_comment;
    END IF;
    
    RETURN NULL; -- 返回NULL表示不执行原始的INSERT操作
END;
$$ LANGUAGE plpgsql;

-- 为Nation表创建触发器
CREATE TRIGGER nation_upsert_trigger
    BEFORE INSERT ON nation
    FOR EACH ROW
    EXECUTE FUNCTION upsert_trigger_function();

-- 为Region表创建触发器
CREATE TRIGGER region_upsert_trigger
    BEFORE INSERT ON region
    FOR EACH ROW
    EXECUTE FUNCTION upsert_trigger_function();

-- 为Part表创建触发器
CREATE TRIGGER part_upsert_trigger
    BEFORE INSERT ON part
    FOR EACH ROW
    EXECUTE FUNCTION upsert_trigger_function();

-- 为Supplier表创建触发器
CREATE TRIGGER supplier_upsert_trigger
    BEFORE INSERT ON supplier
    FOR EACH ROW
    EXECUTE FUNCTION upsert_trigger_function();

-- 为PartSupp表创建触发器
CREATE TRIGGER partsupp_upsert_trigger
    BEFORE INSERT ON partsupp
    FOR EACH ROW
    EXECUTE FUNCTION upsert_trigger_function();

-- 为Customer表创建触发器
CREATE TRIGGER customer_upsert_trigger
    BEFORE INSERT ON customer
    FOR EACH ROW
    EXECUTE FUNCTION upsert_trigger_function();

-- 为Orders表创建触发器
CREATE TRIGGER orders_upsert_trigger
    BEFORE INSERT ON orders
    FOR EACH ROW
    EXECUTE FUNCTION upsert_trigger_function();

-- 为LineItem表创建触发器
CREATE TRIGGER lineitem_upsert_trigger
    BEFORE INSERT ON lineitem
    FOR EACH ROW
    EXECUTE FUNCTION upsert_trigger_function();