# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name              = 'narray'
  s.version           = '0.5.9.9'
  s.platform          = Gem::Platform::RUBY
  s.authors           = ['Masahiro Tanaka']
  s.email             = ['masa16.tanaka@gmail.com']
  s.homepage          = 'http://narray.rubyforge.org/'
  s.summary           = %q{N-dimensional Numerical Array class for Ruby}
  s.description       = %q{Numerical N-dimensional Array class}

  s.rubyforge_project = 'narray'

  s.files      = Dir.glob('lib/*.rb') + Dir.glob('./*{.h,.c}')
  s.extensions = "./extconf.rb"
  s.require_paths << '.'
end

