require 'narray'

a = [1.im, 2.im, 3.im, 1+1.im, 1+2.im, 1+3.im,]

a.each {|x| p [x,Math.tan(x)]}

b = NArray.to_na(a)
c = NMath.tan(b)
c.each {|x| p [x]}


