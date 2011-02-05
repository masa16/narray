require "./narray.so"

for klass in [NArray::DFloat, NArray::Int32, NArray::Int16, NArray::Int8]
  array = [1,2,3]

  describe klass, "when cast(#{array})" do
    before do
      @narray = klass.cast(array)
    end

    it "should be kind_of #{klass}" do
      @narray.should be_kind_of klass
    end

    it "should to_a == #{array} " do
      @narray.to_a.should == array
    end

    after do
      @narray = nil
    end
  end

  result = [0,1,2,3,4]

  describe klass, "when new([#{result.size}]).seq" do
    before do
      @narray = klass.new([result.size]).seq
    end

    it "should be kind_of #{klass}" do
      @narray.should be_kind_of klass
    end

    it "should == #{result} " do
      @narray.eq(result).all?.should be_true
    end

    after do
      @narray = nil
    end
  end

  result2 = [[0,1],[2,3],[4,5]]

  describe klass, "when new([3,2]).seq" do
    before do
      @narray = klass.new([3,2]).seq
    end

    it "should to_a == #{result2} " do
      @narray.to_a.should == result2
    end

    after do
      @narray = nil
    end
  end
end

for klass in [NArray::DFloat, NArray::Int32, NArray::Bit]
  for shape in [[7], [7,8], [7,8,9]]
    ndim = shape.size
    total = shape.inject(1){|x,y| x*y}

    describe klass, "when new(#{shape})" do
      before do
        @narray_dfloat = klass.new(shape)
      end

      it "should be kind_of #{klass}" do
        @narray_dfloat.should be_kind_of klass
      end

      it "fill should be kind_of #{klass}" do
        @narray_dfloat.fill(1).should be_kind_of klass
      end

      it "should be ndim #{ndim}" do
        @narray_dfloat.ndim.should == ndim
      end

      it "should be shape #{shape}" do
        @narray_dfloat.shape.should == shape
      end

      it "should be size #{total}" do
        @narray_dfloat.size.should == total
      end

      after do
        @narray_dfloat = nil
      end
    end

  end
end


for klass in [NArray::DFloat, NArray::Int32]
  for shape in [[7], [7,8], [7,8,9]]
    ndim = shape.size
    total = shape.inject(1){|x,y| x*y}

    describe klass, "when new(#{shape})" do
      before do
        @narray_dfloat = klass.new(shape)
      end

      it "seq should be kind_of #{klass}" do
        @narray_dfloat.seq.should be_kind_of klass
      end

      after do
        @narray_dfloat = nil
      end
    end
  end
end
