# FITS-FFT demo 
require 'fits'
require 'nimage'

f = Fits.new
f.read( $*.shift || 'dss_m51.fits' )
nx,ny = f.data.shape

NImage.show f.data #, 'M51'

# FFT low-pass filtering
x = NArray.float(nx).indgen! / (nx*0.1)
y = NArray.float(ny).indgen! / (nx*0.1)
x[-nx/2] = x[nx/2-1..0]
y[-ny/2] = y[ny/2-1..0]
y.reshape!(1,ny)
# Operation between [nx,1] and [1,ny] arrays makes [nx,ny].
filt = 1 / (x**2 + y**2 + 1)

print "Now executing FFT...\n"
spec = FFTW.fftw(f.data,-1)
lowpass = FFTW.fftw(spec*filt,1).real / (nx*ny)

NImage.show  lowpass #, 'M51 (FFT low-pass)'

print "Hit return key..."
STDIN.getc
