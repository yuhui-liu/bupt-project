program DuplicateFunction;
procedure Foo;  { 第一次定义 Foo }
begin
end;

procedure Foo;  { 错误：重复定义 Foo }
begin
end;

begin
    Foo;      { 调用时无法确定使用哪个 Foo }
end.