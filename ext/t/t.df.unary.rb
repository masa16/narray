require "./narray.so"

a = NArray::DFloat.cast([1,2,3])
p a

puts "-----------------------------------"
b = -a
puts "==================================="
p b
b.debug_info
