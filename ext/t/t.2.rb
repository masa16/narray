require "./narray.so"


a = NArray::DFloat.cast(123)
a.debug_info
p a

b = NArray::DFloat.cast([[1,2],7])
b.debug_info
p b

c = a+b
c.debug_info
p c

exit

a = NArray::DFloat.cast([123])
a.debug_info
p a

b = NArray::DFloat.cast([[1,2],7])
b.debug_info
p b

c = a+b
c.debug_info
p c

exit

a = NArray::DFloat.cast([[1,2],7])
a.debug_info
p a


a = NArray::DFloat.cast([1,2,3])
a.debug_info
p a
a = NArray::DFloat.cast([[1,2],[3,4]])
a.debug_info
p a

a = NArray::DFloat.cast([[1,2],[4,5],[6,7]])
a.debug_info
p a


a = NArray::DFloat.cast([[1,2,4],[2,5],[6]])
a.debug_info
p a

a = NArray::DFloat.cast([[1,2,4],[2,5],7])
a.debug_info
p a

a = NArray::DFloat.cast([[[1,2],7],4,[5]])
a.debug_info
p a


#exit



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
