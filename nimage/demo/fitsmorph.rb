# FITS demo 
require 'fits'
require 'nimage'

def triang x, px
  y = NArray.float(*x.shape)
  i1,i2  = (x>px).where2
  y[i1] = NMath.sin((1-x[i1]) * (Math::PI/2/(1-px)) )
  y[i2] = NMath.sin((1+x[i2]) * (Math::PI/2/(1+px)) )
  p y.min
  y
end

# Read-in Image
f = Fits.new
f.read( $*.shift || 'dss_m51.fits' )

NImage.show f.data

nx,ny = f.data.shape

# Morphing parameters
px,py = [ 0.3, -0.6]
dx,dy = [-0.3,  0.7]

indx = NArray.float(nx,1).indgen!
indy = NArray.float(1,ny).indgen!

ix = triang indx/(nx-1)*2-1, px
iy = triang indy/(ny-1)*2-1, py

ii = ix * iy
ix = (indx + ii*(dx*(nx-1)/2)).round
iy = (indy + ii*(dy*(ny-1)/2)).round

NImage.show f.data[ix+iy*nx]

print "Hit return key..."
STDIN.getc
