program ConstAssignmentError1;
const
    Pi = 3.1415926;  { 声明常量 Pi }
begin
    Pi := 3.14;  { 错误：尝试修改常量 }
end.