program DuplicateParam;
var
  x:integer;
  y:char;
procedure Test(a: integer; a: char);  { 错误：参数名 a 重复 }
begin
    write(a);  { 无法确定 a 是 integer 还是 real }
end;

begin
    Test(x, y);
end.