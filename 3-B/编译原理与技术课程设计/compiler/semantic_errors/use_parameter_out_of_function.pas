program ScopeViolation4;
procedure Q(a: integer);
begin
    a := a + 1;
end;

begin
    Q(5);
    write(a);  { 错误：a 是 Q 的参数，主程序不能访问 }
end.