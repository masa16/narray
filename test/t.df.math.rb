require 'test/unit'
require "complex"
require "./narray.so"
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

  def test_dfloat_math
    #GC.disable
    px "$a = Num::DFloat.new(5).seq"
    px "Num::DFloat.sqrt($a)"
    px "Num::DFloat.cbrt($a)"
    px "Num::DFloat.log($a)"
    px "Num::DFloat.log2($a)"
    px "Num::DFloat.log10($a)"
    px "Num::DFloat.exp($a)"
    px "Num::DFloat.exp2($a)"
    px "Num::DFloat.exp10($a)"
    px "Num::DFloat.sin($a)"
    px "Num::DFloat.cos($a)"
    px "Num::DFloat.tan($a)"
    px "Num::DFloat.asin($a)"
    px "Num::DFloat.acos($a)"
    px "Num::DFloat.atan($a)"
    px "Num::DFloat.sinh($a)"
    px "Num::DFloat.cosh($a)"
    px "Num::DFloat.tanh($a)"
    px "Num::DFloat.asinh($a)"
    px "Num::DFloat.acosh($a)"
    px "Num::DFloat.atanh($a)"
    px "Num::DFloat.atan2($a,$a['::-1'])"
    px "Num::DFloat.hypot($a,$a['::-1'])"
  end
end
