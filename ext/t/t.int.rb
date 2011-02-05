require "./narray.so"

a = NArray::Int32.cast([[1,3,7],[9,10,11],[1,2],[10]])
a.debug_info
p a
p a.to_a

a = NArray::Int32.cast([1,3,9,10])
a.debug_info
p a
p a.to_a

a = NArray::Int32.cast([[[1,3],[9,10]],[[1,2],[10]],0,1])
a.debug_info
p a
p a.to_a
