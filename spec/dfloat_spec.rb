def px(s)
  puts s
  p a = eval(s)
  a.debug_info
  a.debug_print
  puts
  a
end

require "./narray.so"

describe NArray::DFloat, "when new([10,20])" do
  before do
    @narray_dfloat = NArray::DFloat.new([10,20])
  end

  it "should be kind_of NArray::DFloat" do
    @narray_dfloat.should be_kind_of NArray::DFloat
  end

  it "should be kind_of NArray::DFloat" do
    @narray_dfloat.fill(12).should be_kind_of NArray::DFloat
  end

  it "should be ndim 2" do
    @narray_dfloat.ndim.should == 2
  end

  it "should be shape [10,20]" do
    @narray_dfloat.shape.should == [10,20]
  end

  it "should be size 200" do
    @narray_dfloat.size.should == 200
  end

  after do
    @narray_dfloat = nil
  end
end
