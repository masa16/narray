require 'test/unit'
require "complex"
require "narray"
#require 'irb/xmp'

$p = true

class Complex
  def inspect
    sprintf "%g%+gi", self.real, self.imag
  end
end

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

  def test_dcomplex_math
    #GC.disable
    px "$a = Num::DComplex.new(5).seq+0.1+1.im"
    px "$a = Num::DComplex.cast([0.5, 0.5+0.5.im, 0.5.im, -0.5, -0.5-0.5.im, -0.5.im, 2, 2+2.im, 2.im,  -2.im, -2-2.im, -2.im])"
    px "Num::NMath.sqrt($a)"
    px "$a.to_a.collect{|x| Math.sqrt(x)}"
    #px "Num::NMath.cbrt($a)"
    px "Num::NMath.log($a)"
    px "$a.to_a.collect{|x| Math.log(x)}"
    px "Num::NMath.log2($a)"
    px "Num::NMath.log2(2**Num::DComplex.new(5).seq)"
    #px "$a.to_a.collect{|x| Math.log2(x)}"
    px "Num::NMath.log10($a)"
    px "Num::NMath.log10(10**Num::DComplex.new(5).seq)"
    px "$a.to_a.collect{|x| Math.log10(x)}"
    px "Num::NMath.exp($a)"
    px "$a.to_a.collect{|x| Math.exp(x)}"
    px "Num::NMath.exp2($a)"
    px "Num::NMath.exp2(Num::DComplex.new(5).seq)"
    px "Num::NMath.exp10($a)"
    px "Num::NMath.exp10(Num::DComplex.new(5).seq)"
    px "Num::NMath.sin($a)"
    px "$a.to_a.collect{|x| Math.sin(x)}"
    px "Num::NMath.cos($a)"
    px "$a.to_a.collect{|x| Math.cos(x)}"
    px "Num::NMath.tan($a)"
    px "$a.to_a.collect{|x| Math.tan(x)}"
    px "Num::NMath.asin($a)"
    px "$a.to_a.collect{|x| Math.asin(x)}"
    px "Num::NMath.acos($a)"
    px "$a.to_a.collect{|x| Math.acos(x)}"
    px "Num::NMath.atan($a)"
    px "$a.to_a.collect{|x| Math.atan(x)}"
    px "Num::NMath.sinh($a)"
    px "$a.to_a.collect{|x| Math.sinh(x)}"
    px "Num::NMath.cosh($a)"
    px "$a.to_a.collect{|x| Math.cosh(x)}"
    px "Num::NMath.tanh($a)"
    px "$a.to_a.collect{|x| Math.tanh(x)}"
    px "Num::NMath.asinh($a)"
    px "$a.to_a.collect{|x| Math.asinh(x)}"
    px "Num::NMath.acosh($a)"
    px "$a.to_a.collect{|x| Math.acosh(x)}"
    px "Num::NMath.atanh($a)"
    px "$a.to_a.collect{|x| Math.atanh(x)}"
    #px "Num::NMath.atan2($a,$a['::-1'])"
    #px "Num::NMath.hypot($a,$a['::-1'])"
  end
end
