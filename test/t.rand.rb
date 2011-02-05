require 'test/unit'
#require "complex"
require "./narray.so"
#require 'irb/xmp'

$p = true

class TestBinData < Test::Unit::TestCase

  def px(s)
    puts s
    p a = eval(s)
    if a.kind_of? NArray
      a.debug_info
      p a
    end
    puts
    a
  end

  def test_nmath
    #GC.disable
    px "NArray::DFloat.new([10]).rand"
    return
    px "NArray::Int32.new([10]).rand"
    px "NArray::Int32.new([100]).rand(6)"
    #px "NArray::Int32.new([100]).rand(-6)"
    px "NArray::Int32.new([100]).rand(2**30)"
    px "NArray::Int8.new([100]).rand"
    px "NArray::Int8.new([100]).rand(1)"
    px "NArray::Int8.new([100]).rand(127)"
    px "NArray::NArray.srand(0)"
    px "NArray::DFloat.new([10]).rand"
    px "NArray::NArray.srand()"
    px "NArray::DFloat.new([10]).rand"

    px "NArray::Int64.new([10]).rand"
    px "NArray::Int32.new([10]).rand"
    px "NArray::UInt32.new([10]).rand"

    px "NArray::DComplex.new([10]).rand"

    px "NArray::UInt16.new([10]).rand(0x10000-1)"

    px "NArray::DFloat.new([20]).rand_norm"
    px "NArray::DComplex.new([20]).rand_norm"

    #px "NArray::Int8.new([100]).rand(0)"
  end
end
