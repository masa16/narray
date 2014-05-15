=begin
= Numerical Ruby NArray

((<English|URL:index.en.html>)) / ((<Japanese|URL:index.ja.html>))

== NArray development

((<GitHub|URL:https://github.com/masa16/narray-devel>))

Next NArray is under development.
The new version is incompatible but much advanced from the old version.
Development of the old version ended.
Comments are welcome.

== NArray old version

((<GitHub|URL:https://github.com/masa16/narray>))

((*NArray*)) is an Numerical N-dimensional Array class.
Supported element types are 1/2/4-byte Integer,
single/double-precision Real/Complex, and Ruby Object.
This extension library incorporates fast calculation and easy manipulation
of large numerical arrays into the Ruby language.
NArray has features similar to NumPy, but NArray has vector and matrix subclasses.

* Documents
  * ((<NArray method list|URL:SPEC.en>))
  * ((<princelab's NArray documents|URL:https://github.com/princelab/narray/wiki>))
    * Tentative NArray Tutorial, etc.

* Examples
  * ((<Mandelbrot|URL:demo/mandel.en.html>))
  * ((<Image shrinking|URL:demo/image.en.html>))
  * ((<Image smoothing using FFTW|URL:demo/smooth.en.html>))
  * ((<Life game|URL:demo/lifegame.en.html>))
  * ((<NMatrix & NVector - solving linear equation|URL:matrix-e.html>))

* Install
  * Source code: version _narray_version_ (((<.tar.gz|URL:https://github.com/masa16/narray/tarball/_narray_version_>))|((<.zip|URL:https://github.com/masa16/narray/zipball/_narray_version_>))) (_narray_release_date_)
  * Gem install (requires compiler):
 gem install narray

== Extension for NArray

=== FFTW3

A FFTW3 wrapper for NArray.
((<FFTW|URL:http://www.fftw.org/>)) is a fast DFT (Discrete Fourier Transform) library.

* Source code:
  ((<ver 0.1.0|URL:http://rubyforge.org/frs/download.php/7661/ruby-fftw3-0.1.0.tar.gz>))
  (2003-01-04)

=== na_geev

Eigenvalue & Eigenvector method for NMatrix
using dgeev & zgeev routine in ((<Lapack|URL:http://www.netlib.org/lapack/>)).
Effort to wrap other Lapack routines is ongoing very slowly.

* Source code:
  ((<ver 0.1.2|URL:http://rubyforge.org/frs/download.php/7660/na_geev-0.1.2.tar.gz>))
  (2004-03-01)

=== Projects using NArray

* ((<Dennou Ruby Project|URL:http://ruby.gfd-dennou.org/>)) (RubyDCL, RubyNetCDF, ...)
* ((<"Ruby/GSL"|URL:http://rb-gsl.rubyforge.org/>))
* ((<GridFlow|URL:http://gridflow.ca/>))
* ((<"ruby/audio"|URL:http://hans.fugal.net/src/ruby-audio/>))
* ((<"Ruby/PGPLOT"|URL:http://pgplot.rubyforge.org/>))

== Other Tips

* ((<Ruby Multi-dimensional Arrays|URL:mdary.html>))

== Link

* ((<Ruby for Science @ Rwiki|URL:http://pub.cozmixng.org/~the-rwiki/rw-cgi.rb?cmd=view;name=Ruby+for+Science>)) (Japanese)
* ((<SciRuby Wiki|URL:http://sciruby.codeforpeople.com/sr.cgi/FrontPage>))

== Author

Masahiro Tanaka

=end
