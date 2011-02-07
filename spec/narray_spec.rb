require "narray"

for klass in [NArray::DFloat, NArray::Int32, NArray::Int16, NArray::Int8]
  array = [1,2,3]

  describe klass, "when cast(#{array})" do
    before do
      @narray = klass.cast(array)
    end

    it "== #{array} " do
      @narray.should == array
    end

    it "kind_of #{klass}" do
      @narray.should be_kind_of klass
    end

    it ".to_a == #{array} " do
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

    it "kind_of #{klass}" do
      @narray.should be_kind_of klass
    end

    it "== #{result} " do
      @narray == result
    end

    after do
      @narray = nil
    end
  end

  describe klass, "when new([5]).seq" do
    before do
      @narray = klass.new([5]).seq
    end

    it "kind_of #{klass}" do
      @narray.should be_kind_of klass
    end

    it "== #{result} " do
      @narray.should == result
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

    it "== #{result2} " do
      @narray == result2
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

      it "is kind_of #{klass}" do
        @narray_dfloat.should be_kind_of klass
      end

      it ".fill(1) is kind_of #{klass}" do
        @narray_dfloat.fill(1).should be_kind_of klass
      end

      it ".ndim is #{ndim}" do
        @narray_dfloat.ndim.should == ndim
      end

      it ".shape is #{shape}" do
        @narray_dfloat.shape.should == shape
      end

      it ".size is #{total}" do
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

      it ".seq is kind_of #{klass}" do
        @narray_dfloat.seq.should be_kind_of klass
      end

      after do
        @narray_dfloat = nil
      end
    end
  end
end
