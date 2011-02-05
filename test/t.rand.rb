require 'test/unit'
#require "complex"
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

  def test_nmath
    #GC.disable
    px "Num::DFloat.new([10]).rand"
    px "Num::Int32.new([10]).rand"
    px "Num::Int32.new([100]).rand(6)"
    #px "Num::Int32.new([100]).rand(-6)"
    px "Num::Int32.new([100]).rand(2**30)"
    px "Num::Int8.new([100]).rand"
    px "Num::Int8.new([100]).rand(1)"
    px "Num::Int8.new([100]).rand(127)"
    px "Num::NArray.srand(0)"
    px "Num::DFloat.new([10]).rand"
    px "Num::NArray.srand()"
    px "Num::DFloat.new([10]).rand"

    px "Num::Int64.new([10]).rand"
    px "Num::Int32.new([10]).rand"
    px "Num::UInt32.new([10]).rand"

    px "Num::DComplex.new([10]).rand"

    px "Num::UInt16.new([10]).rand(0x10000-1)"

    px "Num::DFloat.new([20]).rand_norm"
    px "Num::DComplex.new([20]).rand_norm"

    #px "Num::Int8.new([100]).rand(0)"
  end
end
