require './narray.so'

NArray.debug=true
p a = NArray::DFloat.cast([1,2,3])
p a.to_a
