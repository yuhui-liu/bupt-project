program ArrayOutOfBounds1;
var
    arr: array[1..5] of integer;  { 索引范围 1~5 }
begin
    arr[0] := 10;   { 错误：索引 0 < 下限 1 }
    arr[6] := 20;   { 错误：索引 6 > 上限 5 }
end.