require "./narray.so"
#NArray.debug=true
p a = NArray::DFloat.cast([5,4,2,1,3,-1,-2,-3,-4,-5])
b = a.sort!
p a
