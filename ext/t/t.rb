require "./narray.so"
#NArray.debug=true

a = NArray::DFloat[[5,4,2,1,3],[-1,-2,-3,-4,-5]]
b = nil
1000.times do
  b = a.sort

  #b = NArray::DFloat[[5,4,2,1,3],[-1,-2,-3,-4,-5]]
  #b.sort!

  #a.flatten
  #a.transpose
  #a.copy
  #a
  #p b
end
p b
