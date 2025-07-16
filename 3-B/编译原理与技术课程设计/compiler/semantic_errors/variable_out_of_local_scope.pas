program ScopeViolation;
procedure Foo;
var
    localVar: integer;
begin
    localVar := 10;  
end;

begin
    Foo();
    write(localVar);  { 错误：localVar 是 Foo 的局部变量，主程序无法访问 }
end.