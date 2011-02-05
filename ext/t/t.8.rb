require './narray.so'

#NArray.debug=true
p a = NArray::DFloat[1,2,3]
p a.to_a
p b = a.eq([1,2,3])
#p b.count_true
