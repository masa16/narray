require "./narray.so"

NArray::DFloat.new([10,20]).debug_info

a = NArray::DFloat.new([4,3])
a.fill(123)
p a

b = NArray::DFloat.new([4,3])
b.fill(456)
p b.__id__
p b

c = a+b
puts "add done"

p a.__id__
p a
p b.__id__
p b
p c.__id__
p c


a = NArray::DFloat.new([4,1])
a.fill(123)
p a

b = NArray::DFloat.new([1,3])
b.fill(456)
p b.__id__
p b

c = a+b
puts "add done"

p a.__id__
p a
p b.__id__
p b
p c.__id__
p c
exit

p NArray::DFloat.new([10,20]) + NArray::DFloat.new([10,20])

p NArray::DFloat.new([10,20]) + 1.0

p NArray::DFloat.new([10,20]) + 1

p NArray::DFloat.cast(1.0)

p NArray::DFloat(1.0)

p NArray::DFloat(1.0).inspect

p NArray::DFloat(1.0).to_type(Array)
