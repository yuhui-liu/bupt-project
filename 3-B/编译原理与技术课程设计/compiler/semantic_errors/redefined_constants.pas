program DuplicateConst;
const
    Pi = 3.14;   { 第一次定义 Pi }
    Pi = 3.1415; { 错误：重复定义 Pi }
begin
    write(Pi); { 常量名冲突 }
end.