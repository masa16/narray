require "mkmf"

srcs = %w(
narray
na_array
na_func
na_index
na_op
na_math
na_linalg
)

have_library("m")
have_func("sincos")
have_func("asinh")

if have_header("fftw.h")
  if have_library("fftw", "fftwnd_create_plan")
    srcs.push "na_fftw"
  else
    $defs.delete "-DHAVE_FFTW_H"
  end
end

$objs = srcs.collect{|i| i+".o"}

dir_config("narray")

create_makefile("narray")
