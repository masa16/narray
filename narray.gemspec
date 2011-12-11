# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = "narray"
  s.version = "0.6.0.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Masahiro Tanaka"]
  s.date = "2011-08-27"
  s.description = "Numerical N-dimensional Array class"
  s.email = "masa16.tanaka@gmail.com"
  s.extensions = ["src/extconf.rb"]
  s.files = ["src/extconf.rb"]
  s.homepage = "http://narray.rubyforge.org/"
  s.require_paths = ["."]
  s.rubyforge_project = "narray"
  s.rubygems_version = "1.8.10"
  s.summary = "N-dimensional Numerical Array class for Ruby"

  if s.respond_to? :specification_version then
    s.specification_version = 2

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end
