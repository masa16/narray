require 'narray'

a = NArray.float(4,3).indgen!
b = NArray.float(4,3)
b[] = 4

print "a = "; p a
print "b = "; p b
print "a<=>b = ";p a<=>b

#c = a.to_type(NArray::ROBJ) <=> b.to_type(NArray::ROBJ)
#p c

a = NArray.float(4,1).indgen!
b = NArray.float(1,3).indgen!

print "a = "; p a
print "b = "; p b
print "a<=>b = ";p a<=>b

exit

## following is obsolete
a = NArray.scomplex(4,1).indgen!
b = NArray.scomplex(1,3).indgen!.add!(-1).mul!(1.im).add!(1)

print "a = "; p a
print "b = "; p b
print "a<=>b = ";p a<=>b
