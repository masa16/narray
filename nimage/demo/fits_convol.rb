# FITS-FFT demo 
require 'fits'
require 'nimage'

f = Fits.new
f.read( $*.shift || 'dss_m51.fits' )
nx,ny = f.data.shape

NImage.show f.data #, 'M51'

# FFT low-pass filtering
z = 50.0/nx
x = NArray.float(nx).indgen! * z
y = NArray.float(ny).indgen! * z
x[-(nx/2)..-1] = x[nx/2..1]
y[-(ny/2)..-1] = y[ny/2..1]
y.reshape!(1,ny)
# Operation between [nx,1] and [1,ny] arrays makes [nx,ny].
filt = NMath::exp(-(x**2)-(y**2))
filt /= filt.sum

print "Now executing FFT...\n"
lowpass = FFTW::convol(f.data,filt)

NImage.show lowpass #, 'M51 (FFT low-pass)'

print "Hit return key..."
STDIN.getc
