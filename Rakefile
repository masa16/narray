
def version
  open("narray.h") do |f|
    f.each_line do |l|
      return $1 if /NARRAY_VERSION "([\d.]+)"/ =~ l
    end
  end
end

GEM = "narray-#{version}.gem"

task :default => GEM

file GEM => "ext" do
  sh "gem build narray.gemspec"
end

file "ext" do
  sh "ln -s . ext"
end

task :install => GEM do
  sh "gem install -V --backtrace #{GEM}"
end

task :push => GEM do
  sh "gem push #{GEM}"
end
