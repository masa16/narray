open("ext/narray.h") do |f|
  f.each_line do |l|
    if /NARRAY_VERSION "([\d.]+)"/ =~ l
      NARRAY_VERSION = $1
      break
    end
  end
end

Gem::Specification.new do |s|
  s.name = "narray"
  s.version = NARRAY_VERSION

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Masahiro Tanaka"]
  s.date = Time.now.strftime("%F")
  s.description = "Numerical N-dimensional Array class"
  s.email = "masa16.tanaka@gmail.com"
  s.extensions = ["ext/extconf.rb"]
  s.files = %w[
ext/ChangeLog
ext/MANIFEST
ext/README.en
ext/README.ja
ext/SPEC.en
ext/SPEC.ja
ext/depend
ext/extconf.rb
ext/mkmath.rb
ext/mknafunc.rb
ext/mkop.rb
ext/na_array.c
ext/na_func.c
ext/na_index.c
ext/na_linalg.c
ext/na_random.c
ext/narray.c
ext/narray.def
ext/narray.h
ext/narray_local.h
ext/lib/narray_ext.rb
ext/lib/nmatrix.rb
]
  s.homepage = "http://narray.rubyforge.org/"
  s.require_paths = ["."]
  s.rubyforge_project = "narray"
  s.rubygems_version = "1.8.10"
  s.summary = "N-dimensional Numerical Array class for Ruby"
  s.license = "Ruby"

  if s.respond_to? :specification_version then
    s.specification_version = 2

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end
