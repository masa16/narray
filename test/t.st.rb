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
      if a.class.kind_of? Num::NStruct
	puts "hoge"
	p a.class::DEFINITIONS
      end
      a.debug_print
    end
    puts
    a
  end

  def test_dfloat_op
    #GC.disable
    px "$a = Num::NStruct.new('A')"
    px "$b = Num::NStruct.new('B'){|m| puts inspect}"
    px "Hoge = Num::NStruct.new(){|m|
        add_type(Num::DFloat, :f64)
        add_type(Num::Int32, :i32)
     }"
    px "Hoge.byte_size"
    px "$x = Hoge.new([3,4])"
    px "$x.byte_size"
    px "$x.definition(0)"
    px "$x.definition(:f64)"
    px "$x.definition(1)"
    px "$x.definitions"
    px "Hoge::DEFINITIONS"
    px "$x[0..1,0]"
    px "$x.field(:f64)"
    px "$x.field(:i32)"
    px "$x.field(:i32).fill(123)"
    px "$x.field(:f64).seq.inplace * 2.4 + 0.5"
    px "$x[0..2,0].field(:f64)"
    px "$x.field(:f64)[0..2,0]"
    px "$x.field(:f64).copy"
    px "$x.field(:i32).seq.inplace * 2 + 3"
    px "$x.field(:i32).copy"
    px "$x.data.to_str"
    px "$x.fill([2.5,3])"
    px "$x.field(:i32)"
    px "$x.field(:f64)"
    px "$x.to_a"

    px "::Foo = Num::NStruct.new(){
        int8   :byte
        dfloat :float
    }"
    px "$y = Foo.new([2,3])"
    px "$y.fill([2,3.45])"
    px "$y.field(:byte)"
    px "$y.field(:byte).seq"
    px "$y.to_a"

    px "::Bar = Num::NStruct.new(){
        int8   :byte
        float64 :float, [2,2]
        dcomplex :compl
    }"
    px "$z = Bar.new([2,3])"
    px "$z.fill([2,[[1,2],[3,4]],0.123])"
    px "$z.field(:float)"
    px "$z[1,1..2].field(:float)"
    px "$z[1,1..2].field(:float).seq.inplace*111"
    px "$z.field(:byte)"
    px "$z.field(:byte).seq"
    px "$z.to_a"
    px "$z.definitions"
  end
end
