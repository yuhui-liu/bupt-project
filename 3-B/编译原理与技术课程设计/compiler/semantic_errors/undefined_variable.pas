program UndefinedVar;
var
    x: integer;  { 只声明了 x，未声明 y 和 z }
begin
    x := y + z;  { 错误：y 和 z 未定义 }
end.