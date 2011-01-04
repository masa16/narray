require 'rubygems'
require 'rake/gempackagetask'
require 'yaml'

spec = YAML.load <<EOF
--- !ruby/object:Gem::Specification 
name: narray
version: !ruby/object:Gem::Version 
  version: 0.5.9.9
platform: ruby
authors: 
- Masahiro Tanaka
autorequire: 
bindir: bin
cert_chain: []

date: 2008-01-27 00:00:00 -08:00
default_executable: 
dependencies: []

description: Numerical N-dimensional Array class
email: masa16.tanaka@gmail.com
executables: []

extensions: 
- extconf.rb
extra_rdoc_files: []

files: 
- ChangeLog
- MANIFEST
- README.en
- README.ja
- SPEC.en
- SPEC.ja
- depend
- extconf.rb
- mkmath.rb
- mknafunc.rb
- mkop.rb
- na_array.c
- na_func.c
- na_index.c
- na_linalg.c
- na_random.c
- narray.c
- narray.def
- narray.h
- narray_local.h
- lib/narray_ext.rb
- lib/nmatrix.rb
- test/statistics.rb
- test/testarray.rb
- test/testbit.rb
- test/testcast.rb
- test/testcomplex.rb
- test/testfftw.rb
- test/testindex.rb
- test/testindexary.rb
- test/testindexset.rb
- test/testmask.rb
- test/testmath.rb
- test/testmath2.rb
- test/testmatrix.rb
- test/testmatrix2.rb
- test/testmatrix3.rb
- test/testminmax.rb
- test/testobject.rb
- test/testpow.rb
- test/testrandom.rb
- test/testround.rb
- test/testsort.rb
- test/teststr.rb
- test/testtrans.rb
- test/testwhere.rb
has_rdoc: false
homepage: http://narray.rubyforge.org/
post_install_message: 
rdoc_options: []

require_paths: 
- .
required_ruby_version: !ruby/object:Gem::Requirement 
  requirements: 
  - - ">="
    - !ruby/object:Gem::Version 
      version: "0"
  version: 
required_rubygems_version: !ruby/object:Gem::Requirement 
  requirements: 
  - - ">="
    - !ruby/object:Gem::Version 
      version: "0"
  version: 
requirements: []

rubyforge_project: narray
rubygems_version: 1.0.1
signing_key: 
specification_version: 2
summary: N-dimensional Numerical Array class for Ruby
test_files: []

EOF

Rake::GemPackageTask.new(spec) do |pkg|
  pkg.need_zip = true
  pkg.need_tar = true
end
