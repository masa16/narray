require "./narray.so"

#NArray.debug=true

c = NArray::Bit.new([2,3,7])
c.store(1)
d = c['*,1,::2']
d.store(0)

p c

p n = c.count_true
p n = c.count_false

p c['+,*,+']

p c['+,*,+'].count_true
p c['+,*,+'].count_false

p c['+,+,*'].count_false
p c['*,+,+'].count_false
p c['*,+,*'].count_false
exit


d = c['+,*,+']
p d.class
p d.shape
puts "----------------------------"
n = d.count_false
puts "----------------------------"
p n.class
p n.ndim
p n.shape
puts "----------------------------"
p n
exit



exit



a = NArray::Bit.cast([0,1,1,0,0,1])
a.debug_info
p a


a = NArray::Bit.cast([[0,1,1],[0,0,1],[0,1],1])
a.debug_info
p a
