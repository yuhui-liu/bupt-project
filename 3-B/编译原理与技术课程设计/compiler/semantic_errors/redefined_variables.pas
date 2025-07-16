program DuplicateVar;
var
    x: integer;  { 第一次定义 x }
    x: real;     { 错误：重复定义 x }
begin
    x := 10;     { 这里 x 的类型冲突 }
end.