require "narray"
require "nimage_c"

def NImage.show image

  r1 = image.min
  r2 = image.max
  b  = NArray.byte(*image.shape)
  b[]= (image.to_f-r1)/(r2-r1)*(NImage.ncolors-1)

  NImage.new b.to_s, *b.shape
end

def NImage.standard_color
  ncol = 32/4
  t = NArray.byte(ncol).indgen! * (255.0/ncol)
  r = NArray.byte(ncol,4)
  g = NArray.byte(ncol,4)
  b = NArray.byte(ncol,4)

  # Color map
  r[0..-1,1] = 
    g[0..-1,2] = 
    b[0..-1,0] = 
    b[-1..0,1] = 
    b[0..-1,3] = t
  r[0..-1,2] = 
    r[0..-1,3] = 
    g[0..-1,3] = 255

  NImage.colormap r.to_s, g.to_s, b.to_s
end

# Initalize colormap
#NImage.standard_color

#
#    TEST TEST TEST
#
if __FILE__ == $0

  x = NArray.sfloat(200,1).indgen!(-100)/50*3.1415
  y = NArray.sfloat(1,200).indgen!(-100)/50*3.1415
  z = NMath.cos(x) * NMath.sin(y)

  v = NImage.show z

  print "Hit return key..."
  STDIN.getc
  v.close
end
