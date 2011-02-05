require 'test/unit'
require "complex"
require "narray"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? Num::NArray # a.class < Num::NArray
      a.debug_info
      a.debug_print
    end
    puts
    a
  end

  def test_dcomplex
    #GC.disable
    px "$a = Num::RObject.new(8).seq"
    px "$a = Num::RObject.cast([1,2,3,-4,-5.5,-6,7,8])"
    px "$a.abs"
    px "-$a"
    px "$a.floor"
    px "$a + $a"
    px "$a.gt -$a"
    px "$a.sum"
    px "$a.im"
    px "$a.to_s"
    px "$a.to_a"
    px "$b = Num::DFloat.new(8).seq"
    px "$c = Num::RObject.cast($b)"
    px "$a + $b"
    #px "Num::DFloat.cast($c)"
    px "$d = Num::Int8.new(8).seq"
    px "$e = Num::RObject.cast($d)"
    px "$f = Num::DComplex.new(8).seq"
    px "$g = Num::RObject.cast($f)"
  end
end
