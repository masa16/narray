require "./narray.so"

c = NArray::DFloat.new([2,3,7])
c.store(1)
#c.fill(1)
p c

f = c[0,0..1,0..4]

p f
