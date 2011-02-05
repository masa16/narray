require "complex"
require "./narray.so"

s = "Num::DFloat.cast([1,[1,[2,4]],3,[[3,4],5],2])"
puts s
p a = eval(s)
a.debug_info
a.debug_print
b = a.to_a
p b
#puts "b = #{b.inspect} "


s = "Num::DFloat.cast(1.23)"
puts s
p a = eval(s)
a.debug_info
a.debug_print
b = a.to_a
p b

s = "Num::DFloat.cast([])"
puts s
a = Num::DFloat.cast([])
#exit
#p a
#exit
a.debug_info
a.debug_print
b = a.to_a
p b

