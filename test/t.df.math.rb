require 'test/unit'
#require "complex"
require "narray"
#require 'irb/xmp'

$p = true

NMath = NArray::DFloat::Math
NMath = NArray::Math

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? NArray
      a.debug_info
    end
    puts
    a
  end

  def test_dfloat_math
    #GC.disable
    px "$a = NArray::DFloat.new([5]).seq"
    px "NMath.sqrt($a)"
    px "NMath.cbrt($a)"
    px "NMath.log($a)"
    px "NMath.log2($a)"
    px "NMath.log10($a)"
    px "NMath.exp($a)"
    px "NMath.exp2($a)"
    px "NMath.exp10($a)"
    px "NMath.sin($a)"
    px "NMath.cos($a)"
    px "NMath.tan($a)"
    px "NMath.asin($a)"
    px "NMath.acos($a)"
    px "NMath.atan($a)"
    px "NMath.sinh($a)"
    px "NMath.cosh($a)"
    px "NMath.tanh($a)"
    px "NMath.asinh($a)"
    px "NMath.acosh($a)"
    px "NMath.atanh($a)"
    px "NMath.atan2($a,$a['::-1'])"
    px "NMath.hypot($a,$a['::-1'])"
  end
end
