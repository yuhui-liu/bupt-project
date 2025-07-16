program ScopeViolation1;
procedure P;
var
    x: integer;  { x 是 P 的局部变量 }
begin
    x := 10;
end;

begin
    P();
    x := 5;  { 错误：x 只在 P 内部可见，主程序不能访问 }
end.