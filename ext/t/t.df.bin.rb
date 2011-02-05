require "./narray.so"

a = NArray::DFloat.cast([1,2,3])
p a

#b = 1-a
b = a**3
p b
b.debug_info
c = a**2.5
p c
c.debug_info
