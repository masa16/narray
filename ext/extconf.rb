require 'mkmf'

srcs = %w(
bytedata
kwarg
step
narray
array
index
loop
datatype
bit
dcomplex
dfloat
int32
uint32
int64
nstruct
robject
pointer
math
rand
SFMT
)

# util method
def create_conf_h(file)
  print "creating #{file}\n"
  hfile = open(file, "w")
  for line in $defs
    line =~ /^-D(.*)/
    hfile.printf "#define %s 1\n", $1
  end
  hfile.close
end

have_header("atlas/cblas.h")
have_library("atlas")

if have_library("blas")
  if have_library("lapack")
    srcs.push "linalg"
    $defs.push "-DHAVE_LAPACK"
  else
    #$defs.delete "-DHAVE_LAPACK"
  end
end

if have_header("sys/types.h")
  header = "sys/types.h"
else
  header = nil
end

have_type("int32_t", header)
have_type("u_int32_t", header)
have_type("int64_t", header)
have_type("u_int64_t", header)
#have_library("m")
#have_func("sincos")
#have_func("asinh")

$objs = srcs.collect{|i| i+".o"}

create_conf_h("narray_config.h")

create_makefile('narray')
