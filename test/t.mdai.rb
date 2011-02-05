require "complex"
require "./narray.so"


#Num::NArray.mdai([[[1,2,2],[3,4]]])
Num::NArray.mdai([])
Num::NArray.mdai([[[]],2])
Num::NArray.mdai(1)
exit

def px (a)
  puts "Num::DFloat.cast(#{a})"
  p b=Num::DFloat.cast(eval(a))
  b.debug_info
  b.debug_print
end
px "[[[1,2,2],[3,4]]]"
px "[[1..3,5..7],[4..7]]"


a = Num::DFloat.new([3,2,5])

p Num::NArray.array_shape([[[true,false],[true,false]]])
p Num::NArray.array_shape([[[1,2,2],[3,4]]])
p Num::NArray.array_shape([[[1,2**32],[3,4]]])
p Num::NArray.array_shape([[[1.5,2.5],[3,4]]])
p Num::NArray.array_shape([[[1,1.im],[3,4]]])
p Num::NArray.array_shape([[[1,"hage"],[3,4]]])
p Num::NArray.array_shape([[a,a]])
#p Num::NArray.array_shape([[1..3,5..7],[4..7]])
#exit

Num::NArray.mdai([[[true,false],[true,false]]])
Num::NArray.mdai([[[1,2,2],[3,4]]])
Num::NArray.mdai([[[1,2**32],[3,4]]])
Num::NArray.mdai([[[1.5,2.5],[3,4]]])
Num::NArray.mdai([[[1,1.im],[3,4]]])
Num::NArray.mdai([[[1,"hage"],[3,4]]])
Num::NArray.mdai([[a,a]])
exit

GC.disable
a = Num::DFloat.new([3,2,5])
a.debug_info
a.fill(2.5)
b = a.to_a
puts "b = #{b.inspect} "
exit

